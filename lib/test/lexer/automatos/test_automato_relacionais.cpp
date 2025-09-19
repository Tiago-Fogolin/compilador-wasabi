#include "automato_tester/AutomatoTester.hpp"
#include "automatos/AutomatoRelacionais.hpp"
#include "test/lexer_test/TestAutomatoRelacionais.hpp"

void testarRelacionais() {
    AutomatoRelacionais automato;
    AutomatoTester tester(automato);

    std::vector<std::pair<std::string, bool>> testes = {
        {"==", true},
        {"!=", true},
        {"<=", true},
        {">=", true},
        {"<", true},
        {">", true},
        {"=", false},
        {"!", false},
        {"<>", false},
        {"===", false},
        {"=>", false},
        {"", false},
        {"palavra", false}
    };

    std::cout << "Testando Automato de Racionais\n";
    tester.testar(testes);
}
