# Wasabi Compiler

Este é o compilador da linguagem **Wasabi**, construído utilizando C++ e
a infraestrutura LLVM.

## 🚀 Instalação e Uso Rápido

Se você apenas quer usar o compilador sem precisar baixar o código
fonte, siga os passos abaixo.

### 1. Pré-requisitos (Obrigatório)

Mesmo baixando o executável pronto, o compilador **depende do Clang**
instalado na sua máquina para gerar os binários finais.

-   **Windows:**
    -   Baixe e instale o **LLVM** (que inclui o Clang): *LLVM Download
        Page* ou via `winget install LLVM`.
    -   **Importante:** Durante a instalação, marque a opção **"Add LLVM
        to the system PATH"**.
-   **Linux:**
    -   Instale via terminal: `sudo apt install clang`

### 2. Download do Binário

Baixe a versão mais recente do compilador já compilado:

[**CLIQUE AQUI PARA BAIXAR O COMPILADOR (LINK DO BINARIO)**](https://github.com/Tiago-Fogolin/compilador-wasabi/releases/download/v1/compilador.exe)

### 3. Como Executar

Após baixar e garantir que o `clang` está instalado, abra o terminal na
pasta onde está o `compilador.exe`:

``` bash
# Compilação básica
./compilador.exe arquivo_teste.wsb

# Compilação definindo nome de saída
./compilador.exe arquivo_teste.wsb -o meu_programa.exe
```

## 🛠️ Compilação

Se você baixou o código fonte e deseja compilar o projeto manualmente,
siga estas instruções.

### Dependências de Desenvolvimento

Além do **Clang/LLVM** (necessário para o funcionamento do Wasabi), para compilar o projeto você precisará de:

1.  **Compilador C++:** (Recomenda-se o `g++` via GCC ou o próprio `clang++`).
2.  **Make:** Ferramenta de automação de build.

**Como instalar:**

-   **Windows:** Recomenda-se instalar o **MinGW-w64**.
    -   Ao instalar, certifique-se de marcar os pacotes `mingw32-make`, `gcc` e `g++`.
-   **Linux:** Instale o pacote essencial de build (inclui Make e G++):
    -   `sudo apt install build-essential`

### Compilando o Projeto

Certifique-se de estar na raiz do projeto.

#### No Windows (PowerShell)

    mingw32-make -f .\MakeFile.mk

#### No Linux (Bash)

    make -f ./MakeFile.mk

Isso irá gerar o executável `compilador.exe` (ou `compilador`) dentro da
pasta *build*.

> **Nota:** O sistema de build atual pode apresentar problemas se o
> caminho da pasta do projeto contiver espaços em branco. Recomenda-se
> colocar o projeto em um caminho sem espaços (ex:
> `C:\Projetos\Wasabi`).

## 📚 Exemplos

Aprenda a sintaxe da linguagem Wasabi e veja códigos de exemplo
(fatoriais, structs, loops, etc.) no repositório oficial:


[**Ver Exemplos**](./docs/examples)
