#pragma once
#include <automato_base/Automato.hpp>
#include <memory>

class AutomatoLogico : public Automato {
public:
    AutomatoLogico();

    std::unique_ptr<Automato> build() override;
    std::unique_ptr<Automato> clone() const override;

    
};