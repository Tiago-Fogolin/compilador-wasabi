
#include <iostream>
#include <algorithm>
#include "semantic/AnalisadorSemantico.hpp"
// ========================================================
// Construtor
// ========================================================
AnalisadorSemantico::AnalisadorSemantico() {
    tabela = TabelaSimbolos{};
}

// ========================================================
// Função principal
// ========================================================
void AnalisadorSemantico::analisar(const std::shared_ptr<NodoPrograma>& programa) {
    erros.clear();
    tabela = TabelaSimbolos{};

    primeiraPassadaColetar(programa);
    segundaPassadaAnalisar(programa);
}

// ========================================================
// PRIMEIRA PASSADA — Registrar símbolos
// ========================================================
void AnalisadorSemantico::primeiraPassadaColetar(const std::shared_ptr<NodoPrograma>& prog) {
    for (auto& d : prog->declaracoes) {
        if (!d) continue;

        if (auto df = std::dynamic_pointer_cast<DeclaracaoFuncao>(d)) {
            coletarAssinaturaFuncao(df);
        }
        else if (auto di = std::dynamic_pointer_cast<DeclaracaoInterface>(d)) {
            analisarDeclaracaoInterface(di);
        }
        else if (auto ds = std::dynamic_pointer_cast<DeclaracaoStruct>(d)) {
            analisarDeclaracaoStruct(ds);
        }
        else if (auto dv = std::dynamic_pointer_cast<DeclaracaoVariavel>(d)) {
            tabela.declarar(dv->nome, dv->tipo, dv->posicao.linha, dv->inicializador != nullptr);
        }
    }
}

// ========================================================
// SEGUNDA PASSADA — Analisar semântica
// ========================================================
void AnalisadorSemantico::segundaPassadaAnalisar(const std::shared_ptr<NodoPrograma>& prog) {
    for (auto& d : prog->declaracoes) {
        analisarDeclaracao(d);
    }

    for (auto& cmd : prog->comandos) {
        analisarComando(cmd);
    }
}

// ========================================================
// Declaracoes
// ========================================================
void AnalisadorSemantico::analisarDeclaracao(const std::shared_ptr<NodoDeclaracao>& decl) {
    if (!decl) return;

    if (auto dv = std::dynamic_pointer_cast<DeclaracaoVariavel>(decl))
        analisarDeclaracaoVariavel(dv);

    else if (auto df = std::dynamic_pointer_cast<DeclaracaoFuncao>(decl))
        analisarDeclaracaoFuncao(df);

    else if (auto ds = std::dynamic_pointer_cast<DeclaracaoStruct>(decl))
        validarImplementacoes(ds);
}

void AnalisadorSemantico::analisarDeclaracaoVariavel(const std::shared_ptr<DeclaracaoVariavel>& dv) {
    if (!dv->inicializador) return;

    std::string tipoInit = inferirTipoExpressao(dv->inicializador);

    if (!tiposCompativeis(dv->tipo, tipoInit)) {
        reportarErro("Tipo incompativel na inicializacao de variável '" + dv->nome +
                     "': esperado " + dv->tipo + ", recebido " + tipoInit, dv->posicao.linha);
    }
}

void AnalisadorSemantico::coletarAssinaturaFuncao(const std::shared_ptr<DeclaracaoFuncao>& df) {
    std::string sig = montarAssinaturaFuncao(*df);
    tabela.declarar(df->nome, sig, df->posicao.linha, true);
}

void AnalisadorSemantico::analisarDeclaracaoFuncao(const std::shared_ptr<DeclaracaoFuncao>& df) {
    tabela.entrarEscopo();

    std::string tipoRetornoAnterior = tipoRetornoAtual;
    tipoRetornoAtual = df->tipoRetorno;

    for (auto& p : df->parametros)
        tabela.declarar(p.first, p.second, df->posicao.linha, true);

    for (auto& cmd : df->corpo)
        analisarComando(cmd);

    tabela.sairEscopo();
    tipoRetornoAtual = tipoRetornoAnterior;
}

void AnalisadorSemantico::analisarDeclaracaoStruct(const std::shared_ptr<DeclaracaoStruct>& ds) {
    SimboloStruct s;
    s.nome = ds->nome;
    s.atributos = extrairAtributosStruct(ds);
    s.metodos = ds->metodos;
    s.implements = ds->implementa;
    s.linha = ds->posicao.linha;

    tabela.registrarStruct(s);
}

void AnalisadorSemantico::analisarDeclaracaoInterface(const std::shared_ptr<DeclaracaoInterface>& di) {
    SimboloStruct interf;
    interf.nome = di->nome;
    interf.metodos = di->metodos;
    interf.eInterface = true;

    tabela.registrarStruct(interf);
}

// ========================================================
// COMANDOS
// ========================================================
void AnalisadorSemantico::analisarComando(const std::shared_ptr<NodoAST>& nodo) {
    if (!nodo) return;

    if (auto ret = std::dynamic_pointer_cast<ComandoRetorno>(nodo))
        analisarComandoRetorno(ret);

    else if (auto at = std::dynamic_pointer_cast<ComandoAtribuicao>(nodo))
        analisarComandoAtribuicao(at);

    else if (auto ci = std::dynamic_pointer_cast<ComandoIdent>(nodo))
        analisarComandoIdent(ci);

    else if (auto cd = std::dynamic_pointer_cast<ComandoDeclaracao>(nodo))
        analisarComandoDeclaracao(cd);

    else if (auto cc = std::dynamic_pointer_cast<ComandoCondicional>(nodo))
        analisarComandoCondicional(cc);

    else if (auto lc = std::dynamic_pointer_cast<ComandoLaco>(nodo))
        analisarComandoLaco(lc);
}

void AnalisadorSemantico::analisarComandoRetorno(const std::shared_ptr<ComandoRetorno>& ret) {
    std::string tipoRetornado = inferirTipoExpressao(ret->expressao);

    std::string tipoEsperado = tipoRetornoAtual;

    if (!tiposCompativeis(tipoEsperado, tipoRetornado)) {
        reportarErro("Tipo de retorno incompativel. Esperado '" + tipoEsperado +
                        "', mas a expressao retorna '" + tipoRetornado + "'",
                        ret->posicao.linha);
    }
}

void AnalisadorSemantico::analisarComandoAtribuicao(const std::shared_ptr<ComandoAtribuicao>& at) {
    auto sim = tabela.buscar(at->nome);
    if (!sim) {
        reportarErro("Variavel nao declarada: " + at->nome, at->posicao.linha);
        return;
    }
    sim->utilizado = true;
    std::string tipoVar = sim->tipo;
    std::string tipoExpr = inferirTipoExpressao(at->expressao);

    if (!tiposCompativeis(tipoVar, tipoExpr)) {
        reportarErro("Atribuicao incompativel: variavel '" + at->nome +
                     "' eh do tipo " + tipoVar + " mas recebeu " + tipoExpr, at->posicao.linha);
    }
}

std::string AnalisadorSemantico::inferirTipoArrayLiteral(const std::shared_ptr<ExpressaoArrayLiteral>& arr) {
    if (arr->elementos.empty()) {
        return "unknown[]";
    }

    std::string tipoBase = inferirTipoExpressao(arr->elementos[0]);

    if (tipoBase == "unknown" || tipoBase == "error") {
        return "error[]";
    }

    for (size_t i = 1; i < arr->elementos.size(); ++i) {
        std::string tipoAtual = inferirTipoExpressao(arr->elementos[i]);


        if (tipoAtual != tipoBase) {
            reportarErro("Tipo incompativel no Array Literal. Esperado '" +
                         tipoBase + "' (baseado no primeiro elemento), mas encontrado '" +
                         tipoAtual + "' na posicao " + std::to_string(i),
                         arr->elementos[i]->posicao.linha);
            return "error[]";
        }
    }

    return tipoBase + "[]";
}

void AnalisadorSemantico::analisarComandoIdent(const std::shared_ptr<ComandoIdent>& ci) {
    std::string tipo = inferirTipoExpressao(ci->expressao);
}

void AnalisadorSemantico::analisarComandoDeclaracao(const std::shared_ptr<ComandoDeclaracao>& cd) {

    auto decl = cd->declaracao;

    if (auto dv = std::dynamic_pointer_cast<DeclaracaoVariavel>(decl)) {

        tabela.declarar(dv->nome,
                        dv->tipo,
                        dv->posicao.linha,
                        dv->inicializador != nullptr);

        if (dv->inicializador) {
            std::string tipoInit = inferirTipoExpressao(dv->inicializador);
            if (!tiposCompativeis(dv->tipo, tipoInit)) {
                reportarErro(
                    "Tipo incompativel na declaracao da variavel '" +
                    dv->nome + "'",
                    dv->posicao.linha
                );
            }
        }

        return;
    }

    if (auto df = std::dynamic_pointer_cast<DeclaracaoFuncao>(decl)) {
        analisarDeclaracaoFuncao(df);
        return;
    }

    if (auto ds = std::dynamic_pointer_cast<DeclaracaoStruct>(decl)) {
        analisarDeclaracaoStruct(ds);
        return;
    }

    if (auto di = std::dynamic_pointer_cast<DeclaracaoInterface>(decl)) {
        analisarDeclaracaoInterface(di);
        return;
    }

    reportarErro("Tipo de declaracao invalida dentro de comando.", cd->posicao.linha);
}

void AnalisadorSemantico::analisarComandoCondicional(const std::shared_ptr<ComandoCondicional>& cc) {
    inferirTipoExpressao(cc->condicao);

    for (auto& cmd : cc->blocoIf) {
        analisarComando(cmd);
    }

    for (auto& cmd : cc->blocoElse) {
        analisarComando(cmd);
    }
}
void AnalisadorSemantico::analisarComandoForWhile(const std::shared_ptr<ComandoLaco>& lc) {

    tabela.entrarEscopo();
    analisarComando(lc->inicializacao);

    std::string tipoCondicao = inferirTipoExpressao(lc->condicao);
    if (tipoCondicao != "bool" && tipoCondicao != "int") {
        reportarErro("A condicao do laco deve ser um valor booleano ou numerico, recebido " +
                     tipoCondicao, lc->posicao.linha);
    }

    for (auto& c : lc->corpo) {
        if (auto cmd = std::dynamic_pointer_cast<NodoComando>(c)) {
            analisarComando(cmd);
        }
    }

    analisarComando(lc->incremento);

    tabela.sairEscopo();
}

void AnalisadorSemantico::analisarComandoLaco(const std::shared_ptr<ComandoLaco>& lc) {
    switch (lc->tipo) {
        case ComandoLaco::TipoLaco::FOR:
        case ComandoLaco::TipoLaco::WHILE:
            analisarComandoForWhile(lc);
            break;

        case ComandoLaco::TipoLaco::FOREACH:
            analisarComandoForEach(lc);
            break;
    }
}



void AnalisadorSemantico::analisarComandoForEach(const std::shared_ptr<ComandoLaco>& lc) {
    tabela.entrarEscopo();

    std::string tipoColecao = inferirTipoExpressao(lc->expressaoForeach);

    if (tipoColecao.size() < 2 || tipoColecao.substr(tipoColecao.length() - 2) != "[]") {
        reportarErro("O laco 'foreach' deve iterar sobre uma colecao (Array), mas a expressao eh do tipo " +
                     tipoColecao, lc->posicao.linha);
        tabela.sairEscopo();
        return;
    }


    std::string tipoBase = tipoColecao.substr(0, tipoColecao.length() - 2);

    std::string nomeVariavel = lc->identificadorForeach;

    if (!tabela.declarar(nomeVariavel, tipoBase, lc->posicao.linha, true)) {
        reportarErro("Erro interno: A variavel de iteracao '" + nomeVariavel + "' nao deveria estar declarada.", lc->posicao.linha);
    }

    for (auto& c : lc->corpo) {
        if (auto cmd = std::dynamic_pointer_cast<NodoComando>(c)) {
            analisarComando(cmd);
        }
    }

    tabela.sairEscopo();
}

// ========================================================
// EXPRESSÕES
// ========================================================
std::string AnalisadorSemantico::inferirTipoExpressao(const std::shared_ptr<NodoExpressao>& expr) {
    if (!expr) return "unknown";

    if (auto lit = std::dynamic_pointer_cast<ExpressaoLiteral>(expr))
        return inferirTipoLiteral(lit);

    if (auto id = std::dynamic_pointer_cast<ExpressaoIdentificador>(expr))
        return inferirTipoIdentificador(id);

    if (auto b = std::dynamic_pointer_cast<ExpressaoBinaria>(expr))
        return inferirTipoBinaria(b);

    if (auto r = std::dynamic_pointer_cast<ExpressaoRelacional>(expr))
        return inferirTipoRelacional(r);

    if (auto ch = std::dynamic_pointer_cast<ExpressaoChamada>(expr))
        return inferirTipoChamada(ch);

    if (auto arrayLiteral = std::dynamic_pointer_cast<ExpressaoArrayLiteral>(expr))
        return inferirTipoArrayLiteral(arrayLiteral);


    return "unknown";
}

std::string AnalisadorSemantico::inferirTipoIdentificador(const std::shared_ptr<ExpressaoIdentificador>& id) {
    auto sim = tabela.buscar(id->nome);
    if (!sim) return "unknown";
    sim->utilizado = true;
    return sim->tipo;
}

std::string AnalisadorSemantico::inferirTipoLiteral(const std::shared_ptr<ExpressaoLiteral>& lit) {
    return lit->tipo;
}

std::string AnalisadorSemantico::inferirTipoRelacional(const std::shared_ptr<ExpressaoRelacional>& r) {
    return "bool";
}

std::string AnalisadorSemantico::inferirTipoBinaria(const std::shared_ptr<ExpressaoBinaria>& b) {
    std::string tipoEsquerda = inferirTipoExpressao(b->esquerda);
    std::string tipoDireita = inferirTipoExpressao(b->direita);

    if (tipoEsquerda == "unknown" || tipoDireita == "unknown") {
        return "unknown";
    }

    if (b->operador == "+" || b->operador == "-" || b->operador == "*" || b->operador == "/") {

        bool esqNum = tipoNumerico(tipoEsquerda);
        bool dirNum = tipoNumerico(tipoDireita);

        if (esqNum && dirNum) {
            if (tipoEsquerda == "float" || tipoDireita == "float") {
                return "float";
            }
            return "int";
        }

        if (b->operador == "+" && tipoEsquerda == "string" && tipoDireita == "string") {
            return "string";
        }

        reportarErro("Operacao ('" + b->operador + "') inválida entre os tipos " +
                     tipoEsquerda + " e " + tipoDireita,
                     b->posicao.linha);
        return "unknown";
    }

    return "unknown";
}

// ========================================================
// Chamada (função, método ou construtor)
// ========================================================
std::string AnalisadorSemantico::inferirTipoChamada(const std::shared_ptr<ExpressaoChamada>& ch) {


    std::string nome = ch->nome;

    // Caso 1: chamada de método → obj.metodo(...)
    if (nome.find('.') != std::string::npos) {
        size_t pos = nome.find('.');
        std::string objeto = nome.substr(0, pos);
        std::string metodo = nome.substr(pos + 1);

        auto objSim = tabela.buscar(objeto);
        if (!objSim) {
            reportarErro("Objeto nao declarado: " + objeto, ch->posicao.linha);
            return "unknown";
        }

        std::string tipoObj = objSim->tipo;
        auto s = tabela.buscarStruct(tipoObj);

        if (!s) {
            reportarErro("Objeto '" + objeto + "' nao eh um struct valido", ch->posicao.linha);
            return "unknown";
        }

        // procurar método
        for (auto& m : s->metodos) {
            if (m->nome == metodo) {
                return m->tipoRetorno;
            }
        }

        reportarErro("Metodo '" + metodo + "' nao existe em struct " + tipoObj, ch->posicao.linha);
        return "unknown";
    }

    // Caso 2: construtor → Pessoa(...)
    auto s = tabela.buscarStruct(nome);
    if (s)
        return nome;

    // Caso 3: função normal
    auto sim = tabela.buscar(nome);
    if (!sim) {
        reportarErro("Funcao nao declarada: " + nome, ch->posicao.linha);
        return "unknown";
    }

    std::string tipoRetorno = extrairTipoRetornoDaAssinatura(sim->tipo);
    return tipoRetorno;
}

std::string AnalisadorSemantico::extrairTipoRetornoDaAssinatura(const std::string& assinatura) {
    size_t pos_dois_pontos = assinatura.find_last_of(':');

    if (pos_dois_pontos != std::string::npos) {
        return assinatura.substr(pos_dois_pontos + 1);
    }

    return "void";
}

// ========================================================
// Struct - auxiliar
// ========================================================
std::vector<std::tuple<std::string, std::string, bool>>
AnalisadorSemantico::extrairAtributosStruct(const std::shared_ptr<DeclaracaoStruct>& ds) const
{
    std::vector<std::tuple<std::string, std::string, bool>> res;

    for (auto& a : ds->atributos) {
        if (auto dv = std::dynamic_pointer_cast<DeclaracaoVariavel>(a)) {
            bool init = (dv->inicializador != nullptr);
            res.emplace_back(dv->nome, dv->tipo, init);
        }
    }

    return res;
}

// ========================================================
// Interfaces
// ========================================================
void AnalisadorSemantico::validarImplementacoes(const std::shared_ptr<DeclaracaoStruct>& ds) {
    for (auto& nomeInterf : ds->implementa) {
        auto interf = tabela.buscarStruct(nomeInterf);
        if (!interf) {
            reportarErro("Interface nao encontrada: " + nomeInterf, ds->posicao.linha);
            continue;
        }

        for (auto& metodoReq : interf->metodos) {
            bool encontrado = false;

            for (auto& m : ds->metodos) {
                if (m->nome == metodoReq->nome) {
                    encontrado = true;
                    break;
                }
            }

            if (!encontrado) {
                reportarErro("Struct " + ds->nome + " nao implementa metodo obrigatorio: " +
                             metodoReq->nome, ds->posicao.linha);
            }
        }
    }
}

// ========================================================
// Tipos
// ========================================================
// Em AnalisadorSemantico.cpp
bool AnalisadorSemantico::tiposCompativeis(const std::string& esperado, const std::string& recebido) const {
    if (esperado == recebido) {
        return true;
    }

    if (esperado == "float" && recebido == "int") {
        return true;
    }

    if (recebido == "null") {
        if (esperado == "int" || esperado == "float" || esperado == "bool" || esperado == "char") {
             return false;
        }
        return true;
    }


    if (esperado == "string" && (recebido == "int" || recebido == "float")) {
        return false;
    }

    return false;
}


bool AnalisadorSemantico::tipoNumerico(const std::string& t) const {
    return t == "int" || t == "float";
}

// ========================================================
// Assinaturas
// ========================================================
std::string AnalisadorSemantico::montarAssinaturaFuncao(const DeclaracaoFuncao& df) {
    std::string s = df.nome + "(";
    for (auto& p : df.parametros) {
        s += p.second + ",";
    }
    s += ")";
    if (!df.tipoRetorno.empty())
        s += ":" + df.tipoRetorno;
    return s;
}

bool AnalisadorSemantico::parseAssinaturaFuncao(
    const std::string& sig,
    std::vector<std::string>& outParams,
    std::string& outRet)
{
    return false;
}

// ========================================================
// Erros
// ========================================================
void AnalisadorSemantico::reportarErro(const std::string& msg, int linha) {
    erros.emplace_back(msg, linha);
}
