#include <OneWire.h>
#include <DallasTemperature.h>

//OneWire onewire(3);
//DallasTemperature sensor(&onewire);
//DeviceAddress addr;

int SENSOR_T = 3;

float temperaturaCLida;
float temperaturaCAnterior;

void setup() {
  Serial.begin(9600);
//  sensor.begin();
//  sensor.getAddress(addr, 0);
}

void loop() {
//  sensor.requestTemperatures();
//  temperaturaCLida = sensor.getTempC(addr);
  temperaturaCLida = (map(analogRead(SENSOR_T),0, 1023, 0, 500))/10;
  
  if(temperaturaCLida!=temperaturaCAnterior){
    Serial.print("Temperatura: ");
    Serial.println(temperaturaCLida);
  }
  temperaturaCAnterior = temperaturaCLida;
  
  delay(100);
}
