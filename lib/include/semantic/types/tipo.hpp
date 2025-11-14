#ifndef TIPO_HPP
#define TIPO_HPP

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

// Forward declarations
class Tipo;
class TipoPrimitivo;
class VariavelTipo;
class TipoFuncao;
class TipoGenerico;

using TipoPtr = std::shared_ptr<Tipo>;

class Tipo {
public:
    virtual ~Tipo() = default;

    virtual std::string mostrar() const = 0;

    virtual bool igual(const Tipo& other) const = 0;
};


class TipoPrimitivo : public Tipo {
public:
    std::string nome;

    explicit TipoPrimitivo(std::string nome);

    std::string mostrar() const override;

    bool igual(const Tipo& other) const override;
};


class VariavelTipo : public Tipo {
public:
    std::string nome;

    explicit VariavelTipo(std::string nome);

    std::string mostrar() const override;

    bool igual(const Tipo& other) const override;
};


class TipoFuncao : public Tipo {
public:
    std::vector<TipoPtr> parametros;
    TipoPtr retorno;

    TipoFuncao(std::vector<TipoPtr> parametros, TipoPtr retorno);

    std::string mostrar() const override;

    bool igual(const Tipo& other) const override;
};


class TipoGenerico : public Tipo {
public:
    std::string construtor;
    std::vector<TipoPtr> argumentos;

    TipoGenerico(std::string construtor, std::vector<TipoPtr> argumentos);

    std::string mostrar() const override;

    bool igual(const Tipo& other) const override;
};

#endif
