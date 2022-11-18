#include "Configuracao.h"
#include <EEPROM.h>

#define ENDERECO_BASE 200
#define ENDERECO_SETOR_INICIAL 198
#define ENDERECO_PULSOS_LIMITE 196
#define ENDERECO_PULSOS_MARCACAO 194

class EEPROMDatabase
{
  private: static void gravarConfiguracao(Configuracao setor) {
    uint16_t posicaoMemoria = ENDERECO_BASE + sizeof(setor);
    EEPROM.put(posicaoMemoria, setor);
  }

  public: static void gravaConfiguracaoSetores(Configuracao setores[]){
    EEPROM.put(ENDERECO_SETOR_INICIAL, 0);
    EEPROM.put(ENDERECO_PULSOS_LIMITE, 0);
    EEPROM.put(ENDERECO_PULSOS_MARCACAO, 0);

    for(uint16_t setor=0; setor < QUANTIDADE_SETORES; setor++){
      gravarConfiguracao(setores[setor]);
    }
  }
  public: static void recuperaConfiguracaoSetores(Configuracao setores[]){
    //carrega também o setor inicial atual, isso é uma variável que utilizo se quiser
    //começar a correr no meio da pista, isso serve pra facilitar o teste
    int zero;
    zero = 0;
    EEPROM.get(ENDERECO_SETOR_INICIAL, zero);
    EEPROM.get(ENDERECO_PULSOS_LIMITE, zero);
    EEPROM.get(ENDERECO_PULSOS_MARCACAO, zero);

    uint16_t posicaoMemoria;

    for(uint16_t setor=0; setor < QUANTIDADE_SETORES; setor++){
      posicaoMemoria = ENDERECO_BASE + setor * sizeof(Configuracao);
      EEPROM.get(posicaoMemoria, setores[setor]);
    }
  }
};
