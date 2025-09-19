
#include <iostream>
#include <automato_base/Automato.hpp>



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
