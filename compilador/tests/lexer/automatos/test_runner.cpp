#include "TestAutomatoInteiro.hpp"
#include "TestAutomatoBooleanos.hpp"
#include "TestAutomatoIdentificadores.hpp"
#include "TestAutomatoFlutuantes.hpp"

int main() {
    testarInteiros();
    testarBooleanos();
    testarIdentificadores();
    testarFlutuantes();
    return 0;
}
