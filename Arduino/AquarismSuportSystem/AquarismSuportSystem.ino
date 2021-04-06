#include <RTClib.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//OneWire onewire(3);
//DallasTemperature sensor(&onewire);
//DeviceAddress addr;

int SENSOR_T = 3;

float ReadCTemperature;
float PastCTemperature;

RTC_DS1307 rtc; //Objeto do tipo rtc_ds1307
char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado"};

int hora = 0;
int minu = 0;
int dia = 6;
int mes = 4;
int ano = 2021;

void setup() {
  Serial.begin(9600);
//  sensor.begin();
//  sensor.getAddress(addr, 0);

/* Inicialização do sensor, tratamento de erro caso sensor não seja inicializado
if (! rtc.begin()) { 
    Serial.println("DS1307 não encontrado"); 
    while(1); 
  }
  if (! rtc.isrunning()) { 
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Captura data e hora que o código foi compilado 

  }
  /*
   
     delay(100);
   */
}
  /****************************
Para usar valores de dia, mes e ano é necessário utilizar 
now.day(),now.month(),now.year(). Já para usar valores de hora minuto e segundo 
se utiliza, now.hour(), now.minute() e now.second(). Já para um dia da semana especifico 
se utiliza daysOfTheWeek[now.dayOfTheWeek()]. Informações relevantes para utilizar nos processos de automação
de horário de ligar e apagar luzes do aquário, tanto como para automatizar o horário de alimentação dos peixes
  ****************************/
void loop() {

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
  
  /*****************************/
  /*     USING THE DS18B20     */
  /*****************************/
//  sensor.requestTemperatures();
//  ReadCTemperature = sensor.getTempC(addr);

  /*******************************/
  /*SIMULATING WITH POTENCIOMETER*/
  /*******************************/
  ReadCTemperature = (map(analogRead(SENSOR_T),0, 1023, 0, 500))/10;
  
  /*****************************/
  /*PRINTING THE DIFFERENT TEMP*/
  /*****************************/
  if(ReadCTemperature != PastCTemperature){
    Serial.print("T:");
    Serial.print(ReadCTemperature);
    Serial.print("|");
    //REFRESHING THE TEMPERATURE
    PastCTemperature = ReadCTemperature;
  }else{
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
  }
  delay(2000);
}
