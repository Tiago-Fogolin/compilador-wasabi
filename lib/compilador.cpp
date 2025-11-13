#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include "include/lexer/Lexer.hpp"
#include "include/parser/Parser.hpp"

constexpr size_t BUFFER_SIZE = 4096;

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

    Lexer lexer;
    std::string resto;
    size_t linhaBase = 1;

    std::vector<Token> tokensAceitos;

    char buffer[BUFFER_SIZE];

    while (arquivo.read(buffer, BUFFER_SIZE) || arquivo.gcount() > 0) {


        size_t bytesLidos = arquivo.gcount();
        std::string bloco = resto + std::string(buffer, bytesLidos);
        std::vector<Token> blocTokens = lexer.analisarTexto(bloco);

        tokensAceitos.insert(tokensAceitos.end(), blocTokens.begin(), blocTokens.end());

        if (!blocTokens.empty()) {
            const Token& ultimo = blocTokens.back();
            size_t posUltimo = bloco.find(ultimo.valor);
            if (posUltimo + ultimo.valor.size() < bloco.size()) {
                resto = bloco.substr(posUltimo + ultimo.valor.size());
            } else {
                resto.clear();
            }
        } else {
            resto = bloco;
        }


        linhaBase += std::count(bloco.begin(), bloco.end(), '\n');
    }


    if (!resto.empty()) {
        std::vector<Token> blocTokens = lexer.analisarTexto(resto);
        tokensAceitos.insert(tokensAceitos.end(), blocTokens.begin(), blocTokens.end());
    }

    Parser parser(tokensAceitos);
    
     try {
        auto programa = parser.analisarPrograma();

        if (programa) {
            std::cout << "=== AST ===\n";
            programa->imprimir(0);
        } else {
            std::cout << "Nenhum programa reconhecido.\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Erro ao analisar: " << e.what() << "\n";
    }
    /*
    for (const auto& token : tokensAceitos) {
        TokenType tipo = token.tipo;
        if(tipo == TokenType::WHITESPACE) continue;
        std::cout << "TokenType: " << tokenTypeToString(tipo) << "\n";
        std::cout << "  " << token.valor << "\n";
        
    }
    */


    return 0;
}