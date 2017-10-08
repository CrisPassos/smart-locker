#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Keypad.h>
#include <Servo.h> 

//******WI-FI********//
//Wireless
const char* ssid = "**YOUR SSID**";
const char* password = "**YOUR PASSWORD**";

//Request GET
const char* http_site = "**IP OR NAME SITE**";
const int http_port = **PORT**;

//Global variables of WI-FI
WiFiClient client;
IPAddress server(000,000,000,000); //SERVICE IP ADDRESS 
//******WI-FI********//

//******KEYPAD******//
//definition matrix of keypad
const byte numRows= 4;
const byte numCols= 3;

char keymap[numRows][numCols]= { {'1', '2', '3'}, 
                                 {'4', '5', '6'}, 
                                 {'7', '8', '9'},
                                 {'*', '0', '#'} };
                                 
//digital ports, in this case we used NODEMCU 
byte rowPins[numRows] = {16,5,4,0};
byte colPins[numCols]= {2,14,12};

char keypressed;
byte data_count = 0;
char Data[5];

Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);
//******KEYPAD******//

//*****SERVO*******//
int servoPin = 13;
Servo ServoMotor;
//*****SERVO*******//

void setup() {
  Serial.begin(115200);
  Serial.println("Aguardando conexão");
  
  //Try connected to Wi-fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nWI-FI conectado com sucesso: ");
  Serial.println(ssid);

}
 
void loop() { 
  //Call method keypad
  keypadClick(); 
}

void keypadClick(){
  keypressed = myKeypad.getKey();
  
  if (keypressed)
  {
    if (keypressed == '*'){
      Serial.println(Data);
      getPage(Data);
            
      while(data_count != 0){
        Data[data_count--] = 0;
      } 
      
    } else {
      Data[data_count] = keypressed;
      data_count++;
      Serial.println(keypressed);
    }

    if (keypressed == '#'){
      closeDoor();
      Data[5];
    }
  }
}


// Web site request
void getPage(char senha[5]) {
  
  if ( !client.connect(server, http_port) ) {
    Serial.println("Falha na conexao com o site ");
    return;
  }
  Serial.print("Tentando chamar");

  String url = "/openbox?pin_code=" + String(senha);
  Serial.println("requesting URL: ");
  Serial.println(url);
  client.print(String("GET ") + url +" HTTP/1.1\r\n" +
               "Host: " + http_site + "\r\n" +
               "Content-Type: application/json; charset=utf-8" +
               "Connection: close\r\n\r\n");

  Serial.println("request enviada");
  
  while (client.connected()){ 
    if(client.available()) {
      String c = client.readString();
      Serial.println(c);
      Serial.println(c.indexOf(":1,"));
      if(c.indexOf(":0,") > 0){
        Serial.print("Chamando o Motor de Passo");
        openDoor();
        data_count = 0;
        for( int i = 0; i < sizeof(Data);  ++i )
          Data[i] = (char)0;
      } else {
        data_count = 0;
        for( int i = 0; i < sizeof(Data);  ++i )
          Data[i] = (char)0;         
      }
    } else {
      keypadClick();
    }
  }
}

void openDoor(){  
  ServoMotor.attach(servoPin);
  ServoMotor.write(90);
}

void closeDoor(){  
  data_count = 0;
  for( int i = 0; i < sizeof(Data);  ++i )
   Data[i] = (char)0;
  ServoMotor.attach(servoPin);
  ServoMotor.write(0);
}
