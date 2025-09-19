# AFD - Operadores Bitwise

```mermaid
stateDiagram-v2
    [*] --> q0

    %% Operadores simples
    q0 --> q1: \&
    q0 --> q2: \|
    q0 --> q3: \^
    q0 --> q4: \~

    %% Operador <<
    q0 --> q5: \<
    q5 --> q6: \<

    %% Operador >>
    q0 --> q7: \>
    q7 --> q8: \>

    %% Estados finais
    q1 --> [*]
    q2 --> [*]
    q3 --> [*]
    q4 --> [*]
    q6 --> [*]
    q8 --> [*]
