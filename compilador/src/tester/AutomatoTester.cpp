#include <AutomatoTester.hpp>


void AutomatoTester::testar(const std::vector<std::pair<std::string,bool>>& testes) {
    int passados = 0;
    int total = testes.size();

    for(const auto& t : testes) {
        const std::string& s = t.first;
        bool esperado = t.second;

        Automato copiaAutomato = automato;
        ResultadoProcessamento resultado = ResultadoProcessamento::INVALIDO;

        for(char c : s) {
            resultado = copiaAutomato.processarCaracter(c);
            if(resultado == ResultadoProcessamento::INVALIDO) break;
        }

        bool aceito = (resultado == ResultadoProcessamento::ACEITO);

        std::cout << "Testando \"" << s << "\": " 
                    << (aceito ? "ACEITO" : "REJEITADO");

        if(aceito == esperado) {
            std::cout << " [OK]\n";
            passados++;
        } else {
            std::cout << " [FAIL] (esperado: " << (esperado ? "ACEITO" : "REJEITADO") << ")\n";
        }
    }

    std::cout << "\nResumo: " << passados << "/" << total << " testes passaram.\n\n";
}


