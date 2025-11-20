
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
        reportarErro("Tipo incompatível na inicialização de variável '" + dv->nome +
                     "': esperado " + dv->tipo + ", recebido " + tipoInit, dv->posicao.linha);
    }
}

void AnalisadorSemantico::coletarAssinaturaFuncao(const std::shared_ptr<DeclaracaoFuncao>& df) {
    std::string sig = montarAssinaturaFuncao(*df);
    tabela.declarar(df->nome, sig, df->posicao.linha, true);
}

void AnalisadorSemantico::analisarDeclaracaoFuncao(const std::shared_ptr<DeclaracaoFuncao>& df) {
    tabela.entrarEscopo();

    for (auto& p : df->parametros)
        tabela.declarar(p.first, p.second, df->posicao.linha, true);

    for (auto& cmd : df->corpo)
        analisarComando(cmd);

    tabela.sairEscopo();
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

void AnalisadorSemantico::analisarComandoRetorno(const std::shared_ptr<ComandoRetorno>& ret) {}

void AnalisadorSemantico::analisarComandoAtribuicao(const std::shared_ptr<ComandoAtribuicao>& at) {
    auto sim = tabela.buscar(at->nome);
    if (!sim) {
        reportarErro("Variável não declarada: " + at->nome, at->posicao.linha);
        return;
    }

    std::string tipoVar = sim->tipo;
    std::string tipoExpr = inferirTipoExpressao(at->expressao);

    if (!tiposCompativeis(tipoVar, tipoExpr)) {
        reportarErro("Atribuição incompatível: variável '" + at->nome +
                     "' é do tipo " + tipoVar + " mas recebeu " + tipoExpr, at->posicao.linha);
    }
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
                    "Tipo incompatível na declaração da variável '" +
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

    reportarErro("Tipo de declaração inválida dentro de comando.", cd->posicao.linha);
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

void AnalisadorSemantico::analisarComandoLaco(const std::shared_ptr<ComandoLaco>& lc) {
    inferirTipoExpressao(lc->condicao);
    for (auto& c : lc->corpo) analisarComando(c);
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

    return "unknown";
}

std::string AnalisadorSemantico::inferirTipoIdentificador(const std::shared_ptr<ExpressaoIdentificador>& id) {
    auto sim = tabela.buscar(id->nome);
    if (!sim) return "unknown";
    return sim->tipo;
}

std::string AnalisadorSemantico::inferirTipoLiteral(const std::shared_ptr<ExpressaoLiteral>& lit) {
    return lit->tipo;
}

std::string AnalisadorSemantico::inferirTipoRelacional(const std::shared_ptr<ExpressaoRelacional>& r) {
    return "bool";
}

std::string AnalisadorSemantico::inferirTipoBinaria(const std::shared_ptr<ExpressaoBinaria>& b) {
    return inferirTipoExpressao(b->esquerda);
}

// ========================================================
// Chamada (função, método ou construtor)
// ========================================================
std::string AnalisadorSemantico::inferirTipoChamada(const std::shared_ptr<ExpressaoChamada>& ch) {
    auto alvoId = std::dynamic_pointer_cast<ExpressaoIdentificador>(ch->alvo);
    if (!alvoId) return "unknown";

    std::string nome = alvoId->nome;

    // Caso 1: chamada de método → obj.metodo(...)
    if (nome.find('.') != std::string::npos) {
        size_t pos = nome.find('.');
        std::string objeto = nome.substr(0, pos);
        std::string metodo = nome.substr(pos + 1);

        auto objSim = tabela.buscar(objeto);
        if (!objSim) {
            reportarErro("Objeto não declarado: " + objeto, alvoId->linha);
            return "unknown";
        }

        std::string tipoObj = objSim->tipo;
        auto s = tabela.buscarStruct(tipoObj);

        if (!s) {
            reportarErro("Objeto '" + objeto + "' não é um struct válido", alvoId->linha);
            return "unknown";
        }

        // procurar método
        for (auto& m : s->metodos) {
            if (m->nome == metodo) {
                return m->tipoRetorno;
            }
        }

        reportarErro("Método '" + metodo + "' não existe em struct " + tipoObj, alvoId->linha);
        return "unknown";
    }

    // Caso 2: construtor → Pessoa(...)
    auto s = tabela.buscarStruct(nome);
    if (s)
        return nome;

    // Caso 3: função normal
    auto sim = tabela.buscar(nome);
    if (!sim)
        reportarErro("Função não declarada: " + nome, alvoId->linha);

    return sim ? sim->tipo : "unknown";
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
            bool init = (dv->expressaoInicial != nullptr);
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
            reportarErro("Interface não encontrada: " + nomeInterf, ds->linhaDecl);
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
                reportarErro("Struct " + ds->nome + " não implementa método obrigatório: " +
                             metodoReq->nome, ds->linhaDecl);
            }
        }
    }
}

// ========================================================
// Tipos
// ========================================================
bool AnalisadorSemantico::tiposCompativeis(const std::string& a, const std::string& b) const {
    return a == b;
}

bool AnalisadorSemantico::tipoNumerico(const std::string& t) const {
    return t == "int";
}

bool AnalisadorSemantico::todosOsCaminhosRetornam(const std::vector<std::shared_ptr<NodoAST>>& corpo) const {
    return true;
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
