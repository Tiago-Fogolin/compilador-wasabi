#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "include/lexer/Lexer.hpp"
#include "include/parser/Parser.hpp"
#include "include/semantic/AnalisadorSemantico.hpp"
#include <iterator>
#include <stdexcept>
#include <codegen/GeradorIR.hpp>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <caminho_do_arquivo>\n";
        return 1;
    }

    std::string caminhoArquivo = argv[1];
    std::ifstream arquivo(caminhoArquivo, std::ios::binary);
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << caminhoArquivo << "\n";
        return 1;
    }

    std::string codigo(
        (std::istreambuf_iterator<char>(arquivo)),
        std::istreambuf_iterator<char>()
    );

    Lexer lexer;
    std::vector<Token> tokensAceitos = lexer.analisarTexto(codigo);

    Parser parser(tokensAceitos);

    try {
        auto programa = parser.analisarPrograma();

        if (programa) {
            std::cout << "=== AST ===\n";
            programa->imprimir(0);

            AnalisadorSemantico semantico;
            semantico.analisar(programa);

            const auto& erros_semanticos = semantico.obterErros();

            if (!erros_semanticos.empty()) {
                std::cerr << "\n=== Erros Semanticos (" << erros_semanticos.size() << ") ===\n";
                for (const auto& erro : erros_semanticos) {
                    std::cerr << "[Linha " << erro.linha << "] Erro: " << erro.mensagem << "\n";
                }
                return 2;
            } else {
                std::cout << "\nAnalise semantica concluida sem erros.\n";

                TabelaSimbolos* tss = semantico.obterTabelaSimbolos();
                GeradorIR gerador(tss);

                std::string codigoIR = gerador.gerar(programa);

                std::ofstream outfile("output.ll");
                if (outfile.is_open()) {
                    outfile << codigoIR;
                    outfile.close();
                    std::cout << "\n Geracao de codigo LLVM IR concluida com sucesso em 'output.ll'.\n";
                } else {
                    std::cerr << "\n ERRO: Nao foi possivel abrir 'output.ll' para escrita.\n";
                    return 3;
                }
            }
        } else {
            std::cout << "Nenhum programa reconhecido.\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Erro ao analisar: " << e.what() << "\n";
    }

    return 0;
}
