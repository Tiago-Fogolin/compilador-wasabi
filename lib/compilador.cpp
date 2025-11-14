#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "include/lexer/Lexer.hpp"
#include "include/parser/Parser.hpp"
#include "include/semantic/AnalisadorSemantico.hpp"

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
        } else {
            std::cout << "Nenhum programa reconhecido.\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Erro ao analisar: " << e.what() << "\n";
    }

    return 0;
}
