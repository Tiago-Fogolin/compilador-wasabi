#include "../include/lexer/Lexer.hpp"
#include "../include/automato_base/Automato.hpp"
#include <iostream>

Lexer::Lexer() {
    this->palavrasReservadas = {
        "interface","struct","implements","def","this","for","foreach","in",
        "if","elif","else","while","break","continue","return",
        "int","float","string","bool","tuple","dict","set","void","null"
    };
}

bool ehSeparador(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == ';' || c == '(' || c == ')' || c == ',' || c == ':' || c == '.' || c == '[' || c == ']';
}

std::string extrairProximoToken(const std::string& texto, size_t& indice, size_t& linha) {
    if (ehSeparador(texto[indice])) {
        if (texto[indice] == '\n') linha++;
        size_t start = indice;
        indice++;
        return texto.substr(start, 1);
    }

    while (indice < texto.size() && ehSeparador(texto[indice])) {
        if (texto[indice] == '\n') linha++;
        indice++;
    }

    if (indice >= texto.size()) return "";

    size_t start = indice;

   
    if (texto[indice] == '/' && indice + 1 < texto.size() && texto[indice + 1] == '/') {
        indice += 2;
        while (indice < texto.size() && texto[indice] != '\n') indice++;
        return texto.substr(start, indice - start);
    }

    
    if (texto[indice] == '/' && indice + 1 < texto.size() && texto[indice + 1] == '*') {
        indice += 2;
        while (indice < texto.size() && !(texto[indice-1] == '*' && texto[indice] == '/')) {
            if (texto[indice] == '\n') {linha++;} 
            indice++;
        }
        if (indice < texto.size()) indice++;
        return texto.substr(start, indice - start);
    }

    while (indice < texto.size() && !ehSeparador(texto[indice])) indice++;

    return texto.substr(start, indice - start);
}



std::unordered_map<std::string, std::vector<std::string>> Lexer::analisarTexto(std::string texto) {
    std::unordered_map<std::string, std::vector<std::string>> tokensAceitos;
    std::vector<std::pair<std::string, std::unique_ptr<Automato>>> automatos = AutomatoFactory::getAutomatos();

    size_t i = 0;
    size_t linha_atual = 1;
    while (i < texto.size()) {

        std::string token = extrairProximoToken(texto, i, linha_atual);
        if (token.empty()) continue;

        bool reconheceu = false;
        for (auto& [tipo, automato] : automatos) {
            if (automato->processarString(token)) {
                tokensAceitos[tipo].push_back(token);
                reconheceu = true;
                break;
            }
        }

        if (!reconheceu) {
            std::cerr << "Erro de sintaxe na linha " << linha_atual <<  ":'" << token << "'\n";
            return tokensAceitos;
        }
    }

    return tokensAceitos;
}

