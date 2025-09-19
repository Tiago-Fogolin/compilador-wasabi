#include "automato_tester/AutomatoTester.hpp"
#include "automatos/AutomatoEspacos.hpp"
#include "test/lexer_test/TestAutomatoEspacos.hpp"

void testarEspacos() {
    AutomatoEspacos automato;
    AutomatoTester tester(automato);

    std::vector<std::pair<std::string,bool>> testes = {
        {" ", true},          
        {"\t", true},          
        {"\n", true},          
        {"\r", true},          
        {"   ", true},         
        {"\t\t", true},        
        {" \t\r\n", true},     
        {"", false},           
        {"a", false},          
        {" abc", false}        
    };

    std::cout << "Testando Automato de Espacos\n";
    tester.testar(testes);
}
