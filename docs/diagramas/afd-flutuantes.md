# AFD - Números de ponto flutuante

```mermaid
stateDiagram-v2
    [*] --> q0

    %% Primeiro dígito
    q0 --> q1: 1-9
    q0 --> q3: 0

    %% Sequência de dígitos antes do ponto
    q1 --> q1: 0-9
    q1 --> q2: .

    %% Caso 0.x
    q3 --> q2: .

    %% Dígitos depois do ponto
    q2 --> q4: 0-9
    q4 --> q4: 0-9

    %% Estados finais
    q2 --> [*]
    q4 --> [*]
