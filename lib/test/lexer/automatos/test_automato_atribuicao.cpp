#include "automato_tester/AutomatoTester.hpp"
#include "automatos/AutomatoAtribuicao.hpp"
#include "test/lexer_test/TestAutomatoAtribuicao.hpp"

void testarAtribuicao() {
    AutomatoAtribuicao automato;
    AutomatoTester tester(automato);

    std::vector<std::pair<std::string, bool>> testes = {
        {"=", true},
        {"+=", true},
        {"-=", true},
        {"*=", true},
        {"/=", true},
        {"%=", true},
        {"==", false},
        {"++", false},
        {"--", false},
        {"", false},
        {"palavra", false}
    };

    std::cout << "Testando Automato de Atribuicao\n";
    tester.testar(testes);
}
