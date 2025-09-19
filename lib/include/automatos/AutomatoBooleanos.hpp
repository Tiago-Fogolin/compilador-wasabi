#pragma once
#include <automato_base/Automato.hpp>
#include <memory>

class AutomatoBooleanos : public Automato {
public:
    AutomatoBooleanos();

    std::unique_ptr<Automato> build() override;
    std::unique_ptr<Automato> clone() const override;
};