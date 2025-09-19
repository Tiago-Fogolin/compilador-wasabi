# 📘 Especificação da Linguagem com Expressões Regulares

## 1. Tokens e Expressões Regulares

* **Identificadores:**

```
[a-zA-Z_][a-zA-Z0-9_]*
```

* **Literais Numéricos:**

* Inteiros:

```
0|[1-9][0-9]*
```

* Decimais:

```
[0-9]+\.[0-9]+
```

* Notação Científica:

```
[0-9]+(\.[0-9]+)?[eE][+-]?[0-9]+
```

* **Strings e Caracteres:**

* String:

```
"([^"\\]|\\.)*"
```

* Char:

```
'(\\.|[^'])'
```

* **Booleanos:**

```
\b(true|false)\b
```

* **Operadores:**

* Aritméticos:

```
(\+{1,2}|-{1,2}|\*{1,2}|/|%)
```

* Racionais:

```
(==|!=|<=|>=|<|>)
```

* Atribuição:

```
(=|\+=|-=|\*=|/=|%=)
```

* Lógicos:

```
(&&|\|\||!)
```

* Bitwise:

```
(&|\||\^|~|<<|>>)
```

* **Delimitadores e Separadores:**

```
(\(|\)|\[|\]|\{|\})
```

```
(,|;|:)
```

```
\.
```

* **Comentários:**

* Linha:

```
//.*
```

* Bloco:

```
/\*([^*]|\*+[^/])*\*+/
```

* **Espaços em Branco:**

```
[ \t\r\n]+ 
```

* **Palavras-chave adicionais:**

```
\b(interface|struct|implements|def|this|for|foreach|in|if|elif|else|while|break|continue|return|int|float|string|bool|tuple|dict|set|void|null)\b
```

## 2. Análise de Ambiguidades e Regras de Resolução

* **Dangling-else:**

  * O `else` associa-se ao `if` mais próximo sem `else`.
* **Identificadores vs. palavras-chave:**

  * Palavras-chave não podem ser reconhecidas como identificadores.
* **Operadores compostos:**

  * Operadores de múltiplos caracteres (ex.: `==`, `!=`, `+=`) têm prioridade sobre operadores simples para evitar ambiguidade.

## 3. Estratégia para Tratamento de Erros Léxicos

* Tokens não reconhecidos pelo analisador léxico geram erro.
* Comentários e espaços em branco são ignorados, não gerando erro.
* Sequências parcialmente válidas devem apontar a posição do erro para facilitar depuração.

## 4. Primeiros Esboços de Mensagens de Erro para Usuários

* **Identificador inválido:** `Erro léxico: identificador inválido na linha X, coluna Y.`
* **Número inválido:** `Erro léxico: literal numérico inválido na linha X, coluna Y.`
* **String ou char mal formado:** `Erro léxico: string ou char inválido na linha X, coluna Y.`
* **Token desconhecido:** `Erro léxico: token desconhecido '%token%' na linha X, coluna Y.`
* **Operador inválido:** `Erro léxico: operador inválido na linha X, coluna Y.`