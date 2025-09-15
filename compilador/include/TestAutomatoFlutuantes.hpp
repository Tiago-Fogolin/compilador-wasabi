#pragma once
#include "AutomatoTester.hpp"
#include "TokensAutomatos.hpp"

void testarFlutuantes() {
    Automato automato = criarAutomatoFlutuantes();
    AutomatoTester tester(automato);

    std::vector<std::pair<std::string,bool>> testes = {
        {"0", false}, 
        {"00000", false}, 
        {"000.43", false}, 
        {"7.4", true}, 
        {"42.21", true}, 
        {"123456.1234", true},
        {"56..34", false},
        {"23...23.52", false},
        {"", false}, 
        {"01.2", false}, 
        {"010.0", false}, 
        {"a123", false}, 
        {"123a4", false},
        {"palavra aleatoria", false}
    };

    std::cout << "Testando Automato de Flutuantes\n";
    tester.testar(testes);
}
