#include <automatos/AutomatoRelacionais.hpp>
#include <utils/lexer/LexerUtils.hpp>

AutomatoRelacionais::AutomatoRelacionais() : Automato({}, {}) {
    std::vector<NodeAutomato> nodes;

    // Nó raiz
    std::unordered_map<char, int> mapaRoot;
    mapaRoot['='] = 1;
    int idxRoot = addNode(nodes, mapaRoot);

    // == 
    int eq1 = addNode(nodes, {{'=', 5}});

    // <=
    int le1 = addNode(nodes, {{'=', 5}});
    nodes[idxRoot].mapaDestino['<'] = le1;

    // >=
    int ge1 = addNode(nodes, {{'=', 5}});
    nodes[idxRoot].mapaDestino['>'] = ge1;

    // !=
    int not1 = addNode(nodes, {{'=', 5}});
    nodes[idxRoot].mapaDestino['!'] = not1;

    // Nó final dos operadores
    addNode(nodes, {});

    std::set<int> finais = {2, 3, 5};
    this->nodes = nodes;
    this->estadosFinais = finais;
}

std::unique_ptr<Automato> AutomatoRelacionais::build() {
    return std::make_unique<AutomatoRelacionais>();
}

std::unique_ptr<Automato> AutomatoRelacionais::clone() const {
    return std::make_unique<AutomatoRelacionais>(*this);
}
