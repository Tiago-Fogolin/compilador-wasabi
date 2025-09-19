#include <vector>
#include <automato_base/Automato.hpp>
#include <utils/lexer/LexerUtils.hpp>


int addNode(std::vector<NodeAutomato>& nodes, const std::unordered_map<char,int>& mapa) {
    nodes.push_back(NodeAutomato(mapa));
    return nodes.size() - 1;
}

void mapRange(std::unordered_map<char,int>& map, char start, char end, int dest) {
    for(char c = start; c <= end; c++) map[c] = dest;
}

void addValidCharLiterals(std::unordered_map<char,int>& mapa, int dest) {
    for(char c = 32; c <= 126; c++) {
        if (c == '\'' || c == '\\') continue; // exclui delimitadores especiais
        mapa[c] = dest;
    }
}

void addValidStringChars(std::unordered_map<char,int>& mapa, int dest) {
    for(char c = 32; c <= 126; c++) {
        if(c == '"' || c == '\\') continue;
        mapa[c] = dest;
    }
}

void addValidEscapes(std::unordered_map<char,int>& mapa, int dest) {
    std::string escapes = "abfnrtv'\"?\\0xuU"; 
    for (char c : escapes) {
        mapa[c] = dest;
    }
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

