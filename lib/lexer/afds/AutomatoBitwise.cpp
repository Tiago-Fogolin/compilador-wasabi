#include <automatos/AutomatoBitwise.hpp>
#include <utils/lexer/LexerUtils.hpp>

AutomatoBitwise::AutomatoBitwise() : Automato({}, {}) {
    std::vector<NodeAutomato> nodes;
    int idxRoot = addNode(nodes, {}); // nó inicial

    // Operadores simples: &, |, ^, ~
    nodes[idxRoot].mapaDestino['&'] = addNode(nodes, {}); // &
    nodes[idxRoot].mapaDestino['|'] = addNode(nodes, {}); // |
    nodes[idxRoot].mapaDestino['^'] = addNode(nodes, {}); // ^
    nodes[idxRoot].mapaDestino['~'] = addNode(nodes, {}); // ~

    // Operador <<
    int ltNode = addNode(nodes, {{'<', static_cast<int>(nodes.size() + 1)}});
    nodes[idxRoot].mapaDestino['<'] = ltNode;
    addNode(nodes, {}); // final de <<

    // Operador >>
    int gtNode = addNode(nodes, {{'>', static_cast<int>(nodes.size() + 1)}});
    nodes[idxRoot].mapaDestino['>'] = gtNode;
    addNode(nodes, {}); // final de >>

    std::set<int> finais = {0, 1, 2, 3, 4, 6, 8};

    this->nodes = nodes;
    this->estadosFinais = finais;
}

std::unique_ptr<Automato> AutomatoBitwise::build() {
    return std::make_unique<AutomatoBitwise>();
}

std::unique_ptr<Automato> AutomatoBitwise::clone() const {
    return std::make_unique<AutomatoBitwise>(*this);
}
