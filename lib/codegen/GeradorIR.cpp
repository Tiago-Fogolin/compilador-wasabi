#include <codegen/GeradorIR.hpp>
#include <sstream>
#include <memory>
#include <vector>
#include <string>
#include <map>


std::string novoRotulo() {
    static int contadorRotulo = 0;
    return "label" + std::to_string(contadorRotulo++);
}


GeradorIR::GeradorIR(TabelaSimbolos *tabela)
    : contadorTemp(0), contadorLaco(0), contadorStrings(0), tss(tabela) {}

std::string GeradorIR::obterTipoLLVM(const std::string &tipoAST) {
    if (tipoAST == "int") return "i32";
    if (tipoAST == "char") return "i8";
    if (tipoAST == "float") return "double";
    if (tipoAST == "bool") return "i1";
    if (tipoAST == "string") return "i8*";
    if (tipoAST == "void") return "void";

    if (tipoAST.find('[') != std::string::npos) {
        std::string tipoBaseLLVM = obterTipoBaseDoArray(tipoAST);
        return "{ i32, " + tipoBaseLLVM + "* }*";
    }

    return "%" + tipoAST + "*";
}

std::string GeradorIR::obterTipoBaseDoArray(const std::string& tipo)
{
    size_t posBracket = tipo.find('[');
    if (posBracket != std::string::npos) {
        std::string base = tipo.substr(0, posBracket);
        return obterTipoLLVM(base);
    }
    return "i32";
}

std::string GeradorIR::novoTemp() {
    return "%t" + std::to_string(contadorTemp++);
}


std::string GeradorIR::gerarExpressaoLiteral(const std::shared_ptr<ExpressaoLiteral> &lit) {
    if (lit->tipo == "int") return lit->valor;
    if (lit->tipo == "float") return lit->valor;
    if (lit->tipo == "bool") return (lit->valor == "true") ? "1" : "0";
    if (lit->tipo == "null") {
        return "null";
    }

    if (lit->tipo == "char") {
        char c = lit->valor[0];

        if (lit->valor.size() >= 3 && lit->valor.front() == '\'' && lit->valor.back() == '\'') {
            c = lit->valor[1];
        }
        else if (!lit->valor.empty()) {
            c = lit->valor[0];
        }

        return std::to_string((int)c);
    }

    if (lit->tipo == "string") {
        std::string nomeLiteral = gerarLiteralString(lit->valor);

        int tamanho = 1;
        auto it = tamanhoLiteralPorNome.find(nomeLiteral);
        if (it != tamanhoLiteralPorNome.end())
            tamanho = it->second;

        std::string temp = novoTemp();

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
            if (cmdLaco->inicializacao) {
                std::vector<std::shared_ptr<NodoAST>> listaInit;
                listaInit.push_back(cmdLaco->inicializacao);
                alocarVariaveisLocais(listaInit, codigoAlocacoes);
            }

            if (cmdLaco->tipo == ComandoLaco::TipoLaco::FOREACH) {
                std::string tipoArray = cmdLaco->expressaoForeach->tipoInferido;
                std::string tipoElemento = "i32";

                if (tipoArray == "string") {
                    tipoElemento = "i8";
                } else {
                    tipoElemento = obterTipoBaseDoArray(tipoArray);
                }

                std::string varNome = cmdLaco->identificadorForeach;

                if (!variaveisLocais.count(varNome)) {
                    std::string allocaName = "%" + varNome + ".addr";
                    codigoAlocacoes << allocaName << " = alloca "
                                    << tipoElemento << ", align 4\n";
                    variaveisLocais[varNome] = allocaName;
                }
            }
            alocarVariaveisLocais(cmdLaco->corpo, codigoAlocacoes);
        }

        else if (auto df = std::dynamic_pointer_cast<DeclaracaoFuncao>(nodo)) {
            // ignora funções aqui (cada função aloca seu próprio espaço)
        }
    }
}

std::string GeradorIR::gerar(const std::shared_ptr<NodoPrograma> &programa) {

    codigo.str(""); codigo.clear();
    globaisString.clear(); internedStrings.clear(); tamanhoLiteralPorNome.clear();
    globais.clear();

     std::stringstream codigoGlobais;

    for (const auto &decl : programa->declaracoes) {
        if (auto dv = std::dynamic_pointer_cast<DeclaracaoVariavel>(decl)) {
            std::string tipoLLVM = obterTipoLLVM(dv->tipo);

            std::string valorPadrao = "zeroinitializer";
            if (tipoLLVM == "i32") valorPadrao = "0";
            else if (tipoLLVM == "double") valorPadrao = "0.0";
            else if (tipoLLVM == "i8*") valorPadrao = "null";
            else if (tipoLLVM == "i1") valorPadrao = "0";
            else if (tipoLLVM.back() == '*') valorPadrao = "null";


            codigoGlobais << "@" << dv->nome << " = global " << tipoLLVM << " " << valorPadrao << ", align 4\n";

            globais.insert(dv->nome);
            tss->declarar(dv->nome, dv->tipo, dv->posicao.linha, true);
        }
    }

    std::stringstream codigoMain;
    codigoMain << "define i32 @main() {\nentry:\n";

    std::stringstream codigoAlocacoesMain;

    std::vector<std::shared_ptr<NodoAST>> nodosAST;
    for (const auto &cmd : programa->comandos) nodosAST.push_back(cmd);
    alocarVariaveisLocais(nodosAST, codigoAlocacoesMain);

    codigoMain << codigoAlocacoesMain.str() << "\n";

    for (const auto &decl : programa->declaracoes) {
        if (auto dv = std::dynamic_pointer_cast<DeclaracaoVariavel>(decl)) {

            std::string enderecoGlobal = "@" + dv->nome;
            std::string tipoLLVM = obterTipoLLVM(dv->tipo);

            if (dv->inicializador) {
                std::string valorInicial = gerarExpressao(dv->inicializador);
                codigoMain << codigo.str(); codigo.str("");

                codigoMain << "store " << tipoLLVM << " " << valorInicial
                           << ", " << tipoLLVM << "* " << enderecoGlobal << ", align 4\n";

                if (auto arrLit = std::dynamic_pointer_cast<ExpressaoArrayLiteral>(dv->inicializador))
                    tamanhosArraysLocais[dv->nome] = (int)arrLit->elementos.size();
            }
            else {
                size_t posAbre = dv->tipo.find('[');
                size_t posFecha = dv->tipo.find(']');


                if (posAbre != std::string::npos && posFecha > posAbre + 1) {
                    std::string tamStr = dv->tipo.substr(posAbre + 1, posFecha - posAbre - 1);
                    int tamanho = std::stoi(tamStr);
                    std::string tipoBaseLLVM = obterTipoBaseDoArray(dv->tipo);

                    std::string dadosPtr = novoTemp();
                    codigo << dadosPtr << " = alloca [" << tamanho << " x " << tipoBaseLLVM << "], align 4\n";
                    std::string structType = "{ i32, " + tipoBaseLLVM + "* }";
                    std::string structPtr = novoTemp();
                    codigo << structPtr << " = alloca " << structType << ", align 8\n";
                    std::string sizePtr = novoTemp();
                    codigo << sizePtr << " = getelementptr " << structType << ", " << structType << "* " << structPtr << ", i32 0, i32 0\n";
                    codigo << "store i32 " << tamanho << ", i32* " << sizePtr << "\n";
                    std::string ptrDecayed = novoTemp();
                    codigo << ptrDecayed << " = getelementptr [" << tamanho << " x " << tipoBaseLLVM << "], ["
                           << tamanho << " x " << tipoBaseLLVM << "]* " << dadosPtr << ", i32 0, i32 0\n";
                    std::string dataFieldPtr = novoTemp();
                    codigo << dataFieldPtr << " = getelementptr " << structType << ", " << structType << "* " << structPtr << ", i32 0, i32 1\n";
                    codigo << "store " << tipoBaseLLVM << "* " << ptrDecayed << ", " << tipoBaseLLVM << "** " << dataFieldPtr << "\n";


                    codigo << "store " << tipoLLVM << " " << structPtr << ", " << tipoLLVM << "* " << enderecoGlobal << ", align 4\n";

                    codigoMain << codigo.str(); codigo.str("");
                }

                else if (dv->tipo != "int" && dv->tipo != "float" && dv->tipo != "bool" && dv->tipo != "string") {
                     std::string tipoStruct = "%" + dv->tipo;
                     std::string tipoPtr = tipoStruct + "*";
                     std::string structInstance = novoTemp();
                     codigo << structInstance << " = alloca " << tipoStruct << ", align 8\n";
                     codigo << "store " << tipoPtr << " " << structInstance << ", " << tipoPtr << "* " << enderecoGlobal << ", align 4\n";
                     codigoMain << codigo.str(); codigo.str("");
                }
            }
        }
    }

    // Comandos do Main
    for (const auto &cmd : programa->comandos) {
        gerarComando(cmd);
        codigoMain << codigo.str(); codigo.str("");
    }
    codigoMain << "ret i32 0\n}\n";

    std::stringstream codigoFuncoes;

    for (const auto &decl : programa->declaracoes) {
        gerarDeclaracao(decl);
        if (!codigo.str().empty()) {
            codigoFuncoes << codigo.str(); codigo.str("");
        }
    }

    for (const auto &decl : programa->declaracoes) {
        if (auto ds = std::dynamic_pointer_cast<DeclaracaoStruct>(decl)) {
            for (auto &metodo : ds->metodos) {
                std::string nomeOriginal = metodo->nome;
                metodo->nome = ds->nome + "_" + metodo->nome;
                gerarDeclaracaoFuncao(metodo, ds->nome);
                if (!codigo.str().empty()) {
                    codigoFuncoes << codigo.str(); codigo.str("");
                }
                metodo->nome = nomeOriginal;
            }
        }
    }

    codigo << "; LLVM IR gerado por Wasabi Compiler\n";
    codigo << "target triple = \"i686-pc-windows-gnu\"\n";
    codigo << "target datalayout = \"e-m:e-p:32:32-f64:32:64-v1024:1024-n32:64-S64\"\n\n";

    gerarDefinicaoStructs(programa->declaracoes);

    codigo << codigoGlobais.str() << "\n";

    codigo << "declare void @print_float(double)\n";
    codigo << "declare void @print_int(i32)\n";
    codigo << "declare void @print(i8*)\n";
    codigo << "declare void @print_int_array({ i32, i32* }*)\n";
    codigo << "declare void @print_char(i8)\n";
    codigo << "declare i32 @str_eq(i8*, i8*)\n";
    codigo << "declare i8* @str_concat(i8*, i8*)\n";
    codigo << "declare i8* @int_to_str(i32)\n";
    codigo << "declare i8* @char_to_str(i8)\n";
    codigo << "declare i8* @float_to_str(double)\n\n";

    for (const auto &g : globaisString) codigo << g << "\n";
    if (!globaisString.empty()) codigo << "\n";

    codigo << codigoFuncoes.str();
    codigo << codigoMain.str();

    return codigo.str();
}



std::string GeradorIR::gerarDeclaracaoVariavel(const std::shared_ptr<DeclaracaoVariavel> &dv) {

    std::string tipoLLVM = obterTipoLLVM(dv->tipo);
    std::string varNome = dv->nome;

    if (!variaveisLocais.count(varNome))
        return "";

    std::string allocaName = variaveisLocais.at(varNome);

    tss->declarar(dv->nome, dv->tipo, dv->posicao.linha, true);

    if (dv->inicializador) {
        std::string valorInicial = gerarExpressao(dv->inicializador);

        codigo << "store " << tipoLLVM << " " << valorInicial
               << ", " << tipoLLVM << "* " << allocaName << ", align 4\n";

        if (auto arrLit = std::dynamic_pointer_cast<ExpressaoArrayLiteral>(dv->inicializador)) {
            tamanhosArraysLocais[varNome] = (int)arrLit->elementos.size();
        }
    }
    else {
        size_t posAbre = dv->tipo.find('[');
        size_t posFecha = dv->tipo.find(']');

        if (posAbre != std::string::npos && posFecha > posAbre + 1) {
            std::string tamanhoStr = dv->tipo.substr(posAbre + 1, posFecha - posAbre - 1);
            int tamanho = std::stoi(tamanhoStr);

            std::string tipoBaseLLVM = obterTipoBaseDoArray(dv->tipo);

            std::string dadosPtr = novoTemp();
            codigo << dadosPtr << " = alloca [" << tamanho << " x " << tipoBaseLLVM << "], align 4\n";

            std::string structType = "{ i32, " + tipoBaseLLVM + "* }";
            std::string structPtr = novoTemp();
            codigo << structPtr << " = alloca " << structType << ", align 8\n";

            std::string sizePtr = novoTemp();
            codigo << sizePtr << " = getelementptr " << structType << ", " << structType << "* " << structPtr << ", i32 0, i32 0\n";
            codigo << "store i32 " << tamanho << ", i32* " << sizePtr << "\n";

            std::string ptrDecayed = novoTemp();
            codigo << ptrDecayed << " = getelementptr [" << tamanho << " x " << tipoBaseLLVM << "], ["
                    << tamanho << " x " << tipoBaseLLVM << "]* " << dadosPtr << ", i32 0, i32 0\n";

            std::string dataFieldPtr = novoTemp();
            codigo << dataFieldPtr << " = getelementptr " << structType << ", " << structType << "* " << structPtr << ", i32 0, i32 1\n";
            codigo << "store " << tipoBaseLLVM << "* " << ptrDecayed << ", " << tipoBaseLLVM << "** " << dataFieldPtr << "\n";

            codigo << "store " << tipoLLVM << " " << structPtr << ", " << tipoLLVM << "* " << allocaName << ", align 4\n";
        }
        else if (dv->tipo != "int" && dv->tipo != "float" && dv->tipo != "bool" && dv->tipo != "string") {
            std::string nomeStruct = dv->tipo;
            std::string tipoStruct = "%" + nomeStruct;
            std::string tipoPtr = tipoStruct + "*";

            // Aloca a estrutura real
            std::string structInstance = novoTemp();
            codigo << structInstance << " = alloca " << tipoStruct << ", align 8\n";

            // Salva o ponteiro na variável
            codigo << "store " << tipoPtr << " " << structInstance << ", "
                << tipoPtr << "* " << allocaName << ", align 4\n";
        }
    }

    return allocaName;
}

std::string GeradorIR::gerarDeclaracao(const std::shared_ptr<NodoDeclaracao> &decl) {
    if (auto df = std::dynamic_pointer_cast<DeclaracaoFuncao>(decl))
        return gerarDeclaracaoFuncao(df);

    return "";
}


std::string GeradorIR::gerarExpressao(const std::shared_ptr<NodoExpressao> &expr) {
    if (!expr) return "";

    if (auto membro = std::dynamic_pointer_cast<ExpressaoAcessoMembro>(expr)) {
        return gerarExpressaoAcessoMembro(membro);
    }

    if (auto arrAcc = std::dynamic_pointer_cast<ExpressaoAcessoArray>(expr))
    {
        return gerarExpressaoAcessoArray(arrAcc);
    }

    if (auto arr = std::dynamic_pointer_cast<ExpressaoArrayLiteral>(expr)) {
        return gerarArrayLiteral(arr);
    }

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

std::string GeradorIR::gerarExpressaoBinaria(const std::shared_ptr<ExpressaoBinaria> &expr) {

    std::string lhs = gerarExpressao(expr->esquerda);
    std::string rhs = gerarExpressao(expr->direita);

    std::string tipoEsq = expr->esquerda->tipoInferido;
    std::string tipoDir = expr->direita->tipoInferido;

    std::string tipoOp = expr->esquerda->tipoInferido;

    if (expr->operador == "+") {

        // Caso 1: String + String
        if (tipoEsq == "string" && tipoDir == "string") {
            std::string temp = novoTemp();
            codigo << temp << " = call i8* @str_concat(i8* " << lhs << ", i8* " << rhs << ")\n";
            return temp;
        }

        // Caso 2: String + Char
        if (tipoEsq == "string" && tipoDir == "char") {
            std::string tempCharStr = novoTemp();
            codigo << tempCharStr << " = call i8* @char_to_str(i8 " << rhs << ")\n";

            std::string tempRes = novoTemp();
            codigo << tempRes << " = call i8* @str_concat(i8* " << lhs << ", i8* " << tempCharStr << ")\n";
            return tempRes;
        }

        if (tipoEsq == "char" && tipoDir == "string") {
            std::string tempCharStr = novoTemp();
            codigo << tempCharStr << " = call i8* @char_to_str(i8 " << lhs << ")\n";

            std::string tempRes = novoTemp();
            codigo << tempRes << " = call i8* @str_concat(i8* " << tempCharStr << ", i8* " << rhs << ")\n";
            return tempRes;
        }
    }

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
        else if (expr->operador == "%")
            codigo << temp << " = srem " << tipoL << " " << lhs << ", " << rhs << "\n";
    }

    return temp;
}

// -----------------------------------------------------------
// Retorno
// -----------------------------------------------------------

std::string GeradorIR::gerarComandoRetorno(const std::shared_ptr<ComandoRetorno> &cmdRet) {
    if (cmdRet->expressao) {
        std::string valor = gerarExpressao(cmdRet->expressao);

        std::string tipoAST = cmdRet->expressao->tipoInferido;
        std::string tipoLLVM = obterTipoLLVM(tipoAST);


        codigo << "ret " << tipoLLVM << " " << valor << "\n";
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

    if (auto atArr = std::dynamic_pointer_cast<ComandoAtribuicaoArray>(cmd))
        return gerarComandoAtribuicaoArray(atArr);

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

std::string GeradorIR::gerarExpressaoRelacional(
    const std::shared_ptr<ExpressaoRelacional> &expr)
{
    std::string lhs = gerarExpressao(expr->esquerda);
    std::string rhs = gerarExpressao(expr->direita);

    std::string tipoEsq = expr->esquerda->tipoInferido;
    std::string tipoDir = expr->direita->tipoInferido;

    std::string tipoOp = expr->esquerda->tipoInferido;
    std::string result = novoTemp();

    if (tipoEsq == "null" || tipoDir == "null") {
        std::string tipoPtr = (tipoEsq != "null") ? obterTipoLLVM(tipoEsq) : obterTipoLLVM(tipoDir);

        if (tipoEsq == "null" && tipoDir == "null") tipoPtr = "i8*";

        std::string op = (expr->operador == "==") ? "eq" : "ne";

        codigo << result << " = icmp " << op << " " << tipoPtr << " " << lhs << ", " << rhs << "\n";

        return result;
    }

    if (tipoOp == "string") {
        std::string callRes = novoTemp();
        codigo << callRes << " = call i32 @str_eq(i8* " << lhs << ", i8* " << rhs << ")\n";

        if (expr->operador == "==") {
            codigo << result << " = icmp ne i32 " << callRes << ", 0\n";
        }
        else if (expr->operador == "!=") {
            codigo << result << " = icmp eq i32 " << callRes << ", 0\n";
        }
        else {
            codigo << result << " = icmp ne i32 0, 0\n";
        }

        return result;
    }

    if (tipoOp == "float") {
        std::string op;
        if (expr->operador == "==") op = "oeq";
        else if (expr->operador == "!=") op = "one";
        else if (expr->operador == "<") op = "olt";
        else if (expr->operador == "<=") op = "ole";
        else if (expr->operador == ">") op = "ogt";
        else if (expr->operador == ">=") op = "oge";

        codigo << result << " = fcmp " << op << " double " << lhs << ", " << rhs << "\n";
        return result;
    }

    std::string op;
    if (expr->operador == "==") op = "eq";
    else if (expr->operador == "!=") op = "ne";
    else if (expr->operador == "<") op = "slt";
    else if (expr->operador == "<=") op = "sle";
    else if (expr->operador == ">") op = "sgt";
    else if (expr->operador == ">=") op = "sge";

    codigo << result << " = icmp " << op << " i32 " << lhs << ", " << rhs << "\n";

    return result;
}


std::string GeradorIR::gerarExpressaoIdentificador(const std::shared_ptr<ExpressaoIdentificador> &id) {
    std::string endereco = obterEnderecoVariavel(id->nome);

    if (!endereco.empty()) {
        std::string tipoAST = tss->obterTipo(id->nome);
        std::string tipoLLVM = obterTipoLLVM(tipoAST);

        std::string temp = novoTemp();

        codigo << temp << " = load " << tipoLLVM << ", " << tipoLLVM << "* " << endereco << ", align 4\n";

        return temp;
    }

    return "0";
}


std::string GeradorIR::gerarComandoAtribuicao(const std::shared_ptr<ComandoAtribuicao> &atrib) {

    std::string valorExpressao = gerarExpressao(atrib->expressao);

    if (atrib->nome.find('.') != std::string::npos) {
        size_t pos = atrib->nome.find('.');
        std::string objetoNome = atrib->nome.substr(0, pos);
        std::string membroNome = atrib->nome.substr(pos + 1);


        std::string allocaObj = obterEnderecoVariavel(objetoNome);

        if (!allocaObj.empty()) {
            std::string tipoObjAST = tss->obterTipo(objetoNome);

            int indice = obterIndiceCampo(tipoObjAST, membroNome);
            if (indice == -1) return "";

            std::string tipoStructPtr = "%" + tipoObjAST + "*";
            std::string tipoStructVal = "%" + tipoObjAST;


            std::string structPtr = novoTemp();
            codigo << structPtr << " = load " << tipoStructPtr << ", " << tipoStructPtr << "* " << allocaObj << "\n";


            std::string campoPtr = novoTemp();
            codigo << campoPtr << " = getelementptr " << tipoStructVal << ", "
                   << tipoStructPtr << " " << structPtr << ", i32 0, i32 " << indice << "\n";

            std::string tipoCampoLLVM = obterTipoLLVM(atrib->expressao->tipoInferido);

            if (atrib->operador == "=") {
                codigo << "store " << tipoCampoLLVM << " " << valorExpressao << ", "
                       << tipoCampoLLVM << "* " << campoPtr << "\n";
            }
            else {
                std::string oldVal = novoTemp();
                codigo << oldVal << " = load " << tipoCampoLLVM << ", " << tipoCampoLLVM << "* " << campoPtr << "\n";

                std::string newVal = novoTemp();
                bool isFloat = (tipoCampoLLVM == "double");
                std::string opLLVM = "";

                if (atrib->operador == "+=") opLLVM = isFloat ? "fadd" : "add";
                else if (atrib->operador == "-=") opLLVM = isFloat ? "fsub" : "sub";
                else if (atrib->operador == "*=") opLLVM = isFloat ? "fmul" : "mul";
                else if (atrib->operador == "/=") opLLVM = isFloat ? "fdiv" : "sdiv";

                codigo << newVal << " = " << opLLVM << " " << tipoCampoLLVM << " " << oldVal << ", " << valorExpressao << "\n";
                codigo << "store " << tipoCampoLLVM << " " << newVal << ", " << tipoCampoLLVM << "* " << campoPtr << "\n";
            }
        }
        return "";
    }


    std::string endereco = obterEnderecoVariavel(atrib->nome);

    if (!endereco.empty()) {
        std::string tipoAST = tss->obterTipo(atrib->nome);
        std::string tipoLLVM = obterTipoLLVM(tipoAST);

        bool isFloat = (tipoLLVM == "double");

        if (atrib->operador == "=") {
             codigo << "store " << tipoLLVM << " " << valorExpressao
                    << ", " << tipoLLVM << "* " << endereco << ", align 4\n";
        }
        else {
            std::string oldVal = novoTemp();
            codigo << oldVal << " = load " << tipoLLVM << ", " << tipoLLVM << "* " << endereco << "\n";

            std::string newVal = novoTemp();
            std::string opLLVM = "";

            if (atrib->operador == "+=") opLLVM = isFloat ? "fadd" : "add";
            else if (atrib->operador == "-=") opLLVM = isFloat ? "fsub" : "sub";
            else if (atrib->operador == "*=") opLLVM = isFloat ? "fmul" : "mul";
            else if (atrib->operador == "/=") opLLVM = isFloat ? "fdiv" : "sdiv";

            codigo << newVal << " = " << opLLVM << " " << tipoLLVM << " " << oldVal << ", " << valorExpressao << "\n";

            codigo << "store " << tipoLLVM << " " << newVal
                   << ", " << tipoLLVM << "* " << endereco << ", align 4\n";
        }

        if (auto arrLit = std::dynamic_pointer_cast<ExpressaoArrayLiteral>(atrib->expressao)) {
             tamanhosArraysLocais[atrib->nome] = (int)arrLit->elementos.size();
        }
    }

    return "";
}

// -----------------------------------------------------------
// ComandoIdent → call
// -----------------------------------------------------------

std::string GeradorIR::gerarComandoIdent(const std::shared_ptr<ComandoIdent> &cmdIdent) {


    if (cmdIdent->nome == "print") {
        if (cmdIdent->argumentos.size() != 1) {
            return "";
        }

        auto arg = cmdIdent->argumentos[0];
        std::string tipoArgLLVM = "i32";

        if (auto lit = std::dynamic_pointer_cast<ExpressaoLiteral>(arg)) {
            if (lit->tipo == "string") tipoArgLLVM = "i8*";
            else if (lit->tipo == "int") tipoArgLLVM = "i32";
            else if (lit->tipo == "float") tipoArgLLVM = "double";
            else if (lit->tipo == "char") tipoArgLLVM = "i8";
        } else if (auto id = std::dynamic_pointer_cast<ExpressaoIdentificador>(arg)) {
            tipoArgLLVM = obterTipoLLVM(id->tipoInferido);
        } else {
            tipoArgLLVM = obterTipoLLVM(std::dynamic_pointer_cast<NodoExpressao>(arg)->tipoInferido);
        }

        std::string argValue = gerarExpressao(arg);

        if (tipoArgLLVM == "i8*")
            codigo << "call void @print(i8* " << argValue << ")\n";
        else if (tipoArgLLVM == "i8") {
            codigo << "call void @print_char(i8 " << argValue << ")\n";
        }
        else if (tipoArgLLVM == "double") {
            codigo << "call void @print_float(double " << argValue << ")\n";
        }
        else
            codigo << "call void @print_int(" << tipoArgLLVM << " " << argValue << ")\n";

        return "";
    }

    std::string argumentosIR = "";

    for (const auto &arg : cmdIdent->argumentos) {
        std::string argValue = gerarExpressao(arg);
        std::string tipoLLVM = "i32";

        if (auto id = std::dynamic_pointer_cast<ExpressaoIdentificador>(arg))
            tipoLLVM = obterTipoLLVM(tss->obterTipo(id->nome));
        else
            tipoLLVM = obterTipoLLVM(arg->tipoInferido);

        argumentosIR += tipoLLVM + " " + argValue + ", ";
    }

    if (!argumentosIR.empty()) {
        argumentosIR.pop_back();
        argumentosIR.pop_back();
    }

    std::string nomeFuncaoLLVM = cmdIdent->nome;
    std::string argsCall = "";
    if (cmdIdent->nome.find('.') != std::string::npos) {
        size_t pos = cmdIdent->nome.find('.');
        std::string objetoNome = cmdIdent->nome.substr(0, pos);
        std::string metodoNome = cmdIdent->nome.substr(pos + 1);

        std::string tipoObjeto = tss->obterTipo(objetoNome);

        nomeFuncaoLLVM = tipoObjeto + "_" + metodoNome;

        auto nodoObj = std::make_shared<ExpressaoIdentificador>(objetoNome);


        std::string ptrObj = gerarExpressao(nodoObj);
        std::string tipoObjLLVM = "%" + tipoObjeto + "*";

        argsCall += tipoObjLLVM + " " + ptrObj;

        if (!argumentosIR.empty()) argsCall += ", ";

    }

    argsCall += argumentosIR;

    codigo << "call void @" << nomeFuncaoLLVM << "(" << argsCall << ")\n";

    return "";
}

std::string GeradorIR::gerarExpressaoChamada(const std::shared_ptr<ExpressaoChamada> &chamada) {

    // 1. Tratamento da função 'str'
    if (chamada->nome == "str") {
        if (chamada->argumentos.empty()) return "null";
        auto arg = chamada->argumentos[0];
        std::string valArg = gerarExpressao(arg);
        std::string tipoArg = arg->tipoInferido;
        std::string temp = novoTemp();
        if (tipoArg == "int") codigo << temp << " = call i8* @int_to_str(i32 " << valArg << ")\n";
        else if (tipoArg == "float") codigo << temp << " = call i8* @float_to_str(double " << valArg << ")\n";
        else if (tipoArg == "string") return valArg;
        else if (tipoArg == "char") codigo << temp << " = call i8* @char_to_str(i8 " << valArg << ")\n";
        else return "null";
        return temp;
    }

    auto structDef = tss->buscarStruct(chamada->nome);
    if (structDef) {
        std::string nomeStruct = chamada->nome;
        std::string tipoStruct = "%" + nomeStruct;
        std::string structPtr = novoTemp();
        codigo << structPtr << " = alloca " << tipoStruct << ", align 8\n";
        for (size_t i = 0; i < chamada->argumentos.size(); ++i) {
            std::string valorArg = gerarExpressao(chamada->argumentos[i]);
            std::string tipoCampo = obterTipoLLVM(chamada->argumentos[i]->tipoInferido);
            std::string campoPtr = novoTemp();
            codigo << campoPtr << " = getelementptr " << tipoStruct << ", " << tipoStruct << "* " << structPtr << ", i32 0, i32 " << i << "\n";
            codigo << "store " << tipoCampo << " " << valorArg << ", " << tipoCampo << "* " << campoPtr << "\n";
        }
        return structPtr;
    }


    std::string argumentosIR = "";
    for (const auto &arg : chamada->argumentos) {
        std::string argValue = gerarExpressao(arg);
        std::string tipoLLVM = obterTipoLLVM(arg->tipoInferido);
        argumentosIR += tipoLLVM + " " + argValue + ", ";
    }
    if (!argumentosIR.empty()) {
        argumentosIR.pop_back(); argumentosIR.pop_back();
    }

    std::string nomeFuncao = chamada->nome;
    std::string argsFinal = argumentosIR;

    if (chamada->nome.find('.') != std::string::npos) {
        size_t pos = chamada->nome.find('.');
        std::string objetoNome = chamada->nome.substr(0, pos);
        std::string metodoNome = chamada->nome.substr(pos + 1);

        std::string enderecoObj = obterEnderecoVariavel(objetoNome);

        if (!enderecoObj.empty()) {
            std::string tipoObjeto = tss->obterTipo(objetoNome);
            nomeFuncao = tipoObjeto + "_" + metodoNome;


            std::string tipoStructPtr = "%" + tipoObjeto + "*";
            std::string ptrThis = novoTemp();
            codigo << ptrThis << " = load " << tipoStructPtr << ", " << tipoStructPtr << "* " << enderecoObj << "\n";


            if (argsFinal.empty()) argsFinal = tipoStructPtr + " " + ptrThis;
            else argsFinal = tipoStructPtr + " " + ptrThis + ", " + argsFinal;
        }
    }

    std::string tipoASTRetorno = chamada->tipoInferido;

    if (tipoASTRetorno.empty() || tipoASTRetorno == "unknown") {
        std::string assinatura = tss->obterTipo(chamada->nome);
        size_t p = assinatura.find_last_of(':');
        if (p != std::string::npos) {
            tipoASTRetorno = assinatura.substr(p+1);
        } else {
            tipoASTRetorno = "void";
        }
    }

    std::string tipoRetornoLLVM = obterTipoLLVM(tipoASTRetorno);
    std::string temp = novoTemp();

    if (tipoRetornoLLVM != "void") {

        codigo << temp << " = call " << tipoRetornoLLVM << " @" << nomeFuncao << "(" << argsFinal << ")\n";
        return temp;
    } else {
        codigo << "call void @" << nomeFuncao << "(" << argsFinal << ")\n";
        return "";
    }
}


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


std::string GeradorIR::gerarExpressaoLogica(const std::shared_ptr<ExpressaoLogica> &logica) {

    std::string tipoL = "i1";


    std::string rotuloEvalLHS = novoRotulo();
    std::string rotuloRHS = novoRotulo();
    std::string rotuloMerge = novoRotulo();

    codigo << rotuloEvalLHS << ":\n";
    std::string lhs = gerarExpressao(logica->esquerda);

    if (logica->operador == "&&") {
        codigo << "br i1 " << lhs << ", label %" << rotuloRHS
               << ", label %" << rotuloMerge << "\n\n";
    } else {
        codigo << "br i1 " << lhs << ", label %" << rotuloMerge
               << ", label %" << rotuloRHS << "\n\n";
    }

    codigo << rotuloRHS << ":\n";
    std::string rhs = gerarExpressao(logica->direita);

    codigo << "br label %" << rotuloMerge << "\n\n";


    codigo << rotuloMerge << ":\n";

    std::string tempResultado = novoTemp();

    if (logica->operador == "&&") {
        codigo << tempResultado << " = phi " << tipoL
               << " [0, %" << rotuloEvalLHS << "], [" << rhs << ", %"
               << rotuloRHS << "]\n";
    } else {
        codigo << tempResultado << " = phi " << tipoL
               << " [1, %" << rotuloEvalLHS << "], [" << rhs << ", %"
               << rotuloRHS << "]\n";
    }

    return tempResultado;
}


std::string GeradorIR::gerarComandoLaco(const std::shared_ptr<ComandoLaco> &laco) {
    if (laco->tipo == ComandoLaco::TipoLaco::FOREACH) {
        std::string rotuloCond = "loop_cond" + std::to_string(contadorLaco);
        std::string rotuloBody = "loop_body" + std::to_string(contadorLaco);
        std::string rotuloInc  = "loop_inc"  + std::to_string(contadorLaco);
        std::string rotuloExit = "loop_exit" + std::to_string(contadorLaco++);

        std::string structPtr = gerarExpressao(laco->expressaoForeach);

        std::string tipoArray = laco->expressaoForeach->tipoInferido;
        if (tipoArray == "string") {
            std::string strBasePtr = gerarExpressao(laco->expressaoForeach);

            std::string indexPtr = novoTemp();
            codigo << indexPtr << " = alloca i32, align 4\n";
            codigo << "store i32 0, i32* " << indexPtr << "\n";

            std::string rotuloCond = "loop_cond" + std::to_string(contadorLaco);
            std::string rotuloBody = "loop_body" + std::to_string(contadorLaco);
            std::string rotuloInc  = "loop_inc"  + std::to_string(contadorLaco);
            std::string rotuloExit = "loop_exit" + std::to_string(contadorLaco++);

            codigo << "br label %" << rotuloCond << "\n\n";

            codigo << rotuloCond << ":\n";
            std::string currI = novoTemp();
            codigo << currI << " = load i32, i32* " << indexPtr << "\n";

            std::string charAddr = novoTemp();
            codigo << charAddr << " = getelementptr i8, i8* " << strBasePtr << ", i32 " << currI << "\n";

            std::string charVal = novoTemp();
            codigo << charVal << " = load i8, i8* " << charAddr << "\n";

            std::string isEnd = novoTemp();
            codigo << isEnd << " = icmp ne i8 " << charVal << ", 0\n";

            codigo << "br i1 " << isEnd << ", label %" << rotuloBody << ", label %" << rotuloExit << "\n\n";

            codigo << rotuloBody << ":\n";

            std::string varUserAddr = variaveisLocais.at(laco->identificadorForeach);
            codigo << "store i8 " << charVal << ", i8* " << varUserAddr << "\n";

            tss->declarar(laco->identificadorForeach, "char", laco->posicao.linha, true);

            for (auto &cmd : laco->corpo)
                gerarComando(std::dynamic_pointer_cast<NodoComando>(cmd));

            codigo << "br label %" << rotuloInc << "\n\n";

            codigo << rotuloInc << ":\n";
            std::string tmpI = novoTemp();
            codigo << tmpI << " = load i32, i32* " << indexPtr << "\n";
            std::string nextI = novoTemp();
            codigo << nextI << " = add i32 " << tmpI << ", 1\n";
            codigo << "store i32 " << nextI << ", i32* " << indexPtr << "\n";
            codigo << "br label %" << rotuloCond << "\n\n";

            codigo << rotuloExit << ":\n";
            return "";
        }
        std::string tipoBaseLLVM = obterTipoBaseDoArray(tipoArray);
        std::string structType = "{ i32, " + tipoBaseLLVM + "* }";

        std::string sizePtr = novoTemp();
        codigo << sizePtr << " = getelementptr " << structType << ", "
                << structType << "* " << structPtr << ", i32 0, i32 0\n";

        std::string tamanhoDin = novoTemp();
        codigo << tamanhoDin << " = load i32, i32* " << sizePtr << "\n";

        std::string indexPtr = novoTemp();
        codigo << indexPtr << " = alloca i32, align 4\n";
        codigo << "store i32 0, i32* " << indexPtr << "\n";

        codigo << "br label %" << rotuloCond << "\n\n";

        codigo << rotuloCond << ":\n";
        std::string currIndex = novoTemp();
        codigo << currIndex << " = load i32, i32* " << indexPtr << "\n";

        std::string cmp = novoTemp();
        codigo << cmp << " = icmp slt i32 " << currIndex << ", " << tamanhoDin << "\n";
        codigo << "br i1 " << cmp << ", label %" << rotuloBody << ", label %" << rotuloExit << "\n\n";

        codigo << rotuloBody << ":\n";

        std::string dataFieldPtr = novoTemp();
        codigo << dataFieldPtr << " = getelementptr " << structType << ", "
                << structType << "* " << structPtr << ", i32 0, i32 1\n";

        std::string rawDataPtr = novoTemp();
        codigo << rawDataPtr << " = load " << tipoBaseLLVM << "*, "
                << tipoBaseLLVM << "** " << dataFieldPtr << "\n";

        std::string elemPtr = novoTemp();
        codigo << elemPtr << " = getelementptr " << tipoBaseLLVM << ", "
                << tipoBaseLLVM << "* " << rawDataPtr << ", i32 " << currIndex << "\n";

        std::string elemVal = novoTemp();
        codigo << elemVal << " = load " << tipoBaseLLVM << ", "
                << tipoBaseLLVM << "* " << elemPtr << "\n";

        std::string varUserAddr = variaveisLocais.at(laco->identificadorForeach);
        codigo << "store " << tipoBaseLLVM << " " << elemVal << ", "
                << tipoBaseLLVM << "* " << varUserAddr << "\n";

        size_t posBracket = tipoArray.find('[');
        std::string tipoBaseAST = "int";
        if (posBracket != std::string::npos) {
            tipoBaseAST = tipoArray.substr(0, posBracket);
        }
        tss->declarar(laco->identificadorForeach, tipoBaseAST, laco->posicao.linha, true);

        for (auto &cmd : laco->corpo)
            gerarComando(std::dynamic_pointer_cast<NodoComando>(cmd));

        codigo << "br label %" << rotuloInc << "\n\n";

        codigo << rotuloInc << ":\n";
        std::string tmpVal = novoTemp();
        codigo << tmpVal << " = load i32, i32* " << indexPtr << "\n";
        std::string tmpInc = novoTemp();
        codigo << tmpInc << " = add i32 " << tmpVal << ", 1\n";
        codigo << "store i32 " << tmpInc << ", i32* " << indexPtr << "\n";
        codigo << "br label %" << rotuloCond << "\n\n";

        codigo << rotuloExit << ":\n";
        return "";
    }

    std::string rotuloCond = "loop_cond" + std::to_string(contadorLaco);
    std::string rotuloBody = "loop_body" + std::to_string(contadorLaco);
    std::string rotuloInc  = "loop_inc"  + std::to_string(contadorLaco);
    std::string rotuloExit = "loop_exit" + std::to_string(contadorLaco++);


    if (laco->inicializacao)
        gerarComando(laco->inicializacao);

    codigo << "br label %" << rotuloCond << "\n\n";

    // condição
    codigo << rotuloCond << ":\n";
    std::string cond = gerarExpressao(laco->condicao);
    codigo << "br i1 " << cond
           << ", label %" << rotuloBody
           << ", label %" << rotuloExit << "\n\n";

    // corpo
    codigo << rotuloBody << ":\n";
    for (auto &cmd : laco->corpo)
        gerarComando(std::dynamic_pointer_cast<NodoComando>(cmd));
    codigo << "br label %" << rotuloInc << "\n\n";

    // incremento
    codigo << rotuloInc << ":\n";
    if (laco->incremento)
        gerarComando(laco->incremento);
    codigo << "br label %" << rotuloCond << "\n\n";

    codigo << rotuloExit << ":\n";

    return "";
}

std::string GeradorIR::gerarDeclaracaoFuncao(const std::shared_ptr<DeclaracaoFuncao> &df, std::string nomeStruct) {

    tss->entrarEscopo();

    auto varsLocaisAntigas = variaveisLocais;
    auto tamanhosAntigos = tamanhosArraysLocais;

    variaveisLocais.clear();
    tamanhosArraysLocais.clear();

    std::stringstream codigoAlocacoes;
    std::stringstream codigoParametros;

    std::string tipoRetornoLLVM = obterTipoLLVM(df->tipoRetorno);

    std::string paramsLLVM = "";

    if (!nomeStruct.empty()) {
        std::string tipoThis = "%" + nomeStruct + "*";
        paramsLLVM += tipoThis + " %this, ";
    }

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
    if (!nomeStruct.empty()) {
        std::string tipoThis = "%" + nomeStruct + "*";
        std::string allocaName = "%this.addr";

        codigoAlocacoes << allocaName << " = alloca " << tipoThis << ", align 8\n";
        codigoParametros << "store " << tipoThis << " %this, " << tipoThis << "* " << allocaName << ", align 8\n";

        variaveisLocais["this"] = allocaName;
        tss->declarar("this", nomeStruct, df->posicao.linha, true);
    }

    for (const auto &[nomeParam, tipoAST] : df->parametros) {

        std::string tipoParam = obterTipoLLVM(tipoAST);
        std::string allocaName = "%" + nomeParam + ".addr";

        codigoAlocacoes << allocaName << " = alloca "
                        << tipoParam << ", align 4\n";

        codigoParametros << "store " << tipoParam << " %" << nomeParam
                         << ", " << tipoParam << "* " << allocaName
                         << ", align 4\n";

        variaveisLocais[nomeParam] = allocaName;

        tss->declarar(nomeParam, tipoAST, df->posicao.linha, true);
    }

    alocarVariaveisLocais(df->corpo, codigoAlocacoes);

    codigo << codigoAlocacoes.str();
    codigo << codigoParametros.str();

    for (const auto &cmd : df->corpo)
        gerarComando(std::dynamic_pointer_cast<NodoComando>(cmd));

    if (df->tipoRetorno == "void")
        codigo << "ret void\n";

    codigo << "}\n\n";

    variaveisLocais = varsLocaisAntigas;
    tamanhosArraysLocais = tamanhosAntigos;
    tss->sairEscopo();

    return "";
}

std::string GeradorIR::gerarLiteralString(const std::string &valorBruto) {
    std::string valor = valorBruto;
    if (valor.size() >= 2 && valor.front() == '"' && valor.back() == '"') {
        valor = valor.substr(1, valor.size() - 2);
    }


    auto itIntern = internedStrings.find(valor);
    if (itIntern != internedStrings.end()) {
        return itIntern->second;
    }

    std::string nome = ".str" + std::to_string(contadorStrings++);

    std::string conteudo = "c\"";
    for (unsigned char uc : valor) {
        char c = static_cast<char>(uc);
        if (c == '\\') {
            conteudo += "\\\\";
        } else if (c == '\n') {
            conteudo += "\\0A";
        } else if (c == '\t') {
            conteudo += "\\09";
        } else if (c == '\"') {
            conteudo += "\\22";
        } else {

            conteudo += c;
        }
    }
    conteudo += "\\00\"";

    int tamanho = (int)valor.size() + 1;

    std::stringstream ss;
    ss << "@" << nome << " = private unnamed_addr constant [" << tamanho << " x i8] " << conteudo;
    globaisString.push_back(ss.str());

    internedStrings[valor] = nome;
    tamanhoLiteralPorNome[nome] = tamanho;

    return nome;
}

std::string GeradorIR::gerarArrayLiteral(const std::shared_ptr<ExpressaoArrayLiteral> &arr) {
    int n = arr->elementos.size();

    std::string tipoArrayTotal = arr->tipoInferido;
    std::string tipoBaseAST = tipoArrayTotal.substr(0, tipoArrayTotal.size() - 2);
    std::string tipoBaseLLVM = obterTipoLLVM(tipoBaseAST);

    std::string dadosPtr = novoTemp();
    codigo << dadosPtr << " = alloca [" << n << " x " << tipoBaseLLVM << "], align 4\n";

    std::string ptrDecayed = novoTemp();
    codigo << ptrDecayed << " = getelementptr [" << n << " x " << tipoBaseLLVM << "], ["
           << n << " x " << tipoBaseLLVM << "]* " << dadosPtr << ", i32 0, i32 0\n";

    for (int i = 0; i < n; i++) {
        std::string valor = gerarExpressao(arr->elementos[i]);
        std::string elemPtr = novoTemp();
        codigo << elemPtr << " = getelementptr " << tipoBaseLLVM << ", "
               << tipoBaseLLVM << "* " << ptrDecayed << ", i32 " << i << "\n";
        codigo << "store " << tipoBaseLLVM << " " << valor << ", " << tipoBaseLLVM << "* " << elemPtr << "\n";
    }

    std::string structType = "{ i32, " + tipoBaseLLVM + "* }";

    std::string structPtr = novoTemp();
    codigo << structPtr << " = alloca " << structType << ", align 8\n";

    std::string sizePtr = novoTemp();
    codigo << sizePtr << " = getelementptr " << structType << ", " << structType << "* " << structPtr << ", i32 0, i32 0\n";
    codigo << "store i32 " << n << ", i32* " << sizePtr << "\n";

    std::string dataFieldPtr = novoTemp();
    codigo << dataFieldPtr << " = getelementptr " << structType << ", " << structType << "* " << structPtr << ", i32 0, i32 1\n";
    codigo << "store " << tipoBaseLLVM << "* " << ptrDecayed << ", " << tipoBaseLLVM << "** " << dataFieldPtr << "\n";

    return structPtr;
}

std::string GeradorIR::gerarExpressaoAcessoArray(const std::shared_ptr<ExpressaoAcessoArray>& acc) {
    std::string structPtr = gerarExpressao(acc->arrayBase);
    std::string indexVal = gerarExpressao(acc->indice);

    std::string tipoArrayAST = acc->arrayBase->tipoInferido;

    if (tipoArrayAST == "string") {
        std::string charPtr = novoTemp();
        codigo << charPtr << " = getelementptr i8, i8* " << structPtr << ", i32 " << indexVal << "\n";

        std::string val = novoTemp();
        codigo << val << " = load i8, i8* " << charPtr << "\n";
        return val;
    }

    std::string tipoBaseLLVM = "i32";

    size_t posBracket = tipoArrayAST.find('[');
    if (posBracket != std::string::npos) {
        std::string tipoBaseAST = tipoArrayAST.substr(0, posBracket);
        tipoBaseLLVM = obterTipoLLVM(tipoBaseAST);
    }

    std::string structType = "{ i32, " + tipoBaseLLVM + "* }";

    std::string dataFieldPtr = novoTemp();
    codigo << dataFieldPtr << " = getelementptr " << structType << ", " << structType << "* " << structPtr << ", i32 0, i32 1\n";

    std::string rawDataPtr = novoTemp();
    codigo << rawDataPtr << " = load " << tipoBaseLLVM << "*, " << tipoBaseLLVM << "** " << dataFieldPtr << "\n";

    std::string elemPtr = novoTemp();
    codigo << elemPtr << " = getelementptr " << tipoBaseLLVM << ", " << tipoBaseLLVM << "* " << rawDataPtr << ", i32 " << indexVal << "\n";

    std::string val = novoTemp();
    codigo << val << " = load " << tipoBaseLLVM << ", " << tipoBaseLLVM << "* " << elemPtr << "\n";

    return val;
}

void GeradorIR::gerarDefinicaoStructs(const std::vector<std::shared_ptr<NodoDeclaracao>>& declaracoes) {
    for (const auto& d : declaracoes) {
        if (auto ds = std::dynamic_pointer_cast<DeclaracaoStruct>(d)) {
            codigo << "%" << ds->nome << " = type { ";

            for (size_t i = 0; i < ds->atributos.size(); ++i) {
                if (auto dv = std::dynamic_pointer_cast<DeclaracaoVariavel>(ds->atributos[i])) {
                    codigo << obterTipoLLVM(dv->tipo);
                    if (i < ds->atributos.size() - 1) codigo << ", ";
                }
            }
            codigo << " }\n";
        }
    }
    codigo << "\n";
}


int GeradorIR::obterIndiceCampo(const std::string& nomeStruct, const std::string& nomeCampo) {
    auto defStruct = tss->buscarStruct(nomeStruct);

    if (!defStruct) return -1;


    for (int i = 0; i < defStruct->atributos.size(); ++i) {
        if (std::get<0>(defStruct->atributos[i]) == nomeCampo) {
            return i;
        }
    }

    return -1;
}

std::string GeradorIR::gerarExpressaoAcessoMembro(const std::shared_ptr<ExpressaoAcessoMembro>& expr) {

    std::string objPtr = gerarExpressao(expr->objeto);
    std::string tipoObj = expr->objeto->tipoInferido;

    if (tipoObj.find('[') != std::string::npos) {

        if (expr->membro == "length" || expr->membro == "tamanho") {

           std::string tipoStruct = obterTipoLLVM(tipoObj);

            std::string tipoBase = tipoStruct;
            if (tipoBase.back() == '*') tipoBase.pop_back();

            std::string sizePtr = novoTemp();
            codigo << sizePtr << " = getelementptr " << tipoBase << ", "
                    << tipoStruct << " " << objPtr << ", i32 0, i32 0\n";

            std::string tamanhoVal = novoTemp();
            codigo << tamanhoVal << " = load i32, i32* " << sizePtr << "\n";

            return tamanhoVal;
        }
        return "0";
    }


    int indice = obterIndiceCampo(tipoObj, expr->membro);

    if (indice != -1) {
        std::string nomeStruct = tipoObj;
        std::string tipoStructPtr = "%" + nomeStruct + "*";
        std::string tipoStructVal = "%" + nomeStruct;

        std::string campoPtr = novoTemp();
        codigo << campoPtr << " = getelementptr " << tipoStructVal << ", "
               << tipoStructPtr << " " << objPtr << ", i32 0, i32 " << indice << "\n";

        std::string val = novoTemp();

        std::string tipoCampoLLVM = obterTipoLLVM(expr->tipoInferido);

        codigo << val << " = load " << tipoCampoLLVM << ", "
               << tipoCampoLLVM << "* " << campoPtr << "\n";

        return val;
    }

    return "0";
}

std::string GeradorIR::gerarComandoAtribuicaoArray(const std::shared_ptr<ComandoAtribuicaoArray>& cmd) {

    std::string valorExpressao = gerarExpressao(cmd->expressao);
    std::string idxVal = gerarExpressao(cmd->indice);

    if (tss->obterTipo(cmd->nome) == "string") {
        std::string enderecoStr = obterEnderecoVariavel(cmd->nome);
        if (enderecoStr.empty()) return "";

        std::string strPtr = novoTemp();
        codigo << strPtr << " = load i8*, i8** " << enderecoStr << "\n";

        std::string charPtr = novoTemp();
        codigo << charPtr << " = getelementptr i8, i8* " << strPtr << ", i32 " << idxVal << "\n";

        codigo << "store i8 " << valorExpressao << ", i8* " << charPtr << "\n";
        return "";
    }

    std::string structPtr = "";
    std::string tipoBaseLLVM = "i32";


    if (cmd->nome.find('.') != std::string::npos) {
        size_t pos = cmd->nome.find('.');
        std::string objetoNome = cmd->nome.substr(0, pos);
        std::string membroNome = cmd->nome.substr(pos + 1);

        std::string allocaObj = obterEnderecoVariavel(objetoNome);
        if (allocaObj.empty()) return "";

        std::string tipoObjAST = tss->obterTipo(objetoNome);


        int indiceCampo = obterIndiceCampo(tipoObjAST, membroNome);
        if (indiceCampo == -1) return "";


        std::string tipoStructPtr = "%" + tipoObjAST + "*";
        std::string tipoStructVal = "%" + tipoObjAST;


        std::string ptrPai = novoTemp();
        codigo << ptrPai << " = load " << tipoStructPtr << ", " << tipoStructPtr << "* " << allocaObj << "\n";

        std::string campoPtr = novoTemp();
        codigo << campoPtr << " = getelementptr " << tipoStructVal << ", "
               << tipoStructPtr << " " << ptrPai << ", i32 0, i32 " << indiceCampo << "\n";

        if (cmd->expressao->tipoInferido != "unknown") {
             tipoBaseLLVM = obterTipoLLVM(cmd->expressao->tipoInferido);
        } else {
        }

        std::string tipoArrayStruct = "{ i32, " + tipoBaseLLVM + "* }*";

        structPtr = novoTemp();
        codigo << structPtr << " = load " << tipoArrayStruct << ", " << tipoArrayStruct << "* " << campoPtr << "\n";
    }

    else {
        std::string endereco = obterEnderecoVariavel(cmd->nome);

        if (!endereco.empty()) {
            std::string tipoArray = tss->obterTipo(cmd->nome);
            tipoBaseLLVM = obterTipoBaseDoArray(tipoArray);

            std::string structType = "{ i32, " + tipoBaseLLVM + "* }";
            structPtr = novoTemp();

            codigo << structPtr << " = load " << structType << "*, " << structType << "** " << endereco << "\n";
        }
        else {
            return "";
        }
    }

    std::string structType = "{ i32, " + tipoBaseLLVM + "* }";

    std::string dataFieldPtr = novoTemp();
    codigo << dataFieldPtr << " = getelementptr " << structType << ", "
           << structType << "* " << structPtr << ", i32 0, i32 1\n";

    std::string rawDataPtr = novoTemp();
    codigo << rawDataPtr << " = load " << tipoBaseLLVM << "*, "
           << tipoBaseLLVM << "** " << dataFieldPtr << "\n";

    std::string elemPtr = novoTemp();
    codigo << elemPtr << " = getelementptr " << tipoBaseLLVM << ", "
           << tipoBaseLLVM << "* " << rawDataPtr << ", i32 " << idxVal << "\n";


    bool isFloat = (tipoBaseLLVM == "double" || tipoBaseLLVM == "float");

    if (cmd->operador == "=") {
        codigo << "store " << tipoBaseLLVM << " " << valorExpressao << ", "
               << tipoBaseLLVM << "* " << elemPtr << "\n";
    }
    else {
        // Lê valor antigo
        std::string oldVal = novoTemp();
        codigo << oldVal << " = load " << tipoBaseLLVM << ", " << tipoBaseLLVM << "* " << elemPtr << "\n";

        // Calcula novo
        std::string newVal = novoTemp();
        std::string opLLVM = "";

        if (cmd->operador == "+=") opLLVM = isFloat ? "fadd" : "add";
        else if (cmd->operador == "-=") opLLVM = isFloat ? "fsub" : "sub";
        else if (cmd->operador == "*=") opLLVM = isFloat ? "fmul" : "mul";
        else if (cmd->operador == "/=") opLLVM = isFloat ? "fdiv" : "sdiv";

        codigo << newVal << " = " << opLLVM << " " << tipoBaseLLVM << " " << oldVal << ", " << valorExpressao << "\n";

        // Guarda novo
        codigo << "store " << tipoBaseLLVM << " " << newVal << ", " << tipoBaseLLVM << "* " << elemPtr << "\n";
    }

    return "";
}

std::string GeradorIR::obterEnderecoVariavel(const std::string& nome) {
    if (variaveisLocais.count(nome)) {
        return variaveisLocais.at(nome);
    }

    if (globais.count(nome)) {
        return "@" + nome;
    }

    return "";
}
