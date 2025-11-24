#pragma once
#include <vector>
#include <memory>
#include <optional>
#include "lexer/Token.hpp"
#include "ast/AstBase.hpp"

class Parser {
private:
    std::vector<Token> tokens;
    size_t posicao_atual;

public:
    Parser(std::vector<Token> tokens);

    // Auxiliares de navegação (SEM ALTERAÇÃO)
    Token peek();
    Token advance();
    bool fimDaEntrada();
    bool verificar(TokenType tipoEsperado);
    Token consumir(TokenType tipoEsperado);

    std::shared_ptr<NodoPrograma> analisarPrograma();



    std::vector<std::shared_ptr<NodoAST>> analisarBlocoComandos(bool dentro_bloco);
    std::vector<std::shared_ptr<NodoDeclaracao>> analisarBlocoDeclaracoes();

    std::shared_ptr<NodoComando> analisarComando(bool dentro_bloco = true);
    std::shared_ptr<NodoComando> analisarComandoCondicional();
    std::shared_ptr<NodoComando> analisarComandoLaco();
    std::shared_ptr<NodoComando> analisarComandoRetorno();
    std::shared_ptr<NodoComando> analisarComandoIdentOuAtribuicao();
    std::shared_ptr<ComandoAtribuicao> analisarComandoAtribuicao(Token l_value_token);

    std::vector<std::shared_ptr<NodoAST>> analisarComandoElse();

    std::shared_ptr<NodoDeclaracao> analisarDeclaracao();
    std::shared_ptr<DeclaracaoVariavel> analisarDeclaracaoVariavel();
    std::shared_ptr<DeclaracaoFuncao> analisarDeclaracaoFuncao();

    std::shared_ptr<NodoExpressao> analisarExpressao();

    std::shared_ptr<NodoExpressao> analisarExpressaoComparacao();

    std::shared_ptr<NodoExpressao> analisarExpressaoAritmetica();

    std::shared_ptr<NodoExpressao> analisarExpressaoBinaria(int prec_min);

    std::shared_ptr<NodoExpressao> analisarTermo();

    std::shared_ptr<NodoExpressao> analisarFator();

    std::shared_ptr<NodoExpressao> analisarChamada();
    std::shared_ptr<NodoExpressao> analisarArrayLiteral();
    std::shared_ptr<NodoExpressao> analisarAcessoMembro(std::shared_ptr<NodoExpressao> objeto);

    std::string tokenTypeToDataType(TokenType tipo);
    int obterPrecedencia(TokenType tipo);
    bool ehOperadorAtribuicao(TokenType tipo);
};
