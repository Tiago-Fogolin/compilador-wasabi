
#include <iostream>
#include <automato_base/Automato.hpp>

#include "automatos/AutomatoAritmeticos.hpp"
#include "automatos/AutomatoAtribuicao.hpp"
#include "automatos/AutomatoBitwise.hpp"
#include "automatos/AutomatoBooleanos.hpp"
#include "automatos/AutomatoChar.hpp"
#include "automatos/AutomatoCientifico.hpp"
#include "automatos/AutomatoEspacos.hpp"
#include "automatos/AutomatoFlutuantes.hpp"
#include "automatos/AutomatoIdentificadores.hpp"
#include "automatos/AutomatoInteiros.hpp"
#include "automatos/AutomatoLogico.hpp"
#include "automatos/AutomatoPalavrasReservadas.hpp"
#include "automatos/AutomatoRelacionais.hpp"
#include "automatos/AutomatoString.hpp"
#include "automatos/AutomatoDelimitadores.hpp"
#include "automatos/AutomatoComentarios.hpp"


NodeAutomato::NodeAutomato(std::unordered_map<char, int> mapaDestino) {
    this->mapaDestino = mapaDestino;
}

bool NodeAutomato::verificaTransicao(char c) {
    if(this->mapaDestino.find(c) == this->mapaDestino.end()) {
        return false;
    }

    return true;
}


Automato::Automato(std::vector<NodeAutomato> nodes, std::set<int> estadosFinais) {
    this->nodes = nodes;
    this->estadosFinais = estadosFinais;
}

bool Automato::verificaTransicao(char caracter) {
    NodeAutomato& nodeAtual = this->nodes[this->indiceEstado];
    return nodeAtual.verificaTransicao(caracter);
}

int Automato::recuperarProximoEstado(char caracter) {
    NodeAutomato& nodeAtual = this->nodes[this->indiceEstado];
    int proximoEstado = nodeAtual.mapaDestino[caracter];

    return proximoEstado;
}


bool Automato::verificaEstadoFinal() {
    return estadosFinais.count(indiceEstado) > 0;
}

ResultadoProcessamento Automato::processarCaracter(char c) {
    if (!this->verificaTransicao(c)) {
        return ResultadoProcessamento::INVALIDO;
    }

    indiceEstado = recuperarProximoEstado(c);

    if (this->verificaEstadoFinal()) {
        return ResultadoProcessamento::ACEITO;
    }

    return ResultadoProcessamento::CONTINUA;
}



bool Automato::processarString(const std::string& s) {
    auto copiaAutomato = clone(); // faz uma cópia para não alterar o original
    ResultadoProcessamento resultado = ResultadoProcessamento::INVALIDO;

    for(char c : s) {
        resultado = copiaAutomato->processarCaracter(c);
        if(resultado == ResultadoProcessamento::INVALIDO)
            return false;
    }

    return (resultado == ResultadoProcessamento::ACEITO);
}

int Automato::estadoAtual() {
    return indiceEstado;
}

int Automato::processarStringComEstadoFinal(const std::string& s, bool& aceito) {
    auto copiaAutomato = clone(); 
    ResultadoProcessamento resultado = ResultadoProcessamento::INVALIDO;

    for(char c : s) {
        resultado = copiaAutomato->processarCaracter(c);
        if(resultado == ResultadoProcessamento::INVALIDO) {
            aceito = false;
            return -1;
        }
    }

    aceito = (resultado == ResultadoProcessamento::ACEITO);
    return copiaAutomato->estadoAtual();
}

std::vector<std::pair<std::string, std::unique_ptr<Automato>>> AutomatoFactory::getAutomatos() {
    std::vector<std::pair<std::string, std::unique_ptr<Automato>>> automatos;
    
    automatos.push_back({"Comentarios", std::make_unique<AutomatoComentarios>()});
    automatos.push_back({"Espacos", std::make_unique<AutomatoEspacos>()});
    automatos.push_back({"Delimitadores", std::make_unique<AutomatoDelimitadores>()});
    automatos.push_back({"Aritmeticos", std::make_unique<AutomatoAritmeticos>()});
    automatos.push_back({"Flutuantes", std::make_unique<AutomatoFlutuantes>()});
    automatos.push_back({"Inteiros", std::make_unique<AutomatoInteiros>()});
    automatos.push_back({"PalavrasReservadas", std::make_unique<AutomatoPalavrasReservadas>()});
    automatos.push_back({"Identificadores", std::make_unique<AutomatoIdentificadores>()});
    automatos.push_back({"Atribuicao", std::make_unique<AutomatoAtribuicao>()});
    automatos.push_back({"Bitwise", std::make_unique<AutomatoBitwise>()});
    automatos.push_back({"Booleanos", std::make_unique<AutomatoBooleanos>()});
    automatos.push_back({"Char", std::make_unique<AutomatoChar>()});
    automatos.push_back({"Cientifico", std::make_unique<AutomatoCientifico>()});
    automatos.push_back({"Logico", std::make_unique<AutomatoLogico>()});
    automatos.push_back({"Relacionais", std::make_unique<AutomatoRelacionais>()});
    automatos.push_back({"String", std::make_unique<AutomatoString>()});

    return automatos;
}
