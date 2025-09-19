#pragma once
#include <automato_base/Automato.hpp>
#include <memory>

class AutomatoFlutuantes : public Automato {
public:
    AutomatoFlutuantes();

    std::unique_ptr<Automato> build() override;
    std::unique_ptr<Automato> clone() const override;
};