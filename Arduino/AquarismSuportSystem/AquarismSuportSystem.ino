#include <OneWire.h>
#include <DallasTemperature.h>

//OneWire onewire(3);
//DallasTemperature sensor(&onewire);
//DeviceAddress addr;

int SENSOR_T = 3;

float ReadCTemperature;
float PastCTemperature;

void setup() {
  Serial.begin(9600);
//  sensor.begin();
//  sensor.getAddress(addr, 0);
}

void loop() {
  
  /*****************************/
  /*     USING THE DS18B20     */
  /*****************************/
//  sensor.requestTemperatures();
//  ReadCTemperature = sensor.getTempC(addr);

  /*****************************/
  /*SIMULANDO COM POTENCIÔMETRO*/
  /*****************************/
  ReadCTemperature = (map(analogRead(SENSOR_T),0, 1023, 0, 500))/10;
  
  /*****************************/
  /*PRINTING THE DIFFERENT TEMP*/
  /*****************************/
  if(ReadCTemperature != PastCTemperature){
    Serial.print("Temperature: ");
    Serial.println(ReadCTemperature);
    //REFRESHING THE TEMPERATURE
    PastCTemperature = ReadCTemperature;
  }
  delay(100);
}
