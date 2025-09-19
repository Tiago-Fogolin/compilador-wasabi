#pragma once
#include <automato_base/Automato.hpp>
#include <memory>
#include <unordered_set>
#include <string>

class AutomatoPalavrasReservadas : public Automato {
private:
    std::unordered_set<std::string> palavras;
public:
    AutomatoPalavrasReservadas();

    bool processarString(const std::string& s) override;
    std::unique_ptr<Automato> build() override;
    std::unique_ptr<Automato> clone() const override;
};