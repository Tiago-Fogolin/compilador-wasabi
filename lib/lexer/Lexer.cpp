#include "../include/lexer/Lexer.hpp"
#include "../include/automato_base/Automato.hpp"
#include "../include/automatos/AutomatoLexer.hpp"
#include "../include/lexer/Token.hpp"
#include <iostream>
#include <algorithm>
#include <vector>

Lexer::Lexer() {
    this->palavrasReservadas = {
        "interface","struct","implements","def","this","for","foreach","in",
        "if","elif","else","while","break","continue","return",
        "int","float","string","bool","tuple","dict","set","void","null"
    };
}

std::vector<TokenType> ordemPrioridade = {
    TokenType::COMMENT,
    TokenType::STRING_LITERAL,
    TokenType::CHAR_LITERAL,
    TokenType::KEYWORD,
    TokenType::IDENTIFIER,
    TokenType::SCIENTIFIC,
    TokenType::FLOAT,
    TokenType::INTEGER,
    TokenType::BITWISE_SHIFT,
    TokenType::LOGICAL,
    TokenType::RELATIONAL,
    TokenType::ASSIGN,
    TokenType::OPERATION,
    TokenType::BITWISE,
    TokenType::DELIMITER,
    TokenType::WHITESPACE
};

std::vector<Token> Lexer::analisarTexto(std::string& bloco) {
    std::string texto = bloco;
    std::vector<Token> tokensAceitos;
    AutomatoLexer automato;
    

    size_t i = 0;
    size_t linha = 1;
    size_t ultimaPosProcessada = 0;

    while (i < texto.size()) {
        int estadoAtual = 0;
        int ultimoEstadoFinal = -1;
        std::vector<std::pair<int,TokenType>> estadosFinais;
        TokenType tipoUltimoEstadoFinal;
        bool tokenPodeContinuar = false;

        size_t j = i;
        for (; j < texto.size(); j++) {
            char c = texto[j];

            auto it = automato.nodes[estadoAtual].mapaDestino.find(c);
            if (it == automato.nodes[estadoAtual].mapaDestino.end()) {
                break;
            }

            estadoAtual = it->second;

            if (automato.estadosFinais.count(estadoAtual)) {
                ultimoEstadoFinal = estadoAtual;
                
                std::pair<int,TokenType> posicaoTipo;

                posicaoTipo.first = j;
                posicaoTipo.second = automato.estadoFinaisTipos.at(estadoAtual);

                estadosFinais.push_back(posicaoTipo);
            }
        }

        if (ultimoEstadoFinal == -1) {
            std::cerr << "Erro léxico na linha " << linha
                      << ": caractere inesperado '" << texto[i] << "'\n";
            i++;
            continue;
        }

        int ordemEstadoFinal = ordemPrioridade.size();
        int indiceEstadoFinal = 0;
        size_t posFinalMax = 0;
        for(int x = 0; x < estadosFinais.size(); x++) {
            auto estadoFinal = estadosFinais[x];

            size_t posFinal = estadoFinal.first;
            TokenType tipo = estadoFinal.second;
            auto it = std::find(ordemPrioridade.begin(), ordemPrioridade.end(), tipo);

            int ordemEstado;
            if (it != ordemPrioridade.end()) {
                ordemEstado = it - ordemPrioridade.begin();
            }

            if (posFinal > posFinalMax || (posFinal == posFinalMax && ordemEstado < ordemEstadoFinal)) {
                posFinalMax = posFinal;
                ordemEstadoFinal = ordemEstado;
                indiceEstadoFinal = x;
            }
        }

        int ultimoPosFinal = estadosFinais[indiceEstadoFinal].first;
        TokenType tipoFinal = estadosFinais[indiceEstadoFinal].second;

        std::string token = texto.substr(i, ultimoPosFinal - i + 1);

        if (tipoFinal == TokenType::IDENTIFIER) {
            if (palavrasReservadas.count(token)) {
                tipoFinal = TokenType::KEYWORD;
            }
        }
        
        if (tipoFinal != TokenType::WHITESPACE && tipoFinal != TokenType::COMMENT) {
            Token tokenStruct;
            tokenStruct.tipo = tipoFinal;
            tokenStruct.valor = token;
            tokenStruct.linha = linha;
            tokenStruct.coluna = i;
            tokensAceitos.push_back(tokenStruct);
            ultimaPosProcessada = i + 1;
        }

        for (char c : token) {
            if (c == '\n') {
                linha++;
            }
        }

        i = ultimoPosFinal + 1; 
    }


    return tokensAceitos;
}

