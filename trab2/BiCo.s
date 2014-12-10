.global set_speed_motor
.global set_speed_motors
.global read_sonar
.global read_sonars

.align 4

set_speed_motor:		@ Seta a velocidade do motor. Em r0 está a velocidade e em r1 está o motor id (0 for left motor, 1 for right motor)

	stmfd SP!, {r7}
	add r7, r1, #9 	@ Soma o id do motor com 126, pois 126 é o codigo para alterar o motor 0 e 127 para o motor 1
	svc 0x0
	ldmfd SP!, {r7}
	mov pc, lr

set_speed_motors: 		@ Seta as velocidades dos motores. Em r0 está a velocidade do motor 0 e em r1 está a velocidade do motor 1

	stmfd SP!, {r7}
	mov r7, #10
	svc 0x0
	ldmfd SP!, {r7}
	mov pc, lr

read_sonar: 			@ Le o sonar de acordo com o sonarId que é passado em r0. O retorno é em r0

	stmfd SP!, {r7}
	mov r7, #8
	svc 0x0
	ldmfd SP!, {r7}
	mov pc, lr

read_sonars:

	ldmfd SP!, {r7}
		mov r2, r0			@Copia o parametro para r2
		mov r3, #0 			@Inicializa o contador
		mov r7, #8		@Seta a função
	loop:
		cmp r3, #15
		bhi fim_loop
		mov r0, r3		    @Seta o radarId
		svc 0x0
		str r0, [r2, #4]!	@Move o resultado da funcao para o vetor r2
		add r3, r3, #1
		b loop
	
	fim_loop: 
		ldmfd SP!, {r7}
		mov pc, lr


add_alarm:
	ldmfd SP!, {r7}
	mov r7, #13
	svc 0x0
	ldmfd SP!, {r7}
	mov pc, lr

get_time:
	ldmfd SP!, {r7}
	mov r7, #11
	svc 0x0
	ldmfd SP!, {r7}
	mov pc, lr

set_time:
	ldmfd SP!, {r7}
	mov r7, #12
	svc 0x0
	ldmfd SP!, {r7}
	mov pc, lr
