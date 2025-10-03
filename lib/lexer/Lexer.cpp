#include "../include/lexer/Lexer.hpp"
#include "../include/automato_base/Automato.hpp"
#include "../include/automatos/AutomatoLexer.hpp"
#include <iostream>
#include <cctype> // Necessário para isalpha e isalnum

Lexer::Lexer() {
    this->palavrasReservadas = {
        "interface","struct","implements","def","this","for","foreach","in",
        "if","elif","else","while","break","continue","return",
        "int","float","string","bool","tuple","dict","set","void","null"
    };
}

std::unordered_map<std::string, std::vector<std::string>> Lexer::analisarTexto(const std::string texto) {
    std::unordered_map<std::string, std::vector<std::string>> tokensAceitos;
    AutomatoLexer automato;

    size_t i = 0;
    size_t linha = 1;

    while (i < texto.size()) {
        int estadoAtual = 0;
        int ultimoEstadoFinal = -1;
        size_t ultimoPosFinal = i - 1;
        TokenType tipoFinal;

        size_t j = i;
        for (; j < texto.size(); j++) {
            char c = texto[j];

            auto it = automato.nodes[estadoAtual].mapaDestino.find(c);
            if (it == automato.nodes[estadoAtual].mapaDestino.end()) {
                // não há transição: termina
                break;
            }

            estadoAtual = it->second;

            if (automato.estadosFinais.count(estadoAtual)) {
                ultimoEstadoFinal = estadoAtual;
                ultimoPosFinal = j;
                tipoFinal = automato.estadoFinaisTipos.at(estadoAtual);
            }
        }

        if (ultimoEstadoFinal == -1) {
            std::cerr << "Erro léxico na linha " << linha
                      << ": caractere inesperado '" << texto[i] << "'\n";
            i++;
            continue;
        }

        std::string token = texto.substr(i, ultimoPosFinal - i + 1);

        // <<< FIX: Pós-validação para números seguidos por letras ou pontos
        if (tipoFinal == TokenType::INTEGER || tipoFinal == TokenType::FLOAT || tipoFinal == TokenType::SCIENTIFIC) {
            size_t posAposToken = ultimoPosFinal + 1;
            if (posAposToken < texto.size()) {
                char charAposToken = texto[posAposToken];

                // Caso 1: Número seguido por letra (ex: 7calcular_area)
                if (std::isalpha(charAposToken) || charAposToken == '_') {
                    // Consome o resto da sequência inválida para reportar o erro completo
                    size_t fimSequenciaInvalida = posAposToken;
                    while (fimSequenciaInvalida < texto.size() && (std::isalnum(texto[fimSequenciaInvalida]) || texto[fimSequenciaInvalida] == '_')) {
                        fimSequenciaInvalida++;
                    }
                    std::string sequenciaInvalida = texto.substr(i, fimSequenciaInvalida - i);

                    std::cerr << "Erro lexico na linha " << linha
                              << ": identificador invalido '" << sequenciaInvalida << "\n";
                    
                    i = fimSequenciaInvalida;
                    continue; 
                }
                
                // Caso 2: Número que já tem um ponto é seguido por outro ponto (ex: 9...10)
                if (charAposToken == '.' && token.find('.') != std::string::npos) {
                    // Consome o resto da sequência inválida para reportar o erro completo
                    size_t fimSequenciaInvalida = posAposToken;
                    while (fimSequenciaInvalida < texto.size() && (texto[fimSequenciaInvalida] == '.' || std::isdigit(texto[fimSequenciaInvalida]))) {
                        fimSequenciaInvalida++;
                    }
                    std::string sequenciaInvalida = texto.substr(i, fimSequenciaInvalida - i);
                    
                    std::cerr << "Erro lexico na linha " << linha
                              << ": identificador invalido '" << sequenciaInvalida << "\n";

                    i = fimSequenciaInvalida;
                    continue;
                }
            }
        }

        if (tipoFinal == TokenType::IDENTIFIER) {
            if (palavrasReservadas.count(token)) {
                tipoFinal = TokenType::KEYWORD;
            }
        }
        
        if (tipoFinal != TokenType::WHITESPACE && tipoFinal != TokenType::COMMENT) {
            tokensAceitos[tokenTypeToString(tipoFinal)].push_back(token);
        }

        // Atualiza a contagem de linhas
        for (char c : token) {
            if (c == '\n') {
                linha++;
            }
        }

        i = ultimoPosFinal + 1; 
    }

    return tokensAceitos;
}

