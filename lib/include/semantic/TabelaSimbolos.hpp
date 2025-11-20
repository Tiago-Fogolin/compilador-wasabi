#ifndef TABELA_SIMBOLOS_HPP
#define TABELA_SIMBOLOS_HPP

#include <vector>
#include <unordered_map>
#include <string>
#include "Simbolo.hpp"

class TabelaSimbolos {
private:
    std::vector<std::unordered_map<std::string, Simbolo>> escopos;
    std::unordered_map<std::string, SimboloStruct> structs;

public:
    TabelaSimbolos();

    int nivelEscopo() const;

    void entrarEscopo();
    void sairEscopo();

    bool declarar(const std::string& nome,
                  const std::string& tipo,
                  int linha,
                  bool inicializado = false);

    void registrarStruct(const SimboloStruct& s);

    SimboloStruct* buscarStruct(const std::string& nome);

    Simbolo* buscar(const std::string& nome);
    Simbolo* buscarEscopoAtual(const std::string& nome);

    void marcarUtilizado(const std::string& nome);
    void marcarInicializado(const std::string& nome);

    bool foiDeclarado(const std::string& nome) const;
    std::string obterTipo(const std::string& nome) const;

    void imprimir() const;
};

#endif
