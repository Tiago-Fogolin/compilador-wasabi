#pragma once
#include <automato_base/Automato.hpp>
#include <memory>

class AutomatoCientifico : public Automato {
public:
    AutomatoCientifico();

    std::unique_ptr<Automato> build() override;
    std::unique_ptr<Automato> clone() const override;

    
};