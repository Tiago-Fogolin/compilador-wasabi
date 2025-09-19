#pragma once
#include <automato_base/Automato.hpp>
#include <memory>

class AutomatoChar : public Automato {
public:
    AutomatoChar();

    std::unique_ptr<Automato> build() override;
    std::unique_ptr<Automato> clone() const override;
};