#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <lexer/Token.hpp>

class Lexer {
private:
    int estadoAtual = 0;
    std::string bufferIncompleto;
    std::unordered_set<std::string> palavrasReservadas;

public:
    Lexer();
    bool err = false;

    std::vector<Token> analisarTexto(std::string& texto);
};
