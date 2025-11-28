# **Definição Formal da Gramática**
G = `(V, Σ, P, S)`

V = `{"interface", "struct", "implements", "def", "this", "for", "foreach", "in",
     "if", "else", "while", "return", "int", "float", "string", "bool", "void", "null", "char",
     "(", ")", "{", "}", "[", "]", ",", ";", ":", ".",
     "=", "+=", "-=", "*=", "/=",
     "==", "!=", ">", "<", ">=", "<=", "+", "-", "*", "/", "%",
     "&&", "||", "!",
     "identificador", "literal_inteiro", "literal_real", "literal_texto", "literal_char", "true", "false"}`


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
(* --- Estrutura Geral --- *)
Programa            = { ElementoPrograma } ;
ElementoPrograma    = Declaração | Comando ;


Declaração          = DeclaraçãoStruct
                    | DeclaraçãoInterface
                    | DeclaraçãoFunção
                    | DeclaraçãoVariável ;

DeclaraçãoVariável  = identificador ":" Tipo [ "=" Expressão ] ;


DeclaraçãoFunção    = "def" identificador "(" [ ListaParametros ] ")" [ ":" Tipo ] Bloco ;

ListaParametros     = Parametro { "," Parametro } ;
Parametro           = identificador ":" Tipo ;

Tipo                = TipoBase { "[" [ literal_inteiro ] "]" } ;
TipoBase            = "int" | "float" | "string" | "bool" | "void" | "char" | identificador ;

DeclaraçãoStruct    = "struct" identificador [ "implements" ListaIdentificadores ] "{" { MembroStruct } "}" ;
ListaIdentificadores= identificador { "," identificador } ;

MembroStruct        = DeclaraçãoFunção  (* Método *)
                    | DeclaraçãoVariável (* Atributo *) ;

DeclaraçãoInterface = "interface" identificador "{" { DeclaraçãoFunção } "}" ;


Bloco               = "{" ListaComandos "}" ;
ListaComandos       = { Comando } ;

Comando             = ComandoCondicional
                    | ComandoLaço
                    | ComandoRetorno
                    | ComandoExpressão
                    | ComandoDeclaração ; (* Variável local *)

ComandoDeclaração   = DeclaraçãoVariável ; (* Parser trata decl como comando dentro de blocos *)

ComandoCondicional  = "if" "(" Expressão ")" Bloco [ ComandoElse ] ;
ComandoElse         = "else" ( Bloco | ComandoCondicional ) ;

ComandoLaço         = "while" "(" Expressão ")" Bloco
                    | "for" "(" [ ComandoExpressão ] "," Expressão "," [ ComandoExpressão ] ")" Bloco
                    | "foreach" "(" identificador "in" Expressão ")" Bloco ;

ComandoRetorno      = "return" [ Expressão ] ;

ComandoExpressão    = LValue ( OperadorAtribuição Expressão | "(" [ ListaArgumentos ] ")" ) ;

LValue              = identificador { AcessoMembro | AcessoIndice } ;
AcessoMembro        = "." identificador ;
AcessoIndice        = "[" Expressão "]" ;

OperadorAtribuição  = "=" | "+=" | "-=" | "*=" | "/=" ;

Expressão           = ExpressãoLógica ;

ExpressãoLógica     = ExpressãoRelacional { ("&&" | "||") ExpressãoRelacional } ;

ExpressãoRelacional = ExpressãoAritmética [ ("==" | "!=" | ">" | "<" | ">=" | "<=") ExpressãoAritmética ] ;

ExpressãoAritmética = ExpressãoMultiplicativa { ("+" | "-") ExpressãoMultiplicativa } ;

ExpressãoMultiplicativa = ExpressãoUnaria { ("*" | "/" | "%") ExpressãoUnaria } ;

ExpressãoUnaria     = [ "-" | "!" ] ExpressãoPrimaria ;

ExpressãoPrimaria   = literal_inteiro
                    | literal_real
                    | literal_texto
                    | literal_char
                    | "true" | "false" | "null"
                    | "this"
                    | "(" Expressão ")"
                    | "[" [ ListaArgumentos ] "]"  (* Array Literal: [1, 2] *)
                    | LValue [ "(" [ ListaArgumentos ] ")" ] ; (* Variável, Campo ou Chamada *)

ListaArgumentos     = Expressão { "," Expressão } ;
