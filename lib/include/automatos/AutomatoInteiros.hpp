#pragma once
#include <automato_base/Automato.hpp>
#include <memory>

class AutomatoInteiros : public Automato {
public:
    AutomatoInteiros();

    std::unique_ptr<Automato> build() override;
    std::unique_ptr<Automato> clone() const override;
};