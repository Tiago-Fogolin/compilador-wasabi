#include "string"
#pragma once
#include <automato_base/Automato.hpp>

struct Token {
    TokenType tipo;
    std::string valor;
    size_t linha;
    size_t coluna;
};