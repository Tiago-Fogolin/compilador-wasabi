#include "automato_tester/AutomatoTester.hpp"
#include "automatos/AutomatoPalavrasReservadas.hpp"
#include "test/lexer_test/TestAutomatoPalavrasReservadas.hpp"


void testarPalavrasReservadas() {
    AutomatoPalavrasReservadas automato;
    AutomatoTester tester(automato);

    std::vector<std::pair<std::string,bool>> testes = {
        {"this", true},
        {"interface", true},
        {"struct", true},
        {"implements", true},
        {"def", true},
        {"this", true},
        {"for", true},
        {"foreach", true},
        {"in", true},
        {"if", true},
        {"else", true},
        {"while", true},
        {"break", true},
        {"continue", true},
        {"return", true},
        {"int", true},
        {"float", true},
        {"string", true},
        {"bool", true},
        {"tuple", true},
        {"dict", true},
        {"set", true},
        {"void", true},
        {"null", true},
        {"fffff", false},
        {"", false},
        {"palavra_aleatoria", false},
        {"pspsppspsps", false},
        {"ifif", false},
        {"ifi", false},
        {"deff", false}
    };

    std::cout << "Testando Automato de PalavrasReservadas \n";
    tester.testar(testes);
}
