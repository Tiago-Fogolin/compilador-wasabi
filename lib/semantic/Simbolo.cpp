#include <semantic/Simbolo.hpp>
#include <sstream>

Simbolo::Simbolo(const std::string& nome,
                 const std::string& tipo,
                 int linha,
                 bool inicializado,
                 bool utilizado,
                 int tam)
    : nome(nome),
      tipo(tipo),
      linha(linha),
      inicializado(inicializado),
      utilizado(utilizado),
      tamanhoConhecido(tam) {}

std::string Simbolo::toString() const {
    std::ostringstream oss;
    oss << "Simbolo(" << nome << ": " << tipo
        << ", linha " << linha
        << ", init=" << (inicializado ? "true" : "false")
        << ", usado=" << (utilizado ? "true" : "false") << ")";
    return oss.str();
}
