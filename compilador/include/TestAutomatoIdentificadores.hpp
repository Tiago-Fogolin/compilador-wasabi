#pragma once
#include "AutomatoTester.hpp"
#include "TokensAutomatos.hpp"

void testarIdentificadores() {
    Automato automato = criarAutomatoIdentificadores();
    AutomatoTester tester(automato);

    std::vector<std::pair<std::string,bool>> testes = {
        {"abc123", true}, 
        {"nome_variavel", true}, 
        {"NomeClasse", true}, 
        {"_private_example", true}, 
        {"__super_private_shh", true},
        {"", false}, 
        {"0abc", false}, 
        {"****adsdsdsd", false}, 
        {"epo0394", true}, 
        {"abc+_=-", false},
        {"**+_+_+-", false}
    };

    std::cout << "Testando Automato de Identificadores\n";
    tester.testar(testes);
}
