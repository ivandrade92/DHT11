#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ESP8266HTTPClient.h>

// ----- Configuración -----
#define DHTPIN 5 // GPIO5 = D1 en NodeMCU
#define DHTTYPE DHT11
#define LED_PIN LED_BUILTIN

const char *ssid = "Ivan-office";
const char *password = "Wifi2026$";
const char *servidor = "http://192.168.1.100:5000/api/iot/sensor";

// ----- Objetos -----
DHT dht(DHTPIN, DHTTYPE);

// ----- Variables -----
float temperatura;
float humedad;

unsigned long ultimaLectura = 0;
unsigned long ultimoIntentoWifi = 0;

const unsigned long INTERVALO_LECTURA = 2000;
const unsigned long INTERVALO_WIFI = 10000;

// ----- Funciones -----

void conectarWiFi()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Intentando conectar WiFi...");
    WiFi.begin(ssid, password);
  }
}

void parpadearLED()
{
  digitalWrite(LED_PIN, LOW); // Encender LED
  delay(100);
  digitalWrite(LED_PIN, HIGH); // Apagar LED
}
void enviarDatos(float temperatura, float humedad)
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi desconectado. No se envían datos.");
    return;
  }

  WiFiClient client;
  HTTPClient http;

  http.begin(client, servidor);
  http.addHeader("Content-Type", "application/json");

  String json = "{";
  json += "\"temperatura\":" + String(temperatura, 1) + ",";
  json += "\"humedad\":" + String(humedad, 1);
  json += "}";

  Serial.print("Enviando: ");
  Serial.println(json);

  int codigo = http.POST(json);

  Serial.print("HTTP Code: ");
  Serial.println(codigo);

  if (codigo == 200)
  {
    parpadearLED();

    Serial.println("POST OK");
  }
  else
  {
    Serial.print("Error HTTP: ");
    Serial.println(codigo);
  }
  http.end();
}

void setup()
{
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  dht.begin();

  WiFi.mode(WIFI_STA);
  conectarWiFi();

  Serial.println("Sistema iniciado");
}
void loop()
{
  // Gestión WiFi sin bloqueos
  if (WiFi.status() != WL_CONNECTED)
  {
    if (millis() - ultimoIntentoWifi >= INTERVALO_WIFI)
    {
      ultimoIntentoWifi = millis();
      conectarWiFi();
    }
  }
  else
  {
    static bool wifiReportado = false;

    if (!wifiReportado)
    {
      Serial.println("WiFi conectado");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());

      wifiReportado = true;
    }
  }

  // Lectura del sensor
  if (millis() - ultimaLectura >= INTERVALO_LECTURA)
  {
    ultimaLectura = millis();

    humedad = dht.readHumidity();
    temperatura = dht.readTemperature();

    if (isnan(humedad) || isnan(temperatura))
    {
      Serial.println("Error al leer DHT11");
      return;
    }

    parpadearLED();

    Serial.print("Temperatura: ");
    Serial.print(temperatura, 1);
    Serial.print(" °C | Humedad: ");
    Serial.print(humedad, 1);
    Serial.println(" %");
  }
}
