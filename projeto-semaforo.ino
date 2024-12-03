#include <LiquidCrystal.h>

// Alunas: Jheniffer de Oliveira e Yasmin Cella Machado - 2024

// Pinos do semáforo
const int redPin = 10;         // LED vermelho do semáforo de carros
const int yellowPin = 9;       // LED amarelo do semáforo de carros
const int greenPin = 8;        // LED verde do semáforo de carros
const int redPedestre = 7;     // LED vermelho do semáforo de pedestres
const int greenPedestre = 6;   // LED verde do semáforo de pedestres
const int buttonPin = 2;       // Botão para travessia de pedestres
const int buttonNight = 3;

// Pinos do LCD
const int rs = 5, en = 4, d4 = A2, d5 = A3, d6 = A4, d7 = A5; 
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // Inicializa o LCD 16x2 com os pinos definidos

// Variáveis para o tempo
volatile bool clockActivated = false;     // Indica se o botão foi ativado
volatile bool nightModeActivated = false; // Indica se o modo noturno foi ativado
unsigned long previousMillis = 0;         // Marca o último tempo para o semáforo
unsigned long lcdPreviousMillis = 0;      // Marca o último tempo para o avanço da hora no LCD
const long interval = 1000;               // Intervalo de 1 segundo para o semáforo
const long fastHourInterval = 3000;       // Intervalo de segundos para "avançar" uma hora
int state = 0;                            // Estado atual do semáforo (0: vermelho, 1: amarelo, 2: verde)
volatile int hours = 6;                   // Hora inicial (6:00)

// Variável para piscar o LED amarelo no modo noturno
bool yellowState = false;

void setup() {
  // Configuração dos pinos do semáforo
  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(redPedestre, OUTPUT);
  pinMode(greenPedestre, OUTPUT);
  pinMode(buttonNight, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP); // Configura o botão com resistor interno de pull-up

  // Inicialização do LCD
  lcd.begin(16, 2);              // Define o tamanho do LCD (16 colunas, 2 linhas)
  lcd.print("Hora: 06:00");      // Exibe a hora inicial no LCD

  // Estado inicial do semáforo
  digitalWrite(redPin, HIGH);    // LED vermelho do carro ligado
  digitalWrite(greenPedestre, HIGH); // LED verde do pedestre ligado
  digitalWrite(yellowPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(redPedestre, LOW);

  // Configuração das interrupções
  attachInterrupt(digitalPinToInterrupt(buttonPin), pedestrianRequest, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonNight), NightMode, CHANGE);
}

void loop() {
  // Controle do horário no LCD
  unsigned long currentMillis = millis(); // Obtém o tempo atual
  if (currentMillis - lcdPreviousMillis >= fastHourInterval) { // Avança uma hora a cada 5 segundos
    lcdPreviousMillis = currentMillis;
    hours = (hours + 1) % 24; // Incrementa a hora, reiniciando para 0 após 23
    lcd.setCursor(0, 0);      // Move o cursor para o início da primeira linha
    lcd.print("Hora: ");
    if (hours < 10) lcd.print("0"); // Adiciona um zero à esquerda para horas menores que 10
    lcd.print(hours);
    lcd.print(":00");
  }

  // Controle do semáforo no modo noturno (entre 00:00 e 05:59)
  if (hours >= 0 && hours < 5) {
    digitalWrite(buttonNight, HIGH);
  }
  else{
      // Controle normal do semáforo
    if (currentMillis - previousMillis >= interval) { // Altera o estado do semáforo a cada 1 segundo
      previousMillis = currentMillis;
      if (state == 0) {
        // Vermelho para carros, verde para pedestres
        digitalWrite(redPin, HIGH);
        digitalWrite(yellowPin, LOW);
        digitalWrite(greenPin, LOW);
        digitalWrite(greenPedestre, HIGH);
        digitalWrite(redPedestre, LOW);
        state = 1; // Próximo estado: amarelo
      } else if (state == 1) {
        // Amarelo para carros, vermelho para pedestres
        digitalWrite(redPin, LOW);
        digitalWrite(yellowPin, HIGH);
        digitalWrite(greenPin, LOW);
        digitalWrite(greenPedestre, LOW);
        digitalWrite(redPedestre, HIGH);
        state = 2; // Próximo estado: verde
      } else if (state == 2) {
        // Verde para carros, vermelho para pedestres
        digitalWrite(redPin, LOW);
        digitalWrite(yellowPin, LOW);
        digitalWrite(greenPin, HIGH);
        digitalWrite(greenPedestre, LOW);
        digitalWrite(redPedestre, HIGH);
        state = 0; // Próximo estado: vermelho
      }
    }
  }
}

void pedestrianRequest() {
  // Interrupção para o botão dos pedestres
  // Sinaliza para carros pararem e pedestres atravessarem
  digitalWrite(redPin, HIGH);
  digitalWrite(yellowPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(greenPedestre, HIGH);
  digitalWrite(redPedestre, LOW);
  delay(5000); // Mantém o estado por 5 segundos
}
  
void NightMode(){
  while (buttonNight == HIGH){
    digitalWrite(yellowPin, HIGH);
    delay(500);
    digitalWrite(yellowPin, LOW);
    delay(500);
  }
}
