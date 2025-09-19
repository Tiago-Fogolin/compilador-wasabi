#include <automatos/AutomatoLogico.hpp>
#include <utils/lexer/LexerUtils.hpp>

AutomatoLogico::AutomatoLogico() : Automato({}, {}) {
    std::vector<NodeAutomato> nodes;
    int idxRoot = addNode(nodes, {}); // nó inicial

    // '!' simples
    nodes[idxRoot].mapaDestino['!'] = 3; // final de '!'

    // '&&'
    int andNode = addNode(nodes, {{'&', 3}});
    nodes[idxRoot].mapaDestino['&'] = andNode;


    // '||'
    int orNode = addNode(nodes, {{'|', 3}});
    nodes[idxRoot].mapaDestino['|'] = orNode;


    addNode(nodes, {}); // final 

    std::set<int> finais = {3};

    this->nodes = nodes;
    this->estadosFinais = finais;
}

std::unique_ptr<Automato> AutomatoLogico::build() {
    return std::make_unique<AutomatoLogico>();
}

std::unique_ptr<Automato> AutomatoLogico::clone() const {
    return std::make_unique<AutomatoLogico>(*this);
}
