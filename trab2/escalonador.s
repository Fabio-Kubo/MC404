.org 0x0
.section .iv,"a"
_start:
interrupt_vector:
	b RESET_HANDLER
.org 0x8
	b SYSCALL_HANDLER
.org 0x18
	b IRQ_HANDLER

@===============================
@ Constantes relativas as pilhas
@===============================
.set SVC_STACK,		0x77701000
.set UND_STACK,		0x77702000
.set ABT_STACK,		0x77703000
.set IRQ_STACK,		0x77704000
.set FIQ_STACK,		0x77705000
.set USR_STACK,		0x7770D000 @ comeco da pilha do processo de PID 1
.set STACK,			0x77701000
.set CONTEXT_SIZE,	80
.set USER_CODE,		0x77802000
@===============================
@ Enderecos relativos a UART
@===============================
.set UART1_USR1,	0x53FBC094
.set UART1_UTXD,	0x53FBC040
@===============================
@ Constantes Relativas ao TZIC
@===============================
.set TZIC_BASE,		0x0FFFC000
.set TZIC_INTCTRL,	0x0
.set TZIC_INTSEC1,	0x84 
.set TZIC_ENSET1,	0x104
.set TZIC_PRIOMASK,	0xC
.set TZIC_PRIORITY9,0x424
@===============================
@ Constantes Relativas ao GPT
@===============================
.set GPT_CR,		0x53FA0000
.set GPT_PR,		0x53FA0004
.set GPT_OCR1,		0x53FA0010
.set GPT_IR,		0x53FA000C
.set GPT_SR,		0x53FA0008
.set TIME_SLICE,	0x50000

.align 4
.org 0x100

.text

@===============================
@ RESET_HANDLER
@===============================
RESET_HANDLER:
								@ Set interrupt table base address on coprocessor 15.
	ldr r0, =interrupt_vector
	mcr p15, 0, r0, c12, c0, 0
								@ CONFIGURA GPT
	ldr r0, =GPT_CR				@ carrega GPT_CR
	mov r1, #0x00000041			@ valor que habilita clock_src para periférico
	str r1, [r0]				@ escrita em GPT_CR
	ldr r0, =GPT_PR				@ carrega GPT_PR
	mov r1, #0x0
	str r1, [r0]				@ zerando GPT_PRUART
	ldr r0, =GPT_OCR1			@ carrega GPT_OCR1
	ldr r1, =TIME_SLICE			@ configura time slice
	str r1, [r0]				@ armazenando o valor em GPT_OCR1
	
	ldr r0, =GPT_IR				@ carrega GPT_IR
	mov r1, #0x1
	str r1, [r0]				@ habilita interrupção Output Compare Channel 1

								@ Liga o controlador de interrupcoes
	ldr r1, =TZIC_BASE			@ R1 <= TZIC_BASE
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
	
	msr CPSR_c, #0x13			@ SUPERVISOR mode, IRQ/FIQ enabled
	ldr sp, =SVC_STACK
	msr CPSR_c, #0xDF			@ Enter system mode, FIQ/IRQ disabled
	ldr sp, =USR_STACK
	msr CPSR_c, #0xD1			@ Enter FIQ mode, FIQ/IRQ disabled
	ldr sp, =FIQ_STACK
	msr CPSR_c, #0xD2			@ Enter IRQ mode, FIQ/IRQ disabled
	ldr sp, =IRQ_STACK
	msr CPSR_c, #0xD7			@ Enter abort mode, FIQ/IRQ disabled
	ldr sp, =ABT_STACK
	msr CPSR_c, #0xDB			@ Enter undefined mode, FIQ/IRQ disabled
	ldr sp, =UND_STACK
	
	msr CPSR_c, 0x30			@ habilita modo de usuário
	ldr pc, =USER_CODE			@ entra no código do programa usuário
	
@===============================
@ SYSCALL_HANDLER
@===============================
SYSCALL_HANDLER:
	cmp r7, #0x4
	beq write
	cmp r7, #0x2
	beq fork
	cmp r7, #0x14
	beq get_pid
	cmp r7, #0x1
	beq exit

@===============================
@ IRQ_HANDLER
@===============================
IRQ_HANDLER:
	push {r0, r1}
	ldr r0, =GPT_SR				@ carrega GPT_SR
	mov r1, #1					@ informa GPT que o processador
	str r1, [r0]				@  está ciente da interrupção
	sub lr, #4 					@ ajustando o PC

	ldr r0, =irq_save_return
	str r0, [sp, #-4]!			@ empilha posição de retorno
	
	ldr r0, [sp, #4]			@ recuperando os valores da pilha
	ldr r1, [sp, #8]
	
	b save_context				@ salva contexto
	irq_save_return:
	
	add sp, sp, #12				@ desaloca r0, r1 e endereço de retorno
	
	ldr r0, =PID
	ldr r0, [r0]
	ldr r1, =process_flag
	ldrb r1, [r1]
	
	mov r2, #1
irq_pid_loop:					@ busca próximo processo
	add r0, #1					@ PID++
	cmp r0, #8
	movhs r0, #0				@ PID % 8
	tst r1, r2, lsl r0			@ verifica se processo existe
	beq irq_pid_loop			@ senão, continua ciclo
	
	ldr r1, =PID
	str r0, [r1]				@ novo PID

	ldr r0, =irq_recover_return	@ empilha posição de retorno
	str r0, [sp, #-4]!
	
	b recover_context			@ recupera contexto
	irq_recover_return:
	
	add sp, #4					@ desaloca endereço de retorno
	movs pc, lr					@ retorna e recupera CPSR

@===============================
@ Salvar o contexto
@===============================
save_context:
	push {r4, r5}				@ empilhando registradores que serao sujos 
	ldr r4, =PID
	ldr r4, [r4]				@ carrega em r4 o PID do processo atual
	mov r5, #CONTEXT_SIZE		@ base do contexto
	mul r5, r4
	ldr r4, =context_vector
	add r5, r4

	mrs r4, SPSR
	str r4,  [r5, #64]			@ salva SPSR
	str lr, [r5, #60]			@ salva PC

	stmia r5, {r0-r12}			@ salva [r0-r3],[r7-r12]
	mov r0, r5
	pop {r4, r5}				@ recupera registradores sujados
	str r4, [r0, #16]			@ salva r4
	str r5, [r0, #20]			@ salva r5

	mrs r2, CPSR				@ guarda estado atual
	
	msr CPSR_c, 0xDF			@ modo system sem interrupcao	
	str sp, [r0, #52]			@ salva sp
	str lr, [r0, #56]			@ salva lr

	msr CPSR_c, 0xD3 			@ modo supervisor sem interrupcao

	mov r1, r2, lsl #16			@ se modo é o supervisor,
	cmp r1, #0xD30000			@  ignora o ultimo elemento
	addeq r5, sp, #4			@  que contem o valor
	streq r5, [r0, #68]			@  de retorno da função
	
	strne sp, [r0, #68]			@ salva sp do supervisor
	str lr, [r0, #72]			@ salva lr do supervisor
	mrs r1, SPSR
	str r1, [r0, #76]			@ salva SPSR do supervisor

	msr CPSR_c, r2				@ volta o modo
	ldr pc, [sp] 				@ sai da função
	
@===============================
@ Recuperar o contexto
@===============================
recover_context:
	ldr r4, =PID
	ldr r4, [r4]				@ carrega em r4 o PID do processo atual
	ldr r5, =context_vector		@ base do contexto
	mov r6, #CONTEXT_SIZE
	mla r5, r4, r6, r5			@ r5 aponta para o comeco do contexto

	ldr r0, [r5, #64]
	msr SPSR_fsxc, r0			@ recupera SPSR
	ldr lr, [r5, #60]			@ recupera lr do modo
	
	mrs r0, CPSR
	
	msr CPSR_c, 0xD3			@ modo supervisor sem interrupcao
	ldr sp, [r5, #68]			@ recupera sp do supervisor
	ldr lr, [r5, #72]			@ recupera lr do supervisor
	ldr r1, [r5, #76]
	msr SPSR_fsxc, r1			@ recupera SPSR do supervisor
	
	msr CPSR_c, 0xDF			@ modo system sem interrupcao

	ldr r1, [r5, #4]			@ recupera registradores do usuário
	ldr r2, [r5, #8]
	ldr r3, [r5, #12]
	ldr r4, [r5, #16]
	
	ldr r6, [r5, #24]
	ldr r7, [r5, #28]
	ldr r8, [r5, #32]
	ldr r9, [r5, #36]
	ldr r10, [r5, #40]
	ldr r11, [r5, #44]
	ldr r12, [r5, #48]
	ldr sp, [r5, #52]
	ldr lr, [r5, #56]

	msr CPSR_c, r0				@ volta o modo

	ldr r0, [r5]				@ recupera r0
	ldr r5, [r5, #20]			@ recupera r5
	
	ldr pc, [sp]

@===============================
@  Syscall Write
@===============================
write:
	msr CPSR_c, 0xD3			@ modo supervisor sem interrupcao
	push {r4, r5, r6}
	
	ldr r4, =UART1_USR1			@ addr ucr1
	ldr r6, =UART1_UTXD			@ Endereco do UTXD
	
write_loop:
	ldr r5, [r4] 
	tst r5, #0x2000
	beq write_loop
	ldrb r5, [r1], #1			@ aponta para proximo byte
	strb r5, [r6]				@ escreve o byte na UTXD
	subs r2, #1
	bne write_loop
		
	pop {r4, r5, r6}

	msr CPSR_c, 0x13			@ modo supervisor com interrupcao
	movs pc, lr

@===============================
@  Syscall Fork
@===============================
fork:
	msr CPSR_c, 0xD3			@ modo supervisor sem interrupcao

	ldr r1, =process_flag
	ldrb r1, [r1]
	cmp r1, #0xFF				@ verifica se todos os processos estao ativos
	moveq r0, #-1				@ caso positivo, retorna um erro
	beq fork_end
	
	mov r0, #7
	mov r2, #1
fork_pid_loop:
	add r0, #1					@ PID++
	cmp r0, #8
	movhs r0, #0				@ PID % 8
	tst r1, r2, lsl r0			@ verifica se processo existe
	bne fork_pid_loop			@ senão, continua ciclo

	orr r1, r2, lsl r0			@ liga bit flag
	ldr r2, =process_flag
	strb r1, [r2]				@ atualiza a flag
	
	ldr r1, =temp_pid
	str r0, [r1]
	add r0, r0, #1				@ armazena o PID do filho
	
	ldr r1, =fork_save_return
	push {r1}
	b save_context
	fork_save_return:
	
	add sp, #4					@ desaloca endereco de retorno
	
	push {r4}
	ldr r0, =temp_pid
	ldr r4, [r0]				@ r4 tem o PID do filho
	ldr r1, =PID
	ldr r2, [r1]				@ r2 tem o PID do pai
	
	ldr r0, =STACK
	ldr r1, =0x5000
	add r0, r1
	ldr r1, =0x1000
	
	rsb r4, #7
	mla r3, r4, r1, r0			@ endereco da pilha do processo novo
	rsb r4, r2, #7
	mla r2, r4, r1, r0			@ endereco da pilha do processo antigo
	
	msr CPSR_c, 0xDF
	sub r0, r2, sp
	sub r0, r3, r0
	mov sp, r0
	msr CPSR_c, 0xD3
	
fork_stack_loop:
	subs r1, #1
	ldrb r0, [r2, -r1]
	strb r0, [r3, -r1]
	bne fork_stack_loop
	
	ldr r1, =PID
	ldr r0, =temp_pid
	ldr r0, [r0]
	str r0, [r1]				@ salva PID
	
	ldr r0, =0x200				@ seta pilha do modo supervisor
	sub r2, r2, r0
	sub r0, r2, sp
	sub r0, r3, r0
	add r0, #4					@ referente ao r4

	ldr r4, [sp]				@ recupera r4
	mov sp, r0
	
	mov r0, #0					@ valor de retorno do processo filho

	fork_end:	
	msr CPSR_c, 0x13			@ modo supervisor com interrupcao
	movs pc, lr
	

@===============================
@  Syscall GetPID
@===============================
get_pid:
	ldr r0, =PID
	ldr r0, [r0]				@ o valor do PID é armazenado
	add r0, #1					@  internamente como [0-7]
	movs pc, lr

@===============================
@  Syscall Exit
@===============================
exit:
	msr CPSR_c, 0x13			@ modo supervisor com interrupcao
	ldr r0, =PID
	ldr r0, [r0]
	
	mov r1, #1
	ldr r2, =process_flag
	ldrb r3, [r2]
	bic r3, r1, ror r0			@ desliga o bit do processo
	strb r3, [r2]				@ processo está desativado
	
exit_loop:						@ entra em loop até ser interrompido
	b exit_loop					@ e outro processo ser escalonado
	
	
.data

PID:			.word 0					@ Processo atual
process_flag:	.byte 0x01				@ Flag dos Processos

@ cada contexto tem 20 registradores: r0-r12,sp,lr,pc,CPSR,sp_svc,lr_svc,SPSR_svc
context_vector:	.space	640				@ Vetor de Contextos (640 = 20*4*8)

temp_pid:	.word 1
