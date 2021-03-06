@ O objetivo deste codigo eh mostrar o n-esimo valor da sequencia de fibonacci em 16 bits.
@ O parametro n tbm eh um 16 bits

.text
.align 4
.globl _start

_start:                       @ main

@@@@@@@@@ Le uma string binaria e converte para um valor em r0 @@@@@@@@@@@@@@
        
        mov r0, #0            @ carrega o valor 0 em r0, indicando que a entrada da
                              @ syscall read sera em stdin
        ldr r1, =string       @ carrega em r1 o endereco da string
        mov r2, #17           @ carrega em r2 o tamanho da string. r0,r1 e r2 serao
                              @ os argumentos da syscall read
        mov r7, #3            @ carrega o valor 3 para r7, indica o tipo da syscall (read)
        svc 0x0               @ realiza uma chamada de sistema (syscall)


        mov  r0, #0           @ para armazenar valor convertido
        ldr r1, =string       @ carrega em r1 o endereco da string
        mov  r2, #0           @ iterador de loop

loop_c:
        mov  r0, r0, LSL #1   @ desloca o valor de r0 para esquerda 
                              @ LSB (bit menos significativo) de r0 recebe 0 
        ldrb r3, [r1, r2]     @ r3 = string[r2]
        cmp  r3, #49          @ compara r3 com '1'  (49 em ASCII)
        blt  reset            @ se for menor: r0 ja esta com 0 no LSB
        orr  r0, r0, #1       @ senao: LSB de r0 recebe 1

reset:
        add r2, r2, #1        @ incrementa variavel de controle da iteracao
        cmp r2, #16           @ compara r2 com o tamanho da string binaria
        blt loop_c            @ se ainda esta com um valor menor que o tamanho da string


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        
        cmp r0, #2             @ Compara o "n" com o 2
        movle r5, #1           @ Se for menor ou igual a 2, o resultado recebe 1 
        ble fim_laco_Fibonacci @ Se for menor ou igual a 2, ele pula para o fim
        
        mov r2, #2             @ Inicializa contador
        mov r4, #1             @ Inicializa o aux (n - 1)
        mov r3, #1             @ Inicializa o aux (n - 2)

laco_Fibonacci:
        add r5, r3, r4         @ r5  recebe a soma de (n - 1) com (n - 2)
        add r2, r2, #1         @ Incrementa o contador
        cmp r2, r0             @ Compara se r3 com r2
        bge fim_laco_Fibonacci @ Se r2 >= r0, ele pula para o fim
        mov r3, r4
        mov r4, r5
        b laco_Fibonacci

fim_laco_Fibonacci:
        mov r0, r5             @ Copia o resultado para r0


@@@@@@@@@@@@@@@@@@@@@@@ Valor de r0 para string binaria @@@@@@@@@@@@@@ 
 

        ldr r1, =string       @ string  para preencher com string binaria
        mov r2, #15           @ contador de iteracao
        mov r3, #1            @ valor para shift

loop:
        mov r4, #49           @ r4 = '1'    
        and r5, r0, r3        @ r5 = r0 AND r3
        cmp r5, #0            @ compara r5 com 0 (zero)
        bgt set               @ se for maior: r4 ja esta com '1'
        mov r4, #48           @ senao: r4 = '0'
set:
        strb r4, [r1, r2]     @ string[r2] = r4
        sub  r2, r2, #1       @ decrementa a variavel de controle do laco
        mov  r3, r3, LSL #1   @ r3 = r3 << 1
        cmp  r2, #0           @ compara r com zero
        bge  loop             @ se ainda e maior ou igual, volta para o loop


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ imprime a string binaria

        mov r0, #1            @ carrega o valor 1 em r0, indicando que a saida da
                              @ syscall write sera em stdout
        ldr r1, =string       @ carrega em r1 o endereco da string
        mov r2, #17           @ carrega em r2 o tamanho da string. r0,r1 e r2 serao
                              @ os argumentos da syscall write
        mov r7, #4            @ carrega o valor 4 para r7, indica o tipo da syscall
        svc 0x0               @ realiza uma chamada de sistema (syscall)


        mov r7, #1            @ carrega o valor 1 em r7, indicando a escolha da
                              @ syscall exit
        svc 0x0         


.data
string:  .asciz "0000000000000000\n"  @coloca a st
