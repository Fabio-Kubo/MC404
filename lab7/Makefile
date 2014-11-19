linker: main.o api_robot.o
	arm-eabi-ld main.o api_robot.o -o robot_ctrl -Ttext=0x77802000
main.o: main.s
	arm-eabi-as main.s -o main.o
main.s: main.c
	arm-eabi-gcc main.c -S -o main.s
api_robot.o: api_robot.s
	arm-eabi-as api_robot.s -o api_robot.o
