 
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "RTClib.h"

const int analogOutPin = 9;         // Saida analogica onde o RELÉ esta conectado
const int cleaning = 10;            // Saida analogica onde o LED AMARELO esta conectado
const int temp = 11;                // Saida analogica onde o LED VERMELHO esta conectado
int ALARME = 13;                    // Saida analogica onde o BUZZER esta conectado

String serialCmd = "";
bool flagControlRxSerial = false;

// Porta do pino de sinal do DS18B20
#define ONE_WIRE_BUS 3

//Objeto rtc da classe DS1307
RTC_DS1307 rtc; 
 
// Define uma instancia do oneWire para comunicacao com o sensor
OneWire oneWire(ONE_WIRE_BUS);
 
// Armazena temperaturas minima e maxima
float tempMin = 999;
float tempMax = 0;
float pastTemp;
float readTemp;

int TA = 0;
int FA = 0;
int CA = 0;
int HM = 1;

//Dias da semana
char diasDaSemana[7][12] = {"Domingo", "Segunda", "Terca", "Quarta", "Quinta", "Sexta", "Sabado"}; 
 
DallasTemperature sensors(&oneWire);
DeviceAddress sensor1;
 
void setup(void)
{
  Serial.begin(9600);
  pinMode(ALARME,OUTPUT);
  sensors.begin();
  sensors.getAddress(sensor1, 0); 
  serialCmd.reserve(10);
 
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
   
}
 
void mostra_endereco_sensor(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // Adiciona zeros se necessário
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
 
void loop()
{ 
  // Le a informacao do sensor
  sensors.requestTemperatures();
  
  //float readTemp = sensors.getreadTemp(sensor1);
  readTemp = sensors.getTempC(sensor1);


    if(TA == 1){            //Temp abaixo do min
      analogWrite(temp, 255);
    }
    if(TA == 2){            //Temp acima do max
      analogWrite(temp, 255);
    }
    if(TA == 0){            //Temp ok
      analogWrite(temp,0);
    }    
    if(FA == 1){            //Alarme para alimentacao
      digitalWrite(ALARME,HIGH);
      delay(500);
      digitalWrite(ALARME,LOW);
    }
    if(FA == 0){            //Alimentacao ok
      digitalWrite(ALARME,LOW);
    }
    if(CA == 1){            //Alarme para limpeza
      analogWrite(cleaning, 255);
    }
    if(CA == 0){            //Limpeza ok
      analogWrite(cleaning, 0);
    }
    if(HM == 0){            //Aciona relé
      analogWrite(analogOutPin, 255);
    }
    if(HM == 1){            //Desarma relé
      analogWrite(analogOutPin, 0);
    }
    
    
    /* Resetando valores para nova recepcao */
    serialCmd = "";
    flagControlRxSerial = false;
      
  // Mostra dados no serial monitor
  
    Serial.print("T:");
    Serial.print(readTemp);
    Serial.print("|");
    pastTemp = readTemp;
 
    DateTime agora = rtc.now();                             // Faz a leitura de dados de data e hora
    Serial.print("D:");
    Serial.print(agora.day(), DEC);                         //Imprime dia
    Serial.print('/');                                      //Imprime barra
    Serial.print(agora.month(), DEC);                       //Imprime mes
    Serial.print('/');                                      //Imprime barra
    Serial.print(agora.year(), DEC);                        //Imprime ano
    Serial.print("|S:");                                    //Imprime texto
    Serial.print(diasDaSemana[agora.dayOfTheWeek()]);       //Imprime dia da semana
    Serial.print("|H:");                                    //Imprime texto
    Serial.print(agora.hour(), DEC);                        //Imprime hora
    Serial.print('/');                                      //Imprime dois pontos
    Serial.print(agora.minute(), DEC);                      //Imprime os minutos
    Serial.print('$');                                    

  delay(2000);
}

void serialEvent() {
    /* Loop verificando se algum byte esta disponivel na serial */
    int pos = 0;
    while (Serial.available()) {
      /* O que chegar pela serial  feito um typecast de char para a variavel caractere */
        char caractere = (char)Serial.read(); 
        
        /* Nossa variavel caractere eh concatenada a nossa variavel serialCmd que eh uma String */
        serialCmd += caractere;
    
        /* Se chegar um CR, nova linha, nossa flag de controle (flagsControlRxSerial) passa para true
           e no loop principal ja podera ser utilizada
        */
        if(pos == 1){
          TA = (int)(caractere)-48;
          pos++;
        }else if(pos == 3){
          FA = (int)(caractere)-48;
          pos++;
        }else if(pos == 5){
          CA = (int)(caractere)-48;
          pos++;
        }else if(pos == 7){
          HM = (int)(caractere)-48;
          pos++;
        }
        
      if(caractere == ':'){
        pos++;
      }
        if (caractere == '$') {
          Serial.print(pos);
          Serial.print(TA);
          Serial.print(FA);
          Serial.print(CA);
          pos = 0;
          flagControlRxSerial = true;
        } 
    }
}
