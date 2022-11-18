#include "Serializacao.h"

#define FORA 10
#define QUANTIDADE_SETORES 15

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

  /*essa função aqui faz leitura dos dados por JSON via string, fiz meu próprio parser*/
  public: static bool calibraVeiculo(Configuracao configuracoes[12]){
    char linhaTemp[100];
    char temp[30];
    char atributo[20];
    char valor[10];
    uint8_t quantidadeAtributos;
    int inicioColchete, fimColchete;
    //apago coisas desnecessárias
    Serializacao::obtemPosicaoVetorString("leituraSerial","leituraSerial",'[',1);
    Serializacao::obtemPosicaoVetorString("leituraSerial","leituraSerial",']',0);
    int quantidadeIndices = Serializacao::retornaTamanhoVetorString("leituraSerial",'}');

    if(quantidadeIndices == 0) return false; //algum erro ocorreu
    
    for(uint8_t i=0; i < quantidadeIndices-1; i++){//o ultimo indice é vazio
      Serializacao::obtemSubstringVetorString(linhaTemp,"leituraSerial",'{','}',i+1);
      Serializacao::removeCaractere(linhaTemp, '"');
      Serial.println(linhaTemp);
        quantidadeAtributos = Serializacao::retornaTamanhoVetorString(linhaTemp,',');
      if(quantidadeAtributos == 0) return false; //algum erro ocorreu

      for(uint8_t k=0; k < quantidadeAtributos; k++){
        Serializacao::obtemPosicaoVetorString(temp,linhaTemp,',',k);
        Serializacao::obtemPosicaoVetorString(atributo,temp,':',0);
        Serializacao::obtemPosicaoVetorString(valor,temp,':',1);
        configuracoes[i].insereValorCalibracao(atributo, valor);
      }

      linhaTemp[0]=0;
    }

    return true;
  }

  public: static void inicioCalibracao(Configuracao configuracoes[]){
    //uint16_t k = 0;
    for(uint8_t k = 0; k < QUANTIDADE_SETORES; k++){
      configuracoes[k].p=0.7;
      configuracoes[k].d=20;
      //calibracao[k].ladoPista=1;
      configuracoes[k].velocidade=50;
      //calibracao[k].tempoIgnoraSetor=100;
      //calibracao[k].tempoMudaSetor=100;
      //calibracao[k].tempoForaLinha=200;
    }
  }
};