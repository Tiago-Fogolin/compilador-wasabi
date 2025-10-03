#include "automato_tester/AutomatoTester.hpp"
#include "automatos/AutomatoLexer.hpp"
#include "test/lexer_test/TestAutomatoLexer.hpp"


void testarLexer() {
    AutomatoLexer automato;
    AutomatoTester tester(automato);

    // Lista de pares <string, aceita?>
    std::vector<std::pair<std::string,bool>> testes = {
        // -------------------
        // WHITESPACE
        // -------------------
        {" ", true}, {"\t", true}, {"\n", true}, {"\r", true}, {"  ", true},

        // -------------------
        // OPERATORS
        // -------------------
        {"+", true}, {"-", true}, {"*", true}, {"/", true},
        {"=", true}, {"==", true}, {"!=", true},
        {"&&", true}, {"||", true}, {"<<", true}, {">>", true},

        // -------------------
        // IDENTIFIERS
        // -------------------
        {"var", true}, {"_temp", true}, {"a123", true}, {"123abc", false},

        // -------------------
        // CHAR LITERAL
        // -------------------
        {"'a'", true}, {"'\\n'", true}, {"'\\''", true}, {"'ab'", false}, {"'\\", false},

        // -------------------
        // STRING LITERAL
        // -------------------
        {"\"hello\"", true}, {"\"world\\n\"", true}, {"\"\"", true},
        {"\"unterminated", false}, {"noquote\"", false},

        // -------------------
        // NUMBERS
        // -------------------
        {"0", true}, {"123", true}, {"0.5", true}, {"3.14", true},
        {"1e10", true}, {"2.5E-3", true}, {"01abc", false}, {"1.2.3", false},

        // -------------------
        // DELIMITERS
        // -------------------
        {"(", true}, {")", true}, {"[", true}, {"]", true},
        {"{", true}, {"}", true}, {",", true}, {";", true},
        {":", true}, {".", true},

        // COMENTARIOS
        {"// comentario simples", true},
        {"//12345", true},
        {"// com simbbolos !@#$%", true},
        {"//", true},
        {"//\n", false},
        {"/* comentario simples */", true},
        {"/* bloco \n com multiplas linhas */", true},
        {"/* bloco /* aninhado */ */", false},
        {"/* bloco sem fechar", false},
    };

    std::cout << "Testando Automato do Lexer\n";
    

    for (const auto& t : testes) {
        const std::string& s = t.first;
        bool esperado = t.second;

        bool aceito;
        int estadoFinal = automato.processarStringComEstadoFinal(s, aceito);

        std::cout << "Testando \"" << s << "\": " 
              << (aceito ? "ACEITO" : "REJEITADO")
              << " (esperado: " << (esperado ? "ACEITO" : "REJEITADO") << ")";


        if (aceito && estadoFinal != -1) {
            auto it = automato.estadoFinaisTipos.find(estadoFinal);
            if (it != automato.estadoFinaisTipos.end()) {
                std::cout << " (Tipo: " << tokenTypeToString(it->second) << ")";
            }
        }

        std::cout << "\n";
    }
}
