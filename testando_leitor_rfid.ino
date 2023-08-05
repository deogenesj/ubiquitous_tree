//Fonte: https://blogmasterwalkershop.com.br/arduino/como-usar-com-arduino-kit-rfid-mfrc522
//https://randomnerdtutorials.com/esp32-spi-communication-arduino/
//https://esp32io.com/tutorials/esp32-rfid-nfc
//https://lobodarobotica.com/blog/wp-content/uploads/2020/09/ESP32-Pinout.jpg
#include <SPI.h> //INCLUSÃO DE BIBLIOTECA
#include <MFRC522.h> //INCLUSÃO DE BIBLIOTECA
 
#define SS_PIN 5 //PINO SDA
#define RST_PIN 4 //PINO DE RESET

#define SCK 18
#define MISO 19
#define MOSI 23
#define SS 5

const int pinoLedVerde = 2; //PINO DIGITAL REFERENTE AO LED VERDE
const int pinoLedVermelho = 15; //PINO DIGITAL REFERENTE AO LED VERMELHO

MFRC522 rfid(SS_PIN, RST_PIN); //PASSAGEM DE PARÂMETROS REFERENTE AOS PINOS

void setup() {
  Serial.begin(9600); //INICIALIZA A SERIAL
  //SPI config
  SPI.begin(SCK, MISO, MOSI, SS);//INICIALIZA O BARRAMENTO SPI
  rfid.PCD_Init(); //INICIALIZA MFRC522
  pinMode(pinoLedVerde, OUTPUT); //DEFINE O PINO COMO SAÍDA
  pinMode(pinoLedVermelho, OUTPUT); //DEFINE O PINO COMO SAÍDA
  
  digitalWrite(pinoLedVerde, LOW); //LED INICIA DESLIGADO
  digitalWrite(pinoLedVermelho, LOW); //LED INICIA DESLIGADO
}
 
void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) //VERIFICA SE O CARTÃO PRESENTE NO LEITOR É DIFERENTE DO ÚLTIMO CARTÃO LIDO. CASO NÃO SEJA, FAZ
    return; //RETORNA PARA LER NOVAMENTE
 
  /***INICIO BLOCO DE CÓDIGO RESPONSÁVEL POR GERAR A TAG RFID LIDA***/
  String strID = "";
  for (byte i = 0; i < 4; i++) {
    strID +=
    (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
    String(rfid.uid.uidByte[i], HEX) +
    (i!=3 ? ":" : "");
  }
  strID.toUpperCase();
  /***FIM DO BLOCO DE CÓDIGO RESPONSÁVEL POR GERAR A TAG RFID LIDA***/
 
  Serial.print("Identificador (UID) da tag: "); //IMPRIME O TEXTO NA SERIAL
  Serial.println(strID); //IMPRIME NA SERIAL O UID DA TAG RFID
 
  //O ENDEREÇO "27:41:AA:AB" DEVERÁ SER ALTERADO PARA O ENDEREÇO DA SUA TAG RFID QUE CAPTUROU ANTERIORMENTE
  if (strID.indexOf("C2:07:A0:1B") >= 0) { //SE O ENDEREÇO DA TAG LIDA FOR IGUAL AO ENDEREÇO INFORMADO, FAZ
    digitalWrite(pinoLedVerde, HIGH); //LIGA O LED VERDE
    delay(3000); //INTERVALO DE 4 SEGUNDOS
    digitalWrite(pinoLedVerde, LOW); //DESLIGA O LED VERDE
  }else{ //SENÃO, FAZ (CASO A TAG LIDA NÃO SEJÁ VÁLIDA)
    digitalWrite(pinoLedVermelho, HIGH); //LIGA O LED VERMELHO
    delay(3000); ////INTERVALO DE 6 SEGUNDOS
    digitalWrite(pinoLedVermelho, LOW); //DESLIGA O LED VERDE
  }

  rfid.PICC_HaltA(); //PARADA DA LEITURA DO CARTÃO
  rfid.PCD_StopCrypto1(); //PARADA DA CRIPTOGRAFIA NO PCD
}