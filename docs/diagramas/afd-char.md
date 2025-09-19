# AFD - Literais de Caractere

```mermaid
stateDiagram-v2
    [*] --> q0

    %% Abre aspas
    q0 --> q1: \'

    %% Caractere simples ou escape
    q1 --> q3: a-z, A-Z, 0-9, ...  %% caractere válido
    q1 --> q2: \\                    %% começa escape

    %% Escape
    q2 --> q3: n, t, r, \', \", \\   %% exemplos de escapes válidos

    %% Fecha aspas
    q3 --> q4: \'

    %% Estado final
    q4 --> [*]
