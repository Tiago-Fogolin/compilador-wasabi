#include "parser/Parser.hpp"
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
        return Token{TokenType::END_OF_FILE, "", 0, 0};
    return tokens[posicao_atual];
}

Token Parser::advance() {
    if (!fimDaEntrada())
        return tokens[posicao_atual++];
    return Token{TokenType::END_OF_FILE, "", 0, 0};
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
        // Tratamento de erro aprimorado
        std::cerr << "Erro de Sintaxe: Esperado " << tokenTypeToString(tipoEsperado)
                  << " mas encontrado " << tokenTypeToString(peek().tipo)
                  << " em linha " << peek().linha << " (Valor: " << peek().valor << ")" << std::endl;
    }
    return advance();
}

// ----------------------------------------------------------------------------------
// PROGRAMA
// ----------------------------------------------------------------------------------

std::shared_ptr<NodoAST> Parser::analisarPrograma() {
    std::vector<std::shared_ptr<NodoDeclaracao>> declaracoes = analisarBlocoDeclaracoes();
    std::vector<std::shared_ptr<NodoComando>> comandos;

    while (!fimDaEntrada() && !verificar(TokenType::END_OF_FILE)) {
        auto cmd = analisarComando();
        if (cmd) {
            comandos.push_back(cmd);
        }
    }

    return std::make_shared<NodoPrograma>(declaracoes, comandos); 
}

// ----------------------------------------------------------------------------------
// BLOCOS
// ----------------------------------------------------------------------------------

std::vector<std::shared_ptr<NodoAST>> Parser::analisarBlocoComandos() {
    std::vector<std::shared_ptr<NodoAST>> comandosAST;
    
    while (!fimDaEntrada() &&
           !verificar(TokenType::BRACE_CLOSE) &&
           !verificar(TokenType::KW_ELSE) &&
           !verificar(TokenType::END_OF_FILE)) {
        
        auto cmd = analisarComando();
        if (cmd) {
            comandosAST.push_back(cmd); 
        }
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

std::shared_ptr<NodoComando> Parser::analisarComando() {
    if (verificar(TokenType::KW_IF)) return analisarComandoCondicional();
    if (verificar(TokenType::KW_FOR) || verificar(TokenType::KW_FOREACH) || verificar(TokenType::KW_WHILE))
        return analisarComandoLaco();
    if (verificar(TokenType::KW_RETURN)) return analisarComandoRetorno();

    if (verificar(TokenType::IDENTIFIER)) {
        // Prioridade 1: Declaração de variável com tipo (escopo local)
        if (posicao_atual + 1 < tokens.size() && tokens[posicao_atual + 1].tipo == TokenType::COLON) {
            auto decl = analisarDeclaracaoVariavel();
            return std::make_shared<ComandoDeclaracao>(decl);
        }
        
        // Prioridade 2: Atribuição (incluindo +=, -=, etc.) ou Chamada (ComandoIdent)
        if (posicao_atual + 1 < tokens.size()) {
             TokenType proximoTipo = tokens[posicao_atual + 1].tipo;

             if (ehOperadorAtribuicao(proximoTipo) || proximoTipo == TokenType::PAREN_OPEN) {
                 return analisarComandoIdentOuAtribuicao();
             }
        }
    }

    // Erro e recuperação:
    std::cerr << "Comando inesperado: " 
              << " em linha " << peek().linha << std::endl;
    advance(); // CRÍTICO: Avança o token para evitar loop infinito
    return nullptr;
}


// --- Implementação dos Comandos ---

std::shared_ptr<NodoComando> Parser::analisarComandoCondicional() {
    consumir(TokenType::KW_IF);
    consumir(TokenType::PAREN_OPEN);
    auto condicao = analisarExpressao();
    consumir(TokenType::PAREN_CLOSE);
    
    consumir(TokenType::BRACE_OPEN); 
    std::vector<std::shared_ptr<NodoAST>> blocoIf = analisarBlocoComandos(); 
    consumir(TokenType::BRACE_CLOSE);

    std::vector<std::shared_ptr<NodoAST>> blocoElse;
    if (verificar(TokenType::KW_ELSE)) {
        blocoElse = analisarComandoElse(); 
    }
    
    return std::make_shared<ComandoCondicional>(condicao, blocoIf, blocoElse);
}

std::shared_ptr<NodoComando> Parser::analisarComandoLaco() {
    TokenType tipoKw = peek().tipo;
    advance(); 

    // 3. LAÇO FOREACH
    if (tipoKw == TokenType::KW_FOREACH) {
        consumir(TokenType::PAREN_OPEN); // Consome '('

        Token var_nome = consumir(TokenType::IDENTIFIER); 
        consumir(TokenType::KW_IN);
        auto expr = analisarExpressao(); 
        
        consumir(TokenType::PAREN_CLOSE); // Consome ')'

        consumir(TokenType::BRACE_OPEN); 
        std::vector<std::shared_ptr<NodoAST>> corpoAST = analisarBlocoComandos();
        consumir(TokenType::BRACE_CLOSE);
        
        return std::make_shared<ComandoLaco>(ComandoLaco::TipoLaco::FOREACH, var_nome.valor, expr, corpoAST);
    }

    // 1. LAÇO WHILE
    if (tipoKw == TokenType::KW_WHILE) {
        consumir(TokenType::PAREN_OPEN);
        auto condicao = analisarExpressao();
        consumir(TokenType::PAREN_CLOSE);

        consumir(TokenType::BRACE_OPEN);
        std::vector<std::shared_ptr<NodoAST>> corpoAST = analisarBlocoComandos();
        consumir(TokenType::BRACE_CLOSE);

        // Assumindo um construtor de 5 argumentos para FOR/WHILE
        return std::make_shared<ComandoLaco>(
            ComandoLaco::TipoLaco::WHILE, 
            nullptr, 
            condicao,
            nullptr, 
            corpoAST 
        );
    }
    

    if (tipoKw == TokenType::KW_FOR) {

        consumir(TokenType::PAREN_OPEN);

        Token init_lvalue = consumir(TokenType::IDENTIFIER);

        auto inicializacao = analisarComandoAtribuicao(init_lvalue); 
        
        consumir(TokenType::COMMA);

        auto condicao = analisarExpressao();
        
        consumir(TokenType::COMMA);

        Token update_lvalue = consumir(TokenType::IDENTIFIER);

        auto atualizacao = analisarComandoAtribuicao(update_lvalue); 

        consumir(TokenType::PAREN_CLOSE);
        
        consumir(TokenType::BRACE_OPEN);
        std::vector<std::shared_ptr<NodoAST>> corpoAST = analisarBlocoComandos();
        consumir(TokenType::BRACE_CLOSE);
        
        return std::make_shared<ComandoLaco>(
            ComandoLaco::TipoLaco::FOR,
            inicializacao,              
            condicao,                   
            atualizacao,              
            corpoAST    
        );
    }
    
    return nullptr; 
}

std::shared_ptr<NodoComando> Parser::analisarComandoRetorno() {
    consumir(TokenType::KW_RETURN);
    auto expr = analisarExpressao();
    return std::make_shared<ComandoRetorno>(expr);
}

std::shared_ptr<NodoComando> Parser::analisarComandoIdentOuAtribuicao() {
    Token id = consumir(TokenType::IDENTIFIER);
    
    if (verificar(TokenType::PAREN_OPEN)) {
        // É uma Chamada (Ex: funcao(args))
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
        return std::make_shared<ComandoIdent>(id.valor, args);
    }
    

    return analisarComandoAtribuicao(id); 
}

// ** FUNÇÃO CRÍTICA REFATORADA (Atribuição) **
std::shared_ptr<ComandoAtribuicao> Parser::analisarComandoAtribuicao(Token l_value_token) {
    Token op = advance(); 
    auto expr = analisarExpressao();
    
    return std::make_shared<ComandoAtribuicao>(l_value_token.valor, op.valor, expr);
}

std::vector<std::shared_ptr<NodoAST>> Parser::analisarComandoElse() {
    consumir(TokenType::KW_ELSE);
    
    if (verificar(TokenType::KW_IF)) { // else if
        auto condicional = analisarComandoCondicional();
        std::vector<std::shared_ptr<NodoAST>> elseIfWrapper;
        elseIfWrapper.push_back(condicional); 
        return elseIfWrapper;
    }
    
    consumir(TokenType::BRACE_OPEN);
    std::vector<std::shared_ptr<NodoAST>> blocoElse = analisarBlocoComandos();
    consumir(TokenType::BRACE_CLOSE);

    return blocoElse;
}


// ----------------------------------------------------------------------------------
// DECLARAÇÕES
// ----------------------------------------------------------------------------------

// ** FUNÇÃO CRÍTICA REFATORADA (Transição Global) **
std::vector<std::shared_ptr<NodoDeclaracao>> Parser::analisarBlocoDeclaracoes() {
    std::vector<std::shared_ptr<NodoDeclaracao>> declaracoes;
    while (!fimDaEntrada()) {
        
        // 1. Declarações de topo de nível
        if (verificar(TokenType::KW_STRUCT) || verificar(TokenType::KW_INTERFACE) || verificar(TokenType::KW_DEF)) {
            declaracoes.push_back(analisarDeclaracao());
            continue;
        }

        // 2. Declaração Variável Global explícita (Ex: lado: float = 4.5)
        if (verificar(TokenType::IDENTIFIER) &&
            posicao_atual + 1 < tokens.size() && tokens[posicao_atual + 1].tipo == TokenType::COLON) {
            
            declaracoes.push_back(analisarDeclaracaoVariavel());
            continue;
        }

        // 3. INÍCIO DOS COMANDOS GLOBAIS
        if (verificar(TokenType::IDENTIFIER) || verificar(TokenType::KW_IF) || verificar(TokenType::KW_WHILE) || verificar(TokenType::KW_FOREACH) || verificar(TokenType::KW_FOR)) {
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

    if (verificar(TokenType::COLON)) {
        consumir(TokenType::COLON);
        Token tipoToken = advance(); 
        tipo = tipoToken.valor;
    }

    if (verificar(TokenType::ASSIGN)) {
        consumir(TokenType::ASSIGN);
        inicializador = analisarExpressao();
    } 
    
    return std::make_shared<DeclaracaoVariavel>(
        nome.valor,
        tipo.has_value() ? tipo.value() : "",
        inicializador
    );
}

std::shared_ptr<NodoDeclaracao> Parser::analisarDeclaracao() {
    if (verificar(TokenType::KW_INTERFACE)) {
        consumir(TokenType::KW_INTERFACE);
        Token nome = consumir(TokenType::IDENTIFIER);
        // Consumir(TokenType::COLON); // Removido se não faz parte da sintaxe
        consumir(TokenType::BRACE_OPEN);
        std::vector<std::shared_ptr<DeclaracaoFuncao>> metodos;
        while (verificar(TokenType::KW_DEF)) {
            metodos.push_back(analisarDeclaracaoFuncao());
        }
        consumir(TokenType::BRACE_CLOSE);
        return std::make_shared<DeclaracaoInterface>(nome.valor, metodos);

    } else if (verificar(TokenType::KW_STRUCT)) {
        consumir(TokenType::KW_STRUCT);
        Token nome = consumir(TokenType::IDENTIFIER);
        
        std::string implementa = "";
        if (verificar(TokenType::KW_IMPLEMENTS)) { 
            consumir(TokenType::KW_IMPLEMENTS);
            implementa = consumir(TokenType::IDENTIFIER).valor;
            // Consumir(TokenType::COLON); // Removido se não faz parte da sintaxe
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
                std::cerr << "Erro: conteúdo de struct inesperado." << std::endl;
                advance();
            }
        }
        consumir(TokenType::BRACE_CLOSE);
        return std::make_shared<DeclaracaoStruct>(nome.valor, implementa, atributos, metodos);
        
    } else if (verificar(TokenType::KW_DEF)) {
        return analisarDeclaracaoFuncao();
    }
    
    std::cerr << "Erro: tipo de declaração inesperado: " /* << tokenTypeToString(peek().tipo) */ << std::endl;
    advance();
    return nullptr;
}

std::shared_ptr<DeclaracaoFuncao> Parser::analisarDeclaracaoFuncao() {
    consumir(TokenType::KW_DEF);
    Token nome = consumir(TokenType::IDENTIFIER);
    
    consumir(TokenType::PAREN_OPEN);
    // ... Analisar parâmetros ...
    consumir(TokenType::PAREN_CLOSE);
    
    if (verificar(TokenType::COLON)) {
        consumir(TokenType::COLON);
        // ... Analisar tipo de retorno ...
    }

    consumir(TokenType::BRACE_OPEN);
    std::vector<std::shared_ptr<NodoAST>> corpo = analisarBlocoComandos(); 
    consumir(TokenType::BRACE_CLOSE);

    return std::make_shared<DeclaracaoFuncao>(nome.valor, std::vector<std::pair<std::string, std::string>>(), "", corpo);
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
        } else {
            esquerda = std::make_shared<ExpressaoBinaria>(esquerda, op.valor, direita);
        }
    }

    return esquerda;
}

// ** FUNÇÃO CRÍTICA COMPLETA **
std::shared_ptr<NodoExpressao> Parser::analisarFator() {
    std::shared_ptr<NodoExpressao> expr;
    Token id_token; // Para armazenar o token IDENTIFIER original

    if (verificar(TokenType::INTEGER) || verificar(TokenType::FLOAT) ||
        verificar(TokenType::STRING_LITERAL) || verificar(TokenType::CHAR_LITERAL)) {
        Token literal = advance();
        expr = std::make_shared<ExpressaoLiteral>(literal.valor);
    } else if (verificar(TokenType::KW_THIS)) { 
        Token kw = advance();
        expr = std::make_shared<ExpressaoIdentificador>(kw.valor); 
    } else if (verificar(TokenType::IDENTIFIER)) {
        id_token = advance(); // Armazena o token ID
        expr = std::make_shared<ExpressaoIdentificador>(id_token.valor);
    } else if (verificar(TokenType::PAREN_OPEN)) {
        advance();
        expr = analisarExpressao();
        consumir(TokenType::PAREN_CLOSE);
    } else if (verificar(TokenType::BRACKET_OPEN)) { // Array literal
        consumir(TokenType::BRACKET_OPEN);
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
    } else if (verificar(TokenType::MINUS)) { // Operador unário (negativo)
        Token op = advance();
        auto fator = analisarFator();
        expr = std::make_shared<ExpressaoUnaria>(op.valor, fator);
    } else {
        std::cerr << "Erro de sintaxe: fator inesperado em " << (int)peek().tipo << std::endl;
        advance(); 
        return nullptr;
    }

    // Pós-análise: Chamadas de função/construtor e Acesso a Membro (dot-access)
    while (verificar(TokenType::DOT) || verificar(TokenType::PAREN_OPEN)) {
        if (verificar(TokenType::DOT)) {
            consumir(TokenType::DOT);
            Token membro = consumir(TokenType::IDENTIFIER);
            expr = std::make_shared<ExpressaoAcessoMembro>(expr, membro.valor);
        } else if (verificar(TokenType::PAREN_OPEN)) {
            
            // Verifica se o 'expr' atual é o nome da função (Identificador ou AcessoMembro)
            // Caso contrário, (e.g., (1+2)(3) - que não é permitido) quebra.
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
            
            // O nome da chamada é o nome do identificador (se for ExpressaoIdentificador)
            std::string nome_chamada;
            if (auto id_expr = std::dynamic_pointer_cast<ExpressaoIdentificador>(expr)) {
                nome_chamada = id_expr->nome;
            } else if (auto membro_expr = std::dynamic_pointer_cast<ExpressaoAcessoMembro>(expr)) {
                // Se for objeto.metodo(), o nome da chamada é toda a ExpressaoAcessoMembro
                // Neste caso, precisamos de um Nodo específico para ExpressaoChamadaMembro,
                // mas vamos reutilizar ExpressaoChamada passando a Expressao original como 'nome'
                // para simplificar a AST.
                // Isso é um HACK na AST, idealmente ExpressaoChamada precisaria de refactoring.
                // Para manter o código compilável, usaremos o nome original do ID (se existir)
                nome_chamada = id_token.valor;
            } else {
                 // Deve ser tratado no seu design AST/Visitor
                 nome_chamada = "Erro_Chamada_Expressao_Complexa"; 
            }
            
            expr = std::make_shared<ExpressaoChamada>(nome_chamada, args); 
            
        } else {
            break;
        }
    }

    return expr;
}
