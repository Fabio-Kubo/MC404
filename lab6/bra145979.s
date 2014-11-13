.text
.align 4
.globl _start

_start:                         @ main
        
        mov r0, #0              @ Carrega em r0 a velocidade do motor 0.
                                @ Lembre-se: apenas os 6 bits menos significativos
                                @ serao utilizados.
        mov r1, #0              @ Carrega em r1 a velocidade do motor 1.
        mov r7, #124            @ Identifica a syscall 124 (write_motors).
        svc 0x0                 @ Faz a chamada da syscall.

        ldr r6, =1200           @ r6 <- 1200 (Limiar para parar o robo)

loop:        
        mov r0, #3              @ Define em r0 o identificador do sonar a ser consultado.
        mov r7, #125            @ Identifica a syscall 125 (read_sonar).
        svc 0x0                 
        mov r5, r0              @ Armazena o retorno da syscall.

        mov r0, #4              @ Define em r0 o sonar.
        mov r7, #125
        svc 0x0

        mov r6, #0              @ Inicializa a variavel de controle de quem está mais perto do obstaculo (0-Sonar_3, 1-Sonar_4)
        cmp r5, r0              @ Compara o retorno (em r0) com r5.
        movge r6, #1            @ Caso o sonar_4 esteja mais perto
        bge min                 @ Se r5 > r0: Salta pra min
        mov r0, r5              @ Senao: r0 <- r5

min:
        cmp r0, r6              @ Compara r0 com r6
        blt rotacionar          @ Se r0 menor que o limiar: Salta para rotacionar

        mov r0, #36            @ Senao define uma velocidade para os 2 motores
        mov r1, #36
        mov r7, #124        
        svc 0x0

        b loop                  @ Refaz toda a logica

rotacionar:

        cmp r6, #1
        bge rotacionarDireita
        b rotacionarEsquerda

rotacionarEsquerda:             @ Rotaciona o Uóli para direita
        
        mov r0, #18  
        mov r1, #0
        mov r7, #124
        svc 0x0

        b loop                  @ Refaz toda a logica

rotacionarDireita:              @ Rotaciona o Uóli para direita
        
        mov r0, #0 
        mov r1, #18
        mov r7, #124        
        svc 0x0

        b loop                  @ Refaz toda a logica


                

