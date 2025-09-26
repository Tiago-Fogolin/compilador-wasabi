#pragma once
#include <automato_base/Automato.hpp>
#include <memory>

class AutomatoComentarios : public Automato {
public:
    AutomatoComentarios();

    std::unique_ptr<Automato> build() override;
    std::unique_ptr<Automato> clone() const override;
};