#include "automato_tester/AutomatoTester.hpp"
#include "automatos/AutomatoCientifico.hpp"
#include "test/lexer_test/TestAutomatoCientifico.hpp"

void testarCientificos() {
    AutomatoCientifico automato;
    AutomatoTester tester(automato);

    std::vector<std::pair<std::string, bool>> testes = {
        // números válidos
        {"1e10", true},
        {"2E+5", true},
        {"3.14e-2", true},
        {"0.001E3", true},
        {"123", false},        
        {"0", false},          
        {"3.", false},         
        {".5e2", false},       
        {"1e", false},         
        {"1e+", false},        
        {"1e-0", true},        
        {"6.022e23", true},
        {"e10", false}, 
        {"12e3.4", false},
        {"abc", false},
        {"", false}
    };

    std::cout << "Testando Automato de Cientifico\n";
    tester.testar(testes);
}
