#pragma once
#include <automato_base/Automato.hpp>
#include <memory>

class AutomatoRelacionais : public Automato {
public:
    AutomatoRelacionais();

    std::unique_ptr<Automato> build() override;
    std::unique_ptr<Automato> clone() const override;
};