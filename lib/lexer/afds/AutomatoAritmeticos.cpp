#include <automatos/AutomatoAritmeticos.hpp>
#include <utils/lexer/LexerUtils.hpp>

AutomatoAritmeticos::AutomatoAritmeticos() : Automato({}, {}) {
    std::vector<NodeAutomato> nodes;
    int idxRoot = addNode(nodes, {}); // nó inicial

    // Operadores simples
    std::unordered_map<char, int> mapaRoot;
    mapaRoot['+'] = 1; 
    mapaRoot['-'] = 2; 
    mapaRoot['*'] = 3; 
    mapaRoot['/'] = 4; 
    mapaRoot['%'] = 5; 
    nodes[idxRoot].mapaDestino = mapaRoot;

    // Operadores duplos
    std::unordered_map<char, int> mapaNode1;
    std::unordered_map<char, int> mapaNode2;
    std::unordered_map<char, int> mapaNode3;
    std::unordered_map<char, int> mapaNode4;
    mapaNode1['+'] = 5;
    mapaNode2['-'] = 6;
    mapaNode3['*'] = 7;
    mapaNode4['/'] = 8;
    addNode(nodes, mapaNode1);
    addNode(nodes, mapaNode2);
    addNode(nodes, mapaNode3);
    addNode(nodes, mapaNode4);

    // Nós finais dos operadores duplos
    int idxNode5 = addNode(nodes, {});
    int idxNode6 = addNode(nodes, {});
    int idxNode7 = addNode(nodes, {});
    int idxNode8 = addNode(nodes, {});
    
    std::set<int> finais;
    for (size_t i = 1; i <= 8; i++) {
        finais.insert(i);
    }
    
    this->nodes = nodes;
    this->estadosFinais = finais;
}

std::unique_ptr<Automato> AutomatoAritmeticos::build() {
    return std::make_unique<AutomatoAritmeticos>();
}

std::unique_ptr<Automato> AutomatoAritmeticos::clone() const {
    return std::make_unique<AutomatoAritmeticos>(*this);
}
