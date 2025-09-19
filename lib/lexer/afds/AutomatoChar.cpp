#include <automatos/AutomatoChar.hpp>
#include <utils/lexer/LexerUtils.hpp>

AutomatoChar::AutomatoChar() : Automato({}, {}) {
    std::vector<NodeAutomato> nodes;

    // 0 - espera abrir '
    std::unordered_map<char,int> rootMap;
    rootMap['\''] = 1;
    addNode(nodes, rootMap);

    // 1 - espera 1 caractere válido ou um escape
    std::unordered_map<char,int> node1Map;
    node1Map['\\'] = 2; // começa escape
    addValidCharLiterals(node1Map, 3); // char simples vai direto pro estado 3
    int idxNode1 = addNode(nodes, node1Map);

    // 2 - espera o caracter de escape, vai pro estado 3
    std::unordered_map<char,int> node2Map;
    addValidEscapes(node2Map, 3);
    int idxNode2 = addNode(nodes, node2Map);

    // 3 - espera fechar com '
    std::unordered_map<char,int> node3Map;
    node3Map['\''] = 4;
    int idxNode3 = addNode(nodes, node3Map);

    // 4 - final
    int idxNode4 = addNode(nodes, {});

    std::set<int> finais = {idxNode4};

    this->nodes = nodes;
    this->estadosFinais = finais;
}

std::unique_ptr<Automato> AutomatoChar::build() {
    return std::make_unique<AutomatoChar>();
}

std::unique_ptr<Automato> AutomatoChar::clone() const {
    return std::make_unique<AutomatoChar>(*this);
}
