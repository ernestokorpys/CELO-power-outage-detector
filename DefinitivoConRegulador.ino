#include <WiFi.h>
#include <HTTPClient.h>
#include "config.h"  // Incluye el archivo de configuración

//const char* ssid = "modem.vdsl";
//const char* password = "celo2018";
const char* ssid = "TIMBO_VIEJO";
const char* password = "wilson261";

//CREAMOS UNA JSON DONDE SE COLOCA EL NUMERO DE TELEFONO Y EL MENSAJE
String payload = "{\"messaging_product\":\"whatsapp\",\"to\":\"543755206404\",\"type\":\"text\",\"text\": {\"body\": \"Corte de energía detectado\"}}";
String payload2 = "{\"messaging_product\":\"whatsapp\",\"to\":\"543755206404\",\"type\":\"text\",\"text\": {\"body\": \"Energía Electrica reestablecida\"}}";

// Definir los pines DAC y ADC
const int dacPin1 = 25;  // DAC1
const int inputPin = 22; // Pin digital para leer el estado (HIGH o LOW)
const int adcPin = 34;
// Variable para almacenar el valor DAC1
int dacValue1 = 0;

// Variable para almacenar el valor ADC leído
int adcValue = 0;

// Variable para almacenar el estado del pin digital
int inputState = 0;
int estadoActual=LOW;
bool antispam=false;

void setup() {
  // Inicializa la comunicación serie
  Serial.begin(115200);
  // Configura el pin GPIO26 como salida DAC con un valor fijo
  dacWrite(dacPin1, dacValue1);
  // Configura el pin 22 como entrada digital
  pinMode(inputPin, INPUT);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Se ha conectado al wifi con la ip: ");
  Serial.println(WiFi.localIP());
  delay(8000);
}

void loop() {
  // Leer el estado del pin digital (GPIO22)
  int estadoActual = digitalRead(inputPin);

  if (estadoActual == HIGH && antispam==false) {
    Serial.println("Hay Corte de Luz");
    antispam=true;
    delay(5000);
    if(WiFi.status()== WL_CONNECTED){
      //INICIAMOS EL OBJETO HTTP QUE POSTERIORMENTE ENVIARA EL MENSAJE
      HTTPClient http;
      //COLOCAMOS LA URL DEL SERVIDOR A DONDE SE ENVIARA EL MENSAJE
      http.begin(servidor.c_str());
      //COLOCAMOS LA CABECERA DONDE INDICAMOS QUE SERA TIPO JSON
      http.addHeader("Content-Type", "application/json"); 
      //AGREGAMOS EL TOKEN EN LA CABECERA DE LOS DATOS A ENVIAR
      http.addHeader("Authorization", token);    
      //ENVIAMOS LOS DATOS VIA POST
      int httpPostCode = http.POST(payload);
      //SI SE LOGRARON ENVIAR LOS DATOS
      if (httpPostCode > 0) {
        //RECIBIMOS LA RESPUESTA QUE NOS ENTREGA META
        int httpResponseCode = http.GET();
        //SI HAY RESPUESTA LA MOSTRAMOS
        if (httpResponseCode>0) {
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          String payload = http.getString();
          Serial.println(payload);
        }
        else {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
        }
      }
      http.end();
    }
    else {
      Serial.println("WiFi Desconectado");
    }
  }
  if(estadoActual == LOW){
    Serial.println("Hay energía");
    //adcValue = analogRead(adcPin);
    //Serial.println("ADC Value: ");
    //Serial.println(adcValue);
    delay(500);
  }

  if(antispam==true){
    delay(60000);
     int estadoActual = digitalRead(inputPin);
     if (estadoActual == HIGH) {
       Serial.println("Aun no ha vuelto la energía eléctrica.");
       //espera otros N minutos.
    }
    if (estadoActual == LOW) {
      antispam=false;
      if(WiFi.status()== WL_CONNECTED){
      //INICIAMOS EL OBJETO HTTP QUE POSTERIORMENTE ENVIARA EL MENSAJE
      HTTPClient http;
      //COLOCAMOS LA URL DEL SERVIDOR A DONDE SE ENVIARA EL MENSAJE
      http.begin(servidor.c_str());
      //COLOCAMOS LA CABECERA DONDE INDICAMOS QUE SERA TIPO JSON
      http.addHeader("Content-Type", "application/json"); 
      //AGREGAMOS EL TOKEN EN LA CABECERA DE LOS DATOS A ENVIAR
      http.addHeader("Authorization", token);    
      //ENVIAMOS LOS DATOS VIA POST
      int httpPostCode = http.POST(payload2);
      //SI SE LOGRARON ENVIAR LOS DATOS
      if (httpPostCode > 0) {
        //RECIBIMOS LA RESPUESTA QUE NOS ENTREGA META
        int httpResponseCode = http.GET();
        //SI HAY RESPUESTA LA MOSTRAMOS
        if (httpResponseCode>0) {
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          String payload = http.getString();
          Serial.println(payload);
        }
        else {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
        }
      }
      http.end();
    }      
    }//Termina if
  }
}
