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

Automato criarAutomatoBooleanos() {
    std::vector<NodeAutomato> nodes;

    // node 0, ou pra f ou pra t
    std::unordered_map<char, int> mapaNode0;

    mapaNode0['f'] = 1; // node 1 -> começo do false
    mapaNode0['t'] = 6; // node 6 -> começo do true

    NodeAutomato root = NodeAutomato(
        mapaNode0
    );

    nodes.push_back(root);

    // Nodes para 'false'
    std::unordered_map<char, int> mapaNode1;
    mapaNode1['a'] = 2;

    NodeAutomato node1 = NodeAutomato(
      mapaNode1  
    );

    nodes.push_back(node1);

    std::unordered_map<char, int> mapaNode2;
    mapaNode2['l'] = 3;

    NodeAutomato node2 = NodeAutomato(
      mapaNode2
    );

    nodes.push_back(node2);

    std::unordered_map<char, int> mapaNode3;
    mapaNode3['s'] = 4;

    NodeAutomato node3 = NodeAutomato(
      mapaNode3  
    );

    nodes.push_back(node3);

    std::unordered_map<char, int> mapaNode4;
    mapaNode4['e'] = 5;

    NodeAutomato node4 = NodeAutomato(
      mapaNode4  
    );

    nodes.push_back(node4);

    std::unordered_map<char, int> mapaNode5; // só pra marcar o ultimo estado, n tem transição

    NodeAutomato node5 = NodeAutomato(
      mapaNode5  
    );

    nodes.push_back(node5);

    // Nodes para 'true'
    std::unordered_map<char, int> mapaNode6;
    mapaNode6['r'] = 7;

    NodeAutomato node6 = NodeAutomato(
      mapaNode6  
    );

    nodes.push_back(node6);

    std::unordered_map<char, int> mapaNode7;
    mapaNode7['u'] = 8;

    NodeAutomato node7 = NodeAutomato(
      mapaNode7  
    );

    nodes.push_back(node7);

    std::unordered_map<char, int> mapaNode8;
    mapaNode8['e'] = 9;

    NodeAutomato node8 = NodeAutomato(
      mapaNode8  
    );

    nodes.push_back(node8);

    std::unordered_map<char, int> mapaNode9; // só pra marcar o ultimo estado, n tem transição

    NodeAutomato node9 = NodeAutomato(
      mapaNode9  
    );

    nodes.push_back(node9);

    // Finais -> 5 e 9
    std::set<int> estadosFinais;
    estadosFinais.insert(5);
    estadosFinais.insert(9);

    Automato automatoBooleano = Automato(
        nodes,
        estadosFinais
    );

    return automatoBooleano;
}