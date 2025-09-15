#include <iostream>
#include <TokensAutomatos.hpp>

struct SequenceResult {
    int first;
    int last;
};

int addNode(std::vector<NodeAutomato>& nodes, const std::unordered_map<char,int>& mapa) {
    nodes.push_back(NodeAutomato(mapa));
    return nodes.size() - 1;
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

    // node 0
    // mapa node incial
    std::unordered_map<char, int> mapaRoot;

    mapaRoot['0'] = 1; // vai pro node 1 (continua no mesmo)

    for(char c = '1'; c <= '9'; c++) {
        mapaRoot[c] = 2; // 1-9 vai pro node 2
    }

    addNode(nodes, mapaRoot);

    // Node 1
    std::unordered_map<char, int> mapaNode1;
    mapaNode1['0'] = 1; // se for zero continua no mesmo
    
    int idxNode1 = addNode(nodes, mapaNode1);


    // Node 2
    std::unordered_map<char, int> mapaNode2;
    for(char c = '0'; c <= '9'; c++) {
        mapaNode2[c] = 2; // 0-9 vai pro node 2 (continua no mesmo)
    }

    int idxNode2 = addNode(nodes, mapaNode2);


    // tanto estado 1 quanto estado 2 são estados finais válidos
    std::set<int> estadosFinais = {idxNode1, idxNode2};

    return Automato(nodes,estadosFinais);
}

Automato criarAutomatoBooleanos() {
    std::vector<NodeAutomato> nodes;

	addNode(nodes, {}); // criei o root primeiro aqui, mas com mapa vazio

	SequenceResult falseSeq = addSequence(nodes, "false");
	SequenceResult trueSeq = addSequence(nodes, "true");

	nodes[0].mapaDestino['f'] = falseSeq.first; // mapeio o root pro começo de cada um
    nodes[0].mapaDestino['t'] = trueSeq.first;

	std::set<int> finais = {falseSeq.last, trueSeq.last};

	return Automato(nodes, finais);
}

Automato criarAutomatoIdentificadores() {
	std::vector<NodeAutomato> nodes;

	std::unordered_map<char, int> mapaRoot;
	for(char c = 'a'; c <= 'z'; c++) {
		mapaRoot[c] = 1;
	}

	for(char c = 'A'; c <= 'Z'; c++) {
		mapaRoot[c] = 1;
	}

	mapaRoot['_'] = 1;
	
	addNode(nodes, mapaRoot);

	std::unordered_map<char, int> mapaNode1 = mapaRoot;

	for(char c = '0'; c <= '9'; c++) {
		mapaNode1[c] = 1;
	}

	addNode(nodes, mapaNode1);

	std::set<int> estadosFinais = {1};

	return Automato(nodes, estadosFinais);
}