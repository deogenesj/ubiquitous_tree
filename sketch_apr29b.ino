//https://learn.adafruit.com/adafruit-neopixel-uberguide/the-magic-of-neopixels
#include <Adafruit_NeoPixel.h>
//#include <Ultrasonic.h>
#include "SoftwareSerial.h"
#include <DFRobotDFPlayerMini.h> //biblioteca para miniplayer mp3
#define PIN 11 //pin da fita de led
#define NUMLED 56 //qtd de leds na fita (coloquei 18 pq eh teste)

//Define os pinos para o trigger e echo
//#define pino_trigger 6
//#define pino_echo 7

//Inicializa o sensor nos pinos definidos acima
//Ultrasonic ultrasonic(pino_trigger, pino_echo);

//para mini player
SoftwareSerial mySoftwareSerial(12, 13);//RX, TX para miniplayer
DFRobotDFPlayerMini myDFPlayer;
int pausa = 0;

int buttonGood = 8; //p                           in do botao no arduino
int buttonBad = 9; //pin do botao no arduino

//variável para acender ou apagar uma posição aleatória da árvore
int randomNumber;

//variaveis para controlar o rebounce do button
int counter = 0, counterBad = 0;
int lastStateGood = LOW; //leitura anterior
int lastStateBad = LOW; //leitura anterior do botao bad
int stateGood, stateBad;
unsigned long lastDebounceTime = 0; //ultimo tempo de toggle do pin good
unsigned long lastDebounceTimeBad = 0; //ultimo tempo de toggle do pin bad
unsigned long debounceDelay = 50; //delay do debounce

int vetor_leds[NUMLED];//vetor que controla a informação de leds ligados e desligados

Adafruit_NeoPixel pixels(NUMLED, PIN, NEO_GRB + NEO_KHZ800); //configuracao da variavel global pixels que é do tipo Adafruit Neopixels (e permite chamar funções)

void setup() {
  pinMode(buttonGood, INPUT);
  pinMode(buttonBad, INPUT);
  pixels.begin();
  uint32_t green = pixels.Color(10, 99, 10);
  pixels.fill(green, 0, 56);
  pixels.show();
  
  //Comunicação serial com o módulo
  mySoftwareSerial.begin(9600);

  Serial.begin(9600);
  //iniciar vetor de LEDs com todos apagados (0's)
  apagar_leds(vetor_leds);

  //Verifica se o módulo está conectado e se cartão SD foi inserido
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini"));
  Serial.println(F("Inicializando módulo DFPlayer..."));
  if(!myDFPlayer.begin(mySoftwareSerial)){
    Serial.println(F("Não inicializado:"));
    Serial.println(F(" - Verifique se módulo foi corretamente conectado"));
    Serial.println(F("- Insira um cartão SD:"));
    while(true);
  }
  //Definições DFMiniPlayer
  myDFPlayer.setTimeOut(500); //Timeout serial 500ms
  myDFPlayer.volume(30); //Volume (de  até 30)
  myDFPlayer.EQ(0); //Equalização normal

  Serial.println();
  Serial.print("Numero de arquivos no cartao: ");
  Serial.print(myDFPlayer.readFileCounts(DFPLAYER_DEVICE_SD));
  Serial.println();
  Serial.println(F("Módulo DFPlayer Mini inicializado corretamente!"));

}

void loop() {
  
  botao(); //funcao para controlar clique do botao 
  //dentro da funcao botao() e chamado o ligar e desligar de LEDs
  
}

void botao(){
  int readGood = digitalRead(buttonGood);
  int readBad = digitalRead(buttonBad);
    
  if(lastStateGood != readGood){
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (readGood != stateGood) {
      stateGood = readGood;
      counter++;
      if(counter > 1){
        acender_bom(vetor_leds);//chama funcao para acender um LED que não esteja aceso (em ordem crescente)
        //Tocar som aqui
        myDFPlayer.play(1);
        counter = 0;
      }
    }
  }

  if(lastStateBad != readBad){
      lastDebounceTimeBad = millis();
  }

  if ((millis() - lastDebounceTimeBad) > debounceDelay) {
    if (readBad != stateBad) {
      stateBad = readBad;
      counterBad++;
      if(counterBad > 1){
        apagar_mal(vetor_leds);//chama funcao para apagar um LED que esteja aceso (em ordem decrescente)

        //Tocar som aqui (outra faixa)
        myDFPlayer.play(2);

        //myDFPlayer.play(2);
        counterBad = 0;
      }
    }
  }

  lastStateBad = readBad;
  lastStateGood = readGood;
}


void apagar_leds(int vet[]){
  for (int i =0; i < NUMLED; i++){
    vet[i] = 0; //0 significa led apagado; 1 significa led aceso
  }
}

void acender_bom(int vet[]){
  //acender um led
  randomNumber = random(0, NUMLED);
  Serial.println(randomNumber);
  int i = 0;
  while(vet[randomNumber] == 1 && i < NUMLED){
    randomNumber = random(0, NUMLED);
    i++;//essa lógica aqui da i não funciona 
  }
  if(i == NUMLED){
    //todos os leds estão ligados, não precisa fazer nada
  }else{
    vet[randomNumber] = 1;
    uint32_t green = pixels.Color(0, 198, 0);
    pixels.setPixelColor(randomNumber, green);//seta a cor do led em verde
    pixels.show();//atualiza a fita com um novo led configurado com a cor green
  }

  /*for(int i = 0; i < NUMLED; i++){//faz o loop no vetor procurando posicoes (leds) com o valor 0 (estado de led apagado) para acender
    if(vet[i] == 0){
      vet[i] = 1;
      uint32_t green = pixels.Color(0, 200, 0);
      pixels.setPixelColor(i, green);//seta a cor do led em verde
      pixels.show();//atualiza a fita com um novo led configurado com a cor green
      break;
    }  
  }*/
}

void apagar_mal(int vet[]){

  randomNumber = random(0, NUMLED);
  int i = 0;
  while(vet[randomNumber] != 0 && i < NUMLED){
    randomNumber = random(0, NUMLED);
    i++;
  }
  if(i == NUMLED){
    //todos os leds estão desligados, não precisa fazer nada
  }else{
    vet[randomNumber] = 0;
    uint32_t black = pixels.Color(0, 0, 0);
    pixels.setPixelColor(randomNumber, black);//seta a cor do led
    pixels.show();//atualiza a fita com um novo led configurado com a cor black
  }
  
  //apagar de tras para frente
  /*for(int i = NUMLED-1; i >= 0; i--){//faz o loop no vetor procurando posicoes (leds) com o valor 1 (estado de led aceso) para apagar
    if(vet[i] == 1){
      vet[i] = 0;
      uint32_t black = pixels.Color(0, 0, 0);
      pixels.setPixelColor(i, black);//seta a cor do led
      pixels.show();//atualiza a fita com um novo led configurado com a cor black
      break;
    }  
  }*/
}


void imprima(int vet[]){
  for (int i = 0; i< NUMLED; i++){
    Serial.print(vet[i]);
  }
  Serial.print("\n");
}
/*
void ula(){
//Le as informacoes do sensor, em cm e pol
  float cmMsec, inMsec;
  long microsec = ultrasonic.timing();
  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
  inMsec = ultrasonic.convert(microsec, Ultrasonic::IN);
  //Exibe informacoes no serial monitor
  Serial.print("Distancia em cm: ");
  Serial.print(cmMsec);
  Serial.print(" - Distancia em polegadas: ");
  Serial.println(inMsec);
  delay(1000);
}
*/

/*
void apagar_mal(int vet[]){
  //apagar de frente para tras
  for(int i = 0; i < NUMLED; i++){
    if(vet[i] == 1){
      vet[i] = 0;
      uint32_t black = pixels.Color(0, 0, 0);
      pixels.fill(black, i, 1);
      pixels.show();
      break;
    }  
  }
}*/
