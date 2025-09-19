# AFD - Palavras Booleanas

```mermaid
stateDiagram-v2
    [*] --> q0

    %% Sequência "false"
    q0 --> q1: f
    q1 --> q2: a
    q2 --> q3: l
    q3 --> q4: s
    q4 --> q5: e

    %% Sequência "true"
    q0 --> q6: t
    q6 --> q7: r
    q7 --> q8: u
    q8 --> q9: e

    %% Estados finais
    q5 --> [*]
    q9 --> [*]
