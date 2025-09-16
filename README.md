# Criar automato

- em compilador/src/lexer/TokensAutomatos.cpp  
- criar uma classe ```Automato criarAutomatoExemplo() {}```  
- criar vetor de nodes com ```std::vector\<NodeAutomato\> nodes;```

## “Dicionário” de mapeamento (node)

- contém um carácter que aponta para qual node esse carácter leva.  
- criar com ```std::unordered\_map\<char, int\> mapaRoot;``` (verificar padrão de nomes no código)  
- aponte para um node com ```mapaRoot\['0'\] \= 1;``` \<- essa linha faz com que, quando o caracter for zero, mova para o node 1\. Analisar codigo para melhor entendimento  
- adicionar node no vetor de nodes assim: ```addNode(nodes, mapaRoot);```

## Estados finais

- caso um node seja um possivel final, ao inves de só adicionar no vetor, gravar em uma variável. Exemplo:  ```int idxNode2 \= addNode(nodes, mapaNode2);```  
- definir os estados finais assim ```std::set\<int\> estadosFinais \= {idxNode1, idxNode2};``` com o(s) node(s) que são finais é claro  
- retorna o vetor de nodes e os estados finais: ```return Automato(nodes,estadosFinais);```

# Compilar e testar

- dentro de compilador/include/TokensAutomatos.hpp   
- adicionar o automato criado igual ta la  
- dentro de compilador/include/  
- criar um arquivo com nome no mesmo padrao dos outros  
- recomendo duplicar algum deles e só mudar os nomes deles  
- colocar la os testes e se for pra funcionar é true se não é false  
- dentro de compilador/tests/lexer/automatos/test\_runner.cpp  
- adicionar o teste igual os outros la  
- esqueci como compila nem deve funcionar.
- Compila assim ó: (estar na pasta compilador) rodar ```mingw32-make -f .\MakeFile.mk```
- Ele vai gerar um executavel na pasta builds

prontoooo
