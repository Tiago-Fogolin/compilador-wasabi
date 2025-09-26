# Lexer - README

## Compilação

Para compilar o lexer, você precisa estar **na pasta raiz do projeto** e ter o `make` instalado.

### No Windows

```bash
mingw32-make -f .\MakeFile.mk
```

### No Linux

```bash
make -f ./MakeFile.mk
```

Isso irá gerar o executável `compilador.exe` dentro da pasta `build`.

---

## Execução

Para rodar o compilador, use o seguinte comando:

```bash
./build/compilador.exe <caminho_do_arquivo>
```

Substitua `<caminho_do_arquivo>` pelo caminho do arquivo que você deseja compilar.

---

## Testes

Para compilar e rodar os testes:

### Compilação dos testes

#### No Windows

```bash
mingw32-make -f .\MakeFile_tests.mk
```

#### No Linux

```bash
make -f ./MakeFile_tests.mk
```

Isso irá gerar o executável `testes.exe` dentro da pasta `build`.

### Execução dos testes

```bash
./build/testes.exe
```

---

## Observações

* Todos os executáveis são gerados na pasta `build`.
* Pode ser que de pau no make se o caminho da sua pasta tiver espaços, então coloque em um caminho sem espaços se der erro, até o burrinho aqui conseguir arrumar isso
* Tem dois makefile separado pq não conseguimos gerar os dois executavéis no mesmo makefile 👍
