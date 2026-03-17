#ifndef PREPROCESADOR_H
#define PREPROCESADOR_H

#include <string>
#include <vector>
#include <unordered_set>

struct PrepRes {
    bool error = false;
    std::string errorMsg;
    std::string headline;
    std::string body;
    std::string topic;
    std::vector<std::string> keywords;
    std::vector<std::string> keyphrases;
    std::vector<std::string> stats;
};

class PrepNoticias {
public:
    PrepNoticias();
    std::string extTema(const std::string &txt) const;
    PrepRes extTodo(const std::string &ruta) const;

private:
    void initStop();
    std::unordered_set<std::string> stops;
};

#endif
