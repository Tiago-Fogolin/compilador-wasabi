#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <string>
#include <TokensAutomatos.hpp> // seu header de NodeAutomato e Automato

struct SequenceResult {
    int first;
    int last;
};

int addNode(std::vector<NodeAutomato>& nodes, const std::unordered_map<char,int>& mapa) {
    nodes.push_back(NodeAutomato(mapa));
    return nodes.size() - 1;
}

void mapRange(std::unordered_map<char,int>& map, char start, char end, int dest) {
    for(char c = start; c <= end; c++) map[c] = dest;
}

SequenceResult addSequence(std::vector<NodeAutomato>& nodes, const std::string& s) {
	int first = nodes.size();
    int current = first;
    for (size_t i = 1; i < s.size(); ++i) {
        std::unordered_map<char,int> mapa;
        mapa[s[i]] = current + 1;
        nodes.push_back(NodeAutomato(mapa));
        current++;
    }

	addNode(nodes, {});

    return {first, current};
}

Automato criarAutomatoInteiros() {
    std::vector<NodeAutomato> nodes;

    std::unordered_map<char,int> rootMap;
    rootMap['0'] = 1;          // zero vai para node 1
    mapRange(rootMap, '1', '9', 2); // 1-9 vai para node 2
    addNode(nodes, rootMap);

    std::unordered_map<char,int> node1Map;
    node1Map['0'] = 1; // zero continua no node1
    int idxNode1 = addNode(nodes, node1Map);

    std::unordered_map<char,int> node2Map;
    mapRange(node2Map, '0', '9', 2); // 0-9 continua no node2
    int idxNode2 = addNode(nodes, node2Map);

    std::set<int> finais = {idxNode1, idxNode2};
    return Automato(nodes, finais);
}

Automato criarAutomatoBooleanos() {
    std::vector<NodeAutomato> nodes;
    int idxRoot = addNode(nodes, {}); // root vazio

    SequenceResult falseSeq = addSequence(nodes, "false");
    SequenceResult trueSeq  = addSequence(nodes, "true");

    nodes[idxRoot].mapaDestino['f'] = falseSeq.first;
    nodes[idxRoot].mapaDestino['t'] = trueSeq.first;

    std::set<int> finais = {falseSeq.last, trueSeq.last};
    return Automato(nodes, finais);
}

Automato criarAutomatoIdentificadores() {
    std::vector<NodeAutomato> nodes;

    std::unordered_map<char,int> rootMap;
    mapRange(rootMap, 'a', 'z', 1);
    mapRange(rootMap, 'A', 'Z', 1);
    rootMap['_'] = 1;
    addNode(nodes, rootMap);

    std::unordered_map<char,int> node1Map = rootMap;
    mapRange(node1Map, '0', '9', 1);
    int idxNode1 = addNode(nodes, node1Map);

    std::set<int> finais = {idxNode1};
    return Automato(nodes, finais);
}

Automato criarAutomatoFlutuantes() {
    std::vector<NodeAutomato> nodes;

    // Root
    std::unordered_map<char,int> rootMap;
    mapRange(rootMap, '1', '9', 1);
    rootMap['0'] = 3; // caso de 0.x
    addNode(nodes, rootMap);

    // Node 1: dígitos 1-9
    std::unordered_map<char,int> node1Map;
    mapRange(node1Map, '0', '9', 1); // continua no node1
    node1Map['.'] = 2; // ponto decimal
    addNode(nodes, node1Map);

    // Node 2: primeiro dígito depois do ponto
    std::unordered_map<char,int> node2Map;
    mapRange(node2Map, '0', '9', 4); // vai para node4
    int idxNode2 = addNode(nodes, node2Map);

    // Node 3: zero seguido de ponto
    std::unordered_map<char,int> node3Map;
    node3Map['.'] = 2;
    addNode(nodes, node3Map);

    // Node 4: dígitos depois do ponto
    std::unordered_map<char,int> node4Map;
    mapRange(node4Map, '0', '9', 4);
    int idxNode4 = addNode(nodes, node4Map);

    std::set<int> finais = {idxNode2, idxNode4};
    return Automato(nodes, finais);
}
