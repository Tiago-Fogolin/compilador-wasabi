#include <iostream>
#include <TokensAutomatos.hpp>


Automato criarAutomatoInteiros() {
    std::vector<NodeAutomato> nodes;

    // node 0
    // mapa node incial
    std::unordered_map<char, int> mapaRoot;

    mapaRoot['0'] = 1; // vai pro node 1 (continua no mesmo)

    for(char c = '1'; c <= '9'; c++) {
        mapaRoot[c] = 2; // 1-9 vai pro node 2
    }

    NodeAutomato root = NodeAutomato(
        mapaRoot
    );

    nodes.push_back(root);

    // Node 1
    std::unordered_map<char, int> mapaNode1;
    mapaNode1['0'] = 1; // se for zero continua no mesmo
    
    NodeAutomato node1 = NodeAutomato(
        mapaNode1
    );

    nodes.push_back(node1);

    // Node 2
    std::unordered_map<char, int> mapaNode2;
    for(char c = '0'; c <= '9'; c++) {
        mapaNode2[c] = 2; // 0-9 vai pro node 2 (continua no mesmo)
    }

    NodeAutomato node2 = NodeAutomato(
        mapaNode2
    );

    nodes.push_back(node2);

    // tanto estado 1 quanto estado 2 são estados finais válidos
    std::set<int> estadosFinais;
    estadosFinais.insert(1); 
    estadosFinais.insert(2);


    Automato automatoInteiros = Automato(
        nodes,
        estadosFinais
    );

    return automatoInteiros;

}