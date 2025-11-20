#ifndef SIMBOLO_HPP
#define SIMBOLO_HPP

#include <string>
#include <parser/ast/AstBase.hpp>

class Simbolo {
public:
    std::string nome;
    std::string tipo;
    int linha;
    bool inicializado;
    bool utilizado;

    Simbolo(const std::string& nome,
            const std::string& tipo,
            int linha,
            bool inicializado = false,
            bool utilizado = false);

    std::string toString() const;
};

struct SimboloStruct {
    std::string nome;
    std::shared_ptr<DeclaracaoStruct> decl;
    std::vector<std::tuple<std::string, std::string, bool>> atributos;
    std::vector<std::shared_ptr<DeclaracaoFuncao>> metodos;
    std::vector<std::string> implements;
    bool eInterface = false;
    int linha = 0;
};
#endif
