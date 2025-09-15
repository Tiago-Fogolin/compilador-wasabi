#pragma once
#include "AutomatoTester.hpp"
#include "TokensAutomatos.hpp"

void testarBooleanos() {
    Automato automato = criarAutomatoBooleanos();
    AutomatoTester tester(automato);

    std::vector<std::pair<std::string,bool>> testes = {
        {"false", true}, 
        {"true", true}, 
        {"trues", false}, 
        {"falses", false}, 
        {"0true", false},
        {"", false}, 
        {"zxczczxc", false}, 
        {"0000false", false}, 
        {"nottrue", false}, 
        {"fffff", false},
        {"palavra aleatoria", false}
    };

    std::cout << "Testando Automato de Booleanos\n";
    tester.testar(testes);
}
