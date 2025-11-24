#include <codegen/GeradorIR.hpp>
#include <sstream>
#include <memory>
#include <vector>
#include <string>
#include <map>

// Assumindo que GeradorIR.hpp define as classes e o cabeçalho do GeradorIR

std::string novoRotulo() {
    static int contadorRotulo = 0;
    return "label" + std::to_string(contadorRotulo++);
}

// -----------------------------------------------------------
// GeradorIR.cpp (Lista de Inicialização Reordenada)
// -----------------------------------------------------------

GeradorIR::GeradorIR(TabelaSimbolos *tabela)
    : contadorTemp(0), contadorLaco(0), contadorStrings(0), tss(tabela) {}

std::string GeradorIR::obterTipoLLVM(const std::string &tipoAST) {
    if (tipoAST == "int") return "i32";
    if (tipoAST == "float") return "double";
    if (tipoAST == "bool") return "i1";
    if (tipoAST == "string") return "i8*";
    if (tipoAST == "void") return "void";

    if (tipoAST.size() > 2 && tipoAST.substr(tipoAST.size() - 2) == "[]") {
        std::string tipoBase = tipoAST.substr(0, tipoAST.size() - 2);
        return obterTipoLLVM(tipoBase) + "*";
    }

    return "i32";
}

// Gera um novo nome temporário (ex: %0, %1, etc.)
std::string GeradorIR::novoTemp() {
    return "%t" + std::to_string(contadorTemp++);
}

// -----------------------------------------------------------
// GERAÇÃO DE LITERAIS (ExpressaoLiteral)
// -----------------------------------------------------------

std::string GeradorIR::gerarExpressaoLiteral(const std::shared_ptr<ExpressaoLiteral> &lit) {
    if (lit->tipo == "int") return lit->valor;
    if (lit->tipo == "float") return lit->valor;
    if (lit->tipo == "bool") return (lit->valor == "true") ? "1" : "0";
    if (lit->tipo == "string") {
        std::string nomeLiteral = gerarLiteralString(lit->valor);
        std::string temp = novoTemp();
        int tamanho = (int)lit->valor.size() + 1;

        codigo << temp << " = getelementptr [" << tamanho << " x i8], [" << tamanho
                << " x i8]* @" << nomeLiteral << ", i32 0, i32 0\n";
        return temp;
    }

    return "0";
}

// -----------------------------------------------------------
// Passagem 1 — Local variable allocation
// -----------------------------------------------------------

void GeradorIR::alocarVariaveisLocais(
    const std::vector<std::shared_ptr<NodoAST>> &nodos,
    std::stringstream &codigoAlocacoes
) {
    for (const auto &nodo : nodos) {

        if (auto cmdDecl = std::dynamic_pointer_cast<ComandoDeclaracao>(nodo)) {
            if (auto dv = std::dynamic_pointer_cast<DeclaracaoVariavel>(cmdDecl->declaracao)) {

                std::string tipoLLVM = obterTipoLLVM(dv->tipo);
                std::string varNome = dv->nome;

                // Use nome local (com %). Evita gerar allocas duplicadas
                if (!variaveisLocais.count(varNome)) {
                    std::string allocaName = "%" + varNome + ".addr";

                    codigoAlocacoes << allocaName << " = alloca "
                                    << tipoLLVM << ", align 4\n";

                    variaveisLocais[varNome] = allocaName;
                }
            }
        }

        else if (auto cmdCond = std::dynamic_pointer_cast<ComandoCondicional>(nodo)) {
            alocarVariaveisLocais(cmdCond->blocoIf, codigoAlocacoes);
            alocarVariaveisLocais(cmdCond->blocoElse, codigoAlocacoes);
        }

        else if (auto cmdLaco = std::dynamic_pointer_cast<ComandoLaco>(nodo)) {
            alocarVariaveisLocais(cmdLaco->corpo, codigoAlocacoes);
        }

        else if (auto df = std::dynamic_pointer_cast<DeclaracaoFuncao>(nodo)) {
            // ignora funções aqui (cada função aloca seu próprio espaço)
        }
    }
}

// GeradorIR.cpp

std::string GeradorIR::gerar(const std::shared_ptr<NodoPrograma> &programa) {
    // 1. Limpa o stream principal e garante o estado inicial.
    codigo.str("");
    codigo.clear();

    std::stringstream codigoMain;


    codigoMain << "define i32 @main() {\n";
    codigoMain << "entry:\n";

    std::stringstream codigoAlocacoes;

    // Aloca variáveis globais locais (Mantido o código original)
    for (const auto &decl : programa->declaracoes) {
        if (auto dv = std::dynamic_pointer_cast<DeclaracaoVariavel>(decl)) {
            std::string tipoLLVM = obterTipoLLVM(dv->tipo);
            std::string varNome = dv->nome;

            if (!variaveisLocais.count(varNome)) {
                std::string allocaName = "%" + varNome + ".addr";
                codigoAlocacoes << allocaName << " = alloca "
                                << tipoLLVM << ", align 4\n";
                variaveisLocais[varNome] = allocaName;
            }
        }
    }

    // Alocar variáveis locais dentro de blocos
    std::vector<std::shared_ptr<NodoAST>> nodosAST;
    for (const auto &cmd : programa->comandos)
        nodosAST.push_back(cmd);

    alocarVariaveisLocais(nodosAST, codigoAlocacoes);

    codigoMain << codigoAlocacoes.str() << "\n";

    // Inicializadores de variáveis
    for (const auto &decl : programa->declaracoes) {
        if (auto dv = std::dynamic_pointer_cast<DeclaracaoVariavel>(decl)) {
            if (dv->inicializador) {
                // As expressões são geradas para o stream principal 'codigo' (membro da classe)
                std::string valorInicial = gerarExpressao(dv->inicializador);
                std::string tipoLLVM = obterTipoLLVM(dv->tipo);
                std::string allocaName = variaveisLocais.at(dv->nome);

                codigo << "store " << tipoLLVM << " " << valorInicial
                       << ", " << tipoLLVM << "* " << allocaName << ", align 4\n";
            }
        }
    }


    for (const auto &cmd : programa->comandos)
        gerarComando(cmd);

    codigoMain << codigo.str();
    codigo.str(""); // Limpa o stream 'codigo' para a injeção final.

    codigoMain << "ret i32 0\n";
    codigoMain << "}\n";


    codigo << "; LLVM IR gerado por Wasabi Compiler\n";
    codigo << "target triple = \"i686-pc-windows-gnu\"\n";
    codigo << "target datalayout = "
             "\"e-m:e-p:32:32-f64:32:64-v1024:1024-n32:64-S64\"\n\n";

    codigo << "declare void @print_int(i32)\n";
    codigo << "declare void @print(i8*)\n\n";

    for (const auto &g : globaisString)
        codigo << g << "\n";
    if (!globaisString.empty())
        codigo << "\n";

    for (const auto &decl : programa->declaracoes)
        gerarDeclaracao(decl);

    codigo << codigoMain.str();

    return codigo.str();
}


// -----------------------------------------------------------
// Declaração de Variável
// -----------------------------------------------------------

std::string GeradorIR::gerarDeclaracaoVariavel(const std::shared_ptr<DeclaracaoVariavel> &dv) {

    std::string tipoLLVM = obterTipoLLVM(dv->tipo);
    std::string varNome = dv->nome;

    if (!variaveisLocais.count(varNome))
        return "";

    std::string allocaName = variaveisLocais.at(varNome);

    if (dv->inicializador) {
        std::string valorInicial = gerarExpressao(dv->inicializador);

        codigo << "store " << tipoLLVM << " " << valorInicial
               << ", " << tipoLLVM << "* " << allocaName << ", align 4\n";
    }

    return allocaName;
}

std::string GeradorIR::gerarDeclaracao(const std::shared_ptr<NodoDeclaracao> &decl) {
    if (auto df = std::dynamic_pointer_cast<DeclaracaoFuncao>(decl))
        return gerarDeclaracaoFuncao(df);

    return "";
}

// -----------------------------------------------------------
// Expressões — Dispatcher
// -----------------------------------------------------------

std::string GeradorIR::gerarExpressao(const std::shared_ptr<NodoExpressao> &expr) {
    if (!expr) return "";

    if (auto lit = std::dynamic_pointer_cast<ExpressaoLiteral>(expr))
        return gerarExpressaoLiteral(lit);

    if (auto id = std::dynamic_pointer_cast<ExpressaoIdentificador>(expr))
        return gerarExpressaoIdentificador(id);

    if (auto chamada = std::dynamic_pointer_cast<ExpressaoChamada>(expr))
        return gerarExpressaoChamada(chamada);

    if (auto bin = std::dynamic_pointer_cast<ExpressaoBinaria>(expr))
        return gerarExpressaoBinaria(bin);

    if (auto rel = std::dynamic_pointer_cast<ExpressaoRelacional>(expr))
        return gerarExpressaoRelacional(rel);

    if (auto unaria = std::dynamic_pointer_cast<ExpressaoUnaria>(expr))
        return gerarExpressaoUnaria(unaria);

    if (auto logica = std::dynamic_pointer_cast<ExpressaoLogica>(expr))
        return gerarExpressaoLogica(logica);

    return "0";
}

// -----------------------------------------------------------
// Expressão Binária
// -----------------------------------------------------------

std::string GeradorIR::gerarExpressaoBinaria(const std::shared_ptr<ExpressaoBinaria> &expr) {

    std::string lhs = gerarExpressao(expr->esquerda);
    std::string rhs = gerarExpressao(expr->direita);

    std::string tipoL = "i32";

    if (auto id = std::dynamic_pointer_cast<ExpressaoIdentificador>(expr->esquerda))
        tipoL = obterTipoLLVM(tss->obterTipo(id->nome));

    bool isFloat = (tipoL == "double");

    std::string temp = novoTemp();

    if (isFloat) {
        if (expr->operador == "+")
            codigo << temp << " = fadd " << tipoL << " " << lhs << ", " << rhs << "\n";
        else if (expr->operador == "-")
            codigo << temp << " = fsub " << tipoL << " " << lhs << ", " << rhs << "\n";
        else if (expr->operador == "*")
            codigo << temp << " = fmul " << tipoL << " " << lhs << ", " << rhs << "\n";
        else if (expr->operador == "/")
            codigo << temp << " = fdiv " << tipoL << " " << lhs << ", " << rhs << "\n";
    } else {
        if (expr->operador == "+")
            codigo << temp << " = add nsw " << tipoL << " " << lhs << ", " << rhs << "\n";
        else if (expr->operador == "-")
            codigo << temp << " = sub nsw " << tipoL << " " << lhs << ", " << rhs << "\n";
        else if (expr->operador == "*")
            codigo << temp << " = mul nsw " << tipoL << " " << lhs << ", " << rhs << "\n";
        else if (expr->operador == "/")
            codigo << temp << " = sdiv " << tipoL << " " << lhs << ", " << rhs << "\n";
    }

    return temp;
}

// -----------------------------------------------------------
// Retorno
// -----------------------------------------------------------

std::string GeradorIR::gerarComandoRetorno(const std::shared_ptr<ComandoRetorno> &cmdRet) {

    std::string tipoRet = "i32";

    if (cmdRet->expressao) {
        std::string valor = gerarExpressao(cmdRet->expressao);

        if (auto id = std::dynamic_pointer_cast<ExpressaoIdentificador>(cmdRet->expressao))
            tipoRet = obterTipoLLVM(tss->obterTipo(id->nome));

        codigo << "ret " << tipoRet << " " << valor << "\n";
    } else {
        codigo << "ret void\n";
    }

    return "";
}

// -----------------------------------------------------------
// IF / ELSE
// -----------------------------------------------------------

std::string GeradorIR::gerarComandoCondicional(const std::shared_ptr<ComandoCondicional> &cmdCond) {

    std::string condicaoIR = gerarExpressao(cmdCond->condicao);

    std::string thenRotulo = novoRotulo();
    std::string elseRotulo = novoRotulo();
    std::string mergeRotulo = novoRotulo();

    std::string rotuloElseFinal =
        cmdCond->blocoElse.empty() ? mergeRotulo : elseRotulo;

    codigo << "br i1 " << condicaoIR
           << ", label %" << thenRotulo
           << ", label %" << rotuloElseFinal << "\n\n";

    codigo << thenRotulo << ":\n";

    for (auto &cmd : cmdCond->blocoIf)
        gerarComando(std::dynamic_pointer_cast<NodoComando>(cmd));

    codigo << "br label %" << mergeRotulo << "\n\n";

    if (!cmdCond->blocoElse.empty()) {

        codigo << elseRotulo << ":\n";

        for (auto &cmd : cmdCond->blocoElse)
            gerarComando(std::dynamic_pointer_cast<NodoComando>(cmd));

        codigo << "br label %" << mergeRotulo << "\n\n";
    }

    codigo << mergeRotulo << ":\n";

    return "";
}

// -----------------------------------------------------------
// Dispatcher comandos
// -----------------------------------------------------------

std::string GeradorIR::gerarComando(const std::shared_ptr<NodoComando> &cmd) {
    if (!cmd) return "";

    if (auto declCmd = std::dynamic_pointer_cast<ComandoDeclaracao>(cmd)) {
        if (auto dv = std::dynamic_pointer_cast<DeclaracaoVariavel>(declCmd->declaracao))
            return gerarDeclaracaoVariavel(dv);
        return "";
    }

    if (auto atrib = std::dynamic_pointer_cast<ComandoAtribuicao>(cmd))
        return gerarComandoAtribuicao(atrib);

    if (auto cmdIdent = std::dynamic_pointer_cast<ComandoIdent>(cmd))
        return gerarComandoIdent(cmdIdent);

    if (auto cmdRet = std::dynamic_pointer_cast<ComandoRetorno>(cmd))
        return gerarComandoRetorno(cmdRet);

    if (auto cmdCond = std::dynamic_pointer_cast<ComandoCondicional>(cmd))
        return gerarComandoCondicional(cmdCond);

    if (auto cmdLaco = std::dynamic_pointer_cast<ComandoLaco>(cmd))
        return gerarComandoLaco(cmdLaco);

    return "";
}

// -----------------------------------------------------------
// Relacional
// -----------------------------------------------------------

std::string GeradorIR::gerarExpressaoRelacional(const std::shared_ptr<ExpressaoRelacional> &rel) {

    std::string lhs = gerarExpressao(rel->esquerda);
    std::string rhs = gerarExpressao(rel->direita);

    std::string tipoL = "i32";

    if (auto id = std::dynamic_pointer_cast<ExpressaoIdentificador>(rel->esquerda))
        tipoL = obterTipoLLVM(tss->obterTipo(id->nome));

    bool isFloat = (tipoL == "double");

    std::string temp = novoTemp();
    std::string codOp = "";

    if (isFloat) {
        if      (rel->operador == "==") codOp = "oeq";
        else if (rel->operador == "!=") codOp = "one";
        else if (rel->operador == "<")  codOp = "olt";
        else if (rel->operador == ">")  codOp = "ogt";
        else if (rel->operador == "<=") codOp = "ole";
        else if (rel->operador == ">=") codOp = "oge";

        codigo << temp << " = fcmp " << codOp << " "
               << tipoL << " " << lhs << ", " << rhs << "\n";
    }

    else {
        if      (rel->operador == "==") codOp = "eq";
        else if (rel->operador == "!=") codOp = "ne";
        else if (rel->operador == "<")  codOp = "slt";
        else if (rel->operador == ">")  codOp = "sgt";
        else if (rel->operador == "<=") codOp = "sle";
        else if (rel->operador == ">=") codOp = "sge";

        codigo << temp << " = icmp " << codOp << " "
               << tipoL << " " << lhs << ", " << rhs << "\n";
    }

    return temp;
}

// -----------------------------------------------------------
// Identificadores (load)
// -----------------------------------------------------------

std::string GeradorIR::gerarExpressaoIdentificador(const std::shared_ptr<ExpressaoIdentificador> &id) {

    if (variaveisLocais.count(id->nome)) {

        std::string allocaName = variaveisLocais.at(id->nome);
        std::string tipoAST = tss->obterTipo(id->nome);
        std::string tipoLLVM = obterTipoLLVM(tipoAST);

        std::string temp = novoTemp();

        codigo << temp << " = load " << tipoLLVM
               << ", " << tipoLLVM << "* " << allocaName
               << ", align 4\n";

        return temp;
    }

    return "0";
}

// -----------------------------------------------------------
// Atribuição
// -----------------------------------------------------------

std::string GeradorIR::gerarComandoAtribuicao(const std::shared_ptr<ComandoAtribuicao> &atrib) {

    std::string valorExpressao = gerarExpressao(atrib->expressao);

    if (variaveisLocais.count(atrib->nome)) {

        std::string allocaName = variaveisLocais.at(atrib->nome);
        std::string tipoAST = tss->obterTipo(atrib->nome);
        std::string tipoLLVM = obterTipoLLVM(tipoAST);

        codigo << "store " << tipoLLVM << " " << valorExpressao
               << ", " << tipoLLVM << "* " << allocaName << ", align 4\n";
    }

    return "";
}

// -----------------------------------------------------------
// ComandoIdent → call
// -----------------------------------------------------------

std::string GeradorIR::gerarComandoIdent(const std::shared_ptr<ComandoIdent> &cmdIdent) {

    if (cmdIdent->argumentos.empty()) return "";

    std::string argumentosIR = "";

    for (const auto &arg : cmdIdent->argumentos) {
        std::string argValue = gerarExpressao(arg);
        std::string tipoLLVM = "i32";

        if (auto id = std::dynamic_pointer_cast<ExpressaoIdentificador>(arg))
            tipoLLVM = obterTipoLLVM(tss->obterTipo(id->nome));

        argumentosIR += tipoLLVM + " " + argValue + ", ";
    }

    if (!argumentosIR.empty()) {
        argumentosIR.pop_back();
        argumentosIR.pop_back();
    }

    if (cmdIdent->nome == "print") {
        // assumindo apenas um argumento para print
        if (cmdIdent->argumentos.size() == 1) {
            auto arg = cmdIdent->argumentos[0];
            // descobrir tipo do argumento
            std::string tipoArgLLVM = "i32"; // default
            if (auto lit = std::dynamic_pointer_cast<ExpressaoLiteral>(arg)) {
                if (lit->tipo == "string") tipoArgLLVM = "i8*";
                else if (lit->tipo == "int") tipoArgLLVM = "i32";
                else if (lit->tipo == "float") tipoArgLLVM = "double";
            } else if (auto id = std::dynamic_pointer_cast<ExpressaoIdentificador>(arg)) {
                tipoArgLLVM = obterTipoLLVM(tss->obterTipo(id->nome));
            } else {
                // outros tipos - chute conservador
            }

            std::string argValue = gerarExpressao(arg);

            if (tipoArgLLVM == "i8*") {
                codigo << "call void @print(" << tipoArgLLVM << " " << argValue << ")\n";
            } else {
                // chama o print_int para inteiros
                codigo << "call void @print_int(" << tipoArgLLVM << " " << argValue << ")\n";
            }
        }
    }
    return "";
}

// -----------------------------------------------------------
// Expressão chamada de função
// -----------------------------------------------------------

std::string GeradorIR::gerarExpressaoChamada(const std::shared_ptr<ExpressaoChamada> &chamada) {

    std::string argumentosIR = "";

    for (const auto &arg : chamada->argumentos) {

        std::string argValue = gerarExpressao(arg);
        std::string tipoLLVM = "i32";

        if (auto id = std::dynamic_pointer_cast<ExpressaoIdentificador>(arg))
            tipoLLVM = obterTipoLLVM(tss->obterTipo(id->nome));

        argumentosIR += tipoLLVM + " " + argValue + ", ";
    }

    if (!argumentosIR.empty()) {
        argumentosIR.pop_back();
        argumentosIR.pop_back();
    }

    std::string tipoASTRetorno = tss->obterTipo(chamada->nome);
    std::string tipoRetornoLLVM = obterTipoLLVM(tipoASTRetorno);

    std::string temp = novoTemp();

    if (tipoRetornoLLVM != "void") {
        codigo << temp << " = call " << tipoRetornoLLVM
               << " @" << chamada->nome
               << "(" << argumentosIR << ")\n";
        return temp;
    } else {
        codigo << "call void @" << chamada->nome
               << "(" << argumentosIR << ")\n";
        return "";
    }
}

// -----------------------------------------------------------
// Expressão Unária
// -----------------------------------------------------------

std::string GeradorIR::gerarExpressaoUnaria(const std::shared_ptr<ExpressaoUnaria> &unaria) {

    std::string valor = gerarExpressao(unaria->operando);
    std::string temp = novoTemp();
    std::string tipoL = "i32";

    if (unaria->operador == "-") {
        codigo << temp << " = sub " << tipoL << " 0, " << valor << "\n";
    }
    else if (unaria->operador == "!") {
        codigo << temp << " = xor i1 " << valor << ", 1\n";
    }

    return temp;
}

// -----------------------------------------------------------
// Expressão Lógica (Short-circuit)
// -----------------------------------------------------------

std::string GeradorIR::gerarExpressaoLogica(const std::shared_ptr<ExpressaoLogica> &logica) {

    std::string tipoL = "i1";

    // rótulos explícitos
    std::string rotuloEvalLHS = novoRotulo();
    std::string rotuloRHS = novoRotulo();
    std::string rotuloMerge = novoRotulo();

    // Começamos um novo bloco para avaliar LHS (emitimos o rótulo)
    codigo << rotuloEvalLHS << ":\n";
    std::string lhs = gerarExpressao(logica->esquerda);

    if (logica->operador == "&&") {
        // se lhs == true -> avalia rhs, senão pula direto para merge com 0
        codigo << "br i1 " << lhs << ", label %" << rotuloRHS
               << ", label %" << rotuloMerge << "\n\n";
    } else { // "||"
        // se lhs == true -> pula para merge com 1, senão avalia rhs
        codigo << "br i1 " << lhs << ", label %" << rotuloMerge
               << ", label %" << rotuloRHS << "\n\n";
    }

    // bloco RHS: avalia o lado direito
    codigo << rotuloRHS << ":\n";
    std::string rhs = gerarExpressao(logica->direita);
    // depois de avaliar rhs, vai para merge
    codigo << "br label %" << rotuloMerge << "\n\n";

    // bloco merge
    codigo << rotuloMerge << ":\n";

    std::string tempResultado = novoTemp();

    if (logica->operador == "&&") {
        // incoming: [false, <evalLHS predecessor>] and [rhs, rotuloRHS]
        // o predecessor do LHS é o bloco anterior — usamos rotuloEvalLHS para quando LHS for false não passamos por rotuloRHS.
        codigo << tempResultado << " = phi " << tipoL
               << " [0, %" << rotuloEvalLHS << "], [" << rhs << ", %"
               << rotuloRHS << "]\n";
    } else { // ||
        codigo << tempResultado << " = phi " << tipoL
               << " [1, %" << rotuloEvalLHS << "], [" << rhs << ", %"
               << rotuloRHS << "]\n";
    }

    return tempResultado;
}

// -----------------------------------------------------------
// Laço (while)
// -----------------------------------------------------------

std::string GeradorIR::gerarComandoLaco(const std::shared_ptr<ComandoLaco> &laco) {

    std::string rotuloCond  = "loop_cond" + std::to_string(contadorLaco);
    std::string rotuloBody  = "loop_body" + std::to_string(contadorLaco);
    std::string rotuloExit  = "loop_exit" + std::to_string(contadorLaco++);

    codigo << "br label %" << rotuloCond << "\n\n";

    codigo << rotuloCond << ":\n";

    std::string condicaoIR = gerarExpressao(laco->condicao);

    codigo << "br i1 " << condicaoIR
           << ", label %" << rotuloBody
           << ", label %" << rotuloExit << "\n\n";

    codigo << rotuloBody << ":\n";

    for (auto &cmd : laco->corpo)
        gerarComando(std::dynamic_pointer_cast<NodoComando>(cmd));

    codigo << "br label %" << rotuloCond << "\n\n";

    codigo << rotuloExit << ":\n";

    return "";
}

// -----------------------------------------------------------
// Declaração de Função
// -----------------------------------------------------------

std::string GeradorIR::gerarDeclaracaoFuncao(const std::shared_ptr<DeclaracaoFuncao> &df) {

    tss->entrarEscopo();

    auto varsLocaisAntigas = variaveisLocais;

    variaveisLocais.clear();

    std::stringstream codigoAlocacoes;

    std::string tipoRetornoLLVM = obterTipoLLVM(df->tipoRetorno);

    std::string paramsLLVM = "";
    for (const auto &param : df->parametros) {
        std::string tipoParam = obterTipoLLVM(param.second);
        paramsLLVM += tipoParam + " %" + param.first + ", ";
    }

    if (!paramsLLVM.empty()) {
        paramsLLVM.pop_back();
        paramsLLVM.pop_back();
    }

    codigo << "define " << tipoRetornoLLVM
           << " @" << df->nome
           << "(" << paramsLLVM << ") {\n";

    codigo << "entry:\n";

    for (const auto &[nomeParam, tipoAST] : df->parametros) {

        std::string tipoParam = obterTipoLLVM(tipoAST);
        std::string allocaName = "%" + nomeParam + ".addr";

        codigoAlocacoes << allocaName << " = alloca "
                        << tipoParam << ", align 4\n";

        codigo << "store " << tipoParam << " %" << nomeParam
               << ", " << tipoParam << "* " << allocaName
               << ", align 4\n";

        variaveisLocais[nomeParam] = allocaName;
    }

    alocarVariaveisLocais(df->corpo, codigoAlocacoes);

    codigo << codigoAlocacoes.str();

    for (const auto &cmd : df->corpo)
        gerarComando(std::dynamic_pointer_cast<NodoComando>(cmd));

    if (df->tipoRetorno == "void")
        codigo << "ret void\n";

    codigo << "}\n\n";

    variaveisLocais = varsLocaisAntigas;
    tss->sairEscopo();

    return "";
}

std::string GeradorIR::gerarLiteralString(const std::string &valor) {
    std::string nome = ".str" + std::to_string(contadorStrings++);

    std::string conteudo = "c\"";
    for (char c : valor) {
        if (c == '\\') {
            conteudo += "\\\\";  // Escape para backslash
        } else if (c == '"') {
            conteudo += "\\22";  // Escape para aspas
        } else if (c == '\n') {
            conteudo += "\\0A";  // Escape para newline
        } else if (c == '\t') {
            conteudo += "\\09";  // Escape para tab
        } else {
            conteudo += c;
        }
    }
    conteudo += "\\00\"";

    int tamanho = valor.size() + 1;

    std::stringstream ss;
    ss << "@" << nome << " = private unnamed_addr constant [" << tamanho << " x i8] " << conteudo;
    globaisString.push_back(ss.str());

    return nome;
}
