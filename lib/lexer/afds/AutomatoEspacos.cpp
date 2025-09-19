#include <automatos/AutomatoEspacos.hpp>
#include <utils/lexer/LexerUtils.hpp>

AutomatoEspacos::AutomatoEspacos() : Automato({}, {}) {
    std::vector<NodeAutomato> nodes;

    // 0 - aceita espaço, tab, \r, \n
    std::unordered_map<char,int> rootMap;
    rootMap[' '] = 1;
    rootMap['\t'] = 1;
    rootMap['\r'] = 1;
    rootMap['\n'] = 1;
    addNode(nodes, rootMap);

    // 1 - loop aceitando os mesmos caracteres
    std::unordered_map<char,int> node1Map = rootMap;
    int idxNode1 = addNode(nodes, node1Map);

    // estado final
    std::set<int> finais = {idxNode1};

    this->nodes = nodes;
    this->estadosFinais = finais;
}

std::unique_ptr<Automato> AutomatoEspacos::build() {
    return std::make_unique<AutomatoEspacos>();
}

std::unique_ptr<Automato> AutomatoEspacos::clone() const {
    return std::make_unique<AutomatoEspacos>(*this);
}
