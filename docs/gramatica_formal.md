# **Definição Formal da Gramática**
G = `(V, Σ, P, S)`

V = `{Programa, BlocoDeclarações, Declaração, DeclaraçãoStruct, DeclaraçãoInterface,
     DeclaraçãoMétodo, DeclaraçãoAtributo, DeclaraçãoFunção, DeclaraçãoVariável,
     BlocoComandos, Comando, ComandoCondicional, ComandoLaço, ComandoAtribuição,
     ComandoChamada, ComandoRetorno, ComandoElse, Expressão, ExpressãoAritmética,
     ExpressãoRelacional, ExpressãoLógica, Termo, Fator, ListaParametros,
     ListaArgumentos, Tipo, Atributo, ListaAtributos, ListaMétodos}`


Σ = `{"interface", "struct", "implements", "def", "this", "for", "foreach", "in",
     "if", "elif", "else", "while", "break", "continue", "return",
     "int", "float", "string", "bool", "tuple", "dict", "set", "void", "null",
     "(", ")", "{", "}", "[", "]", ",", ";", "=", "+", "-", "*", "/",
     "==", "!=", ">", "<", ">=", "<=", "identificador",
     "literal_inteiro", "literal_real", "literal_texto", "literal_booleano"}`


S = `Programa`

P = O conjunto de produções segue abaixo, no formato EBNF.

## Gramática em EBNF (Extended Backus–Naur Form)
```enbf
Programa = BlocoDeclarações BlocoComandos

BlocoDeclarações = { Declaração }

Declaração = DeclaraçãoStruct
             | DeclaraçãoInterface
             | DeclaraçãoMétodo
             | DeclaraçãoAtributo
             | DeclaraçãoFunção
             | DeclaraçãoVariável

DeclaraçãoStruct = "struct" identificador ["implements" identificador] "\n" [ ListaAtributos ] [ ListaMétodos ]

ListaAtributos = { Atributo "\n" } [ Atributo ]

Atributo = "\t" identificador [ ":" Tipo ]

ListaMétodos = { DeclaraçãoMétodo "\n" } [ DeclaraçãoMétodo ]

DeclaraçãoMétodo = "\t" DeclaraçãoFunção

DeclaraçãoInterface = "interface" identificador "{" [ ListaMétodos ] "}"

DeclaraçãoFunção = "def" identificador "(" [ ListaParametros ] ")" ":" BlocoComandos

DeclaraçãoVariável = identificador [ ":" Tipo ] "=" Expressão

Tipo = "int" | "float" | "string" | "bool"
       | "tuple" | "dict" | "set" | "void" | "null"

BlocoComandos = { Comando }

Comando = ComandoCondicional
          | ComandoLaço
          | ComandoAtribuição
          | ComandoChamada
          | ComandoRetorno

ComandoRetorno = "return" [ Expressão ]

ComandoCondicional = "if" "(" Expressão ")" BlocoComandos [ ComandoElse ]

ComandoElse = "else" BlocoComandos
              | "elif" "(" Expressão ")" BlocoComandos [ ComandoElse ]

ComandoLaço = "for" "(" ComandoAtribuição "," Expressão "," ComandoAtribuição ")" BlocoComandos
              | "foreach" identificador "in" Expressão BlocoComandos
              | "while" "(" Expressão ")" BlocoComandos

ComandoAtribuição = identificador ( 
    "="   | 
    "+="  | 
    "-="  | 
    "*="  | 
    "**=" | 
    "/="  | 
    "//=" |
    "&="  |
    "|="  |
    "^="  |
    "~="  |
    ">>=" |
    "<<=" 
) Expressão

ComandoChamada = identificador "(" [ ListaArgumentos ] ")"

Expressão = ExpressãoAritmética
            | ExpressãoRelacional
            | ExpressãoLógica
            | literal
            | identificador

ExpressãoAritmética = Termo { ("+" | "-") Termo }

Termo = Fator { ("*" | "/") Fator }

Fator = "(" Expressão ")" | identificador | literal

ExpressãoRelacional = ExpressãoAritmética ("==" | "!=" | ">" | "<" | ">=" | "<=") ExpressãoAritmética

ExpressãoLógica = ExpressãoRelacional
                  | identificador
                  | literal
                  | "not" ExpressãoLógica
                  | "(" ExpressãoLógica ")"
                  | ExpressãoLógica "and" ExpressãoLógica
                  | ExpressãoLógica "or" ExpressãoLógica

ListaParametros = { Tipo identificador "," } [ Tipo identificador ]

ListaArgumentos = { Expressão "," } [ Expressão ]
```