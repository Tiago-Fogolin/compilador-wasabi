# AFD - Comentários

```mermaid
stateDiagram-v2
    [*] --> q0

    q0 --> q1: "/"
    
    q1 --> q2: "/"
    q1 --> q4: "*"
    
    %% Comentário de linha
    state comentario_linha {
        q2 --> q2: qualquer char exceto \n
        q2 --> q3: "\n"
    }
    
    %% Comentário de bloco  
    state comentario_bloco {
        q4 --> q4: qualquer char exceto *
        q4 --> q5: "*"
        q5 --> q6: "/"
        q5 --> q5: "*"
        q5 --> q4: qualquer char exceto * e /
        q6 --> [*]
    }

    %% Estados finais
    q2 --> [*] : EOF
    q6 --> [*]