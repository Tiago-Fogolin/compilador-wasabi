```mermaid
graph TD

%% ===============================
%% 1. ROOT
%% ===============================
q0["ROOT (inicial)"]

%% ===============================
%% 2. WHITESPACE
%% ===============================
q0 -- "espacos/tab/\r/\n" --> WS_q1["WHITESPACE"]
WS_q1 -- "repete espacos" --> WS_q1
WS_q1:::final

%% ===============================
%% 3. COMMENTS
%% ===============================
q0 -- "/" --> COM_q1["COM"]
COM_q1 -- "/" --> COM_LINE_q2["// comentário de linha"]
COM_q1 -- "*" --> COM_BLOCK_q3["/* bloco */ início"]
COM_LINE_q2 -- "qualquer char até \\n" --> COM_LINE_q2
COM_LINE_q2:::final

COM_BLOCK_q3 -- "qualquer char exceto *" --> COM_BLOCK_q3
COM_BLOCK_q3 -- "*" --> COM_END_STAR["possível fim"]
COM_END_STAR -- "*" --> COM_END_STAR
COM_END_STAR -- "/" --> COM_BLOCK_FINAL["fim */"]
COM_END_STAR -- "outros" --> COM_BLOCK_q3
COM_BLOCK_FINAL:::final

%% ===============================
%% 4. OPERATORS
%% ===============================
q0 -- "=" --> OP_EQ["="]
OP_EQ -- "=" --> OP_EQ_EQ["=="]
q0 -- "<" --> OP_LE["<"]
OP_LE -- "=" --> OP_LE_EQ["<="]
OP_LE -- "<" --> OP_SHIFT_L["<<"]
q0 -- ">" --> OP_GT[">"]
OP_GT -- "=" --> OP_GE_EQ[">="]
OP_GT -- ">" --> OP_SHIFT_R[">>"]
q0 -- "!" --> OP_NE_BANG["!"]
OP_NE_BANG -- "=" --> OP_NE["!="]
q0 -- "+" --> OP_PLUS["+"]
q0 -- "-" --> OP_MINUS["-"]
q0 -- "*" --> OP_STAR["*"]
q0 -- "%" --> OP_MOD["%"]
q0 -- "&" --> OP_AND["&"]
OP_AND -- "&" --> OP_AND_AND["&&"]
q0 -- "|" --> OP_OR["|"]
OP_OR -- "|" --> OP_OR_OR["||"]
q0 -- "^" --> OP_XOR["^"]
q0 -- "~" --> OP_TILDE["~"]

OP_EQ:::final
OP_EQ_EQ:::final
OP_LE:::final
OP_LE_EQ:::final
OP_SHIFT_L:::final
OP_GT:::final
OP_GE_EQ:::final
OP_SHIFT_R:::final
OP_NE_BANG:::final
OP_NE:::final
OP_PLUS:::final
OP_MINUS:::final
OP_STAR:::final
OP_MOD:::final
OP_AND:::final
OP_AND_AND:::final
OP_OR:::final
OP_OR_OR:::final
OP_XOR:::final
OP_TILDE:::final

%% ===============================
%% 5. IDENTIFIER / KEYWORD
%% ===============================
q0 -- "letra ou _" --> ID_q0["IDENTIFIER"]
ID_q0 -- "letra#59;digito#59;_" --> ID_q0
ID_q0:::final

%% ===============================
%% 6. STRING LITERAL
%% ===============================
q0 -- &quot --> STR_START["inicio string"]
STR_START -- "caracteres comuns" --> STR_CONTENT
STR_START -- &quot --> STR_FINAL["fim string"]
STR_CONTENT -- &quot --> STR_FINAL
STR_CONTENT -- "\\" --> STR_ESCAPE
STR_ESCAPE -- "qualquer char" --> STR_CONTENT
STR_FINAL:::final

%% ===============================
%% 7. CHAR LITERAL
%% ===============================
q0 -- "'" --> CHAR_START["inicio char"]
CHAR_START -- "caractere comum" --> CHAR_CONTENT
CHAR_START -- "\\" --> CHAR_ESCAPE
CHAR_ESCAPE -- "qualquer char" --> CHAR_CONTENT
CHAR_CONTENT -- "'" --> CHAR_FINAL["fim char"]
CHAR_FINAL:::final

%% ===============================
%% 8. NUMBERS
%% ===============================
q0 -- "0-9" --> NUM_INT["inteiro"]
NUM_INT -- "0-9" --> NUM_INT
NUM_INT -- "." --> NUM_POINT["ponto decimal"]
NUM_INT -- "e/E" --> NUM_SCI["científico"]
NUM_POINT -- "0-9" --> NUM_FLOAT["float"]
NUM_FLOAT -- "0-9" --> NUM_FLOAT
NUM_FLOAT -- "e/E" --> NUM_SCI
NUM_SCI -- "+/-" --> NUM_SIGN
NUM_SCI -- "0-9" --> NUM_SCI_FINAL
NUM_SIGN -- "0-9" --> NUM_SCI_FINAL
NUM_SCI_FINAL -- "0-9" --> NUM_SCI_FINAL

NUM_INT:::final
NUM_POINT:::final
NUM_FLOAT:::final
NUM_SCI_FINAL:::final

%% ===============================
%% 9. DELIMITERS
%% ===============================
q0 -- "()[]{}#59;#58;" --> DELIM["DELIMITER"]
DELIM:::final

classDef final fill:#bbf,stroke:#333,stroke-width:1px;
