//Se instalan las librerias de el sensor
#include <DHT.h>
#include <DHT_U.h>

//definimos el pin y el tipo de sensor
DHT dht (D1, DHT11);
//definimos variables para el almacenado de datos y humedad
float tmp, hmt;


void setup() {
  //Inicializamos el puerto
  Serial.begin(9600);
  //iniciamos el sensor
  dht.begin();
}

void loop() {
  //Definimos tiempo de actiualizacion
  delay(2000);
  //Leemos los datos y lo almacenamos en variables
  tmp = dht.readTemperature();
  hmt = dht.readHumidity();
  //Muestra de los datos obtenidos
  Serial.println("Temperatura: "+ String(tmp) + " Humedad " + String(hmt));
  delay(2000);
  }
