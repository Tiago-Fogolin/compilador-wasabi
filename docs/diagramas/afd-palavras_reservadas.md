# AFD - Palavras Reservadas

```mermaid
stateDiagram-v2
    [*] --> q0

    %% Checa se a palavra inteira está no conjunto
    q0 --> q1: palavra ∈ { "interface","struct","implements","def","this","for","foreach","in", "if","else","while","break","continue","return", "int","float","string","bool","tuple","dict","set","void","null" }

    %% Estado final
    q1 --> [*]
