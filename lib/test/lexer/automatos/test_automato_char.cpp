#include "automato_tester/AutomatoTester.hpp"
#include "automatos/AutomatoChar.hpp"
#include "test/lexer_test/TestAutomatoChar.hpp"

void testarChar() {
    AutomatoChar automato;
    AutomatoTester tester(automato);

    std::vector<std::pair<std::string,bool>> testes = {
        {"'a'", true},           
        {"'Z'", true},           
        {"'0'", true},           
        {"' '", true},           
        {"'\\n'", true},                  
        {"'\\''", true},         
        {"'\\\\'", true},      
        {"''", false},          
        {"'aa'", false},         
        {"a", false},            
    };

    std::cout << "Testando Automato de Char\n";
    tester.testar(testes);
}
