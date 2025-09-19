#include <automatos/AutomatoInteiros.hpp>
#include <utils/lexer/LexerUtils.hpp>

AutomatoInteiros::AutomatoInteiros() : Automato({}, {}) {
    std::vector<NodeAutomato> nodes;

    std::unordered_map<char,int> rootMap;
    rootMap['0'] = 1;
    mapRange(rootMap, '1', '9', 2);
    addNode(nodes, rootMap);

    std::unordered_map<char,int> node1Map;
    node1Map['0'] = 1;
    int idxNode1 = addNode(nodes, node1Map);

    std::unordered_map<char,int> node2Map;
    mapRange(node2Map, '0', '9', 2);
    int idxNode2 = addNode(nodes, node2Map);

    std::set<int> finais = {idxNode1, idxNode2};

    this->nodes = nodes;
    this->estadosFinais = finais;
}

std::unique_ptr<Automato> AutomatoInteiros::build() {
    return std::make_unique<AutomatoInteiros>();
}

std::unique_ptr<Automato> AutomatoInteiros::clone() const {
    return std::make_unique<AutomatoInteiros>(*this);
}
