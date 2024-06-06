// Inclusao das bibliotecas
#include <OneWire.h>
#include <DallasTemperature.h>

const int PINO_ONEWIRE = 12; // Define pino do sensor
const int BOTAO_PIN = 2;     // Define pino do botão
volatile bool modoPython = false;
volatile bool mudarModoFlag = false;

int entrada_A0;
float temperatura, pH;
OneWire oneWire(PINO_ONEWIRE); // Cria um objeto OneWire
DallasTemperature sensor(&oneWire); // Informa a referencia da biblioteca dallas temperature para Biblioteca onewire
DeviceAddress endereco_temp; // Cria um endereco temporario da leitura do sensor

void python(float temperatura, float pH) {
  Serial.print(temperatura);
  Serial.print(",");
  Serial.println(pH);
}

void normal(float temperatura, float pH) {
  Serial.print("pH = "); 
  Serial.println(pH);
  Serial.print("Temperatura = "); // Imprime a temperatura no monitor serial
  Serial.println(temperatura); // Busca temperatura para dispositivo
}

void mudarModo() {
  mudarModoFlag = true;
}

void setup() {
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(BOTAO_PIN, INPUT_PULLUP);
  Serial.begin(9600); // Inicia a porta serial
  sensor.begin(); // Inicia o sensor
  digitalWrite(10, HIGH);
  Serial.println("Aperte o botão para mudar de modo (com HydroSense-graph(laranja)/sem HydroSense-graph(verde))");
  Serial.println("Para saber mais, entre no GitHub: https://github.com/Mats057/HydroSense-graph");
  Serial.println("Medindo temperatura e pH");
  attachInterrupt(digitalPinToInterrupt(BOTAO_PIN), mudarModo, RISING);
}

void loop() {
  if (mudarModoFlag) {
    static unsigned long lastInterruptTime = 0;
    unsigned long interruptTime = millis();
    
    // Debounce
    if (interruptTime - lastInterruptTime > 200) {
      modoPython = !modoPython;
      if (modoPython) {
        digitalWrite(11, HIGH);
        digitalWrite(10, LOW);
      } else {
        digitalWrite(11, LOW);
        digitalWrite(10, HIGH);
      }
    }
    lastInterruptTime = interruptTime;
    mudarModoFlag = false;
  }
  
  entrada_A0 = analogRead(A0);
  pH = (entrada_A0 * 14.01) / 1024.0;
  sensor.requestTemperatures(); // Envia comando para realizar a conversão de temperatura
  
  if (!sensor.getAddress(endereco_temp, 0)) { // Encontra o endereco do sensor no barramento
    Serial.println("SENSOR DE TEMPERATURA NAO CONECTADO"); // Sensor desconectado, imprime mensagem de erro
  } else {
    temperatura = sensor.getTempC(endereco_temp);
    if (modoPython) {
      python(temperatura, pH);
    } else {
      normal(temperatura, pH);
    }
  }
  
  delay(1000);
}
