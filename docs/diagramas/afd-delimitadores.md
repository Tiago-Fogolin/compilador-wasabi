# AFD - Delimitadores

```mermaid
stateDiagram-v2
    [*] --> q0

    %% Transições do nó raiz para o nó final
    q0 --> q1: "("
    q0 --> q1: ")"
    q0 --> q1: "["
    q0 --> q1: "]"
    q0 --> q1: "{"
    q0 --> q1: "}"
    q0 --> q1: ","
    q0 --> q1: "#59;"
    q0 --> q1: #58;
    q0 --> q1: "."

    %% Estado final
    q1 --> [*]

