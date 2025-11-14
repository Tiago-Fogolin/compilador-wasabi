#ifndef UNIFICADOR_HPP
#define UNIFICADOR_HPP

#include "substituicao.hpp"

class ErroUnificacao : public std::runtime_error {
public:
    explicit ErroUnificacao(const std::string& msg) : std::runtime_error(msg) {}
};

class Unificador {
private:
    bool ocorre(const std::string& var, TipoPtr tipo, const Substituicao& sub) const;

public:
    Substituicao unificar(TipoPtr t1, TipoPtr t2, Substituicao sub = {});
};

#endif
