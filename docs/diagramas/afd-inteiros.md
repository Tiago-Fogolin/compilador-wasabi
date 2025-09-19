# AFD - Números Inteiros

```mermaid
stateDiagram-v2
    [*] --> q0

    %% Primeiro dígito
    q0 --> q1: 0
    q0 --> q2: 1-9

    %% Continuação
    q1 --> q1: 0
    q2 --> q2: 0-9

    %% Estados finais
    q1 --> [*]
    q2 --> [*]
