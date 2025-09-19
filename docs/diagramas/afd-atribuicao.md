# AFD - Operadores de Atribuição

```mermaid
stateDiagram-v2
    [*] --> q0

    %% Caminhos para operadores compostos
    q0 --> q1: +
    q0 --> q2: -
    q0 --> q3: *
    q0 --> q4: /
    q0 --> q5: %

    q1 --> q6: =
    q2 --> q6: =
    q3 --> q6: =
    q4 --> q6: =
    q5 --> q6: =

    %% Atribuição simples
    q0 --> q6: =

    %% Estado final
    q6 --> [*]
