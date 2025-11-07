#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>

struct Posicao {
    int linha;
    int coluna;
};

// ========== BASE ==========

class NodoAST {
public:
    Posicao posicao;
    virtual ~NodoAST() = default;
    virtual void imprimir(int indent = 0) const = 0;
};

// ========== EXPRESSÕES ==========

class NodoExpressao : public NodoAST {};

class ExpressaoLiteral : public NodoExpressao {
public:
    std::string valor;
    ExpressaoLiteral(std::string valor);
    void imprimir(int indent = 0) const override;
};

class ExpressaoIdentificador : public NodoExpressao {
public:
    std::string nome;
    ExpressaoIdentificador(std::string nome);
    void imprimir(int indent = 0) const override;
};

class ExpressaoBinaria : public NodoExpressao {
public:
    std::string operador;
    std::shared_ptr<NodoExpressao> esquerda;
    std::shared_ptr<NodoExpressao> direita;
    ExpressaoBinaria(std::shared_ptr<NodoExpressao> esquerda,
                     std::string operador,
                     std::shared_ptr<NodoExpressao> direita);
    void imprimir(int indent = 0) const override;
};

class ExpressaoUnaria : public NodoExpressao {
public:
    std::string operador;
    std::shared_ptr<NodoExpressao> operando;
    ExpressaoUnaria(std::string operador, std::shared_ptr<NodoExpressao> operando);
    void imprimir(int indent = 0) const override;
};

// ========== DECLARAÇÕES ==========

class NodoDeclaracao : public NodoAST {};

class DeclaracaoVariavel : public NodoDeclaracao {
public:
    std::string nome;
    std::string tipo;
    std::shared_ptr<NodoExpressao> inicializador;
    DeclaracaoVariavel(std::string nome,
                       std::string tipo,
                       std::shared_ptr<NodoExpressao> inicializador);
    void imprimir(int indent = 0) const override;
};

class DeclaracaoFuncao : public NodoDeclaracao {
public:
    std::string nome;
    std::vector<std::pair<std::string, std::string>> parametros; // tipo, nome
    std::string tipoRetorno;
    std::vector<std::shared_ptr<NodoAST>> corpo;
    DeclaracaoFuncao(std::string nome,
                     std::vector<std::pair<std::string, std::string>> parametros,
                     std::string tipoRetorno,
                     std::vector<std::shared_ptr<NodoAST>> corpo);
    void imprimir(int indent = 0) const override;
};

class DeclaracaoStruct : public NodoDeclaracao {
public:
    std::string nome;
    std::string implementa;
    std::vector<std::shared_ptr<NodoDeclaracao>> atributos;
    std::vector<std::shared_ptr<DeclaracaoFuncao>> metodos;
    DeclaracaoStruct(std::string nome,
                     std::string implementa,
                     std::vector<std::shared_ptr<NodoDeclaracao>> atributos,
                     std::vector<std::shared_ptr<DeclaracaoFuncao>> metodos);
    void imprimir(int indent = 0) const override;
};

class DeclaracaoInterface : public NodoDeclaracao {
public:
    std::string nome;
    std::vector<std::shared_ptr<DeclaracaoFuncao>> metodos;
    DeclaracaoInterface(std::string nome,
                        std::vector<std::shared_ptr<DeclaracaoFuncao>> metodos);
    void imprimir(int indent = 0) const override;
};

// ========== COMANDOS ==========

class NodoComando : public NodoAST {};

class ComandoRetorno : public NodoComando {
public:
    std::shared_ptr<NodoExpressao> expressao;
    ComandoRetorno(std::shared_ptr<NodoExpressao> expr);
    void imprimir(int indent = 0) const override;
};

class ComandoAtribuicao : public NodoComando {
public:
    std::string nome;
    std::string operador;
    std::shared_ptr<NodoExpressao> expressao;
    ComandoAtribuicao(std::string nome, std::string operador, std::shared_ptr<NodoExpressao> expr);
    void imprimir(int indent = 0) const override;
};

class ComandoCondicional : public NodoComando {
public:
    std::shared_ptr<NodoExpressao> condicao;
    std::vector<std::shared_ptr<NodoAST>> blocoIf;
    std::vector<std::shared_ptr<NodoAST>> blocoElse;
    ComandoCondicional(std::shared_ptr<NodoExpressao> condicao,
                       std::vector<std::shared_ptr<NodoAST>> blocoIf,
                       std::vector<std::shared_ptr<NodoAST>> blocoElse);
    void imprimir(int indent = 0) const override;
};
