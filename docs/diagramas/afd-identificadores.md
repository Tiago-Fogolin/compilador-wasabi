# AFD - Identificadores

```mermaid
stateDiagram-v2
    [*] --> q0

    %% Primeiro caractere
    q0 --> q1: a-z, A-Z, _

    %% Caracteres seguintes
    q1 --> q1: a-z, A-Z, 0-9, _

    %% Estado final
    q1 --> [*]
