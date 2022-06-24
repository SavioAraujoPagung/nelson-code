#include <avr/wdt.h>

//******** classe dos motores
class Motor {
  private :
    uint8_t  pino1 ; uint8_t  pino2 ; uint8_t  pwm ;
    
  public :
  Motor (uint8_t  pino1 , uint8_t  pino2 , uint8_t  pwm ){
  #define POTENCIA_MAXIMA 255
    this -> pino1 = pino1 ;
    this ->pino2 = pino2 ;
    this ->pwm = pwm ;
    pinMode (pino1 ,OUTPUT );
    digitalWrite  (pino1 ,0);
    pinMode (pino2 ,OUTPUT );
    digitalWrite  (pino2 ,0);
    pinMode (pwm ,OUTPUT );
    digitalWrite  (pwm ,0);
  }
   #define POTENCIA_MINIMA 255
  void potencia(int16_t v){
    if(v > 0){
      v = constrain(v,0,POTENCIA_MINIMA);
      digitalWrite(pino1  ,1);
      digitalWrite(pino2  ,0);
      analogWrite(pwm ,abs(v));
    }else if(v < 0){
      v = constrain(v,-POTENCIA_MINIMA,0);
      digitalWrite(pino1 ,0);
      digitalWrite(pino2 ,1);
      analogWrite(pwm ,abs(v));
    }else{
      digitalWrite(pino1,0);
      digitalWrite(pino2 ,0);
      digitalWrite(pwm ,1);
    }
  }
};
