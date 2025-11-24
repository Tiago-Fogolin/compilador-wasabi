#pragma once
#include "automato_base/Automato.hpp"
#include <string>
#include <vector>
#include <memory>
#include <iostream>

struct Posicao {
    int linha;
    int coluna;
};



class NodoAST {
public:
    Posicao posicao;
    virtual ~NodoAST() = default;
    virtual void imprimir(int indent = 0) const = 0;
};


class NodoExpressao : public NodoAST {};

class ExpressaoLiteral : public NodoExpressao {
public:
    std::string valor;
    std::string tipo;
    ExpressaoLiteral(std::string valor, std::string tipo);
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
    std::vector<std::pair<std::string, std::string>> parametros;
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
    std::vector<std::string> implementa;
    std::vector<std::shared_ptr<NodoDeclaracao>> atributos;
    std::vector<std::shared_ptr<DeclaracaoFuncao>> metodos;
    DeclaracaoStruct(std::string nome,
                     std::vector<std::string> implementa,
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


class NodoPrograma : public NodoAST {
public:
    std::vector<std::shared_ptr<NodoDeclaracao>> declaracoes;
    std::vector<std::shared_ptr<NodoComando>> comandos;
    NodoPrograma(std::vector<std::shared_ptr<NodoDeclaracao>> decls,
                 std::vector<std::shared_ptr<NodoComando>> cmds);
    void imprimir(int indent = 0) const override;
};

class ComandoIdent : public NodoComando {
public:
    std::string nome;
    std::vector<std::shared_ptr<NodoExpressao>> argumentos;
    std::string operadorAtribuicao;
    std::shared_ptr<NodoExpressao> expressao;
    ComandoIdent(std::string nome,
                 std::vector<std::shared_ptr<NodoExpressao>> argumentos);
    ComandoIdent(std::string nome,
                 std::string operadorAtribuicao,
                 std::shared_ptr<NodoExpressao> expressao);
    void imprimir(int indent = 0) const override;
};

class ComandoDeclaracao : public NodoComando {
public:
    std::shared_ptr<NodoDeclaracao> declaracao;
    ComandoDeclaracao(std::shared_ptr<NodoDeclaracao> declaracao);
    void imprimir(int indent = 0) const override;
};

class ComandoLaco : public NodoComando {
public:
    enum class TipoLaco { FOR, FOREACH, WHILE };
    TipoLaco tipo;
    std::shared_ptr<NodoComando> inicializacao;
    std::shared_ptr<NodoExpressao> condicao;
    std::shared_ptr<NodoComando> incremento;
    std::shared_ptr<NodoExpressao> expressaoForeach;
    std::string identificadorForeach;
    std::vector<std::shared_ptr<NodoAST>> corpo;
    ComandoLaco(TipoLaco tipo,
                std::shared_ptr<NodoComando> init,
                std::shared_ptr<NodoExpressao> cond,
                std::shared_ptr<NodoComando> incr,
                std::vector<std::shared_ptr<NodoAST>> corpo);
    ComandoLaco(TipoLaco tipo,
                std::string identificador,
                std::shared_ptr<NodoExpressao> expr,
                std::vector<std::shared_ptr<NodoAST>> corpo);
    void imprimir(int indent = 0) const override;
};

class ExpressaoLogica : public NodoExpressao {
public:
    std::string operador;
    std::shared_ptr<NodoExpressao> esquerda;
    std::shared_ptr<NodoExpressao> direita;
    ExpressaoLogica(std::shared_ptr<NodoExpressao> esquerda,
                    std::string operador,
                    std::shared_ptr<NodoExpressao> direita);
    void imprimir(int indent = 0) const override;
};

class ExpressaoRelacional : public NodoExpressao {
public:
    std::string operador;
    std::shared_ptr<NodoExpressao> esquerda;
    std::shared_ptr<NodoExpressao> direita;
    ExpressaoRelacional(std::shared_ptr<NodoExpressao> esquerda,
                        std::string operador,
                        std::shared_ptr<NodoExpressao> direita);
    void imprimir(int indent = 0) const override;
};

class ExpressaoChamada : public NodoExpressao {
public:
    std::string nome;
    std::vector<std::shared_ptr<NodoExpressao>> argumentos;
    ExpressaoChamada(std::string nome,
                     std::vector<std::shared_ptr<NodoExpressao>> argumentos);
    void imprimir(int indent = 0) const override;
};

class ExpressaoAgrupada : public NodoExpressao {
public:
    std::shared_ptr<NodoExpressao> expressao;
    ExpressaoAgrupada(std::shared_ptr<NodoExpressao> expressao);
    void imprimir(int indent = 0) const override;
};

// Expressão de Acesso a Membro (Ex: objeto.propriedade ou this.metodo())
class ExpressaoAcessoMembro : public NodoExpressao {
public:
    std::shared_ptr<NodoExpressao> objeto;
    std::string membro;
    ExpressaoAcessoMembro(std::shared_ptr<NodoExpressao> objeto, std::string membro);
    void imprimir(int indent = 0) const override;
};

// Expressão de Array Literal (Ex: [1, 2, "a"])
class ExpressaoArrayLiteral : public NodoExpressao {
public:
    std::vector<std::shared_ptr<NodoExpressao>> elementos;
    ExpressaoArrayLiteral(std::vector<std::shared_ptr<NodoExpressao>> elementos);
    void imprimir(int indent = 0) const override;
};

class ExpressaoAcessoArray : public NodoExpressao {
public:
    std::shared_ptr<NodoExpressao> arrayBase;
    std::shared_ptr<NodoExpressao> indice;

    ExpressaoAcessoArray(
        std::shared_ptr<NodoExpressao> arrayBase,
        std::shared_ptr<NodoExpressao> indice
    );
    void imprimir(int indent = 0) const override;
};
