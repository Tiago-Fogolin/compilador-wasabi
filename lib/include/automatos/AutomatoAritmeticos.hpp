#pragma once
#include <automato_base/Automato.hpp>
#include <memory>

class AutomatoAritmeticos : public Automato {
public:
    AutomatoAritmeticos();

    std::unique_ptr<Automato> build() override;
    std::unique_ptr<Automato> clone() const override;
};