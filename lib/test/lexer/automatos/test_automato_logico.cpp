#include "automato_tester/AutomatoTester.hpp"
#include "automatos/AutomatoLogico.hpp"
#include "test/lexer_test/TestAutomatoLogico.hpp"

void testarLogicos() {
    AutomatoLogico automato;
    AutomatoTester tester(automato);

    std::vector<std::pair<std::string, bool>> testes = {
        {"&&", true},
        {"||", true},
        {"!", true},
        {"&", false},
        {"|", false},
        {"!!", false},
        {"", false},
        {"palavra", false}
    };

    std::cout << "Testando Automato de Lógicos\n";
    tester.testar(testes);
}
