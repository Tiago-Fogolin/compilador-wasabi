#include "automato_tester/AutomatoTester.hpp"
#include "automatos/AutomatoBitwise.hpp"
#include "test/lexer_test/TestAutomatoBitwise.hpp"

void testarBitwise() {
    AutomatoBitwise automato;
    AutomatoTester tester(automato);

    std::vector<std::pair<std::string, bool>> testes = {
        {"&", true},
        {"|", true},
        {"^", true},
        {"~", true},
        {"<<", true},
        {">>", true},
        {"<", false},
        {">", false},
        {"&&", false},
        {"||", false},
        {"", false},
        {"palavra", false}
    };

    std::cout << "Testando Automato de Bitwise\n";
    tester.testar(testes);
}
