//Fonte: https://mundoprojetado.com.br/web-server-esp8266/
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define SS_PIN 5 //PINO SDA
#define RST_PIN 4 //PINO DE RESET

#define SCK 18
#define MISO 19
#define MOSI 23
#define SS 5

const int pinoLedVerde = 2; //PINO DIGITAL REFERENTE AO LED VERDE
const int pinoLedVermelho = 15; //PINO DIGITAL REFERENTE AO LED VERMELHO

MFRC522 rfid(SS_PIN, RST_PIN); //PASSAGEM DE PARÂMETROS REFERENTE AOS PINOS

const char* ssid = "ssi"; //put you SSID here
const char* password = "password"; //put your password here

WiFiServer server(80);

String HOST_NAME = "http://"; // change to your PC's IP address
String PATH_NAME   = "/ubiquitous_tree/insert_info.php";
String queryString = "?info=";

void setup() {
  Serial.begin(9600);

  //SPI config
  SPI.begin(SCK, MISO, MOSI, SS);//INICIALIZA O BARRAMENTO SPI
  rfid.PCD_Init(); //INICIALIZA MFRC522
  pinMode(pinoLedVerde, OUTPUT); //DEFINE O PINO COMO SAÍDA
  pinMode(pinoLedVermelho, OUTPUT); //DEFINE O PINO COMO SAÍDA
  
  digitalWrite(pinoLedVerde, LOW); //LED INICIA DESLIGADO
  digitalWrite(pinoLedVermelho, LOW); //LED INICIA DESLIGADO

  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
 // WiFi.config(local_IP, gateway, subnet, dns);
  Serial.println("Passei aqui");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectada.");
  Serial.println("Endereço de IP: ");
  Serial.println(WiFi.localIP());

  server.begin();
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
  
  HTTPClient http;

  queryString = "?info=";
  queryString = queryString + strID;

  http.begin(HOST_NAME + PATH_NAME + queryString); //HTTP
  Serial.print(HOST_NAME);
  Serial.print(PATH_NAME);
  Serial.println(queryString);

  int httpCode = http.GET();
  Serial.println(httpCode);

  // httpCode will be negative on error
  if(httpCode > 0) {
    // file found at server
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    } else {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  
  
  
  /*
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        //delay (100);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.print ("<html>");
            client.print ("<head><meta content=\"width=device-width, initial-scale=1\">");
            client.print ("<style>html ( margin: 0px auto; text-align: center;)");
            client.print (".portao ( background-color: #00FF00; color white; padding: 15px 40px; border-radius: 25px)");
            client.print (".porta ( background-color: #FF0000; color white; padding: 15px 40px; border-radius: 25px)");
            client.print (".alarme ( background-color: #FF0000; color white; padding: 15px 40px; border-radius: 25px)</style></head>");
            client.print ("<body><center><h1>Ja-vi</h1>");

            client.print("<p><a href=\"/externo_portao\"><button class=\"portao\">PORTAO</button></a></p>");
            client.print("<p><a href=\"/porta\"><button class=\"porta\">PORTA</button></a></p>");
            client.print("<p><a href=\"/alarme\"><button class=\"alarme\">ALARME</button></a></p></center>");

            client.print("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
        /*if (currentLine.endsWith("GET /externo_portao")) {
          digitalWrite(R2, LOW);
          delay(250);
          digitalWrite(R2, HIGH);
        }
        if (currentLine.endsWith("GET /porta")) {
          digitalWrite(R3, LOW);
          delay(250);
          digitalWrite(R3, HIGH);
        }
        if (currentLine.endsWith("GET /alarme")) {
          digitalWrite(R1, LOW);
          delay(200);
          digitalWrite(R1, HIGH);
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
  */
}