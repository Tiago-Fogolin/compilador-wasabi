#ifndef SUBSTITUICAO_HPP
#define SUBSTITUICAO_HPP

#include "tipo.hpp"
#include <unordered_map>

class Substituicao {
public:
    std::unordered_map<std::string, TipoPtr> mapa;

    TipoPtr aplicar(const TipoPtr& tipo) const;

    void adicionar(const std::string& var, TipoPtr tipo);

    Substituicao compor(const Substituicao& outra) const;

    void imprimir() const;
};

#endif
