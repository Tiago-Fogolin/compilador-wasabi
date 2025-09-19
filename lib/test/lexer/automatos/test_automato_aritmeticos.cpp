#include "automato_tester/AutomatoTester.hpp"
#include "automatos/AutomatoAritmeticos.hpp"
#include "test/lexer_test/TestAutomatoAritmeticos.hpp"

void testarAritmeticos() {
    AutomatoAritmeticos automato;
    AutomatoTester tester(automato);

    std::vector<std::pair<std::string, bool>> testes = {
        {"+", true},
        {"-", true},
        {"*", true},
        {"/", true},
        {"**", true},
        {"++", true},
        {"//", true},
        {"%", true},
        {"+++", false},
        {"---------", false},
        {"*****************************************", false},
        {"palavra", false},
        {"", false},
        {"aAAAAAAAAAAAAAAAAAAAAAAAAA", false}
    };

    std::cout << "Testando Automato de Aritmeticos\n";
    tester.testar(testes);
}
