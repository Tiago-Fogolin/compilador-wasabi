#include "parser/Parser.hpp"
#include "automato_base/Automato.hpp"
#include "parser/ast/AstBase.hpp"
#include <iostream>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <optional>
#include <string>
#include <vector>
#include <memory>



// ----------------------------------------------------------------------------------
// CONSTRUTOR / AUXILIARES DE NAVEGAÇÃO
// ----------------------------------------------------------------------------------

Parser::Parser(std::vector<Token> tokens) {
    this->tokens = std::move(tokens);
    this->posicao_atual = 0;
}

Token Parser::peek() {
    if (fimDaEntrada())
        return Token{TokenType::END_OF_FILE, "", tokens[posicao_atual].linha, tokens[posicao_atual].coluna};
    return tokens[posicao_atual];
}

Token Parser::advance() {
    if (!fimDaEntrada())
        return tokens[posicao_atual++];
    size_t ultima_linha = tokens.empty() ? 1 : tokens.back().linha;
    return Token{TokenType::END_OF_FILE, "", ultima_linha, 0};
}

bool Parser::fimDaEntrada() {
    return posicao_atual >= tokens.size() || tokens[posicao_atual].tipo == TokenType::END_OF_FILE;
}

bool Parser::verificar(TokenType tipoEsperado) {
    if (fimDaEntrada()) return false;
    return peek().tipo == tipoEsperado;
}

Token Parser::consumir(TokenType tipoEsperado) {
    if (!verificar(tipoEsperado)) {
        Token atual = peek();
        std::string mensagem = "Erro de Sintaxe: Esperado " + tokenTypeToString(tipoEsperado)
                                + " mas encontrado " + tokenTypeToString(atual.tipo)
                                + " em linha " + std::to_string(atual.linha)
                                + " (Valor: " + atual.valor + ")";
        throw std::runtime_error(mensagem);
    }
    return advance();
}
std::string Parser::tokenTypeToDataType(TokenType tipo) {
    switch (tipo) {
        // Literais Numéricos
        case TokenType::INTEGER:
            return "int";
        case TokenType::FLOAT:
        case TokenType::SCIENTIFIC:
            return "float";
        case TokenType::STRING_LITERAL:
            return "string";
        case TokenType::CHAR_LITERAL:
            return "char";
        case TokenType::KW_TRUE:
        case TokenType::KW_FALSE:
        case TokenType::KW_BOOL:
            return "bool";
        case TokenType::KW_NULL:
            return "null";

        default:
            return "unknown";
    }
}
// ----------------------------------------------------------------------------------
// PROGRAMA
// ----------------------------------------------------------------------------------

std::shared_ptr<NodoPrograma> Parser::analisarPrograma() {
    std::vector<std::shared_ptr<NodoDeclaracao>> declaracoes;
    std::vector<std::shared_ptr<NodoComando>> comandos;

    while (!fimDaEntrada() && !verificar(TokenType::END_OF_FILE)) {
        auto decl = analisarBlocoDeclaracoes();
        auto cmd = analisarComando(false);
        if (!decl.empty()) {
            for(auto d: decl) {
                declaracoes.push_back(d);
            }
        }
        if (cmd) comandos.push_back(cmd);
    }

    return std::make_shared<NodoPrograma>(declaracoes, comandos);
}


// ----------------------------------------------------------------------------------
// BLOCOS
// ----------------------------------------------------------------------------------

std::vector<std::shared_ptr<NodoAST>> Parser::analisarBlocoComandos(bool dentro_bloco) {
    std::vector<std::shared_ptr<NodoAST>> comandosAST;

    while (!fimDaEntrada() &&
           !verificar(TokenType::BRACE_CLOSE) &&
           !verificar(TokenType::KW_ELSE) &&
           !verificar(TokenType::END_OF_FILE)) {

        auto cmd = analisarComando(dentro_bloco);
        if (cmd) comandosAST.push_back(cmd);
    }

    return comandosAST;
}


// ----------------------------------------------------------------------------------
// COMANDOS
// ----------------------------------------------------------------------------------

bool Parser::ehOperadorAtribuicao(TokenType tipo) {
    return tipo == TokenType::ASSIGN ||
           tipo == TokenType::ASSIGN_ADD ||
           tipo == TokenType::ASSIGN_SUB ||
           tipo == TokenType::ASSIGN_MUL ||
           tipo == TokenType::ASSIGN_DIV ||
           tipo == TokenType::ASSIGN_POW ||
           tipo == TokenType::ASSIGN_FDIV ||
           tipo == TokenType::ASSIGN_AND ||
           tipo == TokenType::ASSIGN_OR ||
           tipo == TokenType::ASSIGN_XOR ||
           tipo == TokenType::ASSIGN_NOT ||
           tipo == TokenType::ASSIGN_SHR ||
           tipo == TokenType::ASSIGN_SHL;
}

std::shared_ptr<NodoComando> Parser::analisarComando(bool dentro_bloco) {
    // if, for, while
    if (verificar(TokenType::KW_IF)) return analisarComandoCondicional();
    if (verificar(TokenType::KW_FOR) || verificar(TokenType::KW_WHILE) || verificar(TokenType::KW_FOREACH))
        return analisarComandoLaco();
    if (verificar(TokenType::KW_RETURN)) return analisarComandoRetorno();

    // Declaração de variável
    if (verificar(TokenType::IDENTIFIER) &&
        posicao_atual + 1 < tokens.size() &&
        tokens[posicao_atual + 1].tipo == TokenType::COLON) {

        auto decl = analisarDeclaracaoVariavel();
        std::shared_ptr<ComandoDeclaracao> cmd_decl = std::make_shared<ComandoDeclaracao>(decl);
        cmd_decl->posicao.linha = decl->posicao.linha;
        return cmd_decl;
    }

    if (verificar(TokenType::IDENTIFIER)) {
        Token id = consumir(TokenType::IDENTIFIER);

        if (verificar(TokenType::PAREN_OPEN)) {
            consumir(TokenType::PAREN_OPEN);
            std::vector<std::shared_ptr<NodoExpressao>> args;
            if (!verificar(TokenType::PAREN_CLOSE)) {
                args.push_back(analisarExpressao());
                while (verificar(TokenType::COMMA)) {
                    consumir(TokenType::COMMA);
                    args.push_back(analisarExpressao());
                }
            }
            consumir(TokenType::PAREN_CLOSE);

            std::shared_ptr<ComandoIdent> cmd = std::make_shared<ComandoIdent>(id.valor, args);
            cmd->posicao.linha = id.linha;

            return cmd;
        }
        else if (posicao_atual < tokens.size() && ehOperadorAtribuicao(peek().tipo)) {
            return analisarComandoAtribuicao(id);
        }
    }
    std::cerr << "[Linha: " << peek().linha << "] "<< "Token inesperado: "  << peek().valor << std::endl;
    advance();
    return nullptr;
}


// --- Implementação dos Comandos ---

std::shared_ptr<NodoComando> Parser::analisarComandoCondicional() {
    Token inicio = consumir(TokenType::KW_IF);
    consumir(TokenType::PAREN_OPEN);
    auto condicao = analisarExpressao();
    consumir(TokenType::PAREN_CLOSE);

    consumir(TokenType::BRACE_OPEN);
    // Passa dentro_bloco=true para declarar ComandoDeclaracao dentro do if
    std::vector<std::shared_ptr<NodoAST>> blocoIf = analisarBlocoComandos(true);
    consumir(TokenType::BRACE_CLOSE);

    std::vector<std::shared_ptr<NodoAST>> blocoElse;
    if (verificar(TokenType::KW_ELSE)) {
        blocoElse = analisarComandoElse();
    }

    std::shared_ptr<class ComandoCondicional> cmd_if = std::make_shared<ComandoCondicional>(condicao, blocoIf, blocoElse);
    cmd_if->posicao.linha = inicio.linha;

    return cmd_if;
}


std::shared_ptr<NodoComando> Parser::analisarComandoLaco() {
    TokenType tipoKw = peek().tipo;
    Token start = advance();

    // 3. LAÇO FOREACH
    if (tipoKw == TokenType::KW_FOREACH) {
        consumir(TokenType::PAREN_OPEN);

        Token var_nome = consumir(TokenType::IDENTIFIER);
        consumir(TokenType::KW_IN);
        auto expr = analisarExpressao();

        consumir(TokenType::PAREN_CLOSE);

        consumir(TokenType::BRACE_OPEN);
        std::vector<std::shared_ptr<NodoAST>> corpoAST = analisarBlocoComandos(true);
        consumir(TokenType::BRACE_CLOSE);

        std::shared_ptr<ComandoLaco> cmd_laco =  std::make_shared<ComandoLaco>(ComandoLaco::TipoLaco::FOREACH, var_nome.valor, expr, corpoAST);
        cmd_laco->posicao.linha = start.linha;

        return cmd_laco;
    }

    // 1. LAÇO WHILE
    if (tipoKw == TokenType::KW_WHILE) {
        consumir(TokenType::PAREN_OPEN);
        auto condicao = analisarExpressao();
        consumir(TokenType::PAREN_CLOSE);

        consumir(TokenType::BRACE_OPEN);
        std::vector<std::shared_ptr<NodoAST>> corpoAST = analisarBlocoComandos(true);
        consumir(TokenType::BRACE_CLOSE);

        std::shared_ptr<ComandoLaco> cmd_laco = std::make_shared<ComandoLaco>(
            ComandoLaco::TipoLaco::WHILE,
            nullptr,
            condicao,
            nullptr,
            corpoAST
        );

        cmd_laco->posicao.linha = start.linha;

        return cmd_laco;
    }


    if (tipoKw == TokenType::KW_FOR) {


        consumir(TokenType::PAREN_OPEN);

        auto inicializacao = analisarComando();

        consumir(TokenType::COMMA);

        auto condicao = analisarExpressao();

        consumir(TokenType::COMMA);

        auto atualizacao = analisarComando();

        consumir(TokenType::PAREN_CLOSE);

        consumir(TokenType::BRACE_OPEN);
        std::vector<std::shared_ptr<NodoAST>> corpoAST = analisarBlocoComandos(true);
        consumir(TokenType::BRACE_CLOSE);

        std::shared_ptr<ComandoLaco> cmd_laco = std::make_shared<ComandoLaco>(
            ComandoLaco::TipoLaco::FOR,
            inicializacao,
            condicao,
            atualizacao,
            corpoAST
        );

        cmd_laco->posicao.linha = start.linha;

        return cmd_laco;
    }

    return nullptr;
}

std::shared_ptr<NodoComando> Parser::analisarComandoRetorno() {
    Token start = consumir(TokenType::KW_RETURN);
    auto expr = analisarExpressao();

    std::shared_ptr<ComandoRetorno> cmd_retorno = std::make_shared<ComandoRetorno>(expr);
    cmd_retorno->posicao.linha = start.linha;

    return cmd_retorno;
}


std::shared_ptr<ComandoAtribuicao> Parser::analisarComandoAtribuicao(Token l_value_token) {
    Token op = advance();
    auto expr = analisarExpressao();

    std::shared_ptr<ComandoAtribuicao> cmd_atr = std::make_shared<ComandoAtribuicao>(l_value_token.valor, op.valor, expr);
    cmd_atr->posicao.linha = op.linha;

    return cmd_atr;
}

std::vector<std::shared_ptr<NodoAST>> Parser::analisarComandoElse() {
    consumir(TokenType::KW_ELSE);

    if (verificar(TokenType::KW_IF)) {
        auto condicional = analisarComandoCondicional();
        std::vector<std::shared_ptr<NodoAST>> elseIfWrapper;
        elseIfWrapper.push_back(condicional);
        return elseIfWrapper;
    }

    consumir(TokenType::BRACE_OPEN);
    std::vector<std::shared_ptr<NodoAST>> blocoElse = analisarBlocoComandos(true);
    consumir(TokenType::BRACE_CLOSE);

    return blocoElse;
}


// ----------------------------------------------------------------------------------
// DECLARAÇÕES
// ----------------------------------------------------------------------------------

std::vector<std::shared_ptr<NodoDeclaracao>> Parser::analisarBlocoDeclaracoes() {
    std::vector<std::shared_ptr<NodoDeclaracao>> declaracoes;

    while (!fimDaEntrada()) {

        if (verificar(TokenType::KW_STRUCT) ||
            verificar(TokenType::KW_INTERFACE) ||
            verificar(TokenType::KW_DEF)) {

            auto decl = analisarDeclaracao();
            if (decl) declaracoes.push_back(decl);
            continue;
        }

        // Declaração de variável
        if (verificar(TokenType::IDENTIFIER) &&
            posicao_atual + 1 < tokens.size() &&
            tokens[posicao_atual + 1].tipo == TokenType::COLON) {

            auto declVar = analisarDeclaracaoVariavel();
            if (declVar) declaracoes.push_back(declVar);

            continue;
        }

        if (verificar(TokenType::IDENTIFIER) ||
            verificar(TokenType::KW_IF) ||
            verificar(TokenType::KW_WHILE) ||
            verificar(TokenType::KW_FOREACH) ||
            verificar(TokenType::KW_FOR)) {
            break;
        }

        break;
    }

    return declaracoes;
}

std::shared_ptr<DeclaracaoVariavel> Parser::analisarDeclaracaoVariavel() {
    Token nome = consumir(TokenType::IDENTIFIER);
    std::optional<std::string> tipo;
    std::shared_ptr<NodoExpressao> inicializador = nullptr;


    consumir(TokenType::COLON);

    Token tipoToken = consumir(peek().tipo);
    tipo = tipoToken.valor;
    if (verificar(TokenType::BRACKET_OPEN)) {
        consumir(TokenType::BRACKET_OPEN);
        consumir(TokenType::BRACKET_CLOSE);

        *tipo += "[]";
    }

    if (verificar(TokenType::ASSIGN)) {
        consumir(TokenType::ASSIGN);
        inicializador = analisarExpressao();
    }

    std::shared_ptr<DeclaracaoVariavel> decl = std::make_shared<DeclaracaoVariavel>(
        nome.valor,
        tipo.value(),
        inicializador
    );

    decl->posicao.linha = nome.linha;

    return decl;
}

std::shared_ptr<NodoDeclaracao> Parser::analisarDeclaracao() {
    if (verificar(TokenType::KW_INTERFACE)) {
        consumir(TokenType::KW_INTERFACE);
        Token nome = consumir(TokenType::IDENTIFIER);
        consumir(TokenType::BRACE_OPEN);
        std::vector<std::shared_ptr<DeclaracaoFuncao>> metodos;
        while (verificar(TokenType::KW_DEF)) {
            metodos.push_back(analisarDeclaracaoFuncao());
        }
        consumir(TokenType::BRACE_CLOSE);

        std::shared_ptr<DeclaracaoInterface> decl = std::make_shared<DeclaracaoInterface>(nome.valor, metodos);
        decl->posicao.linha = nome.linha;

        return decl;

    } else if (verificar(TokenType::KW_STRUCT)) {
        consumir(TokenType::KW_STRUCT);
        Token nome = consumir(TokenType::IDENTIFIER);

        std::vector<std::string> implementa;
        if (verificar(TokenType::KW_IMPLEMENTS)) {
            consumir(TokenType::KW_IMPLEMENTS);

            implementa.push_back(consumir(TokenType::IDENTIFIER).valor);

            while (verificar(TokenType::COMMA)) {
                consumir(TokenType::COMMA);
                implementa.push_back(consumir(TokenType::IDENTIFIER).valor);
            }
        }

        consumir(TokenType::BRACE_OPEN);
        std::vector<std::shared_ptr<NodoDeclaracao>> atributos;
        std::vector<std::shared_ptr<DeclaracaoFuncao>> metodos;

        while (!verificar(TokenType::BRACE_CLOSE) && !fimDaEntrada()) {
            if (verificar(TokenType::KW_DEF)) {
                metodos.push_back(analisarDeclaracaoFuncao());
            } else if (verificar(TokenType::IDENTIFIER)) {
                atributos.push_back(analisarDeclaracaoVariavel());
            } else {
                std::cerr << "[Linha: " << peek().linha << "] " << "Erro: conteúdo de struct inesperado." << std::endl;
                advance();
            }
        }
        consumir(TokenType::BRACE_CLOSE);

        std::shared_ptr<DeclaracaoStruct> decl = std::make_shared<DeclaracaoStruct>(nome.valor, implementa, atributos, metodos);
        decl->posicao.linha = nome.linha;

        return decl;

    } else if (verificar(TokenType::KW_DEF)) {
        return analisarDeclaracaoFuncao();
    }

    Token tokenInesperado = peek();
    std::cerr << "[Linha: " << peek().linha << "] " << "Erro: tipo de declaração inesperado: " << tokenTypeToString(tokenInesperado.tipo) << std::endl;
    advance();
    return nullptr;
}

std::shared_ptr<DeclaracaoFuncao> Parser::analisarDeclaracaoFuncao() {
    consumir(TokenType::KW_DEF);
    Token nome = consumir(TokenType::IDENTIFIER);

    consumir(TokenType::PAREN_OPEN);

    std::vector<std::pair<std::string, std::string>> parametros;
    if (!verificar(TokenType::PAREN_CLOSE)) {
        while (true) {
            Token paramNome = consumir(TokenType::IDENTIFIER);

            std::string paramTipo = "";

            consumir(TokenType::COLON);
            Token tipoToken = consumir(peek().tipo);
            paramTipo = tipoToken.valor;

            parametros.emplace_back(paramNome.valor, paramTipo);

            if (verificar(TokenType::COMMA)) {
                consumir(TokenType::COMMA);
                continue;
            }
            break;
        }
    }

    consumir(TokenType::PAREN_CLOSE);

    std::string tipoRetorno = "";
    consumir(TokenType::COLON);
    Token tipoToken = consumir(peek().tipo);
    tipoRetorno = tipoToken.valor;

    consumir(TokenType::BRACE_OPEN);
    std::vector<std::shared_ptr<NodoAST>> corpo = analisarBlocoComandos(true);
    consumir(TokenType::BRACE_CLOSE);

    std::shared_ptr<DeclaracaoFuncao> decl = std::make_shared<DeclaracaoFuncao>(nome.valor, parametros, tipoRetorno, corpo);
    decl->posicao.linha = nome.linha;

    return decl;
}

// ----------------------------
// Expressões (Completo)
// ----------------------------

int Parser::obterPrecedencia(TokenType tipo) {
    switch (tipo) {
        case TokenType::GT: case TokenType::LT: case TokenType::GTE:
        case TokenType::LTE: case TokenType::EQ: case TokenType::NEQ: return 1;
        case TokenType::PLUS: case TokenType::MINUS: return 2;
        case TokenType::STAR: case TokenType::SLASH: return 3;
        default: return 0;
    }
}

std::shared_ptr<NodoExpressao> Parser::analisarExpressao() {
    return analisarExpressaoBinaria(1);
}

std::shared_ptr<NodoExpressao> Parser::analisarExpressaoBinaria(int prec_min) {
    auto esquerda = analisarFator();

    while (true) {
        int prec_atual = obterPrecedencia(peek().tipo);

        if (prec_atual < prec_min) {
            break;
        }

        Token op = advance();
        int prec_prox = prec_atual + 1;

        auto direita = analisarExpressaoBinaria(prec_prox);

        if (prec_atual == 1) {
            esquerda = std::make_shared<ExpressaoRelacional>(esquerda, op.valor, direita);
            esquerda->posicao.linha = op.linha;
        } else {
            esquerda = std::make_shared<ExpressaoBinaria>(esquerda, op.valor, direita);
            esquerda->posicao.linha = op.linha;
        }
    }

    return esquerda;
}


std::shared_ptr<NodoExpressao> Parser::analisarFator() {
    std::shared_ptr<NodoExpressao> expr;
    Token id_token;

    if (verificar(TokenType::INTEGER) || verificar(TokenType::FLOAT) ||
        verificar(TokenType::STRING_LITERAL) || verificar(TokenType::CHAR_LITERAL) ||
        verificar(TokenType::KW_TRUE) || verificar(TokenType::KW_FALSE)) {
        Token literal = advance();
        expr = std::make_shared<ExpressaoLiteral>(literal.valor, tokenTypeToDataType(literal.tipo));
        expr->posicao.linha = literal.linha;
    } else if (verificar(TokenType::KW_THIS)) {
        Token kw = advance();
        expr = std::make_shared<ExpressaoIdentificador>(kw.valor);
        expr->posicao.linha = kw.linha;
    } else if (verificar(TokenType::IDENTIFIER)) {
        id_token = advance();
        expr = std::make_shared<ExpressaoIdentificador>(id_token.valor);
        expr->posicao.linha = id_token.linha;
    } else if (verificar(TokenType::PAREN_OPEN)) {
        advance();
        expr = analisarExpressao();
        consumir(TokenType::PAREN_CLOSE);
    } else if (verificar(TokenType::BRACKET_OPEN)) {
        Token brck_open = consumir(TokenType::BRACKET_OPEN);
        std::vector<std::shared_ptr<NodoExpressao>> elementos;
        if (!verificar(TokenType::BRACKET_CLOSE)) {
            elementos.push_back(analisarExpressao());
            while (verificar(TokenType::COMMA)) {
                consumir(TokenType::COMMA);
                elementos.push_back(analisarExpressao());
            }
        }
        consumir(TokenType::BRACKET_CLOSE);
        expr = std::make_shared<ExpressaoArrayLiteral>(elementos);
        expr->posicao.linha = brck_open.linha;
    } else if (verificar(TokenType::MINUS)) { // Operador unário (negativo)
        Token op = advance();
        auto fator = analisarFator();
        expr = std::make_shared<ExpressaoUnaria>(op.valor, fator);
        expr->posicao.linha = op.linha;
    } else {
        std::cerr << "[Linha: " << peek().linha << "]" << "Erro de sintaxe: fator inesperado em " << (int)peek().tipo << std::endl;
        advance();
        return nullptr;
    }

    while (verificar(TokenType::DOT) || verificar(TokenType::PAREN_OPEN) || verificar(TokenType::BRACKET_OPEN)) {
        if (verificar(TokenType::DOT)) {
            consumir(TokenType::DOT);
            Token membro = consumir(TokenType::IDENTIFIER);
            expr = std::make_shared<ExpressaoAcessoMembro>(expr, membro.valor);
            expr->posicao.linha = membro.linha;

        } else if (verificar(TokenType::PAREN_OPEN)) {
            if (!std::dynamic_pointer_cast<ExpressaoIdentificador>(expr) &&
                !std::dynamic_pointer_cast<ExpressaoAcessoMembro>(expr)) {
                break;
            }

            // É uma chamada de função/método
            consumir(TokenType::PAREN_OPEN);
            std::vector<std::shared_ptr<NodoExpressao>> args;
            if (!verificar(TokenType::PAREN_CLOSE)) {
                args.push_back(analisarExpressao());
                while (verificar(TokenType::COMMA)) {
                    consumir(TokenType::COMMA);
                    args.push_back(analisarExpressao());
                }
            }
            consumir(TokenType::PAREN_CLOSE);

            std::string nome_chamada;
            if (auto id_expr = std::dynamic_pointer_cast<ExpressaoIdentificador>(expr)) {
                nome_chamada = id_expr->nome;
            } else if (auto membro_expr = std::dynamic_pointer_cast<ExpressaoAcessoMembro>(expr)) {
                nome_chamada = id_token.valor;
            } else {
                 nome_chamada = "Erro_Chamada_Expressao_Complexa";
            }

            expr = std::make_shared<ExpressaoChamada>(nome_chamada, args);
            expr->posicao.linha = id_token.linha;

        } else if(verificar(TokenType::BRACKET_OPEN)) {
            consumir(TokenType::BRACKET_OPEN);
            std::shared_ptr<NodoExpressao> indice = analisarExpressao();
            consumir(TokenType::BRACKET_CLOSE);
            expr = std::make_shared<ExpressaoAcessoArray>(expr, indice);
            expr->posicao.linha = indice->posicao.linha;
        } else {
            break;
        }
    }


    return expr;
}
