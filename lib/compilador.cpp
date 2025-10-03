#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include "include/lexer/Lexer.hpp"


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <caminho_do_arquivo>\n";
        return 1;
    }

    std::string caminhoArquivo = argv[1];
    std::ifstream arquivo(caminhoArquivo);
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << caminhoArquivo << "\n";
        return 1;
    }

    std::string texto((std::istreambuf_iterator<char>(arquivo)),
                      std::istreambuf_iterator<char>());

    Lexer lexer;

    std::unordered_map<std::string, std::vector<std::string>> tokensAceitos = lexer.analisarTexto(texto);

    
    for (const auto& [tipo, listaTokens] : tokensAceitos) {
        if(tipo == "WHITESPACE") continue;
        std::cout << "TokenType: " << tipo << "\n";
        for (const auto& token : listaTokens) {
            std::cout << "  " << token << "\n";
        }
    }


    return 0;
}