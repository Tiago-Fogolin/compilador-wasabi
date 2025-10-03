#pragma once 
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <set>
#include <memory>

enum class TokenType {
    WHITESPACE,
    COMMENT,
    IDENTIFIER,
    KEYWORD,
    INTEGER,
    FLOAT,
    SCIENTIFIC,
    CHAR_LITERAL,
    STRING_LITERAL,
    ASSIGN,
    OPERATION,
    LOGICAL,
    BITWISE,
    BITWISE_SHIFT,
    RELATIONAL,
    DELIMITER
};

#include <iostream>
#include <string>

inline std::string tokenTypeToString(TokenType tipo) {
    switch(tipo) {
        case TokenType::WHITESPACE:      return "WHITESPACE";
        case TokenType::COMMENT:         return "COMMENT";
        case TokenType::IDENTIFIER:      return "IDENTIFIER";
        case TokenType::KEYWORD:         return "KEYWORD";
        case TokenType::INTEGER:         return "INTEGER";
        case TokenType::FLOAT:           return "FLOAT";
        case TokenType::SCIENTIFIC:      return "SCIENTIFIC";
        case TokenType::CHAR_LITERAL:    return "CHAR_LITERAL";
        case TokenType::STRING_LITERAL:  return "STRING_LITERAL";
        case TokenType::ASSIGN:          return "ASSIGN";
        case TokenType::OPERATION:       return "OPERATION";
        case TokenType::LOGICAL:         return "LOGICAL";
        case TokenType::BITWISE:         return "BITWISE";
        case TokenType::BITWISE_SHIFT:   return "BITWISE_SHIFT";
        case TokenType::RELATIONAL:      return "RELATIONAL";
        case TokenType::DELIMITER:       return "DELIMITER";
        default:                         return "UNKNOWN";
    }
}


enum class ResultadoProcessamento {
    CONTINUA,
    ACEITO,
    INVALIDO
};

class NodeAutomato {
public:
    std::unordered_map<char, int> mapaDestino;

    NodeAutomato(std::unordered_map<char, int> mapaDestino);

    bool verificaTransicao(char c);
};

class Automato {
private:
    int indiceEstado = 0; // estado atual, começamos sempre em 0

    bool verificaTransicao(char caracter);

    int recuperarProximoEstado(char caracter);
public:
    std::vector<NodeAutomato> nodes;
    std::set<int> estadosFinais; // estados finais válidos

    Automato(std::vector<NodeAutomato> nodes, std::set<int> estadosFinais);

    bool verificaEstadoFinal();

    ResultadoProcessamento processarCaracter(char c);

    int estadoAtual();

    virtual bool processarString(const std::string& s);

    virtual int processarStringComEstadoFinal(const std::string& s, bool& aceito);

    virtual std::unique_ptr<Automato> build() = 0;

    virtual std::unique_ptr<Automato> clone() const = 0;
};

class AutomatoFactory {
public:
    static std::vector<std::pair<std::string, std::unique_ptr<Automato>>> getAutomatos();
};
