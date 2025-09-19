# AFD - Operadores Aritméticos

```mermaid
stateDiagram-v2
    [*] --> q0

    %% Operadores simples
    q0 --> q1: \+
    q0 --> q2: \-
    q0 --> q3: \*
    q0 --> q4: \/
    q0 --> q5: \%

    %% Operadores duplos
    q1 --> q5: \+
    q2 --> q6: \-
    q3 --> q7: \*
    q4 --> q8: /

    %% Estados finais
    q1 --> [*]
    q2 --> [*]
    q3 --> [*]
    q4 --> [*]
    q5 --> [*]
    q6 --> [*]
    q7 --> [*]
    q8 --> [*]
