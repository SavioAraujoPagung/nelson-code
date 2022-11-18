#define FORA 10

class Configuracao {
  public:
    int id;
    float p;
    float d;
    float i;
    int16_t velocidade;
  
    //parte do relatorio para envio
    uint32_t tempo;
    int32_t erroAcumulado;

    void insereValorCalibracao (char* chave, char* valor){
      uint8_t temp8;
      float tempFloat;
      if(strncmp(chave,"setor",5)==0){
        temp8 = atoi(valor);
        this->id = temp8;
        return;
      }
      if(strncmp(chave,"velocide",8)==0){
        temp8 = atoi(valor);
        this->velocidade = temp8;
        return;
      }
      if(strncmp(chave,"kp",2)==0){
        tempFloat = atof(valor);
        this->p = tempFloat;
        return;
      }
      if(strncmp(chave,"kd",2)==0){
        tempFloat = atof(valor);
        this->d = tempFloat;
        return;
      }
      if(strncmp(chave,"ki",2)==0){
        tempFloat = atof(valor);
        this->i = tempFloat;
        return;
      }
    }

    void gravarValores(){

    }

  public: static float calculaErro(int16_t a, int16_t b) {
    if (a >= FORA && b >= FORA) { //os dois estão bastante na linha branca
      return a-b;
    }
    else if (a < FORA && b >= FORA) { //o A saiu da linha
      return (200 - b )* -1;
    }
    else if (a >= FORA && b < FORA) { // o B saiu da linha
      return (200 - a);
    }
    return 0;
  }

  public: static bool naLinha(int16_t sensor){
    if(sensor > FORA){
      return true;
    }
    return false;
  }

  public: static bool naLinha(uint8_t e, uint8_t d){
    if(d > FORA || e > FORA) return true;
    else return false;
  }
};