#pragma once
#include <automato_base/Automato.hpp>
#include <memory>

class AutomatoIdentificadores : public Automato {
public:
    AutomatoIdentificadores();

    std::unique_ptr<Automato> build() override;
    std::unique_ptr<Automato> clone() const override;
};