#include <automatos/AutomatoDelimitadores.hpp>
#include <utils/lexer/LexerUtils.hpp>

AutomatoDelimitadores::AutomatoDelimitadores() : Automato({}, {}) {
    std::vector<NodeAutomato> nodes;
    int idxRoot = addNode(nodes, {}); 

    std::unordered_map<char, int> mapaRoot;

    // node final de todos
    int idxNodeFinal = addNode(nodes, {});

    mapaRoot['('] = idxNodeFinal;
    mapaRoot[')'] = idxNodeFinal;
    mapaRoot['['] = idxNodeFinal;
    mapaRoot[']'] = idxNodeFinal;
    mapaRoot['{'] = idxNodeFinal;
    mapaRoot['}'] = idxNodeFinal;
    mapaRoot[','] = idxNodeFinal;
    mapaRoot[';'] = idxNodeFinal;
    mapaRoot[':'] = idxNodeFinal;
    mapaRoot['.'] = idxNodeFinal;

    nodes[idxRoot].mapaDestino = mapaRoot;

    std::set<int> finais = {idxNodeFinal};

    this->nodes = nodes;
    this->estadosFinais = finais;
}

std::unique_ptr<Automato> AutomatoDelimitadores::build() {
    return std::make_unique<AutomatoDelimitadores>();
}

std::unique_ptr<Automato> AutomatoDelimitadores::clone() const {
    return std::make_unique<AutomatoDelimitadores>(*this);
}
