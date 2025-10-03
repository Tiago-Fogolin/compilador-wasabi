```mermaid
stateDiagram-v2
    [*] --> q0


    q0 --> WS_q1: ' ', \t, \r, \n
    WS_q1 --> WS_q1: ' ', \t, \r, \n
    WS_q1 --> [*]  %% WHITESPACE

    q0 --> WS_q2: /
    WS_q2 --> WS_q3: /
    WS_q3 --> WS_q3: qualquer char exceto \n
    WS_q3 --> WS_q4: "\n"
    WS_q3 --> [*]  %% COMMENT (EOF também termina comentário de linha)
    WS_q4 --> [*]  %% COMMENT

    WS_q2 --> WS_q5: *
    WS_q5 --> WS_q5: qualquer char exceto *
    WS_q5 --> WS_q6: *
    WS_q6 --> WS_q7: /
    WS_q6 --> WS_q6: *
    WS_q6 --> WS_q5: qualquer char exceto * e /
    WS_q7 --> [*]  %% COMMENT


    q0 --> OP_q0: =
    q0 --> OP_q1: <
    q0 --> OP_q2: >
    q0 --> OP_q3: !
    q0 --> OP_q4: +
    q0 --> OP_q5: -
    q0 --> OP_q6: *
    q0 --> OP_q7: /
    q0 --> OP_q8: %
    q0 --> OP_q9: &
    q0 --> OP_q10: |
    q0 --> OP_q11: ^
    q0 --> OP_q12: ~
    q0 --> OP_q13: <
    q0 --> OP_q14: >

    OP_q4 --> OP_q15: =
    OP_q0 --> OP_q15: =

    OP_q0 --> OP_q16: =
    OP_q4 --> OP_q16: =
    OP_q4 --> OP_q17: =

    OP_q4 --> OP_q18: =

    %% Bitwise shift
    OP_q13 --> OP_q19: <
    OP_q14 --> OP_q20: >

    %% Lógicos
    OP_q9 --> OP_q21: &
    OP_q10 --> OP_q22: |

    %% Estados finais operadores
    OP_q0 --> [*]   %% ASSIGN (= sozinho)
    OP_q1 --> [*]   %% OPERATION (< sozinho)
    OP_q2 --> [*]   %% OPERATION (> sozinho)
    OP_q3 --> [*]   %% LOGICAL (! sozinho)
    OP_q4 --> [*]   %% OPERATION (+ sozinho)
    OP_q5 --> [*]   %% OPERATION (- sozinho)
    OP_q6 --> [*]   %% OPERATION (* sozinho)
    OP_q7 --> [*]   %% OPERATION (/ sozinho)
    OP_q8 --> [*]   %% OPERATION (% sozinho)
    OP_q9 --> [*]   %% BITWISE (& sozinho)
    OP_q10 --> [*]  %% BITWISE (| sozinho)
    OP_q11 --> [*]  %% BITWISE (^ sozinho)
    OP_q12 --> [*]  %% BITWISE (~ sozinho)
    OP_q13 --> [*]  %% OPERATION (<< sozinho)
    OP_q14 --> [*]  %% OPERATION (>> sozinho)
    OP_q15 --> [*]  %% ASSIGN (atribuição composta)
    OP_q16 --> [*]  %% ASSIGN (atribuição composta)
    OP_q17 --> [*]  %% RELATIONAL (==, !=, <=, >=)
    OP_q18 --> [*]  %% RELATIONAL (==, !=, <=, >=)
    OP_q19 --> [*]  %% BITWISE_SHIFT (<<)
    OP_q20 --> [*]  %% BITWISE_SHIFT (>>)
    OP_q21 --> [*]  %% LOGICAL (&&)
    OP_q22 --> [*]  %% LOGICAL (||)


    q0 --> ID_q0: a-z, A-Z, _  
    q0 --> CHAR_q0: \'
    q0 --> STR_q0: \"

    %% Identificador / palavra reservada
    ID_q0 --> ID_q0: a-z, A-Z, 0-9, _ 
    ID_q0 --> [*]  %% IDENTIFIER/KEYWORD (lexer decide)

    %% Literal de caractere
    CHAR_q0 --> CHAR_q1: a-z, A-Z, 0-9, ...  
    CHAR_q0 --> CHAR_q2: \\                   
    CHAR_q2 --> CHAR_q1: n, t, r, \', \", \\   
    CHAR_q1 --> CHAR_q3: \'                       
    CHAR_q3 --> [*]  %% CHAR_LITERAL

    %% Literal de string
    STR_q0 --> STR_q1: a-z, A-Z, 0-9, ...      
    STR_q0 --> STR_q2: \\                      
    STR_q2 --> STR_q1: n, t, r, \', \", \\     
    STR_q1 --> STR_q3: \"                       
    STR_q3 --> [*]  %% STRING_LITERAL


    q0 --> NUM_q0: 0
    q0 --> NUM_q1: 1-9

    %% Inteiros
    NUM_q0 --> NUM_q0: 0
    NUM_q1 --> NUM_q1: 0-9

    %% Ponto decimal
    NUM_q0 --> NUM_q2: .
    NUM_q1 --> NUM_q2: .

    %% Flutuantes
    NUM_q2 --> NUM_q3: 0-9
    NUM_q3 --> NUM_q3: 0-9

    %% Científico
    NUM_q1 --> NUM_q4: e/E
    NUM_q3 --> NUM_q4: e/E
    NUM_q4 --> NUM_q5: +/-
    NUM_q4 --> NUM_q6: 0-9
    NUM_q5 --> NUM_q6: 0-9
    NUM_q6 --> NUM_q6: 0-9

    %% Estados finais números
    NUM_q0 --> [*]  %% INTEGER
    NUM_q1 --> [*]  %% INTEGER
    NUM_q3 --> [*]  %% FLOAT
    NUM_q6 --> [*]  %% SCIENTIFIC


    q0 --> DEL_q0: (
    q0 --> DEL_q1: )
    q0 --> DEL_q2: [
    q0 --> DEL_q3: ]
    q0 --> DEL_q4: {
    q0 --> DEL_q5: }
    q0 --> DEL_q6: ,
    q0 --> DEL_q7: "#59;"
    q0 --> DEL_q8: #58;
    q0 --> DEL_q9: .

    DEL_q0 --> [*]  %% DELIMITER
    DEL_q1 --> [*]  %% DELIMITER
    DEL_q2 --> [*]  %% DELIMITER
    DEL_q3 --> [*]  %% DELIMITER
    DEL_q4 --> [*]  %% DELIMITER
    DEL_q5 --> [*]  %% DELIMITER
    DEL_q6 --> [*]  %% DELIMITER
    DEL_q7 --> [*]  %% DELIMITER
    DEL_q8 --> [*]  %% DELIMITER
    DEL_q9 --> [*]  %% DELIMITER
