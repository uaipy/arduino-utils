// Bibliotecas utilizadas //
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Define a pinagem de conexão com o arduíno; 
#define DHTPIN 7
#define hallPinD 13 // Marrom
#define hallPinE 12 // Branco

// Selecione a seguir o sensor que está sendo utilizado, retirando as // no início da linha:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

//Configuração do sensor DHT - pinagem e tipagem //
DHT_Unified dht(DHTPIN, DHTTYPE);

// Funções utilizadas 
void bascula();                   // Função que contabiliza basculadas 
void dados();                     // Função que imprime dados do DHT e da báscula 
void espera();                    // Função delay 

unsigned long tempoB=0;
unsigned long tempoD=0;

bool comutarB = false;
bool comutarD = false;

// Variável de delay //
uint32_t delayMS;

// Definições de variáveis 
int b = 0;                       // Nº de basculadas 
float v = 0.037;                   // Volume da basculada
float chuva = 0;                 // Volume de chuva (b*v)
int side = 0;

void setup() {
  pinMode(hallPinE, INPUT_PULLUP);
  pinMode(hallPinD, INPUT_PULLUP);
  Serial.begin(9600);
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
}

// Execução de atividades simultâneas //
void loop() {
  bascula();
  dados();
  espera();
}

// Atividade 01 - Contabilização de basculadas //
void bascula(){
  if(comutarB){
  
  // CALCULO
  chuva = b*v;
  
  // START
  if(side == 0){
    if(digitalRead(hallPinE) == LOW && digitalRead(hallPinD) == LOW){
      side = 1;
    }
    else if(digitalRead(hallPinE) == HIGH && digitalRead(hallPinD) == HIGH){
      side = 2;
    }
    else if(digitalRead(hallPinE) == LOW && digitalRead(hallPinD) == HIGH){
      side = 3;
    }
    else if(digitalRead(hallPinE) == HIGH && digitalRead(hallPinD) == LOW){
      side = 4;
    }
  }
  
  // WORK = LADO 1
  else if(side == 1){
    if(digitalRead(hallPinE) == HIGH){
      b++;
      side = 4;
    }
    if(digitalRead(hallPinD) == HIGH){
      b++;
      side = 3;
    }
  }
  
  // WORK = LADO 2
  else if(side == 2){
    if(digitalRead(hallPinE) == LOW){
      b++;
      side = 3;
    }
    if(digitalRead(hallPinD) == LOW){
      b++;
      side = 4;
    }
  }

  // WORK = LADO 3
  else if(side == 3){
    if(digitalRead(hallPinE) == HIGH){
      b++;
      side = 2;
    }
    if(digitalRead(hallPinD) == LOW){
      b++;
      side = 1;
    }
  }

  // WORK = LADO 4
  else if(side == 4){
    if(digitalRead(hallPinE) == LOW){
      b++;
      side = 1;
    }
    if(digitalRead(hallPinD) == HIGH){
      b++;
      side = 2;
    }
  }
  
  // Comuta atividade 1
  tempoB=millis();
  comutarB = false;    
  }
}

// Atividade 02 - Lê DHT e imprimir dados de umidade, temperatura e chuva //
void dados(){
  if(comutarD){
    sensors_event_t event;
    Serial.print("{\"deviceId\":1,\"data\":");
    
    // RAINFALL
    Serial.print("{\"chuva\":");
    Serial.print(chuva);
    Serial.print(", ");

    //Serial.print("side: ");
    //Serial.print(side);
    //Serial.print(", ");

    // TEMPERATURE
    dht.temperature().getEvent(&event);
    Serial.print("\"temperatura\":");
    Serial.print(event.temperature);
    Serial.print(", ");

    // HUMIDITY
    dht.humidity().getEvent(&event);
    Serial.print("\"umidade\":"); 
    Serial.print(event.relative_humidity);
    Serial.println("}}");
    
    // Comuta atividade 2 
    tempoD=millis();
    comutarD = false;    
  }
}

// Definições de intervalo (delay) de cada atividade //
void espera(){
  if(!comutarB && millis() - tempoB>=200){
    comutarB = true;
  }
  if(!comutarD && millis() - tempoD>=60000){
    comutarD = true;
  }
}
