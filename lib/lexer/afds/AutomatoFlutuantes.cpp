#include <automatos/AutomatoFlutuantes.hpp>
#include <utils/lexer/LexerUtils.hpp>

AutomatoFlutuantes::AutomatoFlutuantes() : Automato({}, {}) {
    std::vector<NodeAutomato> nodes;

    // Root
    std::unordered_map<char,int> rootMap;
    mapRange(rootMap, '1', '9', 1);
    rootMap['0'] = 3; // caso de 0.x
    addNode(nodes, rootMap);

    // Node 1: dígitos 1-9
    std::unordered_map<char,int> node1Map;
    mapRange(node1Map, '0', '9', 1); // continua no node1
    node1Map['.'] = 2; // ponto decimal
    addNode(nodes, node1Map);

    // Node 2: primeiro dígito depois do ponto
    std::unordered_map<char,int> node2Map;
    mapRange(node2Map, '0', '9', 4); // vai para node4
    int idxNode2 = addNode(nodes, node2Map);

    // Node 3: zero seguido de ponto
    std::unordered_map<char,int> node3Map;
    node3Map['.'] = 2;
    addNode(nodes, node3Map);

    // Node 4: dígitos depois do ponto
    std::unordered_map<char,int> node4Map;
    mapRange(node4Map, '0', '9', 4);
    int idxNode4 = addNode(nodes, node4Map);

    std::set<int> finais = {idxNode2, idxNode4};

    this->nodes = nodes;
    this->estadosFinais = finais;
}

std::unique_ptr<Automato> AutomatoFlutuantes::build() {
    return std::make_unique<AutomatoFlutuantes>();
}

std::unique_ptr<Automato> AutomatoFlutuantes::clone() const {
    return std::make_unique<AutomatoFlutuantes>(*this);
}
