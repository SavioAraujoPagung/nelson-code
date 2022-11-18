#include "Configuracao.h"
#include <EEPROM.h>

#define ENDERECO_BASE 200
#define ENDERECO_SETOR_INICIAL 198
#define ENDERECO_PULSOS_LIMITE 196
#define ENDERECO_PULSOS_MARCACAO 194

class EEPROMDatabase
{
  private: static void gravarConfiguracao(Configuracao configuracao) {
    uint16_t posicaoMemoria = ENDERECO_BASE + sizeof(Configuracao);
    EEPROM.put(posicaoMemoria, configuracao);
  }

  public: static void gravaConfiguracaoSetores(Configuracao configuracoes[]){
    EEPROM.put(ENDERECO_SETOR_INICIAL, 0);
    EEPROM.put(ENDERECO_PULSOS_LIMITE, 0);
    EEPROM.put(ENDERECO_PULSOS_MARCACAO, 0);
    
    uint8_t quantidadeSetores;
    quantidadeSetores = sizeof(configuracoes);

    for(uint16_t posicao=0; posicao < quantidadeSetores; posicao++){
      gravarConfiguracao(configuracoes[posicao]);
    }
  }
  public: static void recuperaConfiguracaoSetores(Configuracao configuracoes[]){
    //carrega também o setor inicial atual, isso é uma variável que utilizo se quiser
    //começar a correr no meio da pista, isso serve pra facilitar o teste
    int zero;
    zero = 0;
    EEPROM.get(ENDERECO_SETOR_INICIAL, zero);
    EEPROM.get(ENDERECO_PULSOS_LIMITE, zero);
    EEPROM.get(ENDERECO_PULSOS_MARCACAO, zero);

    uint16_t posicaoMemoria;
    uint8_t quantidadeSetores;
    quantidadeSetores = sizeof(configuracoes);

    for(uint16_t setor=0; setor < quantidadeSetores; setor++){
      posicaoMemoria = ENDERECO_BASE + setor*sizeof(Configuracao);
      EEPROM.get(posicaoMemoria, configuracoes[setor]);
    }
  }
};
