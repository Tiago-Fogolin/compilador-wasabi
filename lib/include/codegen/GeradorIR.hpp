#pragma once

#include <sstream>
#include <string>
#include <map>
#include <memory>
#include "parser/ast/AstBase.hpp"
#include <semantic/TabelaSimbolos.hpp>


class GeradorIR {
public:
    GeradorIR() : contadorTemp(0) {
        codigo.str("");
    }
    GeradorIR(TabelaSimbolos* tabela);

    std::string gerar(const std::shared_ptr<NodoPrograma>& programa);

    std::string gerarComando(const std::shared_ptr<NodoComando>& cmd);
    std::string gerarExpressao(const std::shared_ptr<NodoExpressao>& expr);

    std::string obterTipoLLVM(const std::string& tipoAST);
    std::string gerarDeclaracaoVariavel(const std::shared_ptr<DeclaracaoVariavel>& dv);
    std::string gerarExpressaoLiteral(const std::shared_ptr<ExpressaoLiteral>& lit);
    std::string gerarExpressaoUnaria(const std::shared_ptr<ExpressaoUnaria>& unaria);
    std::string gerarExpressaoLogica(const std::shared_ptr<ExpressaoLogica>& logica);

    std::string gerarExpressaoBinaria(const std::shared_ptr<ExpressaoBinaria>& expr);

    std::string gerarComandoCondicional(const std::shared_ptr<ComandoCondicional>& cmdCond);

    std::string gerarComandoLaco(const std::shared_ptr<ComandoLaco>& laco);

    std::string gerarExpressaoRelacional(const std::shared_ptr<ExpressaoRelacional>& rel);

    std::string gerarComandoRetorno(const std::shared_ptr<ComandoRetorno>& cmdRet);

    std::string gerarDeclaracaoFuncao(const std::shared_ptr<DeclaracaoFuncao>& df);
    std::string novoTemp();
    void alocarVariaveisLocais(
        const std::vector<std::shared_ptr<NodoAST>> &nodos,
        std::stringstream &codigoAlocacoes
    );

    std::vector<std::string> globaisString;

    std::string gerarLiteralString(const std::string &valor);


private:
    std::stringstream codigo;
    int contadorTemp;

    int contadorLaco = 0;
    int contadorStrings = 0;

    TabelaSimbolos* tss;
    std::map<std::string, std::string> variaveisLocais;

    std::string gerarDeclaracao(const std::shared_ptr<NodoDeclaracao>& decl);

    std::string gerarExpressaoIdentificador(const std::shared_ptr<ExpressaoIdentificador>& id);
    std::string gerarExpressaoChamada(const std::shared_ptr<ExpressaoChamada>& chamada);
    std::string gerarComandoAtribuicao(const std::shared_ptr<ComandoAtribuicao>& atrib);
    std::string gerarComandoIdent(const std::shared_ptr<ComandoIdent>& cmdIdent);
};
