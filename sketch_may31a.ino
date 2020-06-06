/*------------------------------------------------------------------------------
  07/01/2018
  Author: Makerbro
  Platforms: ESP8266
  Language: C++/Arduino
  File: webserver_websockets.ino
  ------------------------------------------------------------------------------
  Description: 
  Code for YouTube video demonstrating how to transfer data between a web server
  and a web client in real-time using websockets.
  https://youtu.be/ROeT-gyYZfw
  Do you like my videos? You can support the channel:
  https://patreon.com/acrobotic
  https://paypal.me/acrobotic
  ------------------------------------------------------------------------------
  Please consider buying products from ACROBOTIC to help fund future
  Open-Source projects like this! We'll always put our best effort in every
  project, and release all our design files and code for you to use. 
  https://acrobotic.com/
  https://amazon.com/acrobotic
  ------------------------------------------------------------------------------
  License:
  Please see attached LICENSE.txt file for details.
------------------------------------------------------------------------------*/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Servo.h>

Servo myservo;

ESP8266WebServer server;
WebSocketsServer webSocket = WebSocketsServer(81);

char* ssid = "VIVO-F0A8";
char* password = "nFXEF79U3J";

String cima;
String baixo;
String esquerda;
String direita;
int pos = 90;

char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css'>
    <script>
        //var myVar = setInterval(sendText(), 500);
        var Socket;
        function init() {
          Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
          Socket.onmessage = function(event){
            document.getElementById("rxConsole").value += event.data;
          }
        }
        
        var tecla = [];
        tecla["ArrowUp"]="0";
        tecla["ArrowDown"]="0";
        tecla["ArrowLeft"]="0";
        tecla["ArrowRight"]="0";

        //Tecla pressionada
        document.addEventListener('keydown', keyDown);
        function keyDown(e) {
            rxConsole.textContent = ` ${e.code}`;
            tecla[e.code] = "1";
            sendText();
        }
        //Tecla solta
        document.addEventListener('keyup', keyUp);
        function keyUp(e) {
            //rxConsole.textContent = ` ${e.code}`;
            tecla[e.code] = "0";
            sendText();
        }
        
        function sendText(){
          Socket.send(tecla["ArrowUp"]+tecla["ArrowDown"]+tecla["ArrowLeft"]+tecla["ArrowRight"]);
        }    
      </script>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WebSocket</title>
    
</head>
<body onload="javascript:init()">
    <div class="container pt-5" style="height: 100vh;">
        <div class="m-auto h-50 w-50">
            <div>
                <textarea class="form-control" rows="4" id="rxConsole"></textarea>
            </div>
            <hr/>
            <div>
                <input class="form-control" type="text" id="txBar" />
            </div>
            <hr/>
 
            <div class="form-group justofy-content-center">
                <label class="text-center" for="formControlRange">Controle de alcance</label>
                <input type="range" min="0" max="1023" value="512" id="brightness" class="form-control-range">
            </div>
        </div>
    </div>
    <script src='https://code.jquery.com/jquery-3.3.1.slim.min.js'></script>
    <script src='https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js'></script>
    <script src='https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js'></script>
</body>
</html>
)=====";

void setup()
{
  WiFi.begin(ssid,password);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  myservo.attach(0);
  Serial.begin(115200);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/",[](){
    server.send_P(200, "text/html", webpage);  
  });
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop()
{
  webSocket.loop();
  server.handleClient();
  if(Serial.available() > 0){
    char c[] = {(char)Serial.read()};
    webSocket.broadcastTXT(c, sizeof(c));
  }
  
  if(cima=="1"){
    digitalWrite(4, HIGH);
  }else{
    digitalWrite(4, LOW);
  }
  if(baixo=="1"){
    digitalWrite(5, HIGH);
  }
  else{
    digitalWrite(5, LOW);
  }
  if(esquerda == "1"){
    myservo.write(15); 
  }else if(direita == "1"){
    myservo.write(180); 
  }else{
    myservo.write(90);
  }
   
}
 
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
  if(type == WStype_TEXT){
    
    cima = (char) payload[0];
    baixo = (char) payload[1];
    esquerda = (char) payload[2];
    direita = (char) payload[3];
    

    Serial.print("Em Cima: "+cima+" Baixo: "+baixo+" Esquerda: "+esquerda+" Direita: "+direita);
    Serial.println();

  }

  
}
