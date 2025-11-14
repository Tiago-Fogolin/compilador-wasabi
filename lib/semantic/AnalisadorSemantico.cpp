#include <semantic/AnalisadorSemantico.hpp>
#include <iostream>
#include <sstream>
#include <algorithm>

std::string ErroSemantico::toString() const {
    return "Erro semantico na linha " + std::to_string(linha) + ": " + mensagem;
}

AnalisadorSemantico::AnalisadorSemantico() = default;

void AnalisadorSemantico::analisar(const std::shared_ptr<NodoPrograma>& programa) {
    erros.clear();
    try {
        primeiraPassadaColetar(programa);
        segundaPassadaAnalisar(programa);
    } catch (const std::exception& e) {
        reportarErro(std::string("Erro fatal durante analise: ") + e.what(), 0);
    }

    if (!erros.empty()) {
        std::cout << "\n=== Erros Semanticos Encontrados ===\n";
        for (const auto& e : erros) std::cout << e.toString() << "\n";
    } else {
        std::cout << "\nAnalise semantica concluida sem erros\n";
    }
}

void AnalisadorSemantico::primeiraPassadaColetar(const std::shared_ptr<NodoPrograma>& prog) {
    for (const auto& decl : prog->declaracoes) {
        if (auto f = std::dynamic_pointer_cast<DeclaracaoFuncao>(decl)) {
            coletarAssinaturaFuncao(f);
        }
    }
}

void AnalisadorSemantico::coletarAssinaturaFuncao(const std::shared_ptr<DeclaracaoFuncao>& df) {
    std::string sig = montarAssinaturaFuncao(*df);
    if (!tabela.declarar(df->nome, sig, 0)) {
        reportarErro("Funcao " + df->nome + " ja foi declarada", df->corpo.empty() ? 0 : 0);
    }
}

// -------------------------------------------------
// 2a passada
// -------------------------------------------------
void AnalisadorSemantico::segundaPassadaAnalisar(const std::shared_ptr<NodoPrograma>& prog) {
    for (const auto& decl : prog->declaracoes) {
        analisarDeclaracao(decl);
    }

    for (const auto& cmd : prog->comandos) {
        analisarComando(cmd);
    }
}

void AnalisadorSemantico::analisarDeclaracao(const std::shared_ptr<NodoDeclaracao>& decl) {
    if (auto dv = std::dynamic_pointer_cast<DeclaracaoVariavel>(decl)) {
        analisarDeclaracaoVariavel(dv);
    } else if (auto df = std::dynamic_pointer_cast<DeclaracaoFuncao>(decl)) {
        analisarDeclaracaoFuncao(df);
    } else if (auto ds = std::dynamic_pointer_cast<DeclaracaoStruct>(decl)) {
        // TODO
    } else if (auto di = std::dynamic_pointer_cast<DeclaracaoInterface>(decl)) {
        // TODO
    }
}

void AnalisadorSemantico::analisarDeclaracaoVariavel(const std::shared_ptr<DeclaracaoVariavel>& dv) {
    std::string tipoInicial = "erro";
    if (dv->inicializador) {
        tipoInicial = inferirTipoExpressao(dv->inicializador);
    }

    std::string tipoDeclarado = dv->tipo.empty() ? tipoInicial : dv->tipo;

    if (tipoInicial != "erro" && !dv->tipo.empty()) {
        if (!tiposCompativeis(tipoDeclarado, tipoInicial)) {
            reportarErro(
                "Tipo de inicializador (" + tipoInicial +
                ") incompativel com tipo declarado (" + tipoDeclarado + ")",
                dv->inicializador->posicao.linha 
            );
        }
    }

    bool ok = tabela.declarar(dv->nome, tipoDeclarado, 0, dv->inicializador != nullptr);
    if (!ok) {
        reportarErro("Variavel " + dv->nome + " ja foi declarada neste escopo", 0);
    }
}

void AnalisadorSemantico::analisarDeclaracaoFuncao(const std::shared_ptr<DeclaracaoFuncao>& df) {
    tabela.entrarEscopo();

    for (const auto& p : df->parametros) {
        if (!tabela.declarar(p.first, p.second, 0, true)) {
            reportarErro("Parametro " + p.first + " ja declarado", 0);
        }
    }

    for (const auto& stmt : df->corpo) {
        analisarComando(stmt);
    }

    if (!todosOsCaminhosRetornam(df->corpo) && df->tipoRetorno != "void") {
        reportarErro("Funcao " + df->nome + " nao retorna valor em todos os caminhos", 0);
    }

    tabela.sairEscopo();
}

// -------------------------------------------------
// Comandos
// -------------------------------------------------
void AnalisadorSemantico::analisarComando(const std::shared_ptr<NodoAST>& nodo) {
    if (auto r = std::dynamic_pointer_cast<ComandoRetorno>(nodo)) return analisarComandoRetorno(r);
    if (auto a = std::dynamic_pointer_cast<ComandoAtribuicao>(nodo)) return analisarComandoAtribuicao(a);
    if (auto ci = std::dynamic_pointer_cast<ComandoIdent>(nodo)) return analisarComandoIdent(ci);
    if (auto cd = std::dynamic_pointer_cast<ComandoDeclaracao>(nodo)) return analisarComandoDeclaracao(cd);
    if (auto cc = std::dynamic_pointer_cast<ComandoCondicional>(nodo)) return analisarComandoCondicional(cc);
    if (auto lc = std::dynamic_pointer_cast<ComandoLaco>(nodo)) return analisarComandoLaco(lc);
}

void AnalisadorSemantico::analisarComandoRetorno(const std::shared_ptr<ComandoRetorno>& ret) {
    if (ret->expressao) (void)inferirTipoExpressao(ret->expressao);
}

void AnalisadorSemantico::analisarComandoAtribuicao(const std::shared_ptr<ComandoAtribuicao>& at) {
    auto s = tabela.buscar(at->nome);
    if (!s) {
        reportarErro("Atribuicao para variavel nao declarada: " + at->nome, 0);
        return;
    }

    std::string tipoExpr = inferirTipoExpressao(at->expressao);
    if (!tiposCompativeis(s->tipo, tipoExpr)) {
        reportarErro("Tipo incompativel na atribuicao: " + s->tipo + " <- " + tipoExpr, 0);
    }

    tabela.marcarInicializado(at->nome);
    tabela.marcarUtilizado(at->nome);
}

std::string AnalisadorSemantico::inferirTipoRelacional(const std::shared_ptr<ExpressaoRelacional>& r) {
    std::string te = inferirTipoExpressao(r->esquerda);
    std::string td = inferirTipoExpressao(r->direita);
    const std::string& op = r->operador;


    if (!tiposCompativeis(te, td)) {
        reportarErro("Operandos de comparacao incompativeis: " + te + " e " + td, 0);
    }

    return "bool";
}

void AnalisadorSemantico::analisarComandoIdent(const std::shared_ptr<ComandoIdent>& ci) {
    if (ci->argumentos.empty()) {
        auto s = tabela.buscar(ci->nome);
        if (!s) reportarErro("Identificador nao declarado: " + ci->nome, 0);
        else tabela.marcarUtilizado(ci->nome);
    } else {
        (void)inferirTipoChamada(std::make_shared<ExpressaoChamada>(ci->nome, ci->argumentos));
    }
}

void AnalisadorSemantico::analisarComandoDeclaracao(const std::shared_ptr<ComandoDeclaracao>& cd) {
    analisarDeclaracao(cd->declaracao);
}

void AnalisadorSemantico::analisarComandoCondicional(const std::shared_ptr<ComandoCondicional>& cc) {
    std::string tcond = inferirTipoExpressao(cc->condicao);
    if (tcond != "bool") {
        reportarErro("Condicao do if nao e booleano (encontrado: " + tcond + ")", 0);
    }

    tabela.entrarEscopo();
    for (auto& s : cc->blocoIf) analisarComando(s);
    tabela.sairEscopo();

    if (!cc->blocoElse.empty()) {
        tabela.entrarEscopo();
        for (auto& s : cc->blocoElse) analisarComando(s);
        tabela.sairEscopo();
    }
}

void AnalisadorSemantico::analisarComandoLaco(const std::shared_ptr<ComandoLaco>& lc) {
    if (lc->tipo == ComandoLaco::TipoLaco::FOR || lc->tipo == ComandoLaco::TipoLaco::WHILE) {
        if (lc->condicao) {
            std::string tcond = inferirTipoExpressao(lc->condicao);
            if (tcond != "bool") {
                reportarErro("Condicao de laco nao e bool (encontrado: " + tcond + ")", 0);
            }
        }

        tabela.entrarEscopo();
        if (lc->inicializacao) analisarComando(lc->inicializacao);
        if (lc->incremento) analisarComando(lc->incremento);
        for (auto& s : lc->corpo) analisarComando(s);
        tabela.sairEscopo();
    }

    else if (lc->tipo == ComandoLaco::TipoLaco::FOREACH) {
        if (lc->expressaoForeach) {
            std::string tcol = inferirTipoExpressao(lc->expressaoForeach);
            if (tcol.rfind("List<", 0) != 0) {
                reportarErro("Expressao do foreach nao e colecao (encontrado: " + tcol + ")", 0);
            }
        }
        tabela.entrarEscopo();
        for (auto& s : lc->corpo) analisarComando(s);
        tabela.sairEscopo();
    }
}

// -------------------------------------------------
// Expressoes
// -------------------------------------------------
std::string AnalisadorSemantico::inferirTipoExpressao(const std::shared_ptr<NodoExpressao>& expr) {
    if (!expr) return "erro";

    if (auto lit = std::dynamic_pointer_cast<ExpressaoLiteral>(expr)) return inferirTipoLiteral(lit);
    if (auto id = std::dynamic_pointer_cast<ExpressaoIdentificador>(expr)) return inferirTipoIdentificador(id);
    if (auto bin = std::dynamic_pointer_cast<ExpressaoBinaria>(expr)) return inferirTipoBinaria(bin);
    if (auto ch = std::dynamic_pointer_cast<ExpressaoChamada>(expr)) return inferirTipoChamada(ch);
    if (auto rel = std::dynamic_pointer_cast<ExpressaoRelacional>(expr)) return inferirTipoRelacional(rel);

    return "erro";
}

std::string AnalisadorSemantico::inferirTipoLiteral(const std::shared_ptr<ExpressaoLiteral>& lit) {
    const std::string& v = lit->valor;
    if (v.size() >= 2 && v.front() == '"' && v.back() == '"') return "string";
    if (v == "true" || v == "false") return "bool";
    if (v.find('.') != std::string::npos) return "float";
    return "int";
}

std::string AnalisadorSemantico::inferirTipoIdentificador(const std::shared_ptr<ExpressaoIdentificador>& id) {
    auto s = tabela.buscar(id->nome);
    if (!s) {
        reportarErro("Variavel " + id->nome + " nao foi declarada", 0);
        return "erro";
    }

    if (!s->inicializado) {
        reportarErro("Variavel " + id->nome + " pode estar sendo usada antes de inicializada", 0);
    }

    tabela.marcarUtilizado(id->nome);
    return s->tipo;
}

std::string AnalisadorSemantico::inferirTipoBinaria(const std::shared_ptr<ExpressaoBinaria>& b) {
    std::string te = inferirTipoExpressao(b->esquerda);
    std::string td = inferirTipoExpressao(b->direita);
    const std::string& op = b->operador;

    if (op == "+" || op == "-" || op == "*" || op == "/") {
        if (!tipoNumerico(te)) reportarErro("Operador " + op + " requer operando numerico esquerdo (encontrado: " + te + ")", 0);
        if (!tipoNumerico(td)) reportarErro("Operador " + op + " requer operando numerico direito (encontrado: " + td + ")", 0);
        if (!tiposCompativeis(te, td)) reportarErro("Operandos de " + op + " devem ter mesmo tipo (encontrado: " + te + " e " + td + ")", 0);
        return te;
    }

    if (op == "<" || op == "<=" || op == ">" || op == ">=") {
        if (!tiposCompativeis(te, td)) reportarErro("Operandos de comparacao incompativeis: " + te + " e " + td, 0);
        return "bool";
    }

    if (op == "==" || op == "!=") {
        if (!tiposCompativeis(te, td)) reportarErro("Operandos de igualdade devem ter mesmo tipo: " + te + " e " + td, 0);
        return "bool";
    }

    if (op == "&&" || op == "||") {
        if (te != "bool") reportarErro("Operador logico requer bool a esquerda (encontrado: " + te + ")", 0);
        if (td != "bool") reportarErro("Operador logico requer bool a direita (encontrado: " + td + ")", 0);
        return "bool";
    }

    reportarErro("Operador desconhecido: " + op, 0);
    return "erro";
}

std::string AnalisadorSemantico::inferirTipoChamada(const std::shared_ptr<ExpressaoChamada>& ch) {
    auto s = tabela.buscar(ch->nome);
    if (!s) {
        reportarErro("Chamada para funcao nao declarada: " + ch->nome, 0);
        return "erro";
    }

    std::vector<std::string> params;
    std::string ret;

    if (!parseAssinaturaFuncao(s->tipo, params, ret)) {
        reportarErro("Assinatura da funcao invalida: " + ch->nome, 0);
        return "erro";
    }

    if (params.size() != ch->argumentos.size()) {
        reportarErro(
            "Numero de argumentos invalido em " + ch->nome +
            " (esperado " + std::to_string(params.size()) +
            ", encontrado " + std::to_string(ch->argumentos.size()) + ")",
            0
        );
        return "erro";
    }

    for (size_t i = 0; i < params.size(); ++i) {
        std::string found = inferirTipoExpressao(ch->argumentos[i]);
        if (!tiposCompativeis(params[i], found)) {
            reportarErro(
                "Tipo do argumento " + std::to_string(i+1) +
                " em " + ch->nome + " incompativel (esperado " + params[i] +
                ", encontrado " + found + ")",
                0
            );
        }
    }

    return ret;
}

// -------------------------------------------------
// Helpers
// -------------------------------------------------
bool AnalisadorSemantico::tiposCompativeis(const std::string& a, const std::string& b) const {
    if (a == b) return true;
    if (a == "float" && b == "int") return true;
    if (b == "float" && a == "int") return true;
    if (a == "erro" || b == "erro") return true;
    return false;
}

bool AnalisadorSemantico::tipoNumerico(const std::string& t) const {
    return t == "int" || t == "float";
}

bool AnalisadorSemantico::todosOsCaminhosRetornam(const std::vector<std::shared_ptr<NodoAST>>& corpo) const {
    for (const auto& s : corpo) {
        if (std::dynamic_pointer_cast<ComandoRetorno>(s)) return true;

        if (auto cc = std::dynamic_pointer_cast<ComandoCondicional>(s)) {
            bool ifRet = todosOsCaminhosRetornam(cc->blocoIf);
            bool elseRet = !cc->blocoElse.empty() && todosOsCaminhosRetornam(cc->blocoElse);
            if (ifRet && elseRet) return true;
        }
    }
    return false;
}

std::string AnalisadorSemantico::montarAssinaturaFuncao(const DeclaracaoFuncao& df) {
    std::ostringstream oss;
    oss << "fn(";
    for (size_t i = 0; i < df.parametros.size(); ++i) {
        if (i) oss << ",";
        oss << df.parametros[i].second;
    }
    oss << ")->" << df.tipoRetorno;
    return oss.str();
}

bool AnalisadorSemantico::parseAssinaturaFuncao(const std::string& sig,
                                               std::vector<std::string>& outParams,
                                               std::string& outRet) {
    outParams.clear();
    outRet.clear();

    if (sig.rfind("fn(", 0) != 0) return false;

    auto p = sig.find(")->");
    if (p == std::string::npos) return false;

    std::string inside = sig.substr(3, p - 3);
    outRet = sig.substr(p + 3);

    if (inside.empty()) return true;

    std::istringstream iss(inside);
    std::string token;
    while (std::getline(iss, token, ',')) outParams.push_back(token);

    return true;
}

void AnalisadorSemantico::reportarErro(const std::string& msg, int linha) {
    erros.emplace_back(msg, linha);
}
