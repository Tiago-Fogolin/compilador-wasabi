#include <automatos/AutomatoBooleanos.hpp>
#include <utils/lexer/LexerUtils.hpp>

AutomatoBooleanos::AutomatoBooleanos() : Automato({}, {}) {
    std::vector<NodeAutomato> nodes;
    int idxRoot = addNode(nodes, {}); // root vazio

    SequenceResult falseSeq = addSequence(nodes, "false");
    SequenceResult trueSeq  = addSequence(nodes, "true");

    nodes[idxRoot].mapaDestino['f'] = falseSeq.first;
    nodes[idxRoot].mapaDestino['t'] = trueSeq.first;

    std::set<int> finais = {falseSeq.last, trueSeq.last};
    
    this->nodes = nodes;
    this->estadosFinais = finais;
}

std::unique_ptr<Automato> AutomatoBooleanos::build() {
    return std::make_unique<AutomatoBooleanos>();
}

std::unique_ptr<Automato> AutomatoBooleanos::clone() const {
    return std::make_unique<AutomatoBooleanos>(*this);
}
