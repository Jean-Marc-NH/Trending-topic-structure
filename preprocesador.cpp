
#include "preprocesador.h"
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <cctype>
#include <regex>
#include <map>

using namespace std;

PrepNoticias::PrepNoticias() {
    initStop();
}

void PrepNoticias::initStop() {
    const string palabrasArr[] = {
        "the","a","an","and","or","but","in","on","at","to","for","of","with","by",
        "as","is","was","were","be","been","have","has","had","do","does","did",
        "will","would","could","should","may","might","must","can","this","that",
        "these","those","it","its","they","them","their","i","you","he","she","we",
        "us","our","my","your","his","her","about","from","after","before","over",
        "under","between","during","against","per","via","up","down","out","into",
        "who","whom","which","what","when","where","why","how"
    };
    stops.clear();
    for (auto &w : palabrasArr) stops.insert(w);
}

static string minusc(const string &s) {
    string resultado = s;
    for (char &c : resultado) if (c >= 'A' && c <= 'Z') c = c + ('a' - 'A');
    return resultado;
}

static string limpiar(const string &s) {
    string salida;
    for (char c : s) {
        if (isalnum((unsigned char)c) || c==' ' || c=='-' || c=='/' || c=='.') salida.push_back(c);
        else salida.push_back(' ');
    }
    return salida;
}

static string quitarFecha(const string &texto) {
    size_t pos = 0;
    for (int lineaNum = 0; lineaNum < 2 && pos != string::npos; ++lineaNum) {
        size_t siguiente = texto.find('\n', pos);
        string linea = (siguiente==string::npos) ? texto.substr(pos) : texto.substr(pos, siguiente-pos);
        static regex patronDateline(R"(^\s*[A-Z][A-Z\s\-\.]{2,}\s+[-–—]{2,}\s*)");
        if (regex_search(linea, patronDateline)) {
            string restante = (siguiente==string::npos) ? string() : texto.substr(siguiente+1);
            return restante;
        }
        pos = (siguiente==string::npos) ? string::npos : siguiente + 1;
    }
    return texto;
}

static string extTitular(const string &contenido) {
    size_t pos = 0;
    while (true) {
        size_t siguiente = contenido.find('\n', pos);
        string linea = (siguiente==string::npos) ? contenido.substr(pos) : contenido.substr(pos, siguiente-pos);
        size_t inicio = linea.find_first_not_of(" \t\r");
        if (inicio==string::npos) return "";
        size_t fin = linea.find_last_not_of(" \t\r");
        string trimmed = linea.substr(inicio, fin-inicio+1);
        if (!trimmed.empty()) return trimmed;
        if (siguiente==string::npos) break;
        pos = siguiente + 1;
    }
    return "";
}

static vector<string> tokenizar(const string &s) {
    string limpio = limpiar(s);
    vector<string> tokens;
    string actual;
    for (size_t i=0;i<limpio.size();++i) {
        char c = limpio[i];
        if (isspace((unsigned char)c)) {
            if (!actual.empty()) { tokens.push_back(actual); actual.clear(); }
        } else actual.push_back(c);
    }
    if (!actual.empty()) tokens.push_back(actual);
    return tokens;
}

static vector<string> extFrasesCap(const string &texto) {
    vector<string> frases;
    vector<string> palabras;
    string palabra;
    for (size_t i = 0; i < texto.size(); ++i) {
        char c = texto[i];
        if (isspace((unsigned char)c) || ispunct((unsigned char)c)) {
            if (!palabra.empty()) { palabras.push_back(palabra); palabra.clear(); }
        } else {
            palabra.push_back(c);
        }
    }
    if (!palabra.empty()) palabras.push_back(palabra);

    vector<string> fraseActual;
    for (size_t i = 0; i < palabras.size(); ++i) {
        string w = palabras[i];
        if (w.empty()) continue;
        bool empiezaMayus = isupper((unsigned char)w[0]);
        bool todoMayus = true;
        for (char c : w) {
            if (isalpha((unsigned char)c) && !isupper((unsigned char)c)) {
                todoMayus = false;
                break;
            }
        }
        if (empiezaMayus && !todoMayus && w.length() > 1) {
            fraseActual.push_back(w);
        } else {
            if (fraseActual.size() >= 2) {
                string f;
                for (size_t j = 0; j < fraseActual.size(); ++j) {
                    if (j > 0) f += " ";
                    f += fraseActual[j];
                }
                frases.push_back(f);
            }
            fraseActual.clear();
        }
    }

    if (fraseActual.size() >= 2) {
        string f;
        for (size_t j = 0; j < fraseActual.size(); ++j) {
            if (j > 0) f += " ";
            f += fraseActual[j];
        }
        frases.push_back(f);
    }

    sort(frases.begin(), frases.end());
    frases.erase(unique(frases.begin(), frases.end()), frases.end());
    return frases;
}

static vector<string> extNums(const vector<string> &tokens) {
    vector<string> salida;
    static regex patronStats(R"(^\d+[-/]\d+(\.\d+)?$)");
    for (auto &t : tokens) {
        if (regex_match(t, patronStats) || (t.find("for")!=string::npos && t.size()>=3) ) {
            salida.push_back(t);
        } else if (t.find('.')!=string::npos && t.size()>=3) {
            salida.push_back(t);
        } else if (all_of(t.begin(), t.end(), [](char c){ return isdigit((unsigned char)c); })) {
            salida.push_back(t);
        }
    }
    sort(salida.begin(), salida.end());
    salida.erase(unique(salida.begin(), salida.end()), salida.end());
    return salida;
}

static vector<pair<string,int>> topKwd(const string &texto, const unordered_set<string>& stopwordsSet, int N=5) {
    vector<string> tokens = tokenizar(texto);
    map<string,int> frecuencia;
    for (auto &t : tokens) {
        string tl = minusc(t);
        if (tl.size()<=2) continue;
        if (stopwordsSet.count(tl)) continue;
        frecuencia[tl]++;
    }
    for (size_t i=0;i+1<tokens.size();++i) {
        string a = minusc(tokens[i]);
        string b = minusc(tokens[i+1]);
        if (a.size()<=2 || b.size()<=2) continue;
        if (stopwordsSet.count(a) || stopwordsSet.count(b)) continue;
        string big = a + " " + b;
        frecuencia[big] += 2;
    }
    vector<pair<string,int>> vec;
    for (auto &p : frecuencia) vec.push_back(p);
    sort(vec.begin(), vec.end(), [](const pair<string,int>&A,const pair<string,int>&B){
        if (A.second!=B.second) return A.second>B.second;
        return A.first<B.first;
    });
    if ((int)vec.size() > N) vec.resize(N);
    return vec;
}

string PrepNoticias::extTema(const string &contenido) const {
    if (contenido.empty()) {
        return "No reconocido";
    }

    string sinDateline = quitarFecha(contenido);
    string titular = extTitular(sinDateline);
    string cuerpo;
    size_t posLinea = sinDateline.find('\n');
    if (posLinea != string::npos) cuerpo = sinDateline.substr(posLinea + 1);
    else cuerpo = sinDateline;

    if (titular.empty()) {
        titular = contenido.substr(0, 100);
    }

    vector<string> frasesCapitalizadas = extFrasesCap(titular);
    if (frasesCapitalizadas.empty()) {
        frasesCapitalizadas = extFrasesCap(cuerpo.substr(0, 200));
    }

    string textoParaKeywords = titular + " " + cuerpo.substr(0, 500);
    auto mejoresKeywords = topKwd(textoParaKeywords, stops, 3);

    string topic;

    if (!frasesCapitalizadas.empty()) {
        topic = minusc(frasesCapitalizadas[0]);
        string limpio;
        for (char c : topic) {
            if (isalnum((unsigned char)c) || c == ' ') limpio += c;
        }
        topic = limpio;
        size_t inicio = topic.find_first_not_of(" ");
        size_t fin = topic.find_last_not_of(" ");
        if (inicio != string::npos && fin != string::npos) {
            topic = topic.substr(inicio, fin - inicio + 1);
        }
    }

    if (topic.empty() && !mejoresKeywords.empty()) {
        int cont = 0;
        for (const auto &kw : mejoresKeywords) {
            if (cont >= 2) break;
            if (kw.first.length() <= 2) continue;
            if (!topic.empty()) topic += " ";
            topic += kw.first;
            cont++;
        }
    }

    if (topic.empty()) {
        auto tokens = tokenizar(titular);
        int agregados = 0;
        for (auto &token : tokens) {
            string tokenL = minusc(token);
            if (tokenL.length() <= 2) continue;
            if (stops.count(tokenL)) continue;
            if (!topic.empty()) topic += " ";
            topic += tokenL;
            if (++agregados >= 2) break;
        }
    }

    if (topic.empty()) {
        auto tokens = tokenizar(contenido.substr(0, 100));
        int agregados = 0;
        for (auto &token : tokens) {
            string tokenL = minusc(token);
            if (tokenL.length() <= 2) continue;
            if (stops.count(tokenL)) continue;
            if (!topic.empty()) topic += " ";
            topic += tokenL;
            if (++agregados >= 2) break;
        }
    }

    if (topic.empty()) {
        return "No reconocido";
    }

    if (topic.length() > 40) {
        topic = topic.substr(0, 40);
    }

    size_t inicioFinal = topic.find_first_not_of(" ");
    if (inicioFinal == string::npos) {
        return "No reconocido";
    }

    return topic;
}

PrepRes PrepNoticias::extTodo(const string &rutaArchivo) const {
    PrepRes res;
    ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        res.error = true;
        res.errorMsg = "error_reading_file";
        return res;
    }
    string contenido;
    string linea;
    while (getline(archivo, linea)) {
        contenido += linea + "\n";
    }
    archivo.close();

    string sinDateline = quitarFecha(contenido);
    res.headline = extTitular(sinDateline);
    size_t pos = sinDateline.find('\n');
    res.body = (pos==string::npos) ? string() : sinDateline.substr(pos+1);

    string mezcla = res.headline + " " + res.body;
    auto kw = topKwd(mezcla, stops, 8);
    for (auto &p : kw) res.keywords.push_back(p.first);

    auto cap = extFrasesCap(mezcla);
    res.keyphrases = cap;

    vector<string> toks = tokenizar(mezcla);
    res.stats = extNums(toks);

    res.topic = extTema(contenido);
    res.error = false;
    return res;
}
