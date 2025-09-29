#include "../include/lexer/Lexer.hpp"
#include "../include/automato_base/Automato.hpp"
#include <iostream>

Lexer::Lexer() {
    this->palavrasReservadas = {
        "interface","struct","implements","def","this","for","foreach","in",
        "if","elif","else","while","break","continue","return",
        "int","float","string","bool","tuple","dict","set","void","null"
    };
}

void avancar(const std::string& texto, size_t& indice, size_t& linha, size_t qtd = 1) {
    for (size_t i = 0; i < qtd && indice < texto.size(); i++) {
        if (texto[indice] == '\n') linha++;
        indice++;
    }
}

bool ehEspaco(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

bool ehDelimitador(char c) {
    return c == ';' || c == '(' || c == ')' || c == ',' || 
           c == ':' || c == '[' || c == ']';
}

bool ehOperador(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '=' || c == '<' || c == '>';
}

// alguém com mais habilidade precisa reescrever isso aqui
/*
    Algumas coisas pra arrumar nessa gambiarra:
        - Precisa extrair número negativo, o automato não reconhece nem essa função extrai direito
        - Número científico ainda não ta extraindo
*/
std::string extrairProximoToken(const std::string& texto, size_t& indice, size_t& linha) {
    // 1. Ignorar espaços
    while (indice < texto.size() && ehEspaco(texto[indice])) {
        avancar(texto, indice, linha);
    }
    if (indice >= texto.size()) return "";

    size_t start = indice;

    // 2. Comentário de linha //
    if (texto[indice] == '/' && indice + 1 < texto.size() && texto[indice + 1] == '/') {
        avancar(texto, indice, linha, 2); // consome "//"
        while (indice < texto.size() && texto[indice] != '\n') {
            avancar(texto, indice, linha);
        }
        return texto.substr(start, indice - start);
    }

    // 3. Comentário de bloco /* ... */
    if (texto[indice] == '/' && indice + 1 < texto.size() && texto[indice + 1] == '*') {
        avancar(texto, indice, linha, 2); // consome "/*"
        while (indice + 1 < texto.size()) {
            if (texto[indice] == '*' && texto[indice + 1] == '/') {
                avancar(texto, indice, linha, 2);
                break;
            }
            avancar(texto, indice, linha);
        }
        return texto.substr(start, indice - start);
    }

    // 4. Identificador ou palavra reservada
    if (isalpha(texto[indice]) || texto[indice] == '_') {
        while (indice < texto.size() && (isalnum(texto[indice]) || texto[indice] == '_')) {
            avancar(texto, indice, linha);
        }
        return texto.substr(start, indice - start);
    }

    // 5. Número (inteiro ou float)
    if (isdigit(texto[indice])) {
        while (indice < texto.size() && isdigit(texto[indice])) {
            avancar(texto, indice, linha);
        }

        // Checa se vem letra depois do número → identificador inválido
        if (indice < texto.size() && (isalpha(texto[indice]) || texto[indice] == '_')) {
            std::cerr << "Erro: de sintaxe na linha " << linha 
                      << ": '" << texto.substr(start, 10) << "...'\n";
            
            while (indice < texto.size() && !ehEspaco(texto[indice]) && !ehDelimitador(texto[indice])) {
                avancar(texto, indice, linha);
            }
            return "";
        }

        // Ponto decimal
        if (indice < texto.size() && texto[indice] == '.' && (indice + 1 < texto.size() && isdigit(texto[indice+1]))) {
            avancar(texto, indice, linha); // consome ponto
            while (indice < texto.size() && isdigit(texto[indice])) {
                avancar(texto, indice, linha);
            }
        }

        return texto.substr(start, indice - start);
    }

    // 6. Delimitadores simples
    if (ehDelimitador(texto[indice])) {
        avancar(texto, indice, linha);
        return texto.substr(start, 1);
    }

    // 7. Ponto isolado (ex: obj.x)
    if (texto[indice] == '.') {
        avancar(texto, indice, linha);
        return ".";
    }

    // 8. Operadores
    if (ehOperador(texto[indice])) {
        size_t startOp = indice;
        
        if (indice + 1 < texto.size()) {
            std::string dois = texto.substr(indice, 2);
            if (dois == "**" || dois == "==" || dois == "<=" || dois == ">=" || dois == "!=") {
                avancar(texto, indice, linha, 2);
                return dois;
            }
        }

        std::string op(1, texto[indice]);
        avancar(texto, indice, linha);
        return op;
    }

    // 9. Outros símbolos
    std::cerr << "Erro: caractere inesperado '" << texto[indice] 
              << "' na linha " << linha << "\n";
    avancar(texto, indice, linha);
    return "";
}


std::unordered_map<std::string, std::vector<std::string>> Lexer::analisarTexto(std::string texto) {
    std::unordered_map<std::string, std::vector<std::string>> tokensAceitos;
    std::vector<std::pair<std::string, std::unique_ptr<Automato>>> automatos = AutomatoFactory::getAutomatos();

    size_t i = 0;
    size_t linha_atual = 1;
    while (i < texto.size()) {

        std::string token = extrairProximoToken(texto, i, linha_atual);
        if (token.empty()) continue;

        bool reconheceu = false;
        for (auto& [tipo, automato] : automatos) {
            if (automato->processarString(token)) {
                tokensAceitos[tipo].push_back(token);
                reconheceu = true;
                break;
            }
        }

        if (!reconheceu) {
            std::cerr << "Erro de sintaxe na linha " << linha_atual <<  ":'" << token << "'\n";
            return tokensAceitos;
        }
    }

    return tokensAceitos;
}

