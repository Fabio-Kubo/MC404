.org 0x0
.section .iv,"a"

_start:
interrupt_vector:
    b RESET_HANDLER
.org 0x08
    b SUPERVISOR_HANDLER
.org 0x18
    b IRQ_HANDLER

@--------------------------------------------------------
@ Constantes para os enderecos do gpt
@--------------------------------------------------------
.set GPT_CR,             0x53FA0000
.set GPT_PR,             0x53FA0004
.set GPT_OCR1,           0x53FA0010
.set GPT_IR,             0x53FA000C

@--------------------------------------------------------
@ Constantes para os enderecos do TZIC
@--------------------------------------------------------
.set TZIC_BASE,             0x0FFFC000
.set TZIC_INTCTRL,          0x0
.set TZIC_INTSEC1,          0x84 
.set TZIC_ENSET1,           0x104
.set TZIC_PRIOMASK,         0xC
.set TZIC_PRIORITY9,        0x424

@--------------------------------------------------------
@ Constantes registradores
@--------------------------------------------------------
.set REG_DR,    0x53F84000     @Data Register
.set REG_GDIR,  0x53F84004     @Direction Register
.set REG_PSR,   0x53F84008     @Pad status register - apenas para leitura
.set VALOR_GDIR,               0b01111100000000000011111111111111

@--------------------------------------------------------
@ Mascaras
@--------------------------------------------------------
.set MASCARA_MOTOR_ZERO,            0b11111111111111111100000001111111
.set MASCARA_MOTOR_UM,              0b11111111111111111111111110000000
.set MASCARA_MOTORES,               0b11111111111111111100000000000000
.set MASCARA_SONAR_MUX,             0b10000011111111111111111111111111
.set MASCARA_SINAL_ALTO_TRIGGER,    0b01000000000000000000000000000000
.set MASCARA_SINAL_BAIXO_TRIGGER,   0b10111111111111111111111111111111
.set MASCARA_FLAG,                  0b10000000000000000000000000000000
.set MASCARA_SONAR_DATA,             0b00000011111111111100000000000000

@--------------------------------------------------------
@ Outras constantes
@--------------------------------------------------------
.set MAX_ALARMS, 16
.set LOCO_CODE, 0x77802000


.align 4
.org 0x100

.text

    @ Zera o contador
    ldr r2, =CONTADOR
    mov r0,#0
    str r0,[r2]

RESET_HANDLER:
    
    @Set interrupt table base address on coprocessor 15.
    ldr r0, =interrupt_vector
    mcr p15, 0, r0, c12, c0, 0


    @Habilita e configura o clock_src para periférico
    mov r3, #0x41
    ldr r2, =GPT_CR
    str r3, [r2]

    @Zera o prescaler (GPT_PR)
    mov r3, #0
    ldr r2, =GPT_PR
    str r3, [r2]

    @Configura o GPT_OCR1 com o valor que desejo contar
    mov r3, #64
    ldr r2, = GPT_OCR1
    str r3, [r2]

    @Habilita a interrupção gravando 1 no registrador GPT_IR
    mov r3, #1
    ldr r2, =GPT_IR 
    str r3, [r2]

SET_TZIC:

    @ Liga o controlador de interrupcoes
    @ R1 <= TZIC_BASE

    ldr r1, =TZIC_BASE

    @ Configura interrupcao 39 do GPT como nao segura
    mov r0, #(1 << 7)
    str r0, [r1, #TZIC_INTSEC1]

    @ Habilita interrupcao 39 (GPT)
    @ reg1 bit 7 (gpt)

    mov r0, #(1 << 7)
    str r0, [r1, #TZIC_ENSET1]

    @ Configure interrupt39 priority as 1
    @ reg9, byte 3

    ldr r0, [r1, #TZIC_PRIORITY9]
    bic r0, r0, #0xFF000000
    mov r2, #1
    orr r0, r0, r2, lsl #24
    str r0, [r1, #TZIC_PRIORITY9]

    @ Configure PRIOMASK as 0
    eor r0, r0, r0
    str r0, [r1, #TZIC_PRIOMASK]

    @ Habilita o controlador de interrupcoes
    mov r0, #1
    str r0, [r1, #TZIC_INTCTRL]

    @instrucao msr - habilita interrupcoes
    msr  CPSR_c, #0x13       @ SUPERVISOR mode, IRQ/FIQ enabled


    @Zera o DR
    mov r1, #0
    ldr r0, =REG_DR
    str r1, [r0]

    @Configura o GDIR
    ldr r0, =REG_GDIR
    ldr r1, =VALOR_GDIR @Carrega o valor do gdir
    str r1, [r0]        @Salva o novo valor
    
    msr CPSR_c, 0x30            @ habilita modo de usuário
    ldr pc, =LOCO_CODE          @ entra no código do programa usuário

laco:
        b laco


SUPERVISOR_HANDLER:
    
    cmp r7, #8
    bleq READ_SONAR
    b fim
    
    cmp r7, #9
    bleq SET_MOTOR_SPEED
    b fim
    
    cmp r7, #10
    bleq SET_MOTORS_SPEED
    b fim
    
    cmp r7, #11
    bleq GET_TIME
    b fim 
    
    cmp r7, #12
    bleq SET_TIME
    b fim

    cmp r7, #13
    bleq SET_ALARM

    fim:
    movs pc, lr

@-------------------------------------------
@ Parametro
@    R0: id do alarme 
@ Retorno
@    nenhum
@-------------------------------------------
CHAMA_FUNCAO_ALERTA:

    @Pega a funcao que esta armazenada
    @Altera para usuario padrao
    @ Chama funcao
    @Altera para super ususario

mov pc, lr


IRQ_HANDLER:

    .set GPT_SR,    0x53FA0008

    @Informa ao GPT que o processador já está ciente de que ocorreu a interrupção
    mov r3, #0
    ldr r2, = GPT_SR
    str r3, [r2]

    @Incrementa contador
    ldr r2, = CONTADOR
    ldr r3, [r2]
    add r3, r3, #1
    str r3, [r2]

    @Verifica se existe algum alarme
    mov r0, #0
    ldr r1, =QTD_ALARM
    ldr r1, [r1]    @Carrega a quantidade de alarmes
    ldr r2, =ALARMES @Carrega o endereco do vetor Alarmes

    
    iteraVetor:
        cmp r0, r1
        bhi iteraVetor_fim
        ldr r4, [r2, #4]! 

        cmp r4, r3 @ Compara o tempo do alarme com o tempo do contador 
        blls CHAMA_FUNCAO_ALERTA
        add r0, r0, #1
        b iteraVetor

    iteraVetor_fim:

    @Corrige o lr
    sub lr, lr, #4




DELAY:
    mov pc, lr
    

@-------------------------------------------
@ Parametro
@    R0: Id do sonar 
@ Retorno
@    R0: valor da leitura do sonar ou
@        -1 se o id for inválido.
@-------------------------------------------
READ_SONAR:

    @Valida o id do sonar
    cmp r0, #15
    movhi r0, #-1
    bhi read_sonar_fim

    @Carrega o conteudo 
    ldr r1, =REG_DR @Carrega o registrador REG_DR
    ldr r2, [r1]

    @Seta o id do sonar + Sinal baixo na trigger
    mov  r0, r0, LSL #26 @Desloca o valor para a posicao adequada
    ldr r3, =MASCARA_SONAR_MUX @Seta a mascara
    and r2, r2, r3 @Zera as posicões do id do sonar e da trigger
    str r2, [r1]

    @Delay
    bl DELAY
        
    @Sinal alto na trigger + delay
    ldr r3, =MASCARA_SINAL_ALTO_TRIGGER @Seta a mascara
    orr r2, r2, r3 @Seta sinal alto na trigger
    str r2, [r1]

    @Sinal baixo na trigger
    ldr r3, =MASCARA_SINAL_BAIXO_TRIGGER @Seta a mascara
    and r2, r2, r3 @Seta sinal baixo na trigger   
    str r2, [r1]

    delay:
    bl DELAY
    ldr r2, [r1]


    @delay enquanto não tiver FLAG = 1

    @Pegar a informação e jogar em r0

    read_sonar_fim:
        mov pc, lr


@-------------------------------------------
@ Parametro
@    R0: id do motor
@    R1: velocidade a ser definida
@ Retorno
@    R0: -1 caso o id do motor seja inválido
@        -2 caso a velocidade seja inválida
@         0 caso OK
@-------------------------------------------
SET_MOTOR_SPEED:

    stmfd SP!, {r4}

    @Valida a velocidade
    cmp r1, #0x3f
    movhi r0, #-2
    bhi set_motor_speed_fim

    @Valida o id do motor
    cmp r0, #1
    movhi r0, #-1
    bhi set_motor_speed_fim

    @Indica que os parametros estão OK
    mov r0, #0

    @Carrega o conteudo 
    ldr r4, =REG_DR @Carrega o registrador REG_DR
    ldr r3, [r4]

    @Se for o motor 1, pula para sua configuraçao
    beq motor_um

    mov  r1, r1, LSL #7 @Desloca o valor para a posicao adequada
    ldr r2, =MASCARA_MOTOR_ZERO @Seta a mascara do motor
    b seta_valor

    motor_um:
        ldr r2, =MASCARA_MOTOR_UM @Seta a mascara do motor    
    
    seta_valor:
        and r3, r3, r2 @Zera a posicao com a velocidade do motor e o bit de write
        orr r3, r3, r1 @Configura a nova velocidade
        str r3, [r4] @Guarda o novo valor

    set_motor_speed_fim:
        ldmfd SP!, {r4}
        mov pc, lr

@-------------------------------------------
@ Parametro
@    R0: Velocidade para o motor 0.
@    R1: Velocidade para o motor 1.
@ Retorno
@    R0: -1 caso a velocidade para o motor 0 seja inválida
@        -2 caso a velocidade para o motor 1 seja inválida
@         0 caso Ok.
@-------------------------------------------
SET_MOTORS_SPEED:@TODO

    stmfd SP!, {r4}

    @Valida a velocidade
    cmp r0, #0x3f
    movhi r0, #-1
    bhi set_motors_speed_fim

    @Valida a velocidade
    cmp r1, #0x3f
    movhi r0, #-2
    bhi set_motors_speed_fim

    @Carrega o conteudo 
    ldr r4, =REG_DR @Carrega o registrador REG_DR
    ldr r3, [r4]

    mov  r0, r0, LSL #7 @Desloca o valor para a posicao adequada
    ldr r2, =MASCARA_MOTORES @Carrega a mascara
       
    and r3, r3, r2 @Zera a posicao com as velocidades dos motores e os bits de write
    
    orr r3, r3, r0 @Configura a nova velocidade motor 0
    orr r3, r3, r1 @Configura a nova velocidade motor 1
    str r3, [r4] @Guarda o novo valor

    @Indica que os parametros estão OK
    mov r0, #0

    set_motors_speed_fim:
        ldmfd SP!, {r4}
        mov pc, lr

@-------------------------------------------
@ Parametro
@   Nenhum
@ Retorno
@    R0: Contador
@-------------------------------------------
GET_TIME:
    ldr r1, =CONTADOR
    ldr r0, [r1]
    mov pc, lr

@-------------------------------------------
@ Parametro
@   r0: novo valor do contador
@ Retorno
@    Nenhum
@-------------------------------------------
SET_TIME:
    ldr r1, =CONTADOR
    str r0, [r1]
    mov pc, lr

@-------------------------------------------
@ Parametro
@   R0: ponteiro para função a ser chamada na ocorrência do alarme.
@   R1: tempo do sistema. 
@ Retorno
@    R0: -1 caso o número de alarmes máximo ativo no sistema seja maior do que MAX_ALARMS. 
@        -2 caso o tempo seja menor do que o tempo atual do sistema. 
@         0 caso contrário.
@-------------------------------------------
SET_ALARM:  @TODO

    stmfd SP!, {r4-r9}

    @Incrementa o contador QTD_ALARM
    ldr r2, =QTD_ALARM
    ldr r3, [r2]
    add r3, r3, #1
    
    @Valida quantidade maxima
    ldr r4, =MAX_ALARMS 
    ldr r4, [r4]
    cmp r3, r4
    movhi r0, #-1     @Caso seja maior, retorna o codigo do erro em r0
    bhi set_alarm_fim @Caso seja maior, vai para o final da funcao
    
    @Valida o tempo
    ldr r4, =CONTADOR
    ldr r4, [r4]
    cmp r1, r4
    movls r0, #-2       @Se for menor ou igual, seta o codigo do erro em r0
    blls set_alarm_fim  @vai para o final da função

    str r3, [r2]  @Salva o novo valor no contador
    
    ldr r3, =ALARMES @Carrega o endereço dos alarmes
    ldr r6, =FUNCOES_ALARMES @Carrega o endereço das funcoes dos alarmes

    @Encontra a posicao em que deve ser inserido (a lista é ordenada decrescente)
    procura_posicao:
        ldr r2, [r3]
        cmp r2, #-1
        beq salva_novo_alarme

        cmp r1, r2
        bhi desloca_lista
        add r3, r3, #4
        add r6, r6, #4


    desloca_lista:
    add r5, r3, #4 @ Coloca em r5 o endereco do proximo alarme

    ldr r8, [r6] @Carrega a funçao atual
    add r7, r6, #4 @ Coloca em r7 o endereco do proximo função

    loop_deslocamento:
    
        ldr r4, [r5]           @Carrega o proximo alarme
        ldr r9, [r7]           @Carrega a proxima funçao

        str r2, [r5]            @Armazena o alarme da posicao i em i+1
        str r8, [r7]            @Armazena a funcao da posicao i em i+1

        cmp r4, #-1             @Verifica se o alarme que estava na posicao i+1 está vazia
        beq salva_novo_alarme   @Se estiver vazia, acabou o deslocamento

        mov r2, r4              @Caso contrário, coloco em r2 o alarme do i+1
        mov r8, r9              @Caso contrário, coloco em r2 a função do i+1

        add r5, r5, #4           @Avanço para o alarme de posicao i+2
        add r7, r7, #4           @Avanço para função de posicao i+2
        b loop_deslocamento     


    salva_novo_alarme:
    str r1, [r3] @Salva o alarme
    str r0, [r6] @Salva a função

    mov r0, #0 @Coloca em r0 o valor que indica que o alarme foi adicionado com sucesso!
    
    set_alarm_fim:
    ldmfd SP!, {r4-r9}
    mov pc, lr


.org 0xFFF
.data
CONTADOR:  .word 0
QTD_ALARM: .word 0
ALARMES: 
    .word -1
    .word -1
    .word -1
    .word -1
    .word -1
    .word -1
    .word -1
    .word -1
    .word -1
    .word -1
    .word -1
    .word -1
    .word -1
    .word -1
    .word -1
    .word -1

FUNCOES_ALARMES:
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0


