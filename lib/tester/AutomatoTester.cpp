#include <automato_tester/AutomatoTester.hpp>


void AutomatoTester::testar(const std::vector<std::pair<std::string,bool>>& testes) {
    int passados = 0;
    int total = testes.size();

    for (const auto& t : testes) {
        const std::string& s = t.first;
        bool esperado = t.second;

        bool aceito = automato.processarString(s);

        std::cout << "Testando \"" << s << "\": " 
                  << (aceito ? "ACEITO" : "REJEITADO");

        if (aceito == esperado) {
            std::cout << " [OK]\n";
            passados++;
        } else {
            std::cout << " [FAIL] (esperado: " << (esperado ? "ACEITO" : "REJEITADO") << ")\n";
        }
    }

    std::cout << "\nResumo: " << passados << "/" << total << " testes passaram.\n\n";
}


