#include <iostream>
#include <vector>
#include <string>
#include "TokensAutomatos.hpp"

class AutomatoTester {
private:
    Automato& automato;

public:
    AutomatoTester(Automato& automatoRef) : automato(automatoRef) {}

    void testar(const std::vector<std::pair<std::string,bool>>& testes);
};
