# AFD - Operadores Relacionais

```mermaid
stateDiagram-v2
    [*] --> q0

    %% Transições do estado inicial
    q0 --> q1: \=
    q0 --> q2: \<
    q0 --> q3: \>
    q0 --> q4: \!

    %% Segunda parte dos operadores
    q1 --> q5: \=
    q2 --> q5: \=
    q3 --> q5: \=
    q4 --> q5: \=

    %% Estados finais
    q2 --> [*]
    q3 --> [*]
    q5 --> [*]
