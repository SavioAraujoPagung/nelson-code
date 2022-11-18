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
};