#pragma once
#include <automato_base/Automato.hpp>
#include <memory>

class AutomatoEspacos : public Automato {
public:
    AutomatoEspacos();

    std::unique_ptr<Automato> build() override;
    std::unique_ptr<Automato> clone() const override;
};