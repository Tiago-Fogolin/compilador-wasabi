#include "automato_tester/AutomatoTester.hpp"
#include "automatos/AutomatoComentarios.hpp"
#include "test/lexer_test/TestAutomatoComentarios.hpp"


void testarComentarios() {
    AutomatoComentarios automato;
    AutomatoTester tester(automato);

    std::vector<std::pair<std::string,bool>> testes = {
        {"// comentario simples", true},
        {"//12345", true},
        {"// com simbbolos !@#$%", true},
        {"//", true},
        {"//\n", false},
        {"/* comentario simples */", true},
        {"/* bloco \n com multiplas linhas */", true},
        {"/* bloco /* aninhado */ */", false},
        {"/* bloco sem fechar", false},
        {"/* */", true},
        {"/", false},
        {"/*", false},
        {"abc", false},
        {"", false}
    };

    std::cout << "Testando Automato de Comentários \n";
    tester.testar(testes);
}
