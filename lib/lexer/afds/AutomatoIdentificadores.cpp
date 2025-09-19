#include <automatos/AutomatoIdentificadores.hpp>
#include <utils/lexer/LexerUtils.hpp>

AutomatoIdentificadores::AutomatoIdentificadores() : Automato({}, {}) {
    std::vector<NodeAutomato> nodes;

    std::unordered_map<char,int> rootMap;
    mapRange(rootMap, 'a', 'z', 1);
    mapRange(rootMap, 'A', 'Z', 1);
    rootMap['_'] = 1;
    addNode(nodes, rootMap);

    std::unordered_map<char,int> node1Map = rootMap;
    mapRange(node1Map, '0', '9', 1);
    int idxNode1 = addNode(nodes, node1Map);

    std::set<int> finais = {idxNode1};

    this->nodes = nodes;
    this->estadosFinais = finais;
}

std::unique_ptr<Automato> AutomatoIdentificadores::build() {
    return std::make_unique<AutomatoIdentificadores>();
}

std::unique_ptr<Automato> AutomatoIdentificadores::clone() const {
    return std::make_unique<AutomatoIdentificadores>(*this);
}
