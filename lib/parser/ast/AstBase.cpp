#include "../include/parser/ast/AstBase.hpp"
#include <utility> // Para std::move

#define IND(n) std::string(n, ' ')

// ==========================================================
// EXPRESSÕES
// ==========================================================
ExpressaoLiteral::ExpressaoLiteral(std::string valor): valor(std::move(valor)) {}
void ExpressaoLiteral::imprimir(int indent) const { std::cout << IND(indent) << "Literal(" << valor << ")\n"; }

ExpressaoIdentificador::ExpressaoIdentificador(std::string nome): nome(std::move(nome)) {}
void ExpressaoIdentificador::imprimir(int indent) const { std::cout << IND(indent) << "Ident(" << nome << ")\n"; }

ExpressaoBinaria::ExpressaoBinaria(std::shared_ptr<NodoExpressao> e, std::string op, std::shared_ptr<NodoExpressao> d)
: esquerda(std::move(e)), operador(std::move(op)), direita(std::move(d)) {}
void ExpressaoBinaria::imprimir(int i) const {
    std::cout << IND(i) << "Binaria(" << operador << ")\n";
    esquerda->imprimir(i+2); direita->imprimir(i+2);
}

ExpressaoUnaria::ExpressaoUnaria(std::string op, std::shared_ptr<NodoExpressao> operando)
: operador(std::move(op)), operando(std::move(operando)) {}
void ExpressaoUnaria::imprimir(int i) const {
    std::cout << IND(i) << "Unaria(" << operador << ")\n";
    operando->imprimir(i+2);
}

// ----------------------------------------------------------
// NOVOS NODOS DE EXPRESSÃO
// ----------------------------------------------------------

ExpressaoAcessoMembro::ExpressaoAcessoMembro(std::shared_ptr<NodoExpressao> objeto, std::string membro)
    : objeto(std::move(objeto)), membro(std::move(membro)) {}
void ExpressaoAcessoMembro::imprimir(int i) const {
    std::cout << IND(i) << "AcessoMembro(." << membro << ")\n";
    objeto->imprimir(i+2);
}


ExpressaoArrayLiteral::ExpressaoArrayLiteral(std::vector<std::shared_ptr<NodoExpressao>> elementos)
    : elementos(std::move(elementos)) {}
void ExpressaoArrayLiteral::imprimir(int i) const {
    std::cout << IND(i) << "ArrayLiteral\n";
    for (const auto& el : elementos) {
        el->imprimir(i+2);
    }
}

ExpressaoChamada::ExpressaoChamada(std::string nome, std::vector<std::shared_ptr<NodoExpressao>> argumentos)
    : nome(std::move(nome)), argumentos(std::move(argumentos)) {}
void ExpressaoChamada::imprimir(int i) const {
    std::cout << IND(i) << "Chamada(" << nome << ")\n";
    for (const auto& arg : argumentos) {
        arg->imprimir(i+2);
    }
}


// ==========================================================
// DECLARAÇÕES
// ==========================================================
DeclaracaoVariavel::DeclaracaoVariavel(std::string n, std::string t, std::shared_ptr<NodoExpressao> ini)
: nome(std::move(n)), tipo(std::move(t)), inicializador(std::move(ini)) {}
void DeclaracaoVariavel::imprimir(int i) const {
    std::cout << IND(i) << "VarDecl(" << nome;
    if(!tipo.empty()) std::cout << ": " << tipo;
    std::cout << ")\n";
    if(inicializador) inicializador->imprimir(i+2);
}

DeclaracaoFuncao::DeclaracaoFuncao(std::string n, std::vector<std::pair<std::string,std::string>> p, std::string r, std::vector<std::shared_ptr<NodoAST>> c)
: nome(std::move(n)), parametros(std::move(p)), tipoRetorno(std::move(r)), corpo(std::move(c)) {}
void DeclaracaoFuncao::imprimir(int i) const {
    std::cout << IND(i) << "Func(" << nome << ")\n";
    // Nota: O método imprimir de FuncDecl deve listar os parâmetros e o corpo.
    // O seu código apenas lista o corpo, o que pode ser bom para economizar espaço na saída.
    for (auto& s : corpo) s->imprimir(i+2);
}

DeclaracaoStruct::DeclaracaoStruct(std::string n, std::string imp, std::vector<std::shared_ptr<NodoDeclaracao>> a, std::vector<std::shared_ptr<DeclaracaoFuncao>> m)
: nome(std::move(n)), implementa(std::move(imp)), atributos(std::move(a)), metodos(std::move(m)) {}
void DeclaracaoStruct::imprimir(int i) const {
    std::cout << IND(i) << "Struct(" << nome << (implementa.empty() ? "" : (" implements " + implementa)) << ")\n";
    if(!atributos.empty()) { std::cout << IND(i+2) << "Atributos:\n"; for (auto& a : atributos) a->imprimir(i+4); }
    if(!metodos.empty()) { std::cout << IND(i+2) << "Metodos:\n"; for (auto& m : metodos) m->imprimir(i+4); }
}

DeclaracaoInterface::DeclaracaoInterface(std::string n, std::vector<std::shared_ptr<DeclaracaoFuncao>> m)
: nome(std::move(n)), metodos(std::move(m)) {}
void DeclaracaoInterface::imprimir(int i) const {
    std::cout << IND(i) << "Interface(" << nome << ")\n";
    if(!metodos.empty()) { std::cout << IND(i+2) << "Metodos:\n"; for (auto& m : metodos) m->imprimir(i+4); }
}

// ==========================================================
// COMANDOS
// ==========================================================
ComandoRetorno::ComandoRetorno(std::shared_ptr<NodoExpressao> e): expressao(std::move(e)) {}
void ComandoRetorno::imprimir(int i) const {
    std::cout << IND(i) << "Return\n";
    if(expressao) expressao->imprimir(i+2);
}

ComandoAtribuicao::ComandoAtribuicao(std::string n, std::string op, std::shared_ptr<NodoExpressao> e)
: nome(std::move(n)), operador(std::move(op)), expressao(std::move(e)) {}
void ComandoAtribuicao::imprimir(int i) const {
    std::cout << IND(i) << "Atrib(" << nome << " " << operador << ")\n";
    expressao->imprimir(i+2);
}

ComandoCondicional::ComandoCondicional(std::shared_ptr<NodoExpressao> c, std::vector<std::shared_ptr<NodoAST>> bi, std::vector<std::shared_ptr<NodoAST>> be)
: condicao(std::move(c)), blocoIf(std::move(bi)), blocoElse(std::move(be)) {}
void ComandoCondicional::imprimir(int i) const {
    std::cout << IND(i) << "If\n";
    std::cout << IND(i+2) << "Cond:\n"; condicao->imprimir(i+4);
    std::cout << IND(i+2) << "Bloco If:\n"; for(auto& c : blocoIf) c->imprimir(i+4);
    if(!blocoElse.empty()) {
        std::cout << IND(i) << "Else\n";
        for(auto& c : blocoElse) c->imprimir(i+4);
    }
}

// ----------------------------------------------------------
// NOVO NODO DE COMANDO
// ----------------------------------------------------------

// ComandoDeclaracao (Wrapper para VarDecl dentro de blocos de comandos)
ComandoDeclaracao::ComandoDeclaracao(std::shared_ptr<NodoDeclaracao> declaracao)
    : declaracao(std::move(declaracao)) {}
void ComandoDeclaracao::imprimir(int i) const {
    std::cout << IND(i) << "ComandoDecl\n";
    declaracao->imprimir(i+2);
}

ComandoIdent::ComandoIdent(std::string n, std::vector<std::shared_ptr<NodoExpressao>> args)
    : nome(std::move(n)), argumentos(std::move(args)) {}
ComandoIdent::ComandoIdent(std::string n, std::string op, std::shared_ptr<NodoExpressao> expr)
    : nome(std::move(n)), operadorAtribuicao(std::move(op)), expressao(std::move(expr)) {}
void ComandoIdent::imprimir(int indent) const {
    // Nota: O ComandoIdent tem dois propósitos (chamada ou atribuição que não é VarDecl).
    if (argumentos.empty()) { // Atribuição
        std::cout << IND(indent) << "AtribIdent(" << nome << " " << operadorAtribuicao << ")\n";
        expressao->imprimir(indent+2);
    } else { // Chamada
        std::cout << IND(indent) << "ChamadaCmd(" << nome << ")\n";
        for (auto& arg : argumentos) arg->imprimir(indent + 2);
    }
}

ComandoLaco::ComandoLaco(TipoLaco t, std::shared_ptr<NodoComando> init, std::shared_ptr<NodoExpressao> cond,
                         std::shared_ptr<NodoComando> incr, std::vector<std::shared_ptr<NodoAST>> corpo_)
    : tipo(t), inicializacao(std::move(init)), condicao(std::move(cond)),
      incremento(std::move(incr)), corpo(std::move(corpo_)) {}
ComandoLaco::ComandoLaco(TipoLaco t, std::string id, std::shared_ptr<NodoExpressao> expr,
                         std::vector<std::shared_ptr<NodoAST>> corpo_)
    : tipo(t), identificadorForeach(std::move(id)), expressaoForeach(std::move(expr)),
      corpo(std::move(corpo_)) {}
void ComandoLaco::imprimir(int indent) const {
    std::string tipoStr;
    if (tipo == TipoLaco::FOR) tipoStr = "FOR";
    else if (tipo == TipoLaco::FOREACH) tipoStr = "FOREACH (" + identificadorForeach + ")";
    else tipoStr = "WHILE";
    
    std::cout << IND(indent) << "Laco(" << tipoStr << ")\n";
    if (inicializacao) { std::cout << IND(indent+2) << "Init:\n"; inicializacao->imprimir(indent+4); }
    if (condicao) { std::cout << IND(indent+2) << "Cond:\n"; condicao->imprimir(indent+4); }
    if (incremento) { std::cout << IND(indent+2) << "Incr:\n"; incremento->imprimir(indent+4); }
    if (expressaoForeach) { std::cout << IND(indent+2) << "ExprColecao:\n"; expressaoForeach->imprimir(indent+4); }
    std::cout << IND(indent+2) << "Corpo:\n"; for (auto& c : corpo) c->imprimir(indent+4);
}


// NODO PROGRAMA
NodoPrograma::NodoPrograma(std::vector<std::shared_ptr<NodoDeclaracao>> decls,
                           std::vector<std::shared_ptr<NodoComando>> cmds)
    : declaracoes(std::move(decls)), comandos(std::move(cmds)) {}
void NodoPrograma::imprimir(int indent) const {
    std::cout << IND(indent) << "Programa\n";
    if (!declaracoes.empty()) {
        std::cout << IND(indent+2) << "Declaracoes:\n";
        for (auto& d : declaracoes) d->imprimir(indent + 4);
    }
    if (!comandos.empty()) {
        std::cout << IND(indent+2) << "Comandos:\n";
        for (auto& c : comandos) c->imprimir(indent + 4);
    }
}

ExpressaoRelacional::ExpressaoRelacional(std::shared_ptr<NodoExpressao> esquerda,
                                         std::string operador,
                                         std::shared_ptr<NodoExpressao> direita)
    : operador(std::move(operador)),
      esquerda(std::move(esquerda)),
      direita(std::move(direita)) 
{
}


void ExpressaoRelacional::imprimir(int indent) const {
    std::cout << IND(indent) << "ExpressaoRelacional: (" << operador << ")\n";
    
    std::cout << IND(indent + 2) << "Esquerda:\n";

    if (esquerda) {
        esquerda->imprimir(indent + 4);
    } else {
        std::cout << IND(indent + 4) << "NULL\n";
    }

    std::cout << IND(indent + 2) << "Direita:\n";

    if (direita) {
        direita->imprimir(indent + 4);
    } else {
        std::cout << IND(indent + 4) << "NULL\n";
    }
}