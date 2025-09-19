#pragma once
#include <automato_base/Automato.hpp>
#include <memory>

class AutomatoAtribuicao : public Automato {
public:
    AutomatoAtribuicao();

    std::unique_ptr<Automato> build() override;
    std::unique_ptr<Automato> clone() const override;
};