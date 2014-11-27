	.arch armv5te
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 18, 4
	.file	"main.c"
	.text
	.align	2
	.global	_start
	.type	_start, %function
_start:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #8
.L4:
	mov	r0, #3
	bl	read_sonar
	mov	r3, r0
	str	r3, [fp, #-8]
	mov	r0, #4
	bl	read_sonar
	mov	r3, r0
	str	r3, [fp, #-12]
	ldr	r3, [fp, #-12]
	cmp	r3, #1200
	ble	.L2
	ldr	r3, [fp, #-8]
	cmp	r3, #1200
	ble	.L2
	mov	r0, #25
	mov	r1, #25
	bl	set_speed_motors
	b	.L3
.L2:
	mov	r0, #0
	mov	r1, #15
	bl	set_speed_motors
.L3:
	bl	delay
	b	.L4
	.size	_start, .-_start
	.align	2
	.global	delay
	.type	delay, %function
delay:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #12
	mov	r3, #0
	str	r3, [fp, #-8]
	b	.L7
.L8:
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
.L7:
	ldr	r2, [fp, #-8]
	ldr	r3, .L10
	cmp	r2, r3
	ble	.L8
	add	sp, fp, #0
	ldmfd	sp!, {fp}
	bx	lr
.L11:
	.align	2
.L10:
	.word	9999
	.size	delay, .-delay
	.ident	"GCC: (GNU) 4.4.3"
	.section	.note.GNU-stack,"",%progbits
