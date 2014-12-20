#include "api_robot2.h" /* Robot control API */
#define FRENTE_SONAR_ID 4
#define DIREITA_SONAR_ID 6
#define ESQUERDA_SONAR_ID 1
#define DIREITA_MOTOR_ID 0
#define ESQUERDA_MOTOR_ID 1

void delay(int delay);
void dance();
void race();

/* main function */
void _start(void){

	//add_alarm(dance, 30);
	race();
}

void passinho_180_D(int duracao){
	set_motors_speed(15, 0);
	delay(duracao);
}

void passinho_180_E(int duracao){
	set_motors_speed(0, 15);
	delay(duracao);
}

void passinhoPraFrente(int duracao){
	set_motors_speed(15, 15);
	delay(duracao);
}

void dance(){
	passinho_180_D(10000);
	passinho_180_E(10000);

	passinhoPraFrente(7000);

	passinho_180_E(5000);
	passinho_180_D(5000);
	passinho_180_E(5000);
	passinho_180_D(5000);

	passinhoPraFrente(7000);
}

void race(){
		
	int distanciaFrente, distanciaEsquerda, distanciaDireita;

	set_motors_speed(20, 20);
	
	/*for(;;){
		
		set_motors_speed(0, 0); // para o robozinho

		distanciaFrente = read_sonar(FRENTE_SONAR_ID);
		distanciaFrente = read_sonar(ESQUERDA_SONAR_ID);
		distanciaFrente = read_sonar(DIREITA_SONAR_ID);

		if(distanciaDireita > distanciaEsquerda){
			if(distanciaDireita > distanciaFrente)
				set_motors_speed(0, 10);
		}
		else{
			if(distanciaEsquerda > distanciaFrente){
				set_motors_speed(10, 0);
			}
		}

		set_motors_speed(15,15);
		delay(10000);
	}
		*/
}

/* Spend some time doing nothing. */
void delay(int delay){
  int i;
  /* Not the best way to delay */
  for(i = 0; i < delay; i++ );  
}
