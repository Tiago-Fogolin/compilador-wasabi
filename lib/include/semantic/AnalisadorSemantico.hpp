#ifndef ANALISADOR_SEMANTICO_HPP
#define ANALISADOR_SEMANTICO_HPP

#include <semantic/TabelaSimbolos.hpp>
#include <parser/ast/AstBase.hpp>
#include <vector>
#include <string>
#include <memory>

struct ErroSemantico {
    std::string mensagem;
    int linha;
    ErroSemantico(std::string m, int l) : mensagem(std::move(m)), linha(l) {}
    std::string toString() const;
};

class AnalisadorSemantico {
public:
    AnalisadorSemantico();

    void analisar(const std::shared_ptr<NodoPrograma>& programa);

private:
    TabelaSimbolos tabela;
    std::vector<ErroSemantico> erros;


    void primeiraPassadaColetar(const std::shared_ptr<NodoPrograma>& prog);
    void segundaPassadaAnalisar(const std::shared_ptr<NodoPrograma>& prog);


    void analisarDeclaracao(const std::shared_ptr<NodoDeclaracao>& decl);
    void analisarDeclaracaoVariavel(const std::shared_ptr<DeclaracaoVariavel>& dv);
    void analisarDeclaracaoFuncao(const std::shared_ptr<DeclaracaoFuncao>& df);
    void coletarAssinaturaFuncao(const std::shared_ptr<DeclaracaoFuncao>& df);


    void analisarComando(const std::shared_ptr<NodoAST>& nodo);
    void analisarComandoRetorno(const std::shared_ptr<ComandoRetorno>& ret);
    void analisarComandoAtribuicao(const std::shared_ptr<ComandoAtribuicao>& at);
    void analisarComandoIdent(const std::shared_ptr<ComandoIdent>& ci);
    void analisarComandoDeclaracao(const std::shared_ptr<ComandoDeclaracao>& cd);
    void analisarComandoCondicional(const std::shared_ptr<ComandoCondicional>& cc);
    void analisarComandoLaco(const std::shared_ptr<ComandoLaco>& lc);

    std::string inferirTipoExpressao(const std::shared_ptr<NodoExpressao>& expr);
    std::string inferirTipoBinaria(const std::shared_ptr<ExpressaoBinaria>& b);
    std::string inferirTipoIdentificador(const std::shared_ptr<ExpressaoIdentificador>& id);
    std::string inferirTipoLiteral(const std::shared_ptr<ExpressaoLiteral>& lit);
    std::string inferirTipoChamada(const std::shared_ptr<ExpressaoChamada>& ch);
    std::string inferirTipoRelacional(const std::shared_ptr<ExpressaoRelacional>& r);

    bool tiposCompativeis(const std::string& a, const std::string& b) const;
    bool tipoNumerico(const std::string& t) const;
    bool todosOsCaminhosRetornam(const std::vector<std::shared_ptr<NodoAST>>& corpo) const;


    static std::string montarAssinaturaFuncao(const DeclaracaoFuncao& df);
    static bool parseAssinaturaFuncao(const std::string& sig,
                                      std::vector<std::string>& outParams,
                                      std::string& outRet);

    // Erros
    void reportarErro(const std::string& msg, int linha);

public:
    const std::vector<ErroSemantico>& obterErros() const { return erros; }
};

#endif
