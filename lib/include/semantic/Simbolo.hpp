#ifndef SIMBOLO_HPP
#define SIMBOLO_HPP

#include <string>

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

#endif
