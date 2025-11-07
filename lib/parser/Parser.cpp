#include "include/parser/Parser.hpp";

Parser::Parser(std::vector<Token> tokens) {
    this->tokens = tokens;
    this->posicao_atual = 0;
}

Token Parser::peek() {
    return this->tokens[posicao_atual];
}

Token Parser::advance() {
    Token tokenAtual = this->peek();
    this->posicao_atual++;
    return tokenAtual;
}

bool Parser::fimDaEntrada() {
    return posicao_atual >= tokens.size();
}

bool Parser::verificar(TokenType tipoEsperado) {
    if (this->fimDaEntrada()) {
        return false;
    }

    return this->peek().tipo == tipoEsperado;
}

Token Parser::consumir(TokenType tipoEsperado) {
    if (!this->verificar(tipoEsperado)) {
        std::cerr << "Esperado " << 
                    tokenTypeToString(tipoEsperado) << 
                    " mas encontrado" << 
                    tokenTypeToString(peek().tipo) << std::endl;
    }

    return this->advance();
}

std::shared_ptr<NodoAST> Parser::analisarPrograma() {
    auto blocoDecls = analisarBlocoDeclaracoes();
    // TODO: adicionar análise de BlocoComandos
    return nullptr;
}

std::vector<std::shared_ptr<NodoDeclaracao>> Parser::analisarBlocoDeclaracoes() {
    std::vector<std::shared_ptr<NodoDeclaracao>> declaracoes;

    while (!fimDaEntrada()) {
        declaracoes.push_back(analisarDeclaracao());
    }

    return declaracoes;
}

std::shared_ptr<NodoDeclaracao> Parser::analisarDeclaracao() {
    if (verificar(TokenType::IDENTIFIER)) {
        // Tentamos uma declaração de variável: identificador ":" Tipo "=" Expressão
        return analisarDeclaracaoVariavel();
    }

    // TODO: adicionar suporte para 'struct', 'interface', 'def'
    std::cerr << "Erro: tipo de declaração inesperado: " 
              << tokenTypeToString(peek().tipo) << std::endl;
    advance();
    return nullptr;
}

std::shared_ptr<DeclaracaoVariavel> Parser::analisarDeclaracaoVariavel() {
    Token nome = consumir(TokenType::IDENTIFICADOR);
    std::optional<std::string> tipo;

    if (verificar(TokenType::DOIS_PONTOS)) {
        advance();
        Token tipoToken = advance();
        tipo = tokenTypeToString(tipoToken.tipo);
    }

    consumir(TokenType::IGUAL);
    auto inicializador = analisarExpressao();

    return std::make_shared<DeclaracaoVariavel>(
        nome.lexema,
        tipo.has_value() ? tipo.value() : "",
        inicializador
    );
}


std::shared_ptr<NodoExpressao> Parser::analisarExpressao() {
    return analisarExpressaoAritmetica();
}

std::shared_ptr<NodoExpressao> Parser::analisarExpressaoAritmetica() {
    auto esquerda = analisarTermo();

    while (verificar(TokenType::MAIS) || verificar(TokenType::MENOS)) {
        Token op = advance();
        auto direita = analisarTermo();
        esquerda = std::make_shared<ExpressaoBinaria>(op.lexema, esquerda, direita);
    }

    return esquerda;
}

std::shared_ptr<NodoExpressao> Parser::analisarTermo() {
    auto esquerda = analisarFator();

    while (verificar(TokenType::ASTERISCO) || verificar(TokenType::BARRA)) {
        Token op = advance();
        auto direita = analisarFator();
        esquerda = std::make_shared<ExpressaoBinaria>(op.lexema, esquerda, direita);
    }

    return esquerda;
}

std::shared_ptr<NodoExpressao> Parser::analisarFator() {
    if (verificar(TokenType::LITERAL_INTEIRO) ||
        verificar(TokenType::LITERAL_REAL) ||
        verificar(TokenType::LITERAL_TEXTO) ||
        verificar(TokenType::LITERAL_BOOLEANO)) {

        Token literal = advance();
        return std::make_shared<ExpressaoLiteral>(literal.lexema);
    }

    if (verificar(TokenType::IDENTIFICADOR)) {
        Token id = advance();
        return std::make_shared<ExpressaoIdentificador>(id.lexema);
    }

    if (verificar(TokenType::PARENTESE_ESQUERDO)) {
        advance();
        auto expr = analisarExpressao();
        consumir(TokenType::PARENTESE_DIREITO);
        return expr;
    }

    std::cerr << "Erro de sintaxe: fator inesperado em " 
              << tokenTypeToString(peek().tipo) << std::endl;
    advance();
    return nullptr;
}