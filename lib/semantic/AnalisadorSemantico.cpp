
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
    int tamanho = -1;

    size_t posAbre = dv->tipo.find('[');
    size_t posFecha = dv->tipo.find(']');

    if (posAbre != std::string::npos && posFecha > posAbre + 1) {
        std::string tamStr = dv->tipo.substr(posAbre + 1, posFecha - posAbre - 1);
        try {
            if (std::all_of(tamStr.begin(), tamStr.end(), ::isdigit)) {
                tamanho = std::stoi(tamStr);
            }
        } catch (...) {}
    }

    if (dv->inicializador) {
        std::string tipoInit = inferirTipoExpressao(dv->inicializador);

        if (!tiposCompativeis(dv->tipo, tipoInit)) {
            reportarErro("Tipo incompativel na inicializacao de variável '" + dv->nome +
                         "': esperado " + dv->tipo + ", recebido " + tipoInit, dv->posicao.linha);
        }

        if (auto arrLit = std::dynamic_pointer_cast<ExpressaoArrayLiteral>(dv->inicializador)) {
            int tamanhoReal = (int)arrLit->elementos.size();

            if (tamanho != -1 && tamanhoReal > tamanho) {
                reportarErro("Tamanho do array excede a declaracao. Declarado: " + std::to_string(tamanho) +
                             ", Recebido: " + std::to_string(tamanhoReal), dv->posicao.linha);
            }
        }
        else if (auto strLit = std::dynamic_pointer_cast<ExpressaoLiteral>(dv->inicializador)) {
            if (strLit->tipo == "string") {
                if (strLit->valor.size() >= 2)
                    tamanho = strLit->valor.size() - 2;
                else
                    tamanho = strLit->valor.size();
            }
        }
    }


    tabela.declarar(dv->nome, dv->tipo, dv->posicao.linha, dv->inicializador != nullptr, tamanho);
}

void AnalisadorSemantico::coletarAssinaturaFuncao(const std::shared_ptr<DeclaracaoFuncao>& df) {
    std::string sig = montarAssinaturaFuncao(*df);
    tabela.declarar(df->nome, sig, df->posicao.linha, true);
}

void AnalisadorSemantico::analisarDeclaracaoFuncao(const std::shared_ptr<DeclaracaoFuncao>& df) {
    tabela.entrarEscopo();

    if (!structAtual.empty()) {
        tabela.declarar("this", structAtual, df->posicao.linha, true);
    }

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

    structAtual = ds->nome;

    for (auto& m : ds->metodos) {
        analisarDeclaracaoFuncao(m);
    }

    structAtual = "";
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

    else if (auto atArr = std::dynamic_pointer_cast<ComandoAtribuicaoArray>(nodo))
        analisarComandoAtribuicaoArray(atArr);

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
    if (at->nome.find('.') != std::string::npos) {
        size_t pos = at->nome.find('.');
        std::string objetoNome = at->nome.substr(0, pos);
        std::string membroNome = at->nome.substr(pos + 1);

        auto sim = tabela.buscar(objetoNome);
        if (!sim) {
            reportarErro("Objeto nao declarado: " + objetoNome, at->posicao.linha);
            return;
        }
        sim->utilizado = true;

        auto structDef = tabela.buscarStruct(sim->tipo);
        if (!structDef) {
            reportarErro("A variavel '" + objetoNome + "' nao eh uma struct.", at->posicao.linha);
            return;
        }

        std::string tipoCampo = "";
        bool campoEncontrado = false;

        for (const auto& attr : structDef->atributos) {
            if (std::get<0>(attr) == membroNome) {
                tipoCampo = std::get<1>(attr);
                campoEncontrado = true;
                break;
            }
        }

        if (!campoEncontrado) {
            reportarErro("A struct '" + sim->tipo + "' nao possui o campo '" + membroNome + "'", at->posicao.linha);
            return;
        }

        std::string tipoExpr = inferirTipoExpressao(at->expressao);
        if (!tiposCompativeis(tipoCampo, tipoExpr)) {
            reportarErro("Atribuicao incompativel no campo '" + membroNome +
                            "'. Esperado: " + tipoCampo + ", Recebido: " + tipoExpr, at->posicao.linha);
        }
        return;
    }

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
    std::string tipoInferido = tipoBase + "[]";
    arr->tipoInferido = tipoInferido;
    return tipoInferido;
}

void AnalisadorSemantico::analisarComandoIdent(const std::shared_ptr<ComandoIdent>& ci) {
    if (ci->nome != "print") {

        if (ci->nome.find('.') != std::string::npos) {
            size_t pos = ci->nome.find('.');
            std::string objeto = ci->nome.substr(0, pos);
            if (!tabela.buscar(objeto)) {
                 reportarErro("Objeto nao declarado: " + objeto, ci->posicao.linha);
            }
        }
        else {
            auto sim = tabela.buscar(ci->nome);
            if (!sim) {
                reportarErro("Funcao nao declarada: " + ci->nome, ci->posicao.linha);
            }
        }
    }

    for (auto &arg : ci->argumentos) {
        inferirTipoExpressao(arg);
    }
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
    if (tipoColecao == "string") {
        // Declara a variável de iteração como 'char'
        if (!tabela.declarar(lc->identificadorForeach, "char", lc->posicao.linha, true)) {
                reportarErro("Erro interno...", lc->posicao.linha);
        }

        for (auto& c : lc->corpo) {
            if (auto cmd = std::dynamic_pointer_cast<NodoComando>(c)) {
                analisarComando(cmd);
            }
        }
        tabela.sairEscopo();
        return;
    }

    size_t posBracket = tipoColecao.find('[');

    if (posBracket == std::string::npos) {
        reportarErro("O laco 'foreach' deve iterar sobre uma colecao (Array), mas a expressao eh do tipo " +
                        tipoColecao, lc->posicao.linha);
        tabela.sairEscopo();
        return;
    }

    std::string tipoBase = tipoColecao.substr(0, posBracket);

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

    if (auto membro = std::dynamic_pointer_cast<ExpressaoAcessoMembro>(expr)) {
        return inferirTipoAcessoMembro(membro);
    }
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
    if (auto un = std::dynamic_pointer_cast<ExpressaoUnaria>(expr)) {
        std::string tipoOp = inferirTipoExpressao(un->operando);
        un->tipoInferido = tipoOp;
        return tipoOp;
    }

    if (auto acc = std::dynamic_pointer_cast<ExpressaoAcessoArray>(expr)) {

        std::string tipoBaseArr = inferirTipoExpressao(acc->arrayBase);
        std::string tipoIndice = inferirTipoExpressao(acc->indice);

        if (tipoIndice != "int") {
            reportarErro("O indice de acesso deve ser do tipo int, recebido: " + tipoIndice, acc->posicao.linha);
            acc->tipoInferido = "error";
            return "error";
        }

        if (auto un = std::dynamic_pointer_cast<ExpressaoUnaria>(acc->indice)) {
            if (un->operador == "-") {
                if (std::dynamic_pointer_cast<ExpressaoLiteral>(un->operando)) {
                    reportarErro("Indice de array nao pode ser negativo.", acc->posicao.linha);
                }
            }
        }

        if (auto litIdx = std::dynamic_pointer_cast<ExpressaoLiteral>(acc->indice)) {
            if (litIdx->tipo == "int") {
                try {
                    int idxVal = std::stoi(litIdx->valor);

                    if (auto idArr = std::dynamic_pointer_cast<ExpressaoIdentificador>(acc->arrayBase)) {
                        auto sim = tabela.buscar(idArr->nome);

                        if (sim && sim->tamanhoConhecido != -1) {
                            if (idxVal >= sim->tamanhoConhecido) {
                                reportarErro("Indice fora dos limites. O tamanho eh " + std::to_string(sim->tamanhoConhecido) +
                                                ", mas voce acessou o indice " + litIdx->valor, acc->posicao.linha);
                                acc->tipoInferido = "error";
                                return "error";
                            }
                        }
                    }
                } catch (...) {}
            }
        }

        // 4. Tipo de Retorno
        if (tipoBaseArr == "string") {
            acc->tipoInferido = "char";
            return "char";
        }

        size_t posBracket = tipoBaseArr.find('[');
        if (posBracket != std::string::npos) {
            acc->tipoInferido = tipoBaseArr.substr(0, posBracket);
            return acc->tipoInferido;
        }

        reportarErro("Acesso por indice aplicado a algo que nao eh array nem string: " + tipoBaseArr, acc->posicao.linha);
        acc->tipoInferido = "error";
        return "error";
    }

    if (auto lg = std::dynamic_pointer_cast<ExpressaoLogica>(expr)) {
        inferirTipoExpressao(lg->esquerda);
        inferirTipoExpressao(lg->direita);
        lg->tipoInferido = "bool";
        return "bool";
    }

    return "unknown";
}

std::string AnalisadorSemantico::inferirTipoIdentificador(const std::shared_ptr<ExpressaoIdentificador>& id) {
    auto sim = tabela.buscar(id->nome);
    if (!sim) {
        reportarErro("Variavel nao declarada: " + id->nome, id->posicao.linha);
        id->tipoInferido = "error";
        return "error";
    }

    sim->utilizado = true;

    id->tipoInferido = sim->tipo;
    return sim->tipo;
}

std::string AnalisadorSemantico::inferirTipoLiteral(const std::shared_ptr<ExpressaoLiteral>& lit) {
    lit->tipoInferido = lit->tipo;
    return lit->tipo;
}

std::string AnalisadorSemantico::inferirTipoRelacional(const std::shared_ptr<ExpressaoRelacional>& r) {
    std::string tipoEsq = inferirTipoExpressao(r->esquerda);
    std::string tipoDir = inferirTipoExpressao(r->direita);

    if (tipoEsq == "error" || tipoDir == "error" || tipoEsq == "unknown" || tipoDir == "unknown") {
        r->tipoInferido = "bool";
        return "bool";
    }

    bool esqNum = tipoNumerico(tipoEsq);
    bool dirNum = tipoNumerico(tipoDir);


    if (esqNum && dirNum) {
        r->tipoInferido = "bool";
        return "bool";
    }

    if (r->operador == "==" || r->operador == "!=") {
        if (tipoEsq == tipoDir) {
            r->tipoInferido = "bool";
            return "bool";
        }
        else if(tipoEsq == "null" || tipoDir == "null") {
            r->tipoInferido = "bool";
            return "bool";
        }
    }

    reportarErro("Operacao relacional ('" + r->operador + "') invalida entre " +
                 tipoEsq + " e " + tipoDir, r->posicao.linha);

    r->tipoInferido = "bool";
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
                b->tipoInferido = "float";
                return "float";
            }
            b->tipoInferido = "int";
            return "int";
        }

        if (b->operador == "+") {
            if (tipoEsquerda == "string" && tipoDireita == "string") {
                b->tipoInferido = "string";
                return "string";
            }
            if (tipoEsquerda == "string" && tipoDireita == "char") {
                b->tipoInferido = "string";
                return "string";
            }
            if (tipoEsquerda == "char" && tipoDireita == "string") {
                b->tipoInferido = "string";
                return "string";
            }
            reportarErro("Operacao ('" + b->operador + "') invalida entre os tipos " +
                            tipoEsquerda + " e " + tipoDireita,
                            b->posicao.linha);
            return "unknown";
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

    if (nome == "str") {
        if (ch->argumentos.size() != 1) {
            reportarErro("A funcao 'str' espera exatamente 1 argumento.", ch->posicao.linha);
            return "error";
        }

       std::string tipoArg = inferirTipoExpressao(ch->argumentos[0]);

        if (tipoArg != "int" && tipoArg != "float") {
            reportarErro("A funcao 'str' aceita apenas int ou float, recebido: " + tipoArg, ch->posicao.linha);
        }
        ch->tipoInferido = "string";
        return "string";
    }

    if (nome.find('.') != std::string::npos) {
        size_t pos = nome.find('.');
        std::string objeto = nome.substr(0, pos);
        std::string metodo = nome.substr(pos + 1);

        auto objSim = tabela.buscar(objeto);
        if (!objSim) {
            reportarErro("Objeto nao declarado: " + objeto, ch->posicao.linha);
            return "unknown";
        }

        objSim->utilizado = true;

        std::string tipoObj = objSim->tipo;
        auto s = tabela.buscarStruct(tipoObj);
        if (!s) {
            reportarErro("Objeto '" + objeto + "' nao eh um struct valido (tipo: " + tipoObj + ")", ch->posicao.linha);
            return "unknown";
        }

        for (auto& m : s->metodos) {
            if (m->nome == metodo) {
                if (ch->argumentos.size() != m->parametros.size()) {
                    reportarErro("Metodo '" + metodo + "' espera " + std::to_string(m->parametros.size()) +
                                 " argumentos, mas recebeu " + std::to_string(ch->argumentos.size()),
                                 ch->posicao.linha);
                }

                for (size_t i = 0; i < ch->argumentos.size(); i++) {
                    std::string tipoPassado = inferirTipoExpressao(ch->argumentos[i]);

                    if (i < m->parametros.size()) {
                        std::string tipoEsperado = m->parametros[i].second;
                        if (!tiposCompativeis(tipoEsperado, tipoPassado)) {
                            reportarErro("Argumento " + std::to_string(i+1) + " do metodo '" + metodo +
                                         "' incompativel. Esperado: " + tipoEsperado + ", Recebido: " + tipoPassado,
                                         ch->argumentos[i]->posicao.linha);
                        }
                    }
                }

                ch->tipoInferido = m->tipoRetorno;

                return m->tipoRetorno;
            }
        }

        reportarErro("Metodo '" + metodo + "' nao existe em struct " + tipoObj, ch->posicao.linha);
        return "unknown";
    }

    auto s = tabela.buscarStruct(nome);
    if (s) {
        for (auto& arg : ch->argumentos) {
            inferirTipoExpressao(arg);
        }
        return nome;
    }

    auto sim = tabela.buscar(nome);
    if (!sim) {
        reportarErro("Funcao nao declarada: " + nome, ch->posicao.linha);
        return "unknown";
    }

    std::vector<std::string> tiposEsperados;
    std::string assinatura = sim->tipo;

    size_t abreParen = assinatura.find('(');
    size_t fechaParen = assinatura.find(')');

    if (abreParen != std::string::npos && fechaParen != std::string::npos) {
        std::string paramsStr = assinatura.substr(abreParen + 1, fechaParen - abreParen - 1);

        std::string atual;
        for (char c : paramsStr) {
            if (c == ',') {
                if (!atual.empty()) tiposEsperados.push_back(atual);
                atual.clear();
            } else {
                atual += c;
            }
        }
        if (!atual.empty()) tiposEsperados.push_back(atual);
    }

    if (ch->argumentos.size() != tiposEsperados.size()) {
        reportarErro("Funcao '" + nome + "' espera " + std::to_string(tiposEsperados.size()) +
                     " argumentos, mas recebeu " + std::to_string(ch->argumentos.size()),
                     ch->posicao.linha);
    }

    for (size_t i = 0; i < ch->argumentos.size(); i++) {
       std::string tipoRecebido = inferirTipoExpressao(ch->argumentos[i]);

        if (i < tiposEsperados.size()) {
            if (!tiposCompativeis(tiposEsperados[i], tipoRecebido)) {
                reportarErro("Argumento " + std::to_string(i+1) + " da funcao '" + nome +
                             "' incompativel. Esperado: " + tiposEsperados[i] +
                             ", Recebido: " + tipoRecebido,
                             ch->argumentos[i]->posicao.linha);
            }
        }
    }

    std::string tipoRetorno = extrairTipoRetornoDaAssinatura(sim->tipo);

    ch->tipoInferido = tipoRetorno;

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

    size_t bracketEsp = esperado.find('[');
    size_t bracketRec = recebido.find('[');

    if (bracketEsp != std::string::npos && bracketRec != std::string::npos) {
        std::string baseEsp = esperado.substr(0, bracketEsp);
        std::string baseRec = recebido.substr(0, bracketRec);

        if (baseEsp == baseRec) return true;
    }

    if (recebido == "null") {
        // if (esperado == "int" || esperado == "float" || esperado == "bool" || esperado == "char") {
        //      return false;
        // }
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

std::string AnalisadorSemantico::inferirTipoAcessoMembro(const std::shared_ptr<ExpressaoAcessoMembro>& expr) {

    std::string tipoObj = inferirTipoExpressao(expr->objeto);

    if (tipoObj == "unknown" || tipoObj == "error") {
        return "error";
    }

    if (tipoObj.find('[') != std::string::npos) {
        if (expr->membro == "length" || expr->membro == "tamanho") {
            expr->tipoInferido = "int";
            return "int";
        }
        reportarErro("Arrays possuem apenas a propriedade 'length'", expr->posicao.linha);
        return "error";
    }

    auto structDef = tabela.buscarStruct(tipoObj);
    if (!structDef) {
        reportarErro("O tipo '" + tipoObj + "' nao eh uma struct e nao possui campos.", expr->posicao.linha);
        return "error";
    }

    for (const auto& atributo : structDef->atributos) {
        if (std::get<0>(atributo) == expr->membro) {
            std::string tipoCampo = std::get<1>(atributo);

            expr->tipoInferido = tipoCampo;

            return tipoCampo;
        }
    }

    reportarErro("A struct '" + tipoObj + "' nao possui o campo '" + expr->membro + "'", expr->posicao.linha);
    return "error";
}

void AnalisadorSemantico::analisarComandoAtribuicaoArray(const std::shared_ptr<ComandoAtribuicaoArray>& at) {
    std::string nomeArray = at->nome;
    std::string tipoVar = "";

    // ============================================================
    // ETAPA 1: Descobrir o tipo da variável (Simples ou Membro)
    // ============================================================

    // CASO 1: Acesso a Membro (ex: this.arr ou objeto.arr)
    if (nomeArray.find('.') != std::string::npos) {
        size_t pos = nomeArray.find('.');
        std::string objetoNome = nomeArray.substr(0, pos);  // "this"
        std::string membroNome = nomeArray.substr(pos + 1); // "arr"

        // 1. Busca o objeto pai
        auto sim = tabela.buscar(objetoNome);
        if (!sim) {
            reportarErro("Objeto nao declarado: " + objetoNome, at->posicao.linha);
            return;
        }
        sim->utilizado = true;

        // 2. Verifica se é Struct
        auto structDef = tabela.buscarStruct(sim->tipo);
        if (!structDef) {
            reportarErro("A variavel '" + objetoNome + "' nao eh uma struct.", at->posicao.linha);
            return;
        }

        // 3. Busca o campo dentro da Struct
        bool encontrado = false;
        for (const auto& attr : structDef->atributos) {
            if (std::get<0>(attr) == membroNome) {
                tipoVar = std::get<1>(attr); // Pega o tipo (ex: "int[2]")
                encontrado = true;
                break;
            }
        }

        if (!encontrado) {
            reportarErro("O campo '" + membroNome + "' nao existe na struct '" + sim->tipo + "'", at->posicao.linha);
            return;
        }
    }
    else {
        auto sim = tabela.buscar(nomeArray);
        if (!sim) {
            reportarErro("Variavel nao declarada: " + nomeArray, at->posicao.linha);
            return;
        }
        sim->utilizado = true;
        tipoVar = sim->tipo;
    }

    size_t posBracket = tipoVar.find('[');
    if (posBracket == std::string::npos) {
        reportarErro("A variavel '" + nomeArray + "' nao eh um array.", at->posicao.linha);
        return;
    }

    std::string tipoIndice = inferirTipoExpressao(at->indice);
    if (tipoIndice != "int") {
        reportarErro("O indice do array deve ser do tipo int, recebido: " + tipoIndice, at->posicao.linha);
    }

    std::string tipoBase = tipoVar.substr(0, posBracket);
    std::string tipoValor = inferirTipoExpressao(at->expressao);

    if (!tiposCompativeis(tipoBase, tipoValor)) {
        reportarErro("Atribuicao incompativel no array. Esperado: " + tipoBase + ", Recebido: " + tipoValor, at->posicao.linha);
    }
}
