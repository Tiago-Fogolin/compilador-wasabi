#include "AstBase.hpp";

#define IND(n) std::string(n, ' ')

// EXPRESSÕES
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

// DECLARAÇÕES
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
    for (auto& s : corpo) s->imprimir(i+2);
}

DeclaracaoStruct::DeclaracaoStruct(std::string n, std::string imp, std::vector<std::shared_ptr<NodoDeclaracao>> a, std::vector<std::shared_ptr<DeclaracaoFuncao>> m)
: nome(std::move(n)), implementa(std::move(imp)), atributos(std::move(a)), metodos(std::move(m)) {}
void DeclaracaoStruct::imprimir(int i) const {
    std::cout << IND(i) << "Struct(" << nome << ")\n";
    for (auto& a : atributos) a->imprimir(i+2);
    for (auto& m : metodos) m->imprimir(i+2);
}

DeclaracaoInterface::DeclaracaoInterface(std::string n, std::vector<std::shared_ptr<DeclaracaoFuncao>> m)
: nome(std::move(n)), metodos(std::move(m)) {}
void DeclaracaoInterface::imprimir(int i) const {
    std::cout << IND(i) << "Interface(" << nome << ")\n";
    for (auto& m : metodos) m->imprimir(i+2);
}

// COMANDOS
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
    condicao->imprimir(i+2);
    for(auto& c : blocoIf) c->imprimir(i+4);
    if(!blocoElse.empty()) {
        std::cout << IND(i) << "Else\n";
        for(auto& c : blocoElse) c->imprimir(i+4);
    }
}
