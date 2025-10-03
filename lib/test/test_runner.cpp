#include "test/lexer_test/TestAutomatoInteiro.hpp"
#include "test/lexer_test/TestAutomatoBooleanos.hpp"
#include "test/lexer_test/TestAutomatoIdentificadores.hpp"
#include "test/lexer_test/TestAutomatoFlutuantes.hpp"
#include "test/lexer_test/TestAutomatoChar.hpp"
#include "test/lexer_test/TestAutomatoEspacos.hpp"
#include "test/lexer_test/TestAutomatoRelacionais.hpp"
#include "test/lexer_test/TestAutomatoString.hpp"
#include "test/lexer_test/TestAutomatoAritmeticos.hpp"
#include "test/lexer_test/TestAutomatoBitwise.hpp"
#include "test/lexer_test/TestAutomatoAtribuicao.hpp"
#include "test/lexer_test/TestAutomatoPalavrasReservadas.hpp"
#include "test/lexer_test/TestAutomatoLogico.hpp"
#include "test/lexer_test/TestAutomatoCientifico.hpp"
#include "test/lexer_test/TestAutomatoDelimitadores.hpp"
#include "test/lexer_test/TestAutomatoComentarios.hpp"
#include "test/lexer_test/TestAutomatoLexer.hpp"

int main() {
    // testarInteiros();
    // testarBooleanos();
    // testarIdentificadores();
    // testarFlutuantes();
    // testarAritmeticos();
    // testarRelacionais();
    // testarBitwise();
    // testarAtribuicao();
    // testarChar();
    // testarEspacos();
    // testarStrings();
    // testarPalavrasReservadas();
    // testarLogicos();
    // testarCientificos();
    // testarDelimitadores();
    // testarComentarios();
    testarLexer();
    return 0;
}
