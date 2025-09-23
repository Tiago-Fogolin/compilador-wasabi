# AFD - Notação Científica

```mermaid
stateDiagram-v2
    [*] --> 0

    %% Parte inteira
    q0 --> q1 : '0-9'
    q1 --> q1 : '0-9'

    %% Parte decimal opcional
    q1 --> q2 : '.'
    q2 --> q3 : '0-9'
    q3 --> q3 : '0-9'

    %% exp
    q1 --> q4 : 'e/E'
    q3 --> q4 : 'e/E'

    %% sinal opcional
    q4 --> q5 : '+/-'

    %% exp
    q4 --> q6 : '0-9'
    q5 --> q6 : '0-9'
    q6 --> q6 : '0-9'

    q6 --> [*]

