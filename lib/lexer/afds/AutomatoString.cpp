#include <automatos/AutomatoString.hpp>
#include <utils/lexer/LexerUtils.hpp>

AutomatoString::AutomatoString() : Automato({}, {}) {
    std::vector<NodeAutomato> nodes;

    // 0 - espera abrir aspas "
    std::unordered_map<char,int> rootMap;
    rootMap['"'] = 1;
    addNode(nodes, rootMap);

    // 1 - dentro da string: aceita caracteres válidos ou escape
    std::unordered_map<char,int> node1Map;
    node1Map['"'] = 3;   // fecha string
    node1Map['\\'] = 2;  // inicia escape
    addValidStringChars(node1Map, 1); // caracteres normais continuam no estado 1
    int idxNode1 = addNode(nodes, node1Map);

    // 2 - depois da barra, espera escape válido e volta para 1
    std::unordered_map<char,int> node2Map;
    addValidEscapes(node2Map, 1);
    int idxNode2 = addNode(nodes, node2Map);

    // 3 - estado final (fechou a string)
    int idxNode3 = addNode(nodes, {});

    std::set<int> finais = {idxNode3};

    this->nodes = nodes;
    this->estadosFinais = finais;
}

std::unique_ptr<Automato> AutomatoString::build() {
    return std::make_unique<AutomatoString>();
}

std::unique_ptr<Automato> AutomatoString::clone() const {
    return std::make_unique<AutomatoString>(*this);
}
