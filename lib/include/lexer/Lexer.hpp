#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>

class Lexer {
private:
    std::unordered_set<std::string> palavrasReservadas;

public:
    Lexer();

    std::unordered_map<std::string, std::vector<std::string>> analisarTexto(std::string texto);
};