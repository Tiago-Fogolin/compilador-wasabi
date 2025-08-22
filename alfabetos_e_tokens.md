## **1\. Especificação do Alfabeto da Linguagem**

O alfabeto **Σ** da linguagem será composto pelos seguintes grupos de símbolos:

* **Letras**: `a–z`, `A–Z`  
* **Dígitos:** `0–9`  
* **Underscore:** `_`  
* **Símbolos especiais**: `_` (underscore), `"` (aspas duplas), `'` (aspas simples), `.` (ponto), `,` (vírgula), `:` (dois pontos), `;` (ponto e vírgula)  
* **Operadores aritméticos**: `+ - * /`  
* **Operadores relacionais/lógicos**: `= < > ! & |`  
* **Delimitadores**: `() [] {}`  
* **Espaços em branco**: espaço (`␣`), tabulação (`\t`), quebra de linha (`\n`, `\r\n)`  
* **Comentários:** `// ...` **(linha única) e** `/* ... */` **(bloco)**

## **2. Definição Formal dos Tokens**

### **a) Identificadores**

* Devem começar por letra ou underscore.

* Podem conter letras, dígitos e underscores.

* Case-sensitive.

  ID = `( a−z ∪ A−Z ∪ _ )( a−z ∪ A−Z ∪ 0−9 ∪ _)∗`

---

### **b) Números**

* **Inteiros**:  
  * NUM int  \=  (0−9)\+

Ex: `42, 1000`

* **Decimais**:  
  * NUM dec  \= (0−9)\+  "."  (0−9)\+

Ex: `3.14`, `0.5`

* **Notação científica**:  
  * NUM sci \= (0−9)\+ ((˙0−9)\+) ? \[eE\] \[+−\] ? (0−9) \+

  Ex: `1.2e10`, `5E-3`

---

### **c) Strings**

* Delimitadas por aspas duplas `"..."`.  
* Podem conter qualquer caractere, exceto `"` não escapado.  
  * STR= " ( Σ −{""})∗"

Ex: `"Olá, mundo"`, `"vendas_2025.csv"`

---

### **d) Palavras-chave**

Reservadas da linguagem:

`import, from, clear, filter, statistics, if, elif, else, while, for, return, interface , struct , implements, this, def, foreach , in , int, string, float, bool, tuple, dict, set, void. null, break, continue, try, catch, finally, throw.`  
---

### **e) Operadores e Delimitadores**

* **Aritméticos**: `+ - * /`  
* **Relacionais**: `== != < <= > >=`  
* **Lógicos**: `&& || !`  
* **Delimitadores**: `() [] {}`  
* **Separadores**: `, ;`


# 1. Alfabeto da linguagem:  
* letras `(a-z, A-Z)`  
* dígitos `(0-9)`  
* Underscore `(\_)`  
* operadores aritméticos `( \+ \- \*  /)`  
* operadores relacionais `( \= \< \> \! & | )`  
* delimitadores e agrupadores `( () \[\] {})`
* pontuação `(: , .)`  
* aspas `(“)` para string  
* espaço em branco: `espaço ( ), tab (`\t`), quebra de linha (`\n`, `\r\n`)`   
* comentários `(// /**/)`

# 2. Tipos de tokens 

## 2.1 indentificadores  
  ID → `[a-zA-Z_]\[a-zA-Z0-9_]*`  
## 2.2 Palavras-chave (reservadas)  
	KEYWORD → `interface | struct | implements | this | def | if | else | elif | for | foreach | in | int | string | float | bool | tuple | dict | set | void | null | return | while | break | continue | import | from | try | catch | finally | throw`
## 2.3 Operadores  
	OP → \+ | - | \* | / | % | = | == | != | < | > | <= | >= | && | \|\| | !
## 2.4 Delimitadores  
	DELIM →  \( | \) | \[ | \] | \{ | \} | , | : | ; | \.
## 2.5 Comentários
	LINE_COMMENT → //.*; BLOCK_COMMENT → /\*([^*]|\*+[^/])*\*+/
  
## 2.6 Espaços em Branco  
	WS → [ \t\n\r]+

# 3. Exemplos concretos de programas válidos na linguagem

```
data = import('dados_exemplo.csv')

print(statistics(data))
```

```
interface Forma:
  def calcular_area()

struct Quadrado implements Forma:
  lado: float

  def calcular_area():
    return this.lado ** 2

struct Triangulo implements Forma:
  base: float
  altura: float

  def calcular_area():
    return this.base * this.altura / 2

quadrado = Quadrado(4)
triangulo = Triangulo(2, 5)
formas = [quadrado, triangulo]

foreach forma in formas:
  forma.calcular_area()
```
