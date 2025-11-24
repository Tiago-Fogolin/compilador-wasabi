## First - Follow

```FIRST(BlocoDeclarações) = { "struct", "interface", "def", identificador } ∪ { ε }

FIRST(Declaração) = { "struct", "interface", "def", identificador }

FIRST(DeclaraçãoStruct) = { "struct" }

FIRST(ListaAtributos) = { identificador, ε }

FIRST(Atributo) = { identificador }

FIRST(ListaMétodos) = { "def", ε }

FIRST(DeclaraçãoInterface) = { "interface" }

FIRST(DeclaraçãoFunção) = { "def" }

FIRST(DeclaraçãoVariável) = { identificador }

FIRST(Tipo) = { "int","float","string","bool","tuple","dict","set","void","null" }

FIRST(BlocoComandos) = { "if", "for", "foreach", "while", "return", identificador, "not", "(", literal } ∪ { ε }
(pode ser vazio)

FIRST(Comando) = { "if", "for", "foreach", "while", "return", identificador, "not", "(", literal }

FIRST(ComandoRetorno) = { "return" }

FIRST(ComandoCondicional) = { "if" }

FIRST(ComandoElse) = { "else" }

FIRST(ComandoLaço) = { "for", "foreach", "while" }

FIRST(ComandoIdent) = { identificador }

FIRST(Suffix) = { "(", AssignOp }

FIRST(ComandoAtribuição) = { identificador } (mas o segundo token deve ser um AssignOp)

FIRST(ListaParametros) = { "int","float","string","bool","tuple","dict","set","void","null", ε }

FIRST(ListaArgumentos) = { "(", "identificador", literal, "not", ε }

FIRST(Expressão) = FIRST(LogicalOr) = FIRST(LogicalAnd) = FIRST(LogicalNot) = { "not", "(", identificador, literal }

FIRST(LogicalOr) = { "not", "(", identificador, literal }

FIRST(LogicalAnd) = { "not", "(", identificador, literal }

FIRST(LogicalNot) = { "not", "(", identificador, literal }

FIRST(RelationalOrPrimary) = { "(", identificador, literal }

FIRST(RelOp) = { "==", "!=", ">", "<", ">=", "<=" }

FIRST(Arithmetic) = { "(", identificador, literal }

FIRST(Term) = { "(", identificador, literal }

FIRST(Factor) = { "(", identificador, literal }
```

```
FOLLOW(Programa) = { $ }

FOLLOW(BlocoDeclarações) = { if, for, foreach, while, return, identificador, $ }

FOLLOW(Declaração) = { struct, interface, def, identificador, if, for, foreach, while, return, $ }

FOLLOW(DeclaraçãoStruct) = { struct, interface, def, identificador, if, for, foreach, while, return, $ }

FOLLOW(DeclaraçãoInterface) = { struct, interface, def, identificador, if, for, foreach, while, return, $ }

FOLLOW(DeclaraçãoFunção) = { struct, interface, def, identificador, if, for, foreach, while, return, $ }

FOLLOW(DeclaraçãoVariável) = { struct, interface, def, identificador, if, for, foreach, while, return, $ }

FOLLOW(Tipo) = { identificador, ")", "=", "," }

FOLLOW(BlocoComandos) = { else, $, "}" }

FOLLOW(Comando) = { if, for, foreach, while, return, identificador, else, $, "}" }

FOLLOW(ComandoCondicional) = { if, for, foreach, while, return, identificador, else, $, "}" }

FOLLOW(ComandoLaço) = { if, for, foreach, while, return, identificador, else, $, "}" }

FOLLOW(ComandoAtribuição) = { ",", ")", if, for, foreach, while, return, identificador, else, $, "}" }

FOLLOW(ComandoChamada) = { if, for, foreach, while, return, identificador, else, $, "}" }

FOLLOW(ComandoRetorno) = { if, for, foreach, while, return, identificador, else, $, "}" }

FOLLOW(Expressão) = { ",", ")", "}", "+", "-", "*", "/", "==", "!=", ">", "<", ">=", "<=", "and", "or", if, for, foreach, while, return, identificador, else, $, ";" }

FOLLOW(Fator) = { "*", "/", "+", "-", "==", "!=", ">", "<", ">=", "<=", ")", ",", ";", "and", "or", $ }

FOLLOW(ExpressãoRelacional) = { "and", "or", ")", ",", "}", ";", $ }

FOLLOW(ExpressãoLógica) = { ")", ",", "}", ";", $ }

FOLLOW(ListaParametros) = { ")" }

FOLLOW(ListaArgumentos) = { ")" }
```

## A gramática é LL(1)?

Sim, a gramática é LL(1):

- 1° Condição: FIRST(α) ∩ FIRST(β) = ∅:
```
FIRST(DeclaraçãoStruct) ∩ FIRST(DeclaraçãoInterface) = ∅  
FIRST(DeclaraçãoInterface) ∩ FIRST(DeclaraçãoMétodo) = ∅  
FIRST(DeclaraçãoMétodo) ∩ FIRST(DeclaraçãoAtributo) = ∅  
FIRST(DeclaraçãoAtributo) ∩ FIRST(DeclaraçãoFunção) = ∅  
FIRST(DeclaraçãoFunção) ∩ FIRST(DeclaraçãoVariável) = ∅  
...
(para todos os não-terminais com múltiplas produções)
```

- 2° Condição: se ε ∈ FIRST(α), então FIRST(β) ∩ FOLLOW(A) = ∅:
```
ε ∈ FIRST(BlocoDeclarações’) → FIRST(Declaração) ∩ FOLLOW(BlocoDeclarações’) = ∅  
ε ∈ FIRST(ListaAtributos’) → FIRST(Atributo) ∩ FOLLOW(ListaAtributos’) = ∅  
ε ∈ FIRST(ListaComandos’) → FIRST(Comando) ∩ FOLLOW(ListaComandos’) = ∅
```

- 3° Condição: se ε ∈ FIRST(β), então FIRST(α) ∩ FOLLOW(A) = ∅:
```
ε ∈ FIRST(BlocoDeclarações’) → FIRST(Declaração) ∩ FOLLOW(BlocoDeclarações’) = ∅  
ε ∈ FIRST(ListaAtributos’) → FIRST(Atributo) ∩ FOLLOW(ListaAtributos’) = ∅  
ε ∈ FIRST(ListaComandos’) → FIRST(Comando) ∩ FOLLOW(ListaComandos’) = ∅
```

Todas as condições se satisfazem, portanto a gramática é LL(1).
