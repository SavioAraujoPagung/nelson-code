#include <SoftwareSerial.h>


//macro de funcoes
#define set_bit(y,bit) (y|=(1<<bit))//coloca em 1 o bit x da variável Y
#define clt_bit(y,bit) (y&=~(1<<bit))//coloca em 0 o bit x da variável Y
#define cpl_bit(y,bit) (y^=(1<<bit))// troca o estado logico do bit x da variável Y
#define tst_bit(y,bit) (y&(1<<bit))// retorna 0 ou 1 conforme leitura do bit
#define PINO_LED 13


//pino onde ficam ligado os motores
#define  pwm1 3
#define  PINO1 4
#define  PINO2 2

#define  pwm2 11
#define  PINO3 6
#define  PINO4 7

SoftwareSerial softwareSerial(9, 8); // RX, TX