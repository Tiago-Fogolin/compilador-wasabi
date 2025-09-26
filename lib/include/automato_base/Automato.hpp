#pragma once 
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <set>
#include <memory>

enum class ResultadoProcessamento {
    CONTINUA,
    ACEITO,
    INVALIDO
};

class NodeAutomato {
public:
    std::unordered_map<char, int> mapaDestino;

    NodeAutomato(std::unordered_map<char, int> mapaDestino);

    bool verificaTransicao(char c);
};

class Automato {
private:
    int indiceEstado = 0; // estado atual, começamos sempre em 0

    bool verificaTransicao(char caracter);

    int recuperarProximoEstado(char caracter);
public:
    std::vector<NodeAutomato> nodes;
    std::set<int> estadosFinais; // estados finais válidos

    Automato(std::vector<NodeAutomato> nodes, std::set<int> estadosFinais);

    bool verificaEstadoFinal();

    ResultadoProcessamento processarCaracter(char c);

    virtual bool processarString(const std::string& s);

    virtual std::unique_ptr<Automato> build() = 0;

    virtual std::unique_ptr<Automato> clone() const = 0;
};

class AutomatoFactory {
public:
    static std::vector<std::pair<std::string, std::unique_ptr<Automato>>> getAutomatos();
};
