#include <avr/wdt.h>

#include "Configuracoes.h"
#include "Serializacao.h"
#include "Utils.h"

//todo: transformar para uma função
struct Calibracaos{
	float p;
	float d;
	float i;
	int16_t velocidade;
	int idSetor;
	
	//parte do relatorio para envio
	uint32_t tempoSetor;
	int32_t erroAcumulado;
};

//vetor com o tamanho de setores, isso vai variar de acordo com a pista

//Savio temos que remover isso aqui
Calibracaos calibracao[QUANTIDADE_SETORES];

/*ATENÇÃO!!!!!!
Por algum motivo usar variáveis locais do tipo char nessas funções está dando zica de travar o arduino
Não consegui descobrir o problema, então coloquei elas como globais.
*/
char pTemp[6];
char dTemp[6];
char iTemp[6];
char textoTemp[100];
uint16_t setorInicial=0;
uint16_t pulsosLimite=0;
uint16_t pulsosMarcacao=0;

void imprimeSetor(uint8_t setor);
void imprimeCalibracao();


/*inicio da calibração pra teste.
essa função nao deve existir na verdade, pois os valores devem ficar na memoria
EEPROM do arduino, esses valores devem conseguir ser alterados sem ter q gravar
o código, então essa função aqui foi só pra teste */
void inicioCalibracao(){
    //uint16_t k = 0;
	for(uint8_t k = 0; k < QUANTIDADE_SETORES; k++){
		calibracao[k].p=0.7;
		calibracao[k].d=20;
		//calibracao[k].ladoPista=1;
		calibracao[k].velocidade=50;
		//calibracao[k].tempoIgnoraSetor=100;
		//calibracao[k].tempoMudaSetor=100;
		//calibracao[k].tempoForaLinha=200;
	}
}

//todo: savio isso foi para serializacao.h
int16_t retornaPosicaoCharString(char* texto, char caractere, uint8_t ocorrencia){
	int16_t posicao=0;
	int16_t temp=0;
	if(ocorrencia == 0) return -1; //-1 não achou
	while(texto[posicao]!= 0){
		if(texto[posicao] == caractere){
			temp++;
		}
		if(temp == ocorrencia) return posicao;
		posicao++;
	}
	if(texto[posicao] == caractere){
		return posicao;
	}
	return -1;
}

//pega elemento de uma string como se fosse um vetor separado por um caractere especifico. Como:
//234;234;2232;2;34;556;4;3;5;4;32;
//posicao começa de 0
//todo: savio isso foi para serializacao.h
int16_t obtemPosicaoVetorString(char *substring, char* texto, char caractere, uint8_t posicao){
	int16_t inicio, fim;
	int16_t i=0;
	int16_t k=0;
	if(posicao == 0){
		inicio = 0;
	}else{
		inicio = retornaPosicaoCharString(texto,caractere,posicao)+1;
	}
	fim = retornaPosicaoCharString(texto,caractere,posicao+1);
	/*Serial.print("Inicio: ");
	Serial.print(inicio);
	Serial.print(" - Fim: ");
	Serial.println(fim);
	*/
	if(fim == -1 && inicio != -1){ //encontrou 1 apenas, então considero o fim como final da string
		fim = retornaPosicaoCharString(texto,0,posicao+1);
	}
	if(inicio < 0 || fim <= 0) return -1; //erro
	k=inicio;
	while(k<fim){
		substring[i] = texto[k];
		i++;
		k++;
	}
	
	substring[i]=0;//finaliza a string
	//Serial.print(F("Substring lida: "));
	//Serial.println(substring);
}

//tem q ter os caracteres, se nao dá merda
//todo: savio isso foi para serializacao.h
int16_t obtemSubstringVetorString(char *substring, char* texto, char caractereInicio, char caractereFim, uint8_t posicao){
	int16_t inicio, fim;
	int16_t i=0;
	int16_t k=0;
	if(posicao == 0){
		inicio = 0;
		if(texto[inicio] == caractereInicio){
			inicio++;
		}
	}else{
		inicio = retornaPosicaoCharString(texto,caractereInicio,posicao)+1;
	}
	fim = retornaPosicaoCharString(texto,caractereFim,posicao);
	if(fim == -1 && inicio != -1){ //encontrou 1 apenas, então considero o fim como final da string
		fim = retornaPosicaoCharString(texto,0,posicao);
	}
	if(inicio < 0 || fim <= 0) return -1; //erro
	k=inicio;
	while(k<fim){
		substring[i] = texto[k];
		i++;
		k++;
	}
	substring[i]=0;//finaliza a string
	//Serial.print(F("Substring lida: "));
	//Serial.println(substring);
}

//todo: savio isso foi para serializacao.h
uint8_t retornaTamanhoVetorString(char* texto, char caractere){
	uint8_t i=0;
	uint8_t tamanho=0;
	while(texto[i]!=0){
		if(texto[i]==caractere){
			tamanho++;
		}
		i++;
	}
	
	return tamanho+1;
}

//devemos editar aqui para colocar mais atributos

//TODO: voltar para void
// savio isso foi pra Configuracao.h
bool insereValorCalibracao (char* chave, char* valor, uint8_t indice){
	uint8_t temp8;
	float tempFloat;
	if(strncmp(chave,"setor",5)==0){
		temp8 = atoi(valor);
		calibracao[indice].idSetor = temp8;
		return;
	}
	if(strncmp(chave,"velocide",8)==0){
		temp8 = atoi(valor);
		calibracao[indice].velocidade = temp8;
		return;
	}
	if(strncmp(chave,"kp",2)==0){
		tempFloat = atof(valor);
		calibracao[indice].p = tempFloat;
		return;
	}
	if(strncmp(chave,"kd",2)==0){
		tempFloat = atof(valor);
		calibracao[indice].d = tempFloat;
		return;
	}
	if(strncmp(chave,"ki",2)==0){
		tempFloat = atof(valor);
		calibracao[indice].i = tempFloat;
		return;
	}
}

//todo: savio isso foi para serializacao.h
void removeCaractere(char * texto, char caractere){
	int i=0;
	int j=0;
	if(caractere == 0){ //nao permito remover fim de string
		return;
	}
	while(texto[i] != 0){
		if(texto[i] == caractere){
			j=i;
			while(texto[j] != 0){
				texto[j] = texto[j+1];
				j++;
			}
		}
		i++;
	}
}

char linhaTemp[100];
char temp[30];
char atributo[20];
char valor[10];
uint8_t quantidadeAtributos;
int inicioColchete, fimColchete;
/*essa função aqui faz leitura dos dados por JSON via string, fiz meu próprio parser*/
//todo: savio calibra o veiculo!! lkkk
bool calibraVeiculo(Configuracao configuracoes[12]){

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
			obtemPosicaoVetorString(temp,linhaTemp,',',k);
			obtemPosicaoVetorString(atributo,temp,':',0);
			obtemPosicaoVetorString(valor,temp,':',1);
			configuracoes[i].insereValorCalibracao(atributo, valor);
		}

		linhaTemp[0]=0;
	}

	return true;
}



void imprimeSetor(uint8_t setor){
	String p,d,i;
	//as funçoes de tratamento de string em arduino nao aceitam float.
	//entao precisamos converter em string antes.
	p = calibracao[setor].p;
	d = calibracao[setor].d;
	//textoTemp[0]=0;
	//i = calibracao[setor].i;
	//sprintf(textoTemp,"Setor: %d, p: %s, d: %s, ladoPista: %d, velocidade: %d distanciaSetor: %d\n",setor,p.c_str(),d.c_str(),calibracao[setor].ladoPista,calibracao[setor].velocidade,calibracao[setor].giroRodaSetor);
	sprintf(textoTemp,"Setor: %d, idSetor: %d, p: %s, d: %s, velocidade: %d\n",setor,calibracao[setor].idSetor,p.c_str(),d.c_str(),calibracao[setor].velocidade);
	Serial.print(textoTemp);
}

void imprimeCalibracao(){
	Serial.println(F("Variaveis globais:"));
	Serial.print(F("SetorInicial: "));
	Serial.println(setorInicial);
	Serial.print(F("PulsosLimite: "));
	Serial.println(pulsosLimite);
	Serial.print(F("PulsosMarcacao: "));
	Serial.println(pulsosMarcacao);
	Serial.println(F("Setores:"));
	for(uint16_t setor=0; setor < QUANTIDADE_SETORES; setor++){
		imprimeSetor(setor);
		delay(20);
		
	}
	Serial.println();
}

void limpaRelatorio(){
	for(uint8_t i=0; i < QUANTIDADE_SETORES; i++){
		calibracao[i].tempoSetor = 0;
		calibracao[i].erroAcumulado = 0;
	}
	//gravaCalibracao();
}

/*
  todo:
  
  Envia os dados para o relatório
  esses valores podem mudar dependendo 
  dos atributos da "struct Calibração"  
*/
void enviaRelatorio(){
	uint8_t primeiraVez=1;
	softwareSerial.print(F("["));
	for(uint8_t i=0; i < QUANTIDADE_SETORES; i++){
		if(!primeiraVez){
			softwareSerial.print(F(","));
		}else{
			primeiraVez=0;
		}
		softwareSerial.print(F("{\"idSetor\":"));
		softwareSerial.print(calibracao[i].idSetor);
		softwareSerial.print(F(",\"erroAcumulado\":"));
		softwareSerial.print(calibracao[i].erroAcumulado);
		softwareSerial.print(F(",\"tempoSetor\":"));
		softwareSerial.print(calibracao[i].tempoSetor);
		softwareSerial.print(F("}"));
	}
	softwareSerial.print(F("]"));
}
