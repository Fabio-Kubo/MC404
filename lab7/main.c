#include "api_robot.h" /* Robot control API */

void rotacionaEsquerda();
void rotacionaDireita();
void delay();

/* main function */
void _start(void) 
{
  
  int distanciaD, distanciaE;

  for(;;){
  
    distanciaE = read_sonar(3);
    distanciaD = read_sonar(4);

    //se não estiver próximo a alguma borda, o robozinho vai reto
    if(distanciaD > 1200 && distanciaE > 1200){
      set_speed_motors(25, 25);
    }
    else{
      set_speed_motors(0, 15);
    }
    delay();
  }
}

/* Spend some time doing nothing. */
void delay()
{
  int i;
  /* Not the best way to delay */
  for(i = 0; i < 10000; i++ );  
}
