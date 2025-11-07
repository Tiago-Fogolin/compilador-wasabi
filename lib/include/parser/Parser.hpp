#include <Token.hpp>
#include <vector>
#include <Automato.hpp>
#include <AstBase.hpp>
#include <memory>
#include <iostream>

class Parser {
private:
    size_t posicao_atual;
public:
    std::vector<Token> tokens;

    Parser(std::vector<Token> tokens);

    Token peek();

    Token advance();

    bool fimDaEntrada();

    bool verificar(TokenType tipoEsperado);

    Token consumir(TokenType tipoEsperado);

    std::shared_ptr<NodoAST> analisarPrograma();
    std::vector<std::shared_ptr<NodoDeclaracao>> analisarBlocoDeclaracoes();
    std::shared_ptr<NodoDeclaracao> analisarDeclaracao();
    std::shared_ptr<DeclaracaoVariavel> analisarDeclaracaoVariavel();
    std::shared_ptr<NodoExpressao> analisarExpressao();

    std::shared_ptr<NodoExpressao> analisarExpressaoAritmetica();
    std::shared_ptr<NodoExpressao> analisarTermo();
    std::shared_ptr<NodoExpressao> analisarFator();

};