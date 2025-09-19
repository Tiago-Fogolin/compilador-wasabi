# AFD - Espaços em branco

```mermaid
stateDiagram-v2
    [*] --> q0

    %% Primeira leitura de espaço/tab/quebra de linha
    q0 --> q1: ' ', \t, \r, \n

    %% Loop aceitando os mesmos caracteres
    q1 --> q1: ' ', \t, \r, \n

    %% Estado final
    q1 --> [*]
