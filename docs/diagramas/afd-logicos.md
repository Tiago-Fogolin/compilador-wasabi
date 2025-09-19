# AFD - Literais de Lógicos

```mermaid
stateDiagram-v2
    [*] --> q0

    q0 --> q3 : '!'
    q0 --> q1 : '&'
    q0 --> q2 : '|'

    q1 --> q3 : '&'
    q2 --> q3 : '|'

    q3 --> [*]

