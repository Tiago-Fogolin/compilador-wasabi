#pragma once 
#include <vector>
#include <automato_base/Automato.hpp>


struct SequenceResult {
    int first;
    int last;
};

int addNode(std::vector<NodeAutomato>& nodes, const std::unordered_map<char,int>& mapa);

void mapRange(std::unordered_map<char,int>& map, char start, char end, int dest);

void addValidCharLiterals(std::unordered_map<char,int>& mapa, int dest);

void addValidStringChars(std::unordered_map<char,int>& mapa, int dest);

void addValidEscapes(std::unordered_map<char,int>& mapa, int dest);

SequenceResult addSequence(std::vector<NodeAutomato>& nodes, const std::string& s);

