# **Definição Formal da Gramática**
G = `(V, Σ, P, S)`

V = `{Programa, BlocoDeclarações, Declaração, DeclaraçãoStruct, DeclaraçãoInterface,
     DeclaraçãoMétodo, DeclaraçãoAtributo, DeclaraçãoFunção, DeclaraçãoVariável,
     BlocoComandos, Comando, ComandoCondicional, ComandoLaço, ComandoAtribuição,
     ComandoChamada, ComandoRetorno, ComandoElse, Expressão, ExpressãoAritmética,
     ExpressãoRelacional, ExpressãoLógica, Termo, Fator, ListaParametros,
     ListaArgumentos, Tipo, Atributo, ListaAtributos, ListaMétodos}`


Σ = `{"interface", "struct", "implements", "def", "this", "for", "foreach", "in",
     "if", "else", "while", "break", "continue", "return",
     "int", "float", "string", "bool", "tuple", "dict", "set", "void", "null",
     "(", ")", "{", "}", "[", "]", ",", ";", "=", "+", "-", "*", "/",
     "==", "!=", ">", "<", ">=", "<=", "identificador",
     "literal_inteiro", "literal_real", "literal_texto", "literal_booleano"}`


S = `Programa`

P = O conjunto de produções segue abaixo, no formato EBNF.

## Gramática em EBNF (Extended Backus–Naur Form)
```enbf
Programa            = BlocoDeclarações BlocoComandos

BlocoDeclarações    = { Declaração }

Declaração          = DeclaraçãoStruct
                    | DeclaraçãoInterface
                    | DeclaraçãoFunção
                    | DeclaraçãoVariável

DeclaraçãoStruct    = "struct" identificador [ "implements" identificador ] "{" ListaAtributos ListaMétodos "}"

ListaAtributos      = { Atributo }
Atributo            = identificador [ ":" Tipo ]

ListaMétodos        = { DeclaraçãoMétodo }
DeclaraçãoMétodo    = DeclaraçãoFunção

DeclaraçãoInterface = "interface" identificador "{" [ ListaMétodos ] "}"

DeclaraçãoFunção    = "def" identificador "(" [ ListaParametros ] ")" ":" BlocoComandos

DeclaraçãoVariável  = identificador [ ":" Tipo ] "=" Expressão

Tipo                = "int" | "float" | "string" | "bool"
                    | "tuple" | "dict" | "set" | "void" | "null"

BlocoComandos       = { Comando }

Comando             = ComandoCondicional
                    | ComandoLaço
                    | ComandoRetorno
                    | ComandoIdent

ComandoRetorno      = "return" [ Expressão ]

ComandoCondicional  = "if" "(" Expressão ")" BlocoComandos [ ComandoElse ]
ComandoElse         = "else" BlocoComandos


ComandoLaço         = "for" "(" ComandoAtribuição "," Expressão "," ComandoAtribuição ")" BlocoComandos
                    | "foreach" identificador "in" Expressão BlocoComandos
                    | "while" "(" Expressão ")" BlocoComandos


ComandoIdent        = identificador Suffix
Suffix              = "(" [ ListaArgumentos ] ")"
                    | AssignOp Expressão                 
                      Expressão

ComandoAtribuição   = identificador AssignOp Expressão

AssignOp            = "=" | "+=" | "-=" | "*=" | "**=" | "/=" | "//=" | "&=" | "|=" | "^=" | "~=" | ">>=" | "<<="

ListaParametros     = Tipo identificador { "," Tipo identificador } | ε

ListaArgumentos     = Expressão { "," Expressão } | ε


Expressão           = LogicalOr

LogicalOr           = LogicalAnd { "or" LogicalAnd }

LogicalAnd          = LogicalNot { "and" LogicalNot }

LogicalNot          = "not" LogicalNot
                    | RelationalOrPrimary

RelationalOrPrimary = Arithmetic [ RelOp Arithmetic ]
RelOp               = "==" | "!=" | ">" | "<" | ">=" | "<="

Arithmetic          = Term { ("+" | "-") Term }

Term                = Factor { ("*" | "/") Factor }

Factor              = "(" Expressão ")"
                    | identificador
                    | literal
