

    AREA |.text|, CODE, READONLY
        
main3 PROC
  
    EXPORT main3
        
    ;IMPORT Hardware_Init 
     
    PUSH {R0, LR}
    ;BL Hardware_Init
    POP {R0, LR}
    
    
    ENDP    
       
    NOP
    END
        