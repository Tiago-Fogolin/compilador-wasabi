#ifndef TABELA_SIMBOLOS_HPP
#define TABELA_SIMBOLOS_HPP

#include <vector>
#include <unordered_map>
#include <string>
#include "Simbolo.hpp"

class TabelaSimbolos {
private:
    bool deveEmitirAvisos;
    std::vector<std::map<std::string, std::shared_ptr<Simbolo>>> escopos;
    std::unordered_map<std::string, SimboloStruct> structs;

public:
    TabelaSimbolos(bool avisar = true);

    int nivelEscopo() const;

    void entrarEscopo();
    void sairEscopo();

    bool declarar(const std::string& nome,
                  const std::string& tipo,
                  int linha,
                  bool inicializado = false,
                  int tam = -1);

    void registrarStruct(const SimboloStruct& s);

    SimboloStruct* buscarStruct(const std::string& nome);

    Simbolo* buscar(const std::string& nome);
    Simbolo* buscarEscopoAtual(const std::string& nome);

    void marcarUtilizado(const std::string& nome);
    void marcarInicializado(const std::string& nome);

    bool foiDeclarado(const std::string& nome) const;
    std::string obterTipo(const std::string& nome) const;

    void setEmitirAvisos(bool estado);

    void imprimir() const;
};

#endif
