#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <SPI.h> 
#include <UIPEthernet.h>

//  Constantes e variaveis para a interface de rede

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 54, 49 }; 
EthernetServer server(80); 


//  Constantes e variaveis para o Sensor de Temperatura / Umidade

#define DHTPIN A0
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

float temperatura = 0;
float umidade = 0;
const float dhtCorrecao = -4.0;

// Constantes e variaveis para o Sensor de Pressao

Adafruit_BMP280 sensorBmp;

float pressaoValor = 0;
float altitudeValor = 0;
float temperaturaValor = 0;
const float bpmCorrecao = -2.8;

//  Constantes e variaveis para o Sensor de UV

int uvPorta = 1;
int uvNivel = 0;

// Constantes e variaveis para o Sensor de CO2

int co2DigitalPorta = 6;
int co2AnalogicoPorta = 2;
int co2DigitalValor = 0;
int co2AnalogicoValor = 0;
int co2Nivel = 0;

// Constantes e variaveis para o Sensor de Chuva

int pdigitalChuva = 7;
int panalogicoChuva = 3;
int valDigitalChuva = 0;
int valAnalogicoChuva = 0;
int chuvaNivel = 0;
const int correcaoChuvaNivel = -3;

//  Constantes e variaveis para o Sensor de LDR

int ldrPorta = 4;
int ldrNivel = 0;


String json;

// -------------------------------------- Funcoes --------------------------------------------

// Le Sensor UV

int leSensorUV (int porta) {

  int uvTensao = 0;
  int uvValor = 0;

  uvValor = analogRead(uvPorta);
  uvTensao = (uvValor * (5.0 / 1023.0)) * 1000;

  if (uvTensao >= 0 && uvTensao < 50)   
      
      return (0);

  else

      if (uvTensao > 50 && uvTensao <= 227)
      
          return (0);

      else

          if (uvTensao > 227 && uvTensao <= 318) 
          
              return (1);

          else
          
              if (uvTensao > 318 && uvTensao <= 408)
              
                  return (2);

              else

                  if (uvTensao > 408 && uvTensao <= 503) 
                  
                      return (3);

                  else

                      if (uvTensao > 503 && uvTensao <= 606) 
                      
                          return (4);
                          
                      else  

                          if (uvTensao > 606 && uvTensao <= 696) 
                          
                              return (5);

                          else

                              if (uvTensao > 696 && uvTensao <= 795) 
                              
                                  return (6);

                              else

                                  if (uvTensao > 795 && uvTensao <= 881) 
                                  
                                      return (7);

                                  else
                                  
                                      if (uvTensao > 881 && uvTensao <= 976) 
                                      
                                          return (8);
                                          
                                       else
                                       
                                          if (uvTensao > 976 && uvTensao <= 1079) 
                                          
                                              return (9);

                                          else

                                              if (uvTensao > 1079 && uvTensao <= 1170) 
                                              
                                                  return (10);

                                              else

                                                  return (11);
  
}

// Le Sensor CO2

int leSensorCO2 (int porta) {

  int co2Porncentagem = 0;
  int co2Valor = 0;

  co2Valor = analogRead(porta);
  co2Porncentagem = map(co2Valor, 0, 1023, 0, 100);

  return (co2Porncentagem);

}

// Le Sensor Chuva

int leSensorChuva (int porta) {

  int chuvaPorcentagem = 0;
  int chuvaValor = 0;

  chuvaValor = analogRead(porta);
  chuvaPorcentagem = map(chuvaValor, 0, 1024, 100, 0) + correcaoChuvaNivel;

  if (chuvaPorcentagem < 0) {
    
    chuvaPorcentagem = 0;
    
  }

  return (chuvaPorcentagem);

}

void setup() {
  Serial.begin(9600);

// Inicializa interface de rede

  Ethernet.begin(mac, ip);
  
  server.begin(); 

// Inicializa Sensor de Temperatura / Umidade

  dht.begin();
 

// Inicializa Sensor de Pressao

  if (!sensorBmp.begin(0x76)) {
    Serial.println("ERRO");
  }

// Inicializa Sensor de UV

  pinMode (uvPorta, INPUT);

// Inicializa Sensor de CO2

  pinMode (co2DigitalPorta, INPUT);
  pinMode (co2AnalogicoPorta, INPUT);  

// Inicializa Sensor de Chuva;

  pinMode (pdigitalChuva, INPUT);
  pinMode (panalogicoChuva, INPUT);  

// Inicializa Sensor LDR

  pinMode (ldrPorta, INPUT);
  

}

void loop() {

  EthernetClient client = server.available();

  if (client) {

    while (client.connected()) {

      if (client.available()) {

        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println();
        
//      Le Sensor Temperatura / Umidade

        temperatura = dht.readTemperature() + dhtCorrecao;
        umidade = dht.readHumidity();

//      Le Sensorm Pressao

        pressaoValor = sensorBmp.readPressure() / 100;
        altitudeValor = sensorBmp.readAltitude ();
        temperaturaValor = sensorBmp.readTemperature () + bpmCorrecao;

//      Le Sensor UV

        uvNivel = leSensorUV (uvPorta); 

//      Le Sensor Chuva

        chuvaNivel = leSensorChuva (panalogicoChuva);
        valDigitalChuva = not digitalRead(pdigitalChuva);
        valAnalogicoChuva = analogRead(panalogicoChuva);        

//      Le Sensor CO2

        co2Nivel = leSensorCO2 (co2AnalogicoPorta); 

//      Le Sensor LDR

        ldrNivel = analogRead(ldrPorta);
         
        client.println("{");
        client.print("  \"temperaturaExterna\": ");
        client.print(temperatura);
        client.println(",");
        client.print("  \"umidade\": ");
        client.print(umidade);
        client.println(",");
        client.print("  \"pressao\": ");
        client.print(pressaoValor);
        client.println(",");
        client.print("  \"altitude\": ");
        client.print(altitudeValor);
        client.println(",");
        client.print("  \"temperaturaInterna\": ");
        client.print(temperaturaValor);
        client.println(",");
        client.print("  \"uv\": ");
        client.print(uvNivel);
        client.println(",");
        client.print("  \"co2\": ");
        client.print(co2Nivel);
        client.println(",");
        client.print("  \"chuva\": ");
        client.print(valDigitalChuva);
        client.println(",");
        client.print("  \"chuvaNivel\": ");
        client.print(chuvaNivel);
        client.println(",");
        client.print("  \"luminosidade\": ");
        client.println(ldrNivel);
        client.println("}");

        client.stop();
        
      }
      
    }

    
  }
  
//  Serial.print("Temperatura: ");
//  Serial.println(temperatura);
//  Serial.print("Uumidade: ");
//  Serial.println(umidade);
//  
//
//  Serial.print("Pressao: ");
//  Serial.println(pressaoValor);
//  Serial.print("Altitude: ");
//  Serial.println(altitudeValor);
//  Serial.print("TemperaturaInterna: ");
//  Serial.println(temperaturaValor);
//
//  Serial.print("UV: ");
//  Serial.println(uvNivel);
//
//  Serial.print("CO2: ");
//  Serial.println(co2Nivel);  
//
//  Serial.print("Chuva: ");
//  Serial.println(chuvaNivel);
//  Serial.print("Chuva: ");
//  Serial.println(not valDigitalChuva);
//
//  Serial.print("Luminosidade: ");
//  Serial.println(ldrNivel);
//
//  Serial.println();
  
  delay(50);
  
} 
