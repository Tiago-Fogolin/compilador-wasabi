#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <iterator>

#include "include/lexer/Lexer.hpp"
#include "include/parser/Parser.hpp"
#include "include/semantic/AnalisadorSemantico.hpp"
#include "codegen/GeradorIR.hpp"


const char* CODIGO_RUNTIME_C = R"(
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura compatível com o LLVM { i32, i32* }
typedef struct {
    int size;
    int* data;
} IntArray;

void print_int(int x) { printf("%d\n", x); }
void print_float(double x) { printf("%f\n", x); }
void print(char* s) { printf("%s\n", s ? s : "null"); }

void print_char(char c) { printf("%c\n", c); }

void print_int_array(IntArray* arr) {
    if (!arr) { printf("null\n"); return; }
    printf("[");
    for (int i = 0; i < arr->size; i++) {
        printf("%d", arr->data[i]);
        if (i < arr->size - 1) printf(", ");
    }
    printf("]\n");
}

int str_eq(char* a, char* b) {
    if (!a || !b) return 0;
    while (*a && *b) {
        if (*a != *b) return 0;
        a++; b++;
    }
    return *a == *b;
}

char* str_concat(char* a, char* b) {
    if (!a) a = "";
    if (!b) b = "";
    int lenA = strlen(a);
    int lenB = strlen(b);
    char* result = (char*)malloc(lenA + lenB + 1);
    strcpy(result, a);
    strcat(result, b);
    return result;
}

char* int_to_str(int n) {
    char* buffer = (char*)malloc(12);
    if (!buffer) return "0";
    sprintf(buffer, "%d", n);
    return buffer;
}

char* float_to_str(double n) {
    char* buffer = (char*)malloc(64);
    if (!buffer) return "0.0";
    sprintf(buffer, "%g", n);
    return buffer;
}

char* char_to_str(char c) {
    char* str = (char*)malloc(2);
    if (!str) return "";
    str[0] = c;
    str[1] = '\0';
    return str;
}
)";

std::string lerArquivo(const std::string& caminho) {
    std::ifstream arquivo(caminho, std::ios::binary);
    if (!arquivo.is_open()) {
        throw std::runtime_error("Nao foi possivel abrir o arquivo: " + caminho);
    }
    return std::string((std::istreambuf_iterator<char>(arquivo)),
                        std::istreambuf_iterator<char>());
}

void salvarArquivo(const std::string& caminho, const std::string& conteudo) {
    std::ofstream outfile(caminho);
    if (!outfile.is_open()) {
        throw std::runtime_error("Nao foi possivel escrever no arquivo: " + caminho);
    }
    outfile << conteudo;
    outfile.close();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo.wsb> [-o nome_saida]\n";
        return 1;
    }

    try {
        // --- 1. Processamento de Argumentos ---
        std::string arquivoEntrada;
        std::string arquivoSaida = "programa.exe";

        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "-o") {
                if (i + 1 < argc) {
                    arquivoSaida = argv[i + 1];
                    i++;
                }
            } else {
                arquivoEntrada = arg;
            }
        }

        if (arquivoEntrada.empty()) throw std::runtime_error("Sem arquivo de entrada.");

        std::cout << "[1/5] Lendo arquivo...\n";
        std::string codigoFonte = lerArquivo(arquivoEntrada);

        Lexer lexer;
        auto tokens = lexer.analisarTexto(codigoFonte);

        Parser parser(tokens);
        auto programa = parser.analisarPrograma();
        if (!programa) throw std::runtime_error("Erro de Sintaxe.");

        std::cout << "[2/5] Analise Semantica...\n";
        AnalisadorSemantico semantico;
        semantico.analisar(programa);

        const auto& erros = semantico.obterErros();
        if (!erros.empty()) {
            std::cerr << "Erros Semanticos:\n";
            for (const auto& e : erros) std::cerr << "[Linha " << e.linha << "] " << e.mensagem << "\n";
            return 2;
        }

        std::cout << "[3/5] Gerando LLVM IR...\n";
        TabelaSimbolos* tss = semantico.obterTabelaSimbolos();
        tss->setEmitirAvisos(false);

        GeradorIR gerador(tss);
        std::string codigoIR = gerador.gerar(programa);

        salvarArquivo("output.ll", codigoIR);

        std::cout << "[4/5] Gerando Runtime...\n";
        salvarArquivo("print.c", CODIGO_RUNTIME_C);

        std::cout << "[5/5] Compilando executavel...\n";

        std::string comando = "clang output.ll print.c -o " + arquivoSaida;

        comando += " -Wno-deprecated-declarations";

        int resultado = std::system(comando.c_str());

        if (resultado != 0) {
            throw std::runtime_error("Erro no Clang.");
        }

        std::cout << "\nSucesso! Executavel: " << arquivoSaida << "\n";

        std::remove("output.ll");
        std::remove("print.c");

    } catch (const std::exception& e) {
        std::cerr << "ERRO: " << e.what() << "\n";
        return 3;
    }
    return 0;
}
