#include <automatos/AutomatoPalavrasReservadas.hpp>
#include <utils/lexer/LexerUtils.hpp>

AutomatoPalavrasReservadas::AutomatoPalavrasReservadas() : Automato({}, {}) {
    palavras = {
        "interface","struct","implements","def","this","for","foreach","in",
        "if","else","while","break","continue","return",
        "int","float","string","bool","tuple","dict","set","void","null"
    };
}

bool AutomatoPalavrasReservadas::processarString(const std::string& s) {
    return this->palavras.count(s) > 0;
}

std::unique_ptr<Automato> AutomatoPalavrasReservadas::build() {
    return std::make_unique<AutomatoPalavrasReservadas>();
}

std::unique_ptr<Automato> AutomatoPalavrasReservadas::clone() const {
    return std::make_unique<AutomatoPalavrasReservadas>(*this);
}
