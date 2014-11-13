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
	@ args = 0, pretend = 0, frame = 64
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #64
.L3:
	mov	r0, #25
	mov	r1, #25
	bl	set_speed_motors
	bl	delay
	mov	r0, #10
	mov	r1, #0
	bl	set_speed_motor
	bl	delay
	mov	r0, #25
	mov	r1, #10
	bl	set_speed_motors
	bl	delay
	sub	r3, fp, #68
	mov	r0, r3
	bl	read_sonars
	ldr	r3, [fp, #-52]
	cmp	r3, #1200
	bls	.L4
	ldr	r3, [fp, #-56]
	cmp	r3, #1200
	bhi	.L3
.L4:
	sub	sp, fp, #4
	ldmfd	sp!, {fp, pc}
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
	b	.L6
.L7:
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
.L6:
	ldr	r2, [fp, #-8]
	ldr	r3, .L9
	cmp	r2, r3
	ble	.L7
	add	sp, fp, #0
	ldmfd	sp!, {fp}
	bx	lr
.L10:
	.align	2
.L9:
	.word	9999
	.size	delay, .-delay
	.ident	"GCC: (GNU) 4.4.3"
	.section	.note.GNU-stack,"",%progbits
