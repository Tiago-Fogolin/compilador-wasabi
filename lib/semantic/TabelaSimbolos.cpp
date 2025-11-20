#include <semantic/TabelaSimbolos.hpp>
#include <iostream>

TabelaSimbolos::TabelaSimbolos() {
    entrarEscopo();
}

int TabelaSimbolos::nivelEscopo() const {
    return escopos.size() - 1;
}

void TabelaSimbolos::entrarEscopo() {
    escopos.emplace_back();
}

void TabelaSimbolos::sairEscopo() {
    if (escopos.size() <= 1) {
        throw std::runtime_error("Tentativa de sair do escopo global");
    }

    auto& escopoAtual = escopos.back();
    for (const auto& [nome, simbolo] : escopoAtual) {
        if (!simbolo.utilizado) {
            std::cout << "Aviso: Variavel " << nome
                      << " declarada na linha " << simbolo.linha
                      << " nunca foi utilizada\n";
        }
    }

    escopos.pop_back();
}

bool TabelaSimbolos::declarar(const std::string& nome,
                              const std::string& tipo,
                              int linha,
                              bool inicializado)
{
    auto& atual = escopos.back();

    if (atual.count(nome) > 0)
        return false;

    atual.emplace(nome, Simbolo(nome, tipo, linha, inicializado));
    return true;
}

void TabelaSimbolos::registrarStruct(const SimboloStruct& s) {
    structs[s.nome] = s;
}

SimboloStruct* TabelaSimbolos::buscarStruct(const std::string& nome) {
    auto it = structs.find(nome);
    if (it == structs.end()) return nullptr;
    return &it->second;
}

Simbolo* TabelaSimbolos::buscar(const std::string& nome) {
    for (int i = escopos.size() - 1; i >= 0; --i) {
        auto& mapa = escopos[i];
        auto it = mapa.find(nome);
        if (it != mapa.end()) {
            return &it->second;
        }
    }
    return nullptr;
}

Simbolo* TabelaSimbolos::buscarEscopoAtual(const std::string& nome) {
    auto& mapa = escopos.back();
    auto it = mapa.find(nome);
    return (it != mapa.end() ? &it->second : nullptr);
}

void TabelaSimbolos::marcarUtilizado(const std::string& nome) {
    if (auto* s = buscar(nome)) {
        s->utilizado = true;
    }
}

void TabelaSimbolos::marcarInicializado(const std::string& nome) {
    if (auto* s = buscar(nome)) {
        s->inicializado = true;
    }
}

bool TabelaSimbolos::foiDeclarado(const std::string& nome) const {
    for (int i = escopos.size() - 1; i >= 0; --i) {
        if (escopos[i].count(nome))
            return true;
    }
    return false;
}

std::string TabelaSimbolos::obterTipo(const std::string& nome) const {
    for (int i = escopos.size() - 1; i >= 0; --i) {
        auto it = escopos[i].find(nome);
        if (it != escopos[i].end()) {
            return it->second.tipo;
        }
    }
    return "";
}

void TabelaSimbolos::imprimir() const {
    std::cout << "\n=== Tabela de Simbolos ===\n";
    for (size_t i = 0; i < escopos.size(); i++) {
        std::cout << "Escopo nivel " << i << ":\n";
        for (const auto& [nome, simbolo] : escopos[i]) {
            std::cout << "  " << simbolo.toString() << "\n";
        }
    }
    std::cout << "========================\n\n";
}
