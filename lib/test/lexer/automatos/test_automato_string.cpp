#include "automato_tester/AutomatoTester.hpp"
#include "automatos/AutomatoString.hpp"
#include "test/lexer_test/TestAutomatoString.hpp"


void testarStrings() {
    AutomatoString automato;
    AutomatoTester tester(automato);

    std::vector<std::pair<std::string,bool>> testes = {
        {"\"aeiou\"", true},            
        {"\"aiaiai\\n\"", true},         
        {"\"\\\"teste\\\"\"", true},    
        {"\"\"", true},                 
        {"\"hello", false},             
        {"strings\"", false},             
        {"\"teste\\\"", false},         
    };

    std::cout << "Testando Automato de Strings \n";
    tester.testar(testes);
}
