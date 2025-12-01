# Wasabi Compiler

Este é o compilador da linguagem **Wasabi**, construído utilizando C++ e a infraestrutura LLVM.

## 🚀 Instalação e Uso Rápido

Se você apenas quer usar o compilador sem precisar baixar o código fonte, siga os passos abaixo.

### 1. Pré-requisitos (Windows)

Para que tudo funcione corretamente no Windows, você precisa do **Clang** e das **Ferramentas de Build do C++** (para que o Clang consiga criar executáveis).

A maneira mais fácil de instalar tudo é abrir o **PowerShell (como Administrador)** e rodar:

```powershell
# 1. Instala o LLVM, e o adicone para o path
winget install LLVM.LLVM

# 2. WinLibs
winget install BrechtSanders.WinLibs.POSIX.UCRT

# 3. Instala as ferramentas de build do Visual Studio (Necessário para o Linker do Windows)
winget install Microsoft.VisualStudio.2022.BuildTools --override "--add Microsoft.VisualStudio.Workload.VCTools --includeRecommended --passive --norestart"
```

**Nota:** Se o comando acima das BuildTools falhar ou pedir seleção manual, abra o instalador e marque a opção *"Desenvolvimento para desktop com C++"*.

---

### 2. Download do Binário

Baixe a versão mais recente do compilador já compilado:

[**CLIQUE AQUI PARA BAIXAR O COMPILADOR**](https://github.com/Tiago-Fogolin/compilador-wasabi/releases/download/v1/compilador.exe)

---

### 3. Como Executar

Abra o terminal (CMD ou PowerShell) na pasta onde está o `compilador.exe`:

**No Windows (CMD ou PowerShell):**

```powershell
.\compilador.exe arquivo_teste.wsb
```

**Opções:**

```powershell
# Definir nome de saída personalizado
.\compilador.exe arquivo_teste.wsb -o meu_executavel.exe
```

---

## 🛠️ Compilação (Para Desenvolvedores)

Se você baixou o código fonte e deseja compilar o projeto manualmente (`.cpp` -> `.exe`), você precisará do Make e do G++.

### Instalar Dependências de Build

No Windows, recomendamos usar o pacote **WinLibs** via `winget`, que já traz o MinGW (GCC, G++, Make) configurado:

```powershell
winget install BrechtSanders.WinLibs.POSIX.UCRT
```

(Após instalar, pode ser necessário reiniciar o terminal para reconhecer os comandos `make` ou `mingw32-make`).

---

### Compilando o Projeto

Certifique-se de estar na **raiz do projeto**.

#### No Windows

```powershell
mingw32-make -f .\MakeFile.mk
```

(Se o comando `mingw32-make` não for encontrado, tente apenas `make` ou verifique se a pasta *bin* do WinLibs está no seu PATH).

#### No Linux

```bash
# Instalar dependências: sudo apt install build-essential clang
make -f ./MakeFile.mk
```

Isso irá gerar o executável `compilador.exe` dentro da pasta **build**.

⚠️ *Atenção:* O sistema de build atual pode apresentar problemas se o caminho da pasta do projeto contiver espaços em branco. Recomenda-se colocar o projeto em um caminho simples (ex: `C:\Projetos\Wasabi`).

---

## 📚 Exemplos

Aprenda a sintaxe da linguagem Wasabi e veja códigos de exemplo (fatoriais, structs, loops, etc.) no link abaixo:

[**Ver Exemplos**](./docs/examples)
