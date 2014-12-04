.org 0x0
.section .iv,"a"

_start:		

interrupt_vector:

    b RESET_HANDLER
.org 0x18
    b IRQ_HANDLER

.org 0x100
.text

    @ Zera o contador
    ldr r2, =CONTADOR
    mov r0,#0
    str r0,[r2]

RESET_HANDLER:
    
    @ Constantes para os enderecos do gpt
    .set GPT_CR,             0x53FA0000
    .set GPT_PR,             0x53FA0004
    .set GPT_OCR1,           0x53FA0010
    .set GPT_IR,             0x53FA000C

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
    @ Constantes para os enderecos do TZIC
    .set TZIC_BASE,             0x0FFFC000
    .set TZIC_INTCTRL,          0x0
    .set TZIC_INTSEC1,          0x84 
    .set TZIC_ENSET1,           0x104
    .set TZIC_PRIOMASK,         0xC
    .set TZIC_PRIORITY9,        0x424

    @ Liga o controlador de interrupcoes
    @ R1 <= TZIC_BASE

    ldr	r1, =TZIC_BASE

    @ Configura interrupcao 39 do GPT como nao segura
    mov	r0, #(1 << 7)
    str	r0, [r1, #TZIC_INTSEC1]

    @ Habilita interrupcao 39 (GPT)
    @ reg1 bit 7 (gpt)

    mov	r0, #(1 << 7)
    str	r0, [r1, #TZIC_ENSET1]

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
    mov	r0, #1
    str	r0, [r1, #TZIC_INTCTRL]

    @instrucao msr - habilita interrupcoes
    msr  CPSR_c, #0x13       @ SUPERVISOR mode, IRQ/FIQ enabled


laco:
        b laco


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

    @Corrige o lr
    sub lr, lr, #4

    movs pc, lr


@Constantes
.set PSR, 0x53F84008


@-------------------------------------------
@ Parametro
@    R0: Id do sonar 
@ Retorno
@    R0: valor da leitura do sonar ou
@        -1 se o id for inválido.
@-------------------------------------------
READ_SONAR:



    @Valida o id do sonar
    cmp r0, 15
    movhi r0, #-1
    bhi id_invalido
    
    @Seta o id do sonar 

    @Sinal baixo na trigger + delay

    @Sinal alto na trigger + delay

    @Sinal baixo na trigger
    @delay enquanto não tiver FLAG = 1

    @Pegar a informação e jogar em r0

    read_sonar_fim:
        movs pc, lr


@-------------------------------------------
@ Parametro
@    R0: id do motor
@    R1: velocidade a ser definida
@ Retorno
@    R0: -1 caso o identificador do motor seja inválido
@        -2 caso a velocidade seja inválida
@         0 caso OK
@-------------------------------------------
SET_MOTOR_SPEED:

    @Constantes
    .set MASCARA_MOTOR_ZERO,  #0b11111111111111111100000001111111
    .set MASCARA_MOTOR_UM,    #0b11111111111111111111111110000000

    @Valida a velocidade
    cmp r1, #0x3f
    movhi r0, #-2
    bhi set_motor_speed_fim

    @Valida o id do motor
    cmp r0, #1
    movhi r0, #-1
    bhi set_motor_speed_fim

    @Carrega o conteudo 
    ldr r3, =PSR @Carrega o registrador PSR

    beq motor_um

    mov  r1, r1, LSL #12 @Desloca o valor para a posicao adequada
    ldr r4, =MASCARA_MOTOR_ZERO @Seta a mascara do motor
    b fim_motor_um

    motor_um:
        ldr r4, =MASCARA_MOTOR_UM @Seta a mascara do motor
    
    fim_motor_um:
        and r3, r3, r4
        orr r3, r3, r1

    set_motor_speed_fim:
        movs pc, lr

SET_MOTORS_SPEED:@TODO

    @Ler r0 bit por bit
    @setar cada bit do registrador responsavel pelos motores
    @Jogar no bit de write o valor 0

    

    
    ldr r1, =PSR    @Carrega o endereço do registrador PSR

    @Procedimento para pegar os ultimos 6 bits, faz uma mascara e faz um or

    mov r2, #1




    
    movs pc, lr

GET_TIME:
    r1, =CONTADOR
    ldr r0, [r1]
    movs pc, lr

SET_TIME:
    r1, =CONTADOR
    stor r0, [r1]
    movs pc, lr

SET_ALARM:  @TODO
    movs pc, lr






.org 0xFFF
.data
CONTADOR: .word 0 
