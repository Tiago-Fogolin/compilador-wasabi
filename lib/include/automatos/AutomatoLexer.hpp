#pragma once
#include <automato_base/Automato.hpp>
#include <memory>


class AutomatoLexer : public Automato {
public:
    std::unordered_map<int, TokenType> estadoFinaisTipos;
    AutomatoLexer();

    std::unique_ptr<Automato> build() override;
    std::unique_ptr<Automato> clone() const override;
};