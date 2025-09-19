# **Definição da Gramática Inicial**
## 1. Definição Completa G = (V, T, P, S)
V = {Programa, BlocoDeclarações, Declaração, DeclaraçãoStruct, DeclaraçãoInterface,
     DeclaraçãoMétodo, DeclaraçãoAtributo, DeclaraçãoFunção, DeclaraçãoVariável,
     BlocoComandos, Comando, ComandoCondicional, ComandoLaço, ComandoAtribuição,
     ComandoChamada, ComandoRetorno, ComandoElse, Expressão, ExpressãoAritmética,
     ExpressãoRelacional, ExpressãoLógica, Termo, Fator, ListaParametros,
     ListaArgumentos, Tipo, Atributo, ListaAtributos, ListaMétodos}


T = {"interface", "struct", "implements", "def", "this", "for", "foreach", "in",
     "if", "elif", "else", "while", "break", "continue", "return",
     "int", "float", "string", "bool", "tuple", "dict", "set", "void", "null",
     "(", ")", "{", "}", "[", "]", ",", ";", "=", "+", "-", "*", "/",
     "==", "!=", ">", "<", ">=", "<=", "identificador",
     "literal_inteiro", "literal_real", "literal_texto", "literal_booleano"}


S = Programa

## 2. Gramática (versão inicial)
Programa -> BlocoDeclarações BlocoComandos

BlocoDeclarações -> Declaração BlocoDeclarações | ε

Declaração -> DeclaraçãoStruct | DeclaraçãoInterface | DeclaraçãoMétodo 
            | DeclaraçãoAtributo | DeclaraçãoFunção | DeclaraçãoVariável

DeclaraçãoStruct -> "struct" identificador ":\n" ListaAtributos ListaMétodos

ListaAtributos -> Atributo | Atributo "\n" ListaAtributos | ε 

Atributo -> "\t" identificador | "\t" identificador ":" Tipo

ListaMétodos -> DeclaraçãoMétodo | DeclaraçãoMétodo "\n" ListaMétodos | ε

DeclaraçãoMétodo -> "\t" DeclaraçãoFunção

DeclaraçãoInterface -> "interface" identificador "{" ListaMétodos "}"

DeclaraçãoFunção -> "def" identificador "(" ListaParametros ")" ":" BlocoComandos

DeclaraçãoVariável -> identificador "=" Expressão | identificador ":" Tipo "=" Expressão

Tipo -> "int" | "float" | "string" | "bool" | "tuple" | "dict" | "set" | "void" | "null"

BlocoComandos -> Comando BlocoComandos | ε

Comando -> ComandoCondicional | ComandoLaço | ComandoAtribuição | ComandoChamada | ComandoRetorno

ComandoRetorno -> "return" Expressão

ComandoCondicional -> "if" "(" Expressão ")" BlocoComandos ComandoElse

ComandoElse -> "else" BlocoComandos | "elif" "(" Expressão ")" BlocoComandos ComandoElse | ε

ComandoLaço -> "for" "(" ComandoAtribuição "," Expressão "," ComandoAtribuição ")" BlocoComandos  
             | "foreach" identificador "in" Expressão BlocoComandos  
             | "while" "(" Expressão ")" BlocoComandos

ComandoAtribuição -> identificador "=" Expressão

ComandoChamada -> identificador "(" ListaArgumentos ")"

Expressão -> ExpressãoAritmética | ExpressãoRelacional | ExpressãoLógica | literal | identificador

ExpressãoAritmética -> ExpressãoAritmética "+" Termo
                      | ExpressãoAritmética "-" Termo
                      | Termo

Termo -> Termo "*" Fator
       | Termo "/" Fator
       | Fator

Fator -> "(" Expressão ")" | identificador | literal

ExpressãoRelacional -> ExpressãoAritmética ("==" | "!=" | ">" | "<" | ">=" | "<=") ExpressãoAritmética

ExpressãoLógica -> ExpressãoLógica "E" ExpressãoLógica
                  | ExpressãoLógica "Ou" ExpressãoLógica
                  | "Não" ExpressãoLógica
                  | "(" ExpressãoLógica ")"
                  | ExpressãoRelacional
                  | identificador
                  | literal

ListaParametros -> Tipo identificador ("," Tipo identificador)* | ε

ListaArgumentos -> Expressão ("," Expressão)* | ε

## 3. Classificação na Hierarquia de Chomsky
Tipo: 2 — Livre de Contexto (Context-Free Grammar, CFG).
Justificativa:
- Todas as regras possuem exatamente um não-terminal à esquerda (forma A → α),
logo se enquadram em CFG.
- O reconhecimento da linguagem requer uma pilha para gerenciar aninhamentos (por
exemplo, if...end e while...end), algo natural para autômatos de pilha e fora
do poder de autômatos finitos (não-regulares).
- Restrições contextuais (ex.: declaração prévia de variáveis, concordância de tipos)
serão tratadas na análise semântica posterior através de uma tabela de símbolos — não via gramática.
- Portanto, a gramática é adequadamente classificada como Tipo 2.

## 4. Exemplos de derivações
### 4.1 Declaração de variável com inicialização
``` 
Programa → BlocoDeclarações BlocoComandos → DeclaraçãoVariável BlocoDeclarações BlocoComandos → Tipo identificador
```

### 4.2 Expressão aritmética com precedência
```
ExpressãoAritmética → Termo + Termo → Fator + Termo → identificador + (identificador * identificador)
```

### 4.3 Estrutura condicional com else
```
ComandoCondicional → if (a > b) { BlocoComandos } else { BlocoComandos }
```

### 4.4 foreach(loop)
```
ComandoLaço → foreach identificador in Expressão BlocoComandos
```


## 5. Ambiguidades potenciais e estratégias de resolução
### 5.1 Dangling-else

- Problema: associação ambígua do else.
- Solução: else associa ao if mais próximo sem else.

### 5.2 Precedência e associatividade
- Operadores definidos em camadas (Fator → Termo → Arit → Rel →
Lógica).
- Associatividade natural:
○ +, -, *, / → à esquerda.
○ comparadores → não-associativos.

### 5.3 Comparações encadeadas
- a < b < c proibido diretamente.
- Deve-se usar (a < b) and (b < c).

### 5.4 Identificadores vs. palavras-chave
- Regra: palavras-chave não podem ser identificadores.
