#include <semantic/types/tipo.hpp>
#include <sstream>



TipoPrimitivo::TipoPrimitivo(std::string nome) : nome(std::move(nome)) {}

std::string TipoPrimitivo::mostrar() const {
    return nome;
}

bool TipoPrimitivo::igual(const Tipo& other) const {
    const auto* o = dynamic_cast<const TipoPrimitivo*>(&other);
    return o && o->nome == nome;
}



VariavelTipo::VariavelTipo(std::string nome) : nome(std::move(nome)) {}

std::string VariavelTipo::mostrar() const {
    return nome;
}

bool VariavelTipo::igual(const Tipo& other) const {
    const auto* o = dynamic_cast<const VariavelTipo*>(&other);
    return o && o->nome == nome;
}



TipoFuncao::TipoFuncao(std::vector<TipoPtr> parametros, TipoPtr retorno)
    : parametros(std::move(parametros)), retorno(std::move(retorno)) {}

std::string TipoFuncao::mostrar() const {
    std::ostringstream out;
    out << "(";
    for (size_t i = 0; i < parametros.size(); i++) {
        out << parametros[i]->mostrar();
        if (i + 1 < parametros.size())
            out << ", ";
    }
    out << ") -> " << retorno->mostrar();
    return out.str();
}

bool TipoFuncao::igual(const Tipo& other) const {
    const auto* o = dynamic_cast<const TipoFuncao*>(&other);
    if (!o || o->parametros.size() != parametros.size())
        return false;

    for (size_t i = 0; i < parametros.size(); i++)
        if (!parametros[i]->igual(*o->parametros[i])) return false;

    return retorno->igual(*o->retorno);
}


TipoGenerico::TipoGenerico(std::string construtor, std::vector<TipoPtr> argumentos)
    : construtor(std::move(construtor)), argumentos(std::move(argumentos)) {}

std::string TipoGenerico::mostrar() const {
    std::ostringstream out;
    out << construtor << "<";
    for (size_t i = 0; i < argumentos.size(); i++) {
        out << argumentos[i]->mostrar();
        if (i + 1 < argumentos.size())
            out << ", ";
    }
    out << ">";
    return out.str();
}

bool TipoGenerico::igual(const Tipo& other) const {
    const auto* o = dynamic_cast<const TipoGenerico*>(&other);
    if (!o || o->construtor != construtor || o->argumentos.size() != argumentos.size())
        return false;

    for (size_t i = 0; i < argumentos.size(); i++)
        if (!argumentos[i]->igual(*o->argumentos[i])) return false;

    return true;
}
