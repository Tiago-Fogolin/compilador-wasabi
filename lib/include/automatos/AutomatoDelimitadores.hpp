#pragma once
#include <automato_base/Automato.hpp>
#include <memory>

class AutomatoDelimitadores : public Automato {
public:
    AutomatoDelimitadores();

    std::unique_ptr<Automato> build() override;
    std::unique_ptr<Automato> clone() const override;
};