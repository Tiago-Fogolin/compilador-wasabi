#pragma once
#include <automato_base/Automato.hpp>
#include <memory>

class AutomatoString : public Automato {
public:
    AutomatoString();

    std::unique_ptr<Automato> build() override;
    std::unique_ptr<Automato> clone() const override;
};