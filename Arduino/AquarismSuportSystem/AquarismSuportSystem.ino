#include <RTClib.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//OneWire onewire(3);
//DallasTemperature sensor(&onewire);
//DeviceAddress addr;

int SENSOR_T = 3;
const int buzz = 10; // Pino do Buzzer, pode ser alterado
int lembrete_alimentacao=1; // 1 para lembrete ativado, 0 para lembrete desativado
float ReadCTemperature;
float PastCTemperature;

//RTC_DS1307 rtc; //Objeto do tipo rtc_ds1307
char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado"};

int hora = 0;
int minu = 0;
int dia = 6;
int mes = 4;
int ano = 2021;

int ALARME = 13;

String serialCmd = "";
bool flagControlRxSerial = false;


void setup() {
  pinMode(ALARME,OUTPUT);
  Serial.begin(19200);
  serialCmd.reserve(10);
}

void loop() {
  
  if (flagControlRxSerial) {

    /*********************/
    /*simulating the date*/
    /*********************/
    minu = minu+1;
    if(minu == 60){
      minu = 0;
      hora=hora+1;
      if(hora == 24){
        hora = 0;
        dia=dia+1;
        if(dia == 31){
          dia = 1;
          mes=mes+1;
          if(mes == 13){
            mes = 1;
            ano=ano+1;
          }
        }
      }
    }
  
    ReadCTemperature = (map(analogRead(SENSOR_T),0, 1023, 0, 500))/10;
    
    
    if( serialCmd == "TA:0|FA:0|CA:0$" ) {
      digitalWrite(ALARME,LOW);
    }
    else {
      digitalWrite(ALARME,HIGH);
    }
       
    /* Resetando valores para nova recepcao */
    serialCmd = "";
    flagControlRxSerial = false;
    
  /*****************************/
  /*PRINTING THE DIFFERENT TEMP*/
  /*****************************/
    Serial.print("T:");
    Serial.print(ReadCTemperature);
    Serial.print("|");
    //REFRESHING THE TEMPERATURE
    PastCTemperature = ReadCTemperature;
    Serial.print("D:");
    Serial.print(dia);
    Serial.print("/");
    Serial.print(mes);
    Serial.print("/");
    Serial.print(ano);
    Serial.print("|S:");
    Serial.print("Seg");
    Serial.print("|H:");
    Serial.print(hora);
    Serial.print("/");
    Serial.print(minu);
    Serial.print("$");
  }
  
  delay(2000);
}

void serialEvent() {
    /* Loop verificando se algum byte esta disponivel na serial */
    while (Serial.available()) {
        
      /* O que chegar pela serial  feito um typecast de char para a variavel caractere */
        char caractere = (char)Serial.read(); 
        
        /* Nossa variavel caractere eh concatenada a nossa variavel serialCmd que eh uma String */
        serialCmd += caractere;
    
        /* Se chegar um CR, nova linha, nossa flag de controle (flagsControlRxSerial) passa para true
           e no loop principal ja podera ser utilizada
        */
        if (caractere == '$') {
            flagControlRxSerial = true;
        } 
    }
}
