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
    KW_STRUCT,
    KW_INTERFACE,
    KW_IMPLEMENTS,
    KW_DEF,
    KW_THIS,
    KW_FOR,
    KW_FOREACH,
    KW_IN,
    KW_IF,
    KW_ELIF,
    KW_ELSE,
    KW_WHILE,
    KW_BREAK,
    KW_CONTINUE,
    KW_RETURN,
    KW_INT,
    KW_FLOAT,
    KW_STRING,
    KW_BOOL,
    KW_TUPLE,
    KW_DICT,
    KW_SET,
    KW_VOID,
    KW_NULL,
    ASSIGN,         // =
    ASSIGN_ADD,     // +=
    ASSIGN_SUB,     // -=
    ASSIGN_MUL,     // *=
    ASSIGN_DIV,     // /=
    ASSIGN_POW,     // **=
    ASSIGN_FDIV,    // //=
    ASSIGN_AND,     // &=
    ASSIGN_OR,      // |=
    ASSIGN_XOR,     // ^=
    ASSIGN_NOT,     // ~= 
    ASSIGN_SHR,     // >>=
    ASSIGN_SHL,     // <<=
    ASSIGN_MOD,     // %=
    PLUS,           // +
    MINUS,          // -
    STAR,           // *
    SLASH,          // /
    DOUBLE_STAR,    // **
    DOUBLE_SLASH,   // //
    MOD,            // %
    AMP,            // &
    PIPE,           // |
    CARET,          // ^
    TILDE,          // ~
    RSHIFT,         // >>
    LSHIFT,         // <<
    EQ,             // ==
    NEQ,            // !=
    GT,             // >
    LT,             // <
    GTE,            // >=
    LTE,            // <=
    AND,            // and
    OR,             // or
    NOT,            // not
    PAREN_OPEN,     // (
    PAREN_CLOSE,    // )
    BRACE_OPEN,     // {
    BRACE_CLOSE,    // }
    BRACKET_OPEN,   // [
    BRACKET_CLOSE,  // ]
    COMMA,          // ,
    SEMICOLON,      // ;
    COLON,          // :
    DOT,            // .
    END_OF_FILE,
    UNKNOWN,
};

#include <iostream>
#include <string>

inline std::string tokenTypeToString(TokenType tipo) {
    switch (tipo) {
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::COMMENT: return "COMMENT";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::KEYWORD: return "KEYWORD";
        case TokenType::INTEGER: return "INTEGER";
        case TokenType::FLOAT: return "FLOAT";
        case TokenType::SCIENTIFIC: return "SCIENTIFIC";
        case TokenType::CHAR_LITERAL: return "CHAR_LITERAL";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::KW_STRUCT: return "KW_STRUCT";
        case TokenType::KW_INTERFACE: return "KW_INTERFACE";
        case TokenType::KW_IMPLEMENTS: return "KW_IMPLEMENTS";
        case TokenType::KW_DEF: return "KW_DEF";
        case TokenType::KW_THIS: return "KW_THIS";
        case TokenType::KW_FOR: return "KW_FOR";
        case TokenType::KW_FOREACH: return "KW_FOREACH";
        case TokenType::KW_IN: return "KW_IN";
        case TokenType::KW_IF: return "KW_IF";
        case TokenType::KW_ELIF: return "KW_ELIF";
        case TokenType::KW_ELSE: return "KW_ELSE";
        case TokenType::KW_WHILE: return "KW_WHILE";
        case TokenType::KW_BREAK: return "KW_BREAK";
        case TokenType::KW_CONTINUE: return "KW_CONTINUE";
        case TokenType::KW_RETURN: return "KW_RETURN";
        case TokenType::KW_INT: return "KW_INT";
        case TokenType::KW_FLOAT: return "KW_FLOAT";
        case TokenType::KW_STRING: return "KW_STRING";
        case TokenType::KW_BOOL: return "KW_BOOL";
        case TokenType::KW_TUPLE: return "KW_TUPLE";
        case TokenType::KW_DICT: return "KW_DICT";
        case TokenType::KW_SET: return "KW_SET";
        case TokenType::KW_VOID: return "KW_VOID";
        case TokenType::KW_NULL: return "KW_NULL";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::ASSIGN_ADD: return "ASSIGN_ADD";
        case TokenType::ASSIGN_SUB: return "ASSIGN_SUB";
        case TokenType::ASSIGN_MUL: return "ASSIGN_MUL";
        case TokenType::ASSIGN_DIV: return "ASSIGN_DIV";
        case TokenType::ASSIGN_POW: return "ASSIGN_POW";
        case TokenType::ASSIGN_FDIV: return "ASSIGN_FDIV";
        case TokenType::ASSIGN_AND: return "ASSIGN_AND";
        case TokenType::ASSIGN_OR: return "ASSIGN_OR";
        case TokenType::ASSIGN_XOR: return "ASSIGN_XOR";
        case TokenType::ASSIGN_NOT: return "ASSIGN_NOT";
        case TokenType::ASSIGN_SHR: return "ASSIGN_SHR";
        case TokenType::ASSIGN_SHL: return "ASSIGN_SHL";
        case TokenType::ASSIGN_MOD: return "ASSIGN_MOD";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::DOUBLE_STAR: return "DOUBLE_STAR";
        case TokenType::DOUBLE_SLASH: return "DOUBLE_SLASH";
        case TokenType::MOD: return "MOD";
        case TokenType::AMP: return "AMP";
        case TokenType::PIPE: return "PIPE";
        case TokenType::CARET: return "CARET";
        case TokenType::TILDE: return "TILDE";
        case TokenType::RSHIFT: return "RSHIFT";
        case TokenType::LSHIFT: return "LSHIFT";
        case TokenType::EQ: return "EQ";
        case TokenType::NEQ: return "NEQ";
        case TokenType::GT: return "GT";
        case TokenType::LT: return "LT";
        case TokenType::GTE: return "GTE";
        case TokenType::LTE: return "LTE";
        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        case TokenType::NOT: return "NOT";
        case TokenType::PAREN_OPEN: return "PAREN_OPEN";
        case TokenType::PAREN_CLOSE: return "PAREN_CLOSE";
        case TokenType::BRACE_OPEN: return "BRACE_OPEN";
        case TokenType::BRACE_CLOSE: return "BRACE_CLOSE";
        case TokenType::BRACKET_OPEN: return "BRACKET_OPEN";
        case TokenType::BRACKET_CLOSE: return "BRACKET_CLOSE";
        case TokenType::COMMA: return "COMMA";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";
        case TokenType::DOT: return "DOT";

        case TokenType::END_OF_FILE: return "END_OF_FILE";
        default: return "UNKNOWN";
    }
}


inline TokenType stringToTokenType(const std::string& palavra) {
    if (palavra == "interface") return TokenType::KW_INTERFACE;
    if (palavra == "struct") return TokenType::KW_STRUCT;
    if (palavra == "implements") return TokenType::KW_IMPLEMENTS;
    if (palavra == "def") return TokenType::KW_DEF;
    if (palavra == "this") return TokenType::KW_THIS;
    if (palavra == "for") return TokenType::KW_FOR;
    if (palavra == "foreach") return TokenType::KW_FOREACH;
    if (palavra == "in") return TokenType::KW_IN;
    if (palavra == "if") return TokenType::KW_IF;
    if (palavra == "elif") return TokenType::KW_ELIF;
    if (palavra == "else") return TokenType::KW_ELSE;
    if (palavra == "while") return TokenType::KW_WHILE;
    if (palavra == "break") return TokenType::KW_BREAK;
    if (palavra == "continue") return TokenType::KW_CONTINUE;
    if (palavra == "return") return TokenType::KW_RETURN;
    if (palavra == "int") return TokenType::KW_INT;
    if (palavra == "float") return TokenType::KW_FLOAT;
    if (palavra == "string") return TokenType::KW_STRING;
    if (palavra == "bool") return TokenType::KW_BOOL;
    if (palavra == "tuple") return TokenType::KW_TUPLE;
    if (palavra == "dict") return TokenType::KW_DICT;
    if (palavra == "set") return TokenType::KW_SET;
    if (palavra == "void") return TokenType::KW_VOID;
    if (palavra == "null") return TokenType::KW_NULL;

    return TokenType::UNKNOWN; 
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
