#include <automatos/AutomatoAtribuicao.hpp>
#include <utils/lexer/LexerUtils.hpp>

AutomatoAtribuicao::AutomatoAtribuicao() : Automato({}, {}) {
    std::vector<NodeAutomato> nodes;
    int idxRoot = addNode(nodes, {}); // nó inicial

    // '+='
    int plusNode = addNode(nodes, {{'=', 6}});
    nodes[idxRoot].mapaDestino['+'] = plusNode;

    // '-='
    int minusNode = addNode(nodes, {{'=', 6}});
    nodes[idxRoot].mapaDestino['-'] = minusNode;

    // '*='
    int multNode = addNode(nodes, {{'=', 6}});
    nodes[idxRoot].mapaDestino['*'] = multNode;

    // '/='
    int divNode = addNode(nodes, {{'=', 6}});
    nodes[idxRoot].mapaDestino['/'] = divNode;

    // '%='
    int modNode = addNode(nodes, {{'=', 6}});
    nodes[idxRoot].mapaDestino['%'] = modNode;

    // '=' simples
    addNode(nodes, {});
    nodes[idxRoot].mapaDestino['='] = 6;

    std::set<int> finais = {6};
    this->nodes = nodes;
    this->estadosFinais = finais;
}

std::unique_ptr<Automato> AutomatoAtribuicao::build() {
    return std::make_unique<AutomatoAtribuicao>();
}

std::unique_ptr<Automato> AutomatoAtribuicao::clone() const {
    return std::make_unique<AutomatoAtribuicao>(*this);
}
