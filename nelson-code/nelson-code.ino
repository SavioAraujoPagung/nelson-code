#include "EEPROM.h"
#include "EEPROMDatabase.h"
#include "Configuracoes.h"
#include "Utils.h"
#include "Sensor.h"
#include "Motor.h"

char leituraSerial[MAXSIZEBUFFER];

int16_t tentativa=0;

float erro;
float erroAnterior;
float Kp = 3;
float Kd = 50;
int16_t velocidade = 100;
uint16_t contadorFora = 0;
char pTemp[6];
char dTemp[6];
char iTemp[6];
char textoTemp[100];

#define TEMPOMAXIMOFORA 200

#define TEMPOLEDACESSO_IGNORARNOVOSETOR 50

#define QUANTIDADELEITURAMARCADOR 4

#define APERTADO LOW

int tempoLed=0;

Sensor sensorLinha;
Motor motorDireito(PINO4 ,PINO3 ,pwm2 );
Motor motorEsquerdo(PINO2 ,PINO1 ,pwm1 );

Configuracao setores[12];


uint32_t tt;

#define BOTAO2 12
#define PINOMARCADORSETOR 18
#define PINOMARCADORSETOR2 17
#define PINOMARCADORFIMPISTA 16

#define PINOENCODERESQUERDO 19

uint8_t sensorEsquerdo, sensorDireito;
int16_t potenciaMotorEsquerdo, potenciaMotorDireito;
uint16_t setorAtual=0;
uint16_t contadorMarcadorSetor=0;
uint16_t contadorMarcadorFimPista=0;
uint8_t marcadorDireito=0;
uint8_t marcadorEsquerdo=0;

#define QUANTIDADETOTALMARCADORFIMPISTA 13
uint16_t quantidadeMarcadorFimPista=0;
uint32_t tempoMilis;
//coloque o código nessa função
//atenção, não pode ter um while travando a saida da função. Ela tem que ficar em loop
//ou seja, essa função é sempre chamada
//automaticamente essas variáveis são atualizadas por outros códigos
void setup() {
	EEPROMDatabase::gravaConfiguracaoSetores(setores);

	Serial.begin(38400);
	softwareSerial.begin(38400);
	motorDireito.potencia(0);
	motorEsquerdo.potencia(0);
	
	pinMode(BOTAO2, INPUT_PULLUP);
	pinMode(BOTAO, INPUT_PULLUP);
	pinMode(PINOENCODERESQUERDO,INPUT_PULLUP);

	sensorLinha.iniciaSensor();
	Configuracao::inicioCalibracao(setores); //Savio, isso aqui deixa toda a configuracao estática

	EEPROMDatabase::recuperaConfiguracaoSetores(setores);
	
  //	imprimeCalibracao(); savio, volta com isso ao normal
	//gravaCalibracao();
	//pinMode(8,INPUT_PULLUP);
	//pinMode(9,INPUT_PULLUP);
	aguardaInicio();
	setorAtual=setorInicial;
	//ladoPista = calibracao[setorAtual].ladoPista;
	//vai começar, adiciona um na tentativa
	EEPROM.get(ENDERECOTENTATIVA,tentativa);
	tentativa++;
	EEPROM.put(ENDERECOTENTATIVA,tentativa);
	tempoMilis=millis();
}

void aguardaInicio(){
	uint8_t sensor0=0, sensor1=0;
	uint8_t marcador_curva1=0, marcador_curva2=0, marcador_final=0;
	int i=0,x=0;
	while(digitalRead(BOTAO) != APERTADO && digitalRead(BOTAO2) != APERTADO ){
		sensorLinha.atualizaSensor();
		sensor0 = sensorLinha.obtemIntensidade(0);
		sensor1 = sensorLinha.obtemIntensidade(1);
		marcador_curva1 = (digitalRead(PINOMARCADORSETOR)==APERTADO);
		marcador_curva2 = (digitalRead(PINOMARCADORSETOR2)==APERTADO);
		marcador_final = (digitalRead(PINOMARCADORFIMPISTA)==APERTADO);
		//sensor2 = sensorLinha.obtemIntensidade(2);
		//sensor3 = sensorLinha.obtemIntensidade(3);
        //sensor4 = sensorLinha.obtemIntensidade(4);
        //sensor5 = sensorLinha.obtemIntensidade(5);
		if(sensor0 > CONSIDERADO_LINHA || sensor1 > CONSIDERADO_LINHA
		|| marcador_curva1 == true || marcador_curva2 == true || marcador_final == true){
			digitalWrite(PINO_LED,HIGH);
		//}else if(digitalRead(8) == APERTADO || digitalRead(9) == APERTADO/*|| digitalRead(5)== APERTADO*/){
		//	digitalWrite(PINO_LED,HIGH);
		}
		else{
			digitalWrite(PINO_LED,LOW);
		}
		//verifica se tem algo para calibrar
		if(softwareSerial.available()>0){
			uint32_t tempo = millis();
			i=0;
			while(i < MAXSIZEBUFFER){
				x = softwareSerial.read();
				if(x!=-1){
					leituraSerial[i] = (char)x;
					i++;
				}
				if((millis() - tempo) > 2000) {
					leituraSerial[i]=0;
					break;
				}

			}
			if(i == MAXSIZEBUFFER){
				leituraSerial[MAXSIZEBUFFER-1]=0;
			}
			if(leituraSerial[0] == 'r'){//solicita relatorio
				Utils::enviaRelatorio(setores); //DOTO:envia os dados do relatorio do dos setores 
			}else{
				Serial.println(F("entrei aqui pra calibrar"));
				Serial.println(leituraSerial);
				Configuracao::calibraVeiculo(setores);
				//gravaCalibracao();//todo: savio, falta passar o vetor com os setores aqui 
				Utils::imprimeCalibracao(setores);
			}
		}
		/*
		Serial.print("\nerro: ");
		erro = calculaErro(sensor0,sensor1);
		Serial.println(erro);
		Serial.print("MD: ");
		Serial.print(velocidade - ((int)((erro*sqrt(abs(erro))*Kp))));
		Serial.print(" ME: ");
		Serial.print(velocidade + ((int)((erro*sqrt(abs(erro))*Kp))));
		delay(100);
		*/
	}
	Serial.println(F("saiu do aguardaInicio"));
	Utils::limpaRelatorio(setores);
	delay(1000);
	/*
	if(digitalRead(BOTAO2) == APERTADO){
		calibracao[0].velocidade = calibracao[1].velocidade;
		calibracao[0].p = calibracao[1].p;
		calibracao[0].d = calibracao[1].d;
	}
	*/
	
}

uint32_t tempoInicial, tempoFinal;
uint16_t contadorLoop=0;
char leituraVelocidade=0;

void loop(){
	tt = micros();
	//verifica se tem algo na serial para calibrar
		/*if(Serial.available()>0){
			leituraSerial = softwareSerial.readStringUntil(']');
			calibraVeiculo();
		}*/
		
	//leitura dos sensores analógicos e marcadores analogicos
	sensorLinha.atualizaSensor();
	sensorDireito = sensorLinha.obtemIntensidade(1);
	sensorEsquerdo = sensorLinha.obtemIntensidade(0);
	
	//já digo que os marcadores nao estão na linha, assim a lógica abaixo simplifica
	marcadorEsquerdo = marcadorDireito = 0;

	//só entra em leitura de setor quando o led apaga, isso significa que ele ignora qualquer leitura muito proxima uma da outra
	if(tempoLed==0 || contadorMarcadorSetor!=0){
		if(digitalRead(PINOMARCADORSETOR)==APERTADO || digitalRead(PINOMARCADORSETOR2)==APERTADO){
				contadorMarcadorSetor++;
		}else{
			contadorMarcadorSetor=0;
		}
	}
	
	marcadorDireito = (digitalRead(PINOMARCADORFIMPISTA) == APERTADO);
	//marcador de fim de pista
	if(marcadorDireito){
		contadorMarcadorFimPista++;
	}else{
		contadorMarcadorFimPista=0;
	}

	//Muda de setor
	if(contadorMarcadorSetor == QUANTIDADELEITURAMARCADOR){
		tempoLed=TEMPOLEDACESSO_IGNORARNOVOSETOR;
		setores[setorAtual].tempo = (millis() - tempoMilis);
		setorAtual++;
		tempoMilis = millis();
		Serial.print(F("Setor: "));
		Serial.println(setorAtual);
	}

	if(contadorMarcadorFimPista == QUANTIDADELEITURAMARCADOR){
		quantidadeMarcadorFimPista++;
		if(quantidadeMarcadorFimPista == QUANTIDADETOTALMARCADORFIMPISTA){
			tempoFinal = millis();
			quantidadeMarcadorFimPista++; //para entrar só uma vez nesse if
		}
		//guardo o tempo da volta
		if(contadorMarcadorFimPista == 0){
			tempoInicial = millis();
			Serial.print("Comecou: ");
			Serial.print(tempoInicial);			
		}
	}
	//acabou a corrida trava o carrinho
	if(quantidadeMarcadorFimPista >= QUANTIDADETOTALMARCADORFIMPISTA){
		potenciaMotorDireito = potenciaMotorEsquerdo = 0;
		//delay pra garantir que o carrinho vai passar do fim
		motorDireito.potencia(50);
		motorEsquerdo.potencia(50);
		delay(100);
		motorDireito.potencia(potenciaMotorDireito);
		motorEsquerdo.potencia(potenciaMotorEsquerdo);
		/*Serial.print(F("Total de pulsos: "));
		Serial.println(giroRodaTotal);*/
		delay(50);
		Serial.print(F("Tempo da volta: "));
		Serial.println(tempoFinal - tempoInicial);
		Serial.print(F("Parcial por setor: "));
		for(int16_t i; i < QUANTIDADE_SETORES; i++){
			Serial.print(F("setor "));
			Serial.print(i);
			Serial.print(F(": "));
			//Serial.println(giroRodaSetor[i]);
			delay(50);
		}
		setorAtual=QUANTIDADE_SETORES-1;
		Utils::enviaRelatorio(setores);
		while(1){
			digitalWrite(PINO_LED,HIGH);
			delay(200);
			digitalWrite(PINO_LED,LOW);
			delay(200);
		}
	}
	
	//acabou SETOR trava o carrinho
	if(setorAtual > QUANTIDADE_SETORES){
		potenciaMotorDireito = potenciaMotorEsquerdo = 0;
		motorDireito.potencia(potenciaMotorDireito);
		motorEsquerdo.potencia(potenciaMotorEsquerdo);
		while(1){
			digitalWrite(PINO_LED,HIGH);
			delay(1000);
			digitalWrite(PINO_LED,LOW);
			delay(1000);
		}
	}
	
	
	//fim da logida de mudar o setor ou fim de pista
	//mantem o led aceso enquanto o tempo for > 0
	if(tempoLed > 0){
		digitalWrite(13,1);
		tempoLed--;
	
	}else{
		digitalWrite(13,0);
	}
	
	//////////////calculo do PID
	//carrega valores da calibração
	Kp = setores[setorAtual].p;
	Kd = setores[setorAtual].d;
	velocidade = setores[setorAtual].velocidade;
	
	erroAnterior = erro;

	erro = Configuracao::calculaErro(sensorDireito,sensorEsquerdo);

	setores[setorAtual].erroAcumulado += abs(erro);
  	if(!Configuracao::naLinha(sensorEsquerdo, sensorDireito)){
		erro = erroAnterior;
	}
    if(!Configuracao::naLinha(sensorEsquerdo, sensorDireito)){
		if(contadorFora > TEMPOMAXIMOFORA){
			potenciaMotorDireito = potenciaMotorEsquerdo = 0;
			motorDireito.potencia(potenciaMotorDireito);
			motorEsquerdo.potencia(potenciaMotorEsquerdo);
			return;
		}
		contadorFora++;
    }else{
		contadorFora=0;
	}
	if(setorAtual==QUANTIDADE_SETORES-1){
		velocidade = 0;
	}
	
	potenciaMotorDireito = velocidade - ((int)((erro*sqrt(abs(erro))*Kp) + Kd*(erro - erroAnterior)));
	potenciaMotorEsquerdo = velocidade + ((int)((erro*sqrt(abs(erro))*Kp) + Kd*(erro - erroAnterior)));
	
	motorDireito.potencia(potenciaMotorDireito);
	motorEsquerdo.potencia(potenciaMotorEsquerdo);
	
    while(micros()-tt < 1000 ); //500hz//1khz//2Khz
}
