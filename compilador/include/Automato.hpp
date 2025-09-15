#pragma once 
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <set>



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

    bool verificaEstadoFinal();
public:
    std::vector<NodeAutomato> nodes;
    std::set<int> estadosFinais; // estados finais válidos

    Automato(std::vector<NodeAutomato> nodes, std::set<int> estadosFinais);

    ResultadoProcessamento processarCaracter(char c);
};
