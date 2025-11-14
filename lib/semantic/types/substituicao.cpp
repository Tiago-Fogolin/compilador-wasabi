#include <semantic/types/substituicao.hpp>
#include <iostream>

TipoPtr Substituicao::aplicar(const TipoPtr& tipo) const {
    if (auto* v = dynamic_cast<VariavelTipo*>(tipo.get())) {
        if (mapa.count(v->nome)) {
            return aplicar(mapa.at(v->nome));
        }
        return tipo;
    }


    if (auto* f = dynamic_cast<TipoFuncao*>(tipo.get())) {
        std::vector<TipoPtr> novos_params;
        for (auto& p : f->parametros)
            novos_params.push_back(aplicar(p));

        return std::make_shared<TipoFuncao>(novos_params, aplicar(f->retorno));
    }


    if (auto* g = dynamic_cast<TipoGenerico*>(tipo.get())) {
        std::vector<TipoPtr> novos_args;
        for (auto& a : g->argumentos)
            novos_args.push_back(aplicar(a));

        return std::make_shared<TipoGenerico>(g->construtor, novos_args);
    }

    return tipo;
}

void Substituicao::adicionar(const std::string& var, TipoPtr tipo) {
    tipo = aplicar(tipo);
    mapa[var] = tipo;

    for (auto& [k, v] : mapa)
        if (k != var)
            v = aplicar(v);
}

Substituicao Substituicao::compor(const Substituicao& outra) const {
    Substituicao r;

    for (auto& [k, v] : mapa)
        r.mapa[k] = outra.aplicar(v);

    for (auto& [k, v] : outra.mapa)
        if (!r.mapa.count(k))
            r.mapa[k] = v;

    return r;
}

void Substituicao::imprimir() const {
    std::cout << "Substituicoes:\n";
    for (auto& [k, v] : mapa)
        std::cout << "  " << k << " = " << v->mostrar() << "\n";
}
