#include <automatos/AutomatoLexer.hpp>
#include <utils/lexer/LexerUtils.hpp>
#include <vector>
#include <unordered_map>
#include <unordered_set>



AutomatoLexer::AutomatoLexer() : Automato({}, {}) {
    std::vector<NodeAutomato> nodes;

    // ----------------------------
    // 1. ROOT (ESTADO INICIAL: ÍNDICE 0)
    // ----------------------------
    std::unordered_map<char,int> q0; 
    int idxRoot = addNode(nodes, {}); 

    // ----------------------------
    // 2. WHITESPACE
    // ----------------------------
    std::unordered_map<char,int> WS_q1;
    int idxWS_q1 = addNode(nodes, WS_q1);

    WS_q1[' '] = idxWS_q1; WS_q1['\t'] = idxWS_q1;
    WS_q1['\r'] = idxWS_q1; WS_q1['\n'] = idxWS_q1;
    nodes[idxWS_q1].mapaDestino = WS_q1; 

    q0[' '] = idxWS_q1; q0['\t'] = idxWS_q1;
    q0['\r'] = idxWS_q1; q0['\n'] = idxWS_q1;

    estadosFinais.insert(idxWS_q1);
    estadoFinaisTipos[idxWS_q1] = TokenType::WHITESPACE;

    // ----------------------------
    // 3. COMMENTS (CORRIGIDO)
    // ----------------------------
    int idxCOM_q1 = addNode(nodes, {});
    
    // --- SINGLE LINE COMMENT (//...) ---
    int idxCOM_LINE_q2 = addNode(nodes, {});
    nodes[idxCOM_q1].mapaDestino['/'] = idxCOM_LINE_q2;
    estadosFinais.insert(idxCOM_LINE_q2);
    estadoFinaisTipos[idxCOM_LINE_q2] = TokenType::COMMENT;

    
    mapRange(nodes[idxCOM_LINE_q2].mapaDestino, '\0', '\t', idxCOM_LINE_q2);
    mapRange(nodes[idxCOM_LINE_q2].mapaDestino, '\v', '~', idxCOM_LINE_q2);
    
    // --- BLOCK COMMENT (/* ... */) ---
    int idxCOM_BLOCK_q3 = addNode(nodes, {});
    int idxCOM_END_STAR = addNode(nodes, {});
    int idxCOM_BLOCK_FINAL = addNode(nodes, {});

    nodes[idxCOM_q1].mapaDestino['*'] = idxCOM_BLOCK_q3;

    estadosFinais.insert(idxCOM_BLOCK_FINAL);
    estadoFinaisTipos[idxCOM_BLOCK_FINAL] = TokenType::COMMENT;

    mapRange(nodes[idxCOM_BLOCK_q3].mapaDestino, '\0', ')', idxCOM_BLOCK_q3); 
    mapRange(nodes[idxCOM_BLOCK_q3].mapaDestino, '+', '~', idxCOM_BLOCK_q3);
    nodes[idxCOM_BLOCK_q3].mapaDestino['*'] = idxCOM_END_STAR;


    std::unordered_map<char,int> COM_END_STAR;
    COM_END_STAR['/'] = idxCOM_BLOCK_FINAL; 
    COM_END_STAR['*'] = idxCOM_END_STAR;
    

    mapRange(COM_END_STAR, '\0', ')', idxCOM_BLOCK_q3); 
    mapRange(COM_END_STAR, '+', '.', idxCOM_BLOCK_q3);
    mapRange(COM_END_STAR, '0', '~', idxCOM_BLOCK_q3);
    nodes[idxCOM_END_STAR].mapaDestino = COM_END_STAR;


    q0['/'] = idxCOM_q1;
    int idxOP_q7 = idxCOM_q1;


    // ----------------------------
    // 4. OPERATORS
    // ----------------------------

    int idxOP_EQ_EQ   = addNode(nodes, {}); int idxOP_LE      = addNode(nodes, {}); 
    int idxOP_GE      = addNode(nodes, {}); int idxOP_NE      = addNode(nodes, {}); 
    int idxOP_AND_AND = addNode(nodes, {}); int idxOP_OR_OR   = addNode(nodes, {}); 
    int idxOP_SHIFT_L = addNode(nodes, {}); int idxOP_SHIFT_R = addNode(nodes, {}); 

  
    std::unordered_map<char,int> OP_q0 = {{'=', idxOP_EQ_EQ}};
    std::unordered_map<char,int> OP_q1 = {{'=', idxOP_LE}, {'<', idxOP_SHIFT_L}};
    std::unordered_map<char,int> OP_q2 = {{'=', idxOP_GE}, {'>', idxOP_SHIFT_R}};
    std::unordered_map<char,int> OP_q3 = {{'=', idxOP_NE}};
    std::unordered_map<char,int> OP_q9 = {{'&', idxOP_AND_AND}};
    std::unordered_map<char,int> OP_q10 = {{'|', idxOP_OR_OR}};


    std::unordered_map<char,int> OP_q4 = {}; std::unordered_map<char,int> OP_q5 = {};
    std::unordered_map<char,int> OP_q6 = {}; std::unordered_map<char,int> OP_q8 = {};
    std::unordered_map<char,int> OP_q11 = {}; std::unordered_map<char,int> OP_q12 = {};


    int idxOP_q0 = addNode(nodes, OP_q0);  int idxOP_q1 = addNode(nodes, OP_q1); 
    int idxOP_q2 = addNode(nodes, OP_q2);  int idxOP_q3 = addNode(nodes, OP_q3); 
    int idxOP_q4 = addNode(nodes, OP_q4);  int idxOP_q5 = addNode(nodes, OP_q5); 
    int idxOP_q6 = addNode(nodes, OP_q6);  
    int idxOP_q8 = addNode(nodes, OP_q8);  int idxOP_q9 = addNode(nodes, OP_q9);
    int idxOP_q10 = addNode(nodes, OP_q10); int idxOP_q11 = addNode(nodes, OP_q11); 
    int idxOP_q12 = addNode(nodes, OP_q12); 

    q0['='] = idxOP_q0; q0['<'] = idxOP_q1; q0['>'] = idxOP_q2; q0['!'] = idxOP_q3;
    q0['+'] = idxOP_q4; q0['-'] = idxOP_q5; q0['*'] = idxOP_q6; q0['%'] = idxOP_q8;
    q0['&'] = idxOP_q9; q0['|'] = idxOP_q10; q0['^'] = idxOP_q11; q0['~'] = idxOP_q12;


    estadosFinais.insert(idxOP_q0); estadoFinaisTipos[idxOP_q0] = TokenType::ASSIGN;
    estadosFinais.insert(idxOP_q1); estadoFinaisTipos[idxOP_q1] = TokenType::RELATIONAL;
    estadosFinais.insert(idxOP_q2); estadoFinaisTipos[idxOP_q2] = TokenType::RELATIONAL;
    estadosFinais.insert(idxOP_q3); estadoFinaisTipos[idxOP_q3] = TokenType::LOGICAL;
    estadosFinais.insert(idxOP_q4); estadoFinaisTipos[idxOP_q4] = TokenType::OPERATION; // +
    estadosFinais.insert(idxOP_q5); estadoFinaisTipos[idxOP_q5] = TokenType::OPERATION; // -
    estadosFinais.insert(idxOP_q6); estadoFinaisTipos[idxOP_q6] = TokenType::OPERATION; // *
    estadosFinais.insert(idxOP_q7); estadoFinaisTipos[idxOP_q7] = TokenType::OPERATION; // / (idxCOM_q1)
    estadosFinais.insert(idxOP_q8); estadoFinaisTipos[idxOP_q8] = TokenType::OPERATION; // %
    estadosFinais.insert(idxOP_q9); estadoFinaisTipos[idxOP_q9] = TokenType::BITWISE; // &
    estadosFinais.insert(idxOP_q10); estadoFinaisTipos[idxOP_q10] = TokenType::BITWISE; // |
    estadosFinais.insert(idxOP_q11); estadoFinaisTipos[idxOP_q11] = TokenType::BITWISE; // ^
    estadosFinais.insert(idxOP_q12); estadoFinaisTipos[idxOP_q12] = TokenType::BITWISE; // ~
    
    // Multi-caracteres
    estadosFinais.insert(idxOP_EQ_EQ); estadoFinaisTipos[idxOP_EQ_EQ] = TokenType::RELATIONAL;
    estadosFinais.insert(idxOP_LE); estadoFinaisTipos[idxOP_LE] = TokenType::RELATIONAL;
    estadosFinais.insert(idxOP_GE); estadoFinaisTipos[idxOP_GE] = TokenType::RELATIONAL;
    estadosFinais.insert(idxOP_NE); estadoFinaisTipos[idxOP_NE] = TokenType::RELATIONAL;
    estadosFinais.insert(idxOP_AND_AND); estadoFinaisTipos[idxOP_AND_AND] = TokenType::LOGICAL;
    estadosFinais.insert(idxOP_OR_OR); estadoFinaisTipos[idxOP_OR_OR] = TokenType::LOGICAL;
    estadosFinais.insert(idxOP_SHIFT_L); estadoFinaisTipos[idxOP_SHIFT_L] = TokenType::BITWISE_SHIFT;
    estadosFinais.insert(idxOP_SHIFT_R); estadoFinaisTipos[idxOP_SHIFT_R] = TokenType::BITWISE_SHIFT;

    // ----------------------------
    // 5. IDENTIFIER / KEYWORD
    // ----------------------------
    std::unordered_map<char,int> ID_q0;
    int idxID_q0 = addNode(nodes, ID_q0); 

    mapRange(nodes[idxID_q0].mapaDestino,'a','z', idxID_q0);
    mapRange(nodes[idxID_q0].mapaDestino,'A','Z', idxID_q0);
    mapRange(nodes[idxID_q0].mapaDestino,'0','9', idxID_q0);
    nodes[idxID_q0].mapaDestino['_'] = idxID_q0;

    mapRange(q0,'a','z', idxID_q0);
    mapRange(q0,'A','Z', idxID_q0);
    q0['_'] = idxID_q0;

    estadosFinais.insert(idxID_q0);
    estadoFinaisTipos[idxID_q0] = TokenType::IDENTIFIER;

    // ----------------------------
    // 6. STRING LITERAL (SEM MUDANÇAS)
    // ----------------------------
    int idxSTR_FINAL = addNode(nodes, {});
    

    std::unordered_map<char,int> STR_CONTENT;
    STR_CONTENT['"'] = idxSTR_FINAL; 
    STR_CONTENT['\\'] = -1;

    mapRange(STR_CONTENT, '#', '[', -1);  
    mapRange(STR_CONTENT, ']', '~', -1);  
    int idxSTR_CONTENT = addNode(nodes, STR_CONTENT);
    

    std::unordered_map<char,int> STR_ESCAPE;
    STR_ESCAPE['n'] = idxSTR_CONTENT; STR_ESCAPE['t'] = idxSTR_CONTENT;
    STR_ESCAPE['r'] = idxSTR_CONTENT; STR_ESCAPE['\\'] = idxSTR_CONTENT;
    STR_ESCAPE['\''] = idxSTR_CONTENT; STR_ESCAPE['"'] = idxSTR_CONTENT;
    // Aceita qualquer caractere após barra invertida
    mapRange(STR_ESCAPE, ' ', '~', idxSTR_CONTENT);
    int idxSTR_ESCAPE = addNode(nodes, STR_ESCAPE);
    
   
    nodes[idxSTR_CONTENT].mapaDestino['\\'] = idxSTR_ESCAPE;
    mapRange(nodes[idxSTR_CONTENT].mapaDestino, ' ', '!', idxSTR_CONTENT);
    mapRange(nodes[idxSTR_CONTENT].mapaDestino, '#', '[', idxSTR_CONTENT);
    mapRange(nodes[idxSTR_CONTENT].mapaDestino, ']', '~', idxSTR_CONTENT);
    
    // Estado inicial da string
    std::unordered_map<char,int> STR_START;
    STR_START['"'] = idxSTR_FINAL;      
    STR_START['\\'] = idxSTR_ESCAPE;   

    mapRange(STR_START, ' ', '!', idxSTR_CONTENT);
    mapRange(STR_START, '#', '[', idxSTR_CONTENT);
    mapRange(STR_START, ']', '~', idxSTR_CONTENT);
    int idxSTR_START = addNode(nodes, STR_START);
    
    q0['"'] = idxSTR_START;
    
    estadosFinais.insert(idxSTR_FINAL);
    estadoFinaisTipos[idxSTR_FINAL] = TokenType::STRING_LITERAL;

    // ----------------------------
    // 7. CHAR LITERAL (CORRIGIDO)
    // ----------------------------
    int idxCHAR_FINAL = addNode(nodes, {});
    
    
    std::unordered_map<char,int> CHAR_ESCAPE;
    int idxCHAR_ESCAPE = addNode(nodes, CHAR_ESCAPE);
    std::unordered_map<char,int> CHAR_CONTENT;
    int idxCHAR_CONTENT = addNode(nodes, {});

    mapRange(nodes[idxCHAR_ESCAPE].mapaDestino, '\0', '~', idxCHAR_CONTENT); 

    
    CHAR_CONTENT['\''] = idxCHAR_FINAL;
    nodes[idxCHAR_CONTENT].mapaDestino = CHAR_CONTENT;


    std::unordered_map<char,int> CHAR_START;
    CHAR_START['\\'] = idxCHAR_ESCAPE; 
    


    mapRange(CHAR_START, ' ', '&', idxCHAR_CONTENT); 
    mapRange(CHAR_START, '(', '[', idxCHAR_CONTENT);
    mapRange(CHAR_START, ']', '~', idxCHAR_CONTENT);
    int idxCHAR_START = addNode(nodes, CHAR_START);
    
    q0['\''] = idxCHAR_START;
    
    estadosFinais.insert(idxCHAR_FINAL);
    estadoFinaisTipos[idxCHAR_FINAL] = TokenType::CHAR_LITERAL;

    // ----------------------------
    // 8. NUMBERS
    // ----------------------------

    int idxNUM_INT_FINAL = addNode(nodes, {});   
    int idxNUM_FLOAT_FINAL = addNode(nodes, {});
    int idxNUM_SCI_FINAL = addNode(nodes, {}); 


    int idxNUM_POINT = addNode(nodes, {});    
    int idxNUM_SCI = addNode(nodes, {});
    int idxNUM_SIGN = addNode(nodes, {});     

    q0['0'] = idxNUM_INT_FINAL;
    mapRange(q0,'1','9', idxNUM_INT_FINAL);

    // -------------------
    // Inteiros
    // -------------------
    mapRange(nodes[idxNUM_INT_FINAL].mapaDestino, '0', '9', idxNUM_INT_FINAL); // continua lendo dígitos
    nodes[idxNUM_INT_FINAL].mapaDestino['.'] = idxNUM_POINT;                   // ponto decimal inicia float
    nodes[idxNUM_INT_FINAL].mapaDestino['e'] = idxNUM_SCI;                     // expoente inicia científico
    nodes[idxNUM_INT_FINAL].mapaDestino['E'] = idxNUM_SCI;

    // -------------------
    // Float
    // -------------------
    mapRange(nodes[idxNUM_POINT].mapaDestino, '0', '9', idxNUM_FLOAT_FINAL); // aceita dígitos após ponto
    nodes[idxNUM_FLOAT_FINAL].mapaDestino['0'] = idxNUM_FLOAT_FINAL;         // continua aceitando dígitos
    mapRange(nodes[idxNUM_FLOAT_FINAL].mapaDestino, '1', '9', idxNUM_FLOAT_FINAL);
    nodes[idxNUM_FLOAT_FINAL].mapaDestino['e'] = idxNUM_SCI;
    nodes[idxNUM_FLOAT_FINAL].mapaDestino['E'] = idxNUM_SCI;

    // -------------------
    // Scientific
    // -------------------
    mapRange(nodes[idxNUM_SCI].mapaDestino, '0', '9', idxNUM_SCI_FINAL);     // dígitos após 'e' direto
    nodes[idxNUM_SCI].mapaDestino['+'] = idxNUM_SIGN;                        // sinal opcional
    nodes[idxNUM_SCI].mapaDestino['-'] = idxNUM_SIGN;

    mapRange(nodes[idxNUM_SIGN].mapaDestino, '0', '9', idxNUM_SCI_FINAL);    // dígitos após sinal
    mapRange(nodes[idxNUM_SCI_FINAL].mapaDestino, '0', '9', idxNUM_SCI_FINAL); // continua aceitando dígitos

    // -------------------
    // Estados finais
    // -------------------
    estadosFinais.insert(idxNUM_INT_FINAL); estadoFinaisTipos[idxNUM_INT_FINAL] = TokenType::INTEGER;
    estadosFinais.insert(idxNUM_POINT); estadoFinaisTipos[idxNUM_POINT] = TokenType::FLOAT; // <<< FIX: 123. é um float válido
    estadosFinais.insert(idxNUM_FLOAT_FINAL); estadoFinaisTipos[idxNUM_FLOAT_FINAL] = TokenType::FLOAT;
    estadosFinais.insert(idxNUM_SCI_FINAL); estadoFinaisTipos[idxNUM_SCI_FINAL] = TokenType::SCIENTIFIC;

    // ----------------------------
    // 9. DELIMITERS
    // ----------------------------
    std::vector<char> delimiters = {'(', ')', '[', ']', '{', '}', ',', ';', ':'};
    for(char c : delimiters){
        int idx = addNode(nodes, {});
        q0[c] = idx;
        estadosFinais.insert(idx);
        estadoFinaisTipos[idx] = TokenType::DELIMITER;
    }
    
    // Garante que o ponto é adicionado como delimitador.
    if(q0.find('.') == q0.end()){ 
        int idxDOT = addNode(nodes, {});
        q0['.'] = idxDOT;
        estadosFinais.insert(idxDOT);
        estadoFinaisTipos[idxDOT] = TokenType::DELIMITER;
    }
    
    nodes[idxRoot].mapaDestino = q0;
    this->nodes = nodes;
}

std::unique_ptr<Automato> AutomatoLexer::build() {
    return std::make_unique<AutomatoLexer>();
}

std::unique_ptr<Automato> AutomatoLexer::clone() const {
    return std::make_unique<AutomatoLexer>(*this);
}
