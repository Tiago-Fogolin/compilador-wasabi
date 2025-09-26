#include <automatos/AutomatoComentarios.hpp>
#include <utils/lexer/LexerUtils.hpp>

AutomatoComentarios::AutomatoComentarios() : Automato({}, {}) {
    std::vector<NodeAutomato> nodes;
    
    int q0 = addNode(nodes, {});
    std::unordered_map<char, int> mapaQ0;
    mapaQ0['/'] = 1;
    nodes[q0].mapaDestino = mapaQ0;

    int q1 = addNode(nodes, {});
    std::unordered_map<char, int> mapaQ1;
    mapaQ1['/'] = 2;
    mapaQ1['*'] = 4;
    nodes[q1].mapaDestino = mapaQ1;


    int q2 = addNode(nodes, {});
    std::unordered_map<char, int> mapaQ2;
    
    for (int c = 0; c <= 255; c++) {
        if (c != '\n') {
            mapaQ2[static_cast<char>(c)] = q2;
        }
    }

    mapaQ2['\n'] = 3;
    nodes[q2].mapaDestino = mapaQ2;
    
    int q3 = addNode(nodes, {});

    int q4 = addNode(nodes, {});
    std::unordered_map<char, int> mapaQ4;
    for (int c = 0; c <= 255; c++) {
        char ch = static_cast<char>(c);
        if (ch == '*') {
            mapaQ4[ch] = 5;
        } else {
            mapaQ4[ch] = q4;
        }
    }
    nodes[q4].mapaDestino = mapaQ4;


    int q5 = addNode(nodes, {});
    std::unordered_map<char, int> mapaQ5;
    for (int c = 0; c <= 255; c++) {
        char ch = static_cast<char>(c);
        if (ch == '/') {
            mapaQ5[ch] = 6;
        } else if (ch == '*') {
            mapaQ5[ch] = q5;
        } else {
            mapaQ5[ch] = q4;
        }
    }
    nodes[q5].mapaDestino = mapaQ5;


    int q6 = addNode(nodes, {});

    std::set<int> finais = {q2, q6};

    this->nodes = nodes;
    this->estadosFinais = finais;
}

std::unique_ptr<Automato> AutomatoComentarios::build() {
    return std::make_unique<AutomatoComentarios>();
}

std::unique_ptr<Automato> AutomatoComentarios::clone() const {
    return std::make_unique<AutomatoComentarios>(*this);
}