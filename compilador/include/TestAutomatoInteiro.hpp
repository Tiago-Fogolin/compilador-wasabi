#pragma once
#include "AutomatoTester.hpp"
#include "TokensAutomatos.hpp"

void testarInteiros() {
    Automato automato = criarAutomatoInteiros();
    AutomatoTester tester(automato);

    std::vector<std::pair<std::string,bool>> testes = {
        {"0", true}, 
        {"00000", true}, 
        {"7", true}, 
        {"42", true}, 
        {"123456", true},
        {"", false}, 
        {"01", false}, 
        {"010", false}, 
        {"a123", false}, 
        {"123a4", false},
        {"palavra aleatoria", false}
    };

    std::cout << "Testando Automato de Inteiros\n";
    tester.testar(testes);
}
