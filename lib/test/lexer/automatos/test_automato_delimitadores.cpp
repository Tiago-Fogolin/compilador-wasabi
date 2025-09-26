#include "automato_tester/AutomatoTester.hpp"
#include "automatos/AutomatoDelimitadores.hpp"
#include "test/lexer_test/TestAutomatoDelimitadores.hpp"


void testarDelimitadores() {
    AutomatoDelimitadores automato;
    AutomatoTester tester(automato);

    std::vector<std::pair<std::string,bool>> testes = {
        {"(", true},
        {")", true},
        {"[", true},
        {"]", true},
        {"{", true},
        {"}", true},
        {",", true},
        {";", true},
        {":", true},
        {".", true},
        {"()", false},
        {"[}", false},
        {"abc", false},
        {"", false},      
    };

    std::cout << "Testando Automato de Delimitadores \n";
    tester.testar(testes);
}
