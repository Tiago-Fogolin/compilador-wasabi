# AFD - Literais de String

```mermaid
stateDiagram-v2
    [*] --> q0

    %% Abre aspas
    q0 --> q1: \"

    %% Dentro da string: caracteres válidos ou escape
    q1 --> q1: a-z, A-Z, 0-9, ...        %% caracteres válidos
    q1 --> q2: \\                        %% começa escape
    q1 --> q3: \"                        %% fecha string

    %% Escape
    q2 --> q1: n, t, r, \", \', \\       %% exemplos de escapes válidos

    %% Estado final
    q3 --> [*]
