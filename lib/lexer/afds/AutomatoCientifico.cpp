#include <automatos/AutomatoCientifico.hpp>
#include <utils/lexer/LexerUtils.hpp>

AutomatoCientifico::AutomatoCientifico() : Automato({}, {}) {
    std::vector<NodeAutomato> nodes;

    int idxRoot = addNode(nodes, {}); // nó inicial

    // Parte inteira [0-9]+
    int intPartNode = addNode(nodes, {});
    mapRange(nodes[idxRoot].mapaDestino, '0', '9', intPartNode);
    mapRange(nodes[intPartNode].mapaDestino, '0', '9', intPartNode); // múltiplos dígitos

    // Parte decimal opcional
    int decimalNode = addNode(nodes, {});
    nodes[intPartNode].mapaDestino['.'] = decimalNode;

    int fracNode = addNode(nodes, {});
    mapRange(nodes[decimalNode].mapaDestino, '0', '9', fracNode);
    mapRange(nodes[fracNode].mapaDestino, '0', '9', fracNode); // múltiplos dígitos

    // Exponencial [eE][+-]?[0-9]+
    int expNode = addNode(nodes, {});
    nodes[intPartNode].mapaDestino['e'] = expNode;
    nodes[intPartNode].mapaDestino['E'] = expNode;
    nodes[fracNode].mapaDestino['e'] = expNode;
    nodes[fracNode].mapaDestino['E'] = expNode;

    int signNode = addNode(nodes, {});
    nodes[expNode].mapaDestino['+'] = signNode;
    nodes[expNode].mapaDestino['-'] = signNode;

    int expPartNode = addNode(nodes, {});
    mapRange(nodes[expNode].mapaDestino, '0', '9', expPartNode);   // sem sinal
    mapRange(nodes[signNode].mapaDestino, '0', '9', expPartNode);  // com sinal
    mapRange(nodes[expPartNode].mapaDestino, '0', '9', expPartNode); // múltiplos dígitos

    std::set<int> finais = {expPartNode};

    this->nodes = nodes;
    this->estadosFinais = finais;
}

std::unique_ptr<Automato> AutomatoCientifico::build() {
    return std::make_unique<AutomatoCientifico>();
}

std::unique_ptr<Automato> AutomatoCientifico::clone() const {
    return std::make_unique<AutomatoCientifico>(*this);
}
