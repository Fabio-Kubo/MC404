.global set_speed_motor
.global set_speed_motors
.global read_sonar
.global read_sonars

.align 4

set_speed_motor:		@ Seta a velocidade do motor. Em r0 está a velocidade e em r1 está o motor id (0 for left motor, 1 for right motor)

	stmfd SP!, {r7, lr}
	add r7, r1, #126 	@ Soma o id do motor com 126, pois 126 é o codigo para alterar o motor 0 e 127 para o motor 1
	svc 0x0
	ldmfd SP!, {r7, lr}
	mov pc, lr

set_speed_motors: 		@ Seta as velocidades dos motores. Em r0 está a velocidade do motor 0 e em r1 está a velocidade do motor 1

	stmfd SP!, {r7, lr}
	mov r7, #124
	svc 0x0
	ldmfd SP!, {r7, lr}
	mov pc, lr

read_sonar: 			@ Le o sonar de acordo com o sonarId que é passado em r0. O retorno é em r0

	stmfd SP!, {r7, lr}
	mov r7, #125
	svc 0x0
	ldmfd SP!, {r7, lr}
	mov pc, lr

read_sonars:

	ldmfd SP!, {r7, lr}
		mov r2, r0			@Copia o parametro para r2
		mov r3, #0 			@Inicializa o contador
		mov r7, #125		@Seta a função
	loop:
		cmp r3, #15
		bhi fim_loop
		mov r0, r3		    @Seta o radarId
		svc 0x0
		str r0, [r2, #4]!	@Move o resultado da funcao para o vetor r2
		add r3, r3, #1
		b loop
	
	fim_loop: 
		ldmfd SP!, {r7, lr}
		mov pc, lr
