#include <semantic/types/unificador.hpp>
#include <sstream>

bool Unificador::ocorre(const std::string& var, TipoPtr tipo, const Substituicao& sub) const {
    tipo = sub.aplicar(tipo);

    if (auto* v = dynamic_cast<VariavelTipo*>(tipo.get()))
        return v->nome == var;

    if (auto* f = dynamic_cast<TipoFuncao*>(tipo.get())) {
        for (auto& p : f->parametros)
            if (ocorre(var, p, sub)) return true;
        return ocorre(var, f->retorno, sub);
    }

    if (auto* g = dynamic_cast<TipoGenerico*>(tipo.get())) {
        for (auto& a : g->argumentos)
            if (ocorre(var, a, sub)) return true;
    }

    return false;
}

Substituicao Unificador::unificar(TipoPtr t1, TipoPtr t2, Substituicao sub) {
    t1 = sub.aplicar(t1);
    t2 = sub.aplicar(t2);

    if (t1->igual(*t2))
        return sub;

    // Variável
    if (auto* v = dynamic_cast<VariavelTipo*>(t1.get())) {
        if (ocorre(v->nome, t2, sub))
            throw ErroUnificacao("Tipo recursivo infinito: " + v->nome);
        sub.adicionar(v->nome, t2);
        return sub;
    }

    if (auto* v = dynamic_cast<VariavelTipo*>(t2.get())) {
        if (ocorre(v->nome, t1, sub))
            throw ErroUnificacao("Tipo recursivo infinito: " + v->nome);
        sub.adicionar(v->nome, t1);
        return sub;
    }

    if (auto* f1 = dynamic_cast<TipoFuncao*>(t1.get())) {
        auto* f2 = dynamic_cast<TipoFuncao*>(t2.get());
        if (!f2 || f1->parametros.size() != f2->parametros.size())
            throw ErroUnificacao("Funções incompatíveis");

        for (size_t i = 0; i < f1->parametros.size(); i++)
            sub = unificar(f1->parametros[i], f2->parametros[i], sub);

        return unificar(f1->retorno, f2->retorno, sub);
    }


    if (auto* g1 = dynamic_cast<TipoGenerico*>(t1.get())) {
        auto* g2 = dynamic_cast<TipoGenerico*>(t2.get());
        if (!g2 || g1->construtor != g2->construtor
                || g1->argumentos.size() != g2->argumentos.size())
            throw ErroUnificacao("Tipos genéricos incompatíveis");

        for (size_t i = 0; i < g1->argumentos.size(); i++)
            sub = unificar(g1->argumentos[i], g2->argumentos[i], sub);

        return sub;
    }

    throw ErroUnificacao("Não é possível unificar " + t1->mostrar() + " com " + t2->mostrar());
}
