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
    TokenType::COMMENT,          // comentários sempre primeiro
    TokenType::STRING_LITERAL,   // strings antes de char e identificador
    TokenType::CHAR_LITERAL,
    TokenType::KEYWORD,          // palavras-chave antes de identificador
    TokenType::IDENTIFIER,
    TokenType::SCIENTIFIC,       // números com expoente
    TokenType::FLOAT,             // float
    TokenType::INTEGER,           // inteiro
    TokenType::DOUBLE_STAR,       // operadores multi-caracter
    TokenType::DOUBLE_SLASH,
    TokenType::ASSIGN_POW,
    TokenType::ASSIGN_FDIV,
    TokenType::ASSIGN_SHR,
    TokenType::ASSIGN_SHL,
    TokenType::ASSIGN_ADD,
    TokenType::ASSIGN_SUB,
    TokenType::ASSIGN_MUL,
    TokenType::ASSIGN_DIV,
    TokenType::ASSIGN_AND,
    TokenType::ASSIGN_OR,
    TokenType::ASSIGN_XOR,
    TokenType::ASSIGN_NOT,
    TokenType::ASSIGN,
    TokenType::EQ,
    TokenType::NEQ,
    TokenType::GTE,
    TokenType::LTE,
    TokenType::GT,
    TokenType::LT,
    TokenType::PLUS,
    TokenType::MINUS,
    TokenType::STAR,
    TokenType::SLASH,
    TokenType::AMP,
    TokenType::PIPE,
    TokenType::CARET,
    TokenType::TILDE,
    TokenType::RSHIFT,
    TokenType::LSHIFT,
    TokenType::AND,
    TokenType::OR,
    TokenType::NOT,
    TokenType::PAREN_OPEN,
    TokenType::PAREN_CLOSE,
    TokenType::BRACE_OPEN,
    TokenType::BRACE_CLOSE,
    TokenType::BRACKET_OPEN,
    TokenType::BRACKET_CLOSE,
    TokenType::COMMA,
    TokenType::SEMICOLON,
    TokenType::COLON,
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
                tipoFinal = stringToTokenType(token);
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

    Token eof;

    eof.tipo = TokenType::END_OF_FILE;
    eof.valor = "";
    eof.linha = linha;
    tokensAceitos.push_back(eof);

    return tokensAceitos;
}
