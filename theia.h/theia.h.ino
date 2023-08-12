
// Code based on various Arduino Examples that are in public domain
// https://github.com/arduino-libraries/WiFi101/tree/master/examples

#include <WiFiNINA.h>
#include <SPI.h>

#include "secrets.h"

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the Wifi radio's status
int green = 2;
int red = 1;
int blue = 0;
byte mac[6];  

int rs[] = {250, 0, 0};
int gs[] = {200, 0, 0};
int bs[] = {0, 0, 0};
int currentColor = 0;
float cR = 0;
float cG = 0;
float cB = 0;
float rate = 0.1;
int colors = 1;
float brightness = 1;
int count = 0;
int DELAY = 20;
float STAY = 10;
float stay = 100;
int DEBUG = false;
int curStay = 0;

IPAddress ip(192,168,1,229);
WiFiServer server(80);


void printData() {
  Serial.println("Board Information:");
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println();
  Serial.print("Mac Address: ");
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);

  Serial.println();
  Serial.println("Network Information:");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
}


// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  pinMode(red, OUTPUT); 
  pinMode(green, OUTPUT); 
  pinMode(blue, OUTPUT); 
//  while (!Serial);

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to network: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    WiFi.config(ip);

    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(100);
  }
  

  // you're connected now, so print out the data:
  Serial.println("You're connected to the network");

  Serial.println("----------------------------------------");
  printData();
  Serial.println("----------------------------------------");
  server.begin();  
  printWiFiStatus();  
  Serial.println("Server started.");
}




// the loop function runs over and over again forever
void loop() {

  WiFiClient client = server.available();   // listen for incoming clients
  
  if (client) {                             // if you get a client,

    Serial.println("new client");           // print a message out the serial port

    String currentLine = "";                // make a String to hold incoming data from the client

    while (client.connected()) {            // loop while the client's connected

      if (client.available()) {             // if there's bytes to read from the client,

        char c = client.read();             // read a byte, then

        Serial.write(c);                    // print it out the serial monitor

        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.

          // that's the end of the client HTTP request, so send a response:

          if (currentLine.length() == 0) {

            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)

            // and a content-type so the client knows what's coming, then a blank line:

            client.println("HTTP/1.1 200 OK");

            client.println("Content-type:text/html");

            client.println();
            client.println();

            // break out of the while loop:

            break;

          }

          else {      // if you got a newline, then clear currentLine:

            currentLine = "";

          }

        }

        else if (c != '\r') {    // if you got anything else but a carriage return character,

          currentLine += c;      // add it to the end of the currentLine

        }


        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.indexOf("GET")>=0) {
          
          String command = currentLine.substring(currentLine.indexOf("GET")+5);    

          if (command.indexOf("/")>0){
            if (command.indexOf(" ")<0){
              Serial.println("Command received is: ");
              command = command.substring(0,command.length()-1);
              Serial.println(command);

              switch (command[0]) {
                  case 'c':
                    colors = (int) command[1] - '0';
                    currentColor = 0;
  //                c2-0-23-2-34-51-0
                    {
                    int colorIdx = 0;
                    int currentRGB = 1;
                    while(command.indexOf("-") > 0){
                      command = command.substring(command.indexOf("-")+1);
                      int col = 0;
                      if (command.indexOf("-") > 0){
                        col = command.substring(0, command.indexOf("-")).toInt();
                      }
                      else{
                        col = command.toInt();
                      }
                      switch (currentRGB){
                        case 1: 
                          rs[colorIdx] = col; 
                          currentRGB = 2;
                          break;
                        case 2:
                          gs[colorIdx] = col;
                          currentRGB = 3;
                          break;
                        case 3:
                          bs[colorIdx] = col;
                          currentRGB = 1;
                          colorIdx++;
                          break;
                        default:
                          break;}
                      }
                    } 
  
                  break;
                case 'f':
                  rate = rate * 1.7;
                  break;
                case 's':
                  rate = rate / 1.7;
                  break;
                case 'b':
                  brightness =  ((double) command.substring(1).toInt() / 100.0);
                  break;
                case 'o':
                  colors = 1;
                  bs[0] = 0;
                  gs[0] = 0;
                  rs[0] = 0;
                  break;
                case 'm':
                  colors = 1;
                  bs[0] = 0;
                  gs[0] = 200;
                  rs[0] = 255;
                  break;
                default:
                  break;
    
              }

              
            }
          }
          
        }


      }

    }

    // close the connection:

    client.stop();
    Serial.println("client disconnected");
  }
//  for (int i = 0; i < colors; i++){
  int dR = (int) (rs[currentColor]*brightness - cR);
  int dG = (int) (gs[currentColor]*brightness - cG);
  int dB = (int) (bs[currentColor]*brightness - cB);
  stay = STAY / rate;
  if (abs(dR) < 1 && abs(dG) < 1 && abs(dB) < 1){
    if (curStay > stay){
      currentColor = (currentColor + 1) % colors;
      curStay = 0;
      dR = (int) (rs[currentColor]*brightness - cR);
      dG = (int) (gs[currentColor]*brightness - cG);
      dB = (int) (bs[currentColor]*brightness - cB);
    }
    else{
      curStay++;
    }
   
//    dR =(int) rs[currentColor]*brightness - cR;
//    dG = (int) gs[currentColor]*brightness - cG;
//    dB = (int) bs[currentColor]*brightness - cB;
  }
  count++;
  if (count%50 == 0){
    if (DEBUG){
      Serial.println();
      Serial.println(currentColor);
      Serial.println(colors);
      Serial.println(brightness);
      Serial.println("cR, cG, cB");
      Serial.print(cR);
      Serial.print(" ");
      Serial.print(cG);
      Serial.print(" ");
      Serial.println(cB);
      Serial.println("targets ");
      Serial.print(rs[currentColor]*brightness);
      Serial.print(" ");
      Serial.print(gs[currentColor]*brightness);
      Serial.print(" ");
      Serial.println(bs[currentColor]*brightness);
      Serial.println("value");
      Serial.print((int) ( cR));
      Serial.print(" ");
      Serial.print((int) (cG));
      Serial.print(" ");
      Serial.print((int) (cB));
    }
    count = 0;
  }

  
  
  
  float maxVal = max(cG, max(cR, cB));
  float scale = (255/maxVal)*brightness;
  scale = 1;

  if (abs(dR) + abs(dG) + abs(dB) < 5){
    cG = gs[currentColor]*brightness;
    cB = bs[currentColor] * brightness;
    cR = rs[currentColor] * brightness;
  }
  else{
    cR = cR +  rate * getDiff(dR);
    cG = cG + rate * getDiff(dG);
    cB = cB + rate * getDiff(dB);
  }
  cR = max(min(cR, 255), 0);
  cG = max(min(cG, 255), 0);
  cB = max(min(cB, 255), 0);
  analogWrite(green, (int) ( cG));
  analogWrite(red, (int) (cR));
  analogWrite(blue, (int) (cB));
  delay(DELAY);
  if (currentColor >= colors){
    currentColor = 0;
  }
  
}

float getDiff(float x){
  float sign = 1.0;
  if (x<0){
    sign = -1;
  }
  
  if (abs(x) > 100){
    return sign*5.5;
  }
  if (abs(x) > 75){
    return sign*5.1;
  }
  if (abs(x) > 50){
    return sign * 4.2;
  }
  if (abs(x) > 30){
    return sign * 3.4;
  }
  if (abs(x) > 20){
    return sign * 2.4;
  }
  if (abs(x) > 5){
    return sign * 1.0;
  }

  return sign;
}
void printWiFiStatus() {

  // print the SSID of the network you're attached to:

  Serial.print("SSID: ");

  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:

  IPAddress ip = WiFi.localIP();

  Serial.print("IP Address: ");

  Serial.println(ip);

  // print the received signal strength:

  long rssi = WiFi.RSSI();

  Serial.print("signal strength (RSSI):");

  Serial.print(rssi);

  Serial.println(" dBm");

  // print where to go in a browser:

  Serial.print("To see this page in action, open a browser to http://");

  Serial.println(ip);
}
