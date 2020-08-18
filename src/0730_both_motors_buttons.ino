/* 10 Meter Track Bogie Control System
 *  
 *  Description: Testing github with carlos
 *  
 *  Created by: Lucas, Logan and Carlos
 *  Modified: 08/18
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <VescUART.h>

// Create VescUart object to provide access to the class functions
VescUart UART;
float current = 0.0; // current for hub motors
float currentBrake = 3.0; // braking current

// Constants for wifi connection (Spartan Superway settings)
const char* ssid     = "ATTwvqYWQs";
const char* password = "ds?37=qg545n";
const char* host ="192.168.1.181";

WiFiServer server(80);

void setup()
{
    // Initialize serial for UART communication
    Serial.begin(115200);
    while (!Serial) {;}
    UART.setSerialPort(&Serial);
    delay(10);

    // Connect to Wifi
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();
}

void loop(){
 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      UART.setCurrent(current);
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

            // the content of the HTTP response follows the header:
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            
            // CSS to style the in/out buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            
            client.println(".button {background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;display: inline-grid;}");
            
            client.println(".button2 {background-color: #FF0000; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;display: inline-grid;}");
            
            client.println(".button3 {background-color: #0064FF; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;");
            client.println("display: inline-grid; width: 13%;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Control System for Bogie</h1>");

            // Buttons
            client.print("<h2>Hub Motor Controls</h2><a class=\"button\" href=\"/F\">FWD</a>");
            client.print("<a class=\"button\" href=\"/B\">REV</a><br>");
            client.print("<p></p><a class=\"button2\" href=\"/S\">Stop</a>");

            // client.print("<h2>Switch Motor Controls</h2><a class=\"button3\" href=\"/R\">Right</a><br>");
            // client.print("<a class=\"button3\" href=\"/L\">Left</a>");
             
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /F" or "GET /B" or "GET /S" to control the DC motor:
        if (currentLine.endsWith("GET /F")) {
          // UART.setCurrent(current);
          current = 3;
          Serial.println("forward");
          delay(50);
        }
        else if (currentLine.endsWith("GET /B")) {
          // UART.setCurrent(0);
          current = -3;
          Serial.println("backward");
          delay(50);
        }
        else if (currentLine.endsWith("GET /S")) {
          current = 0;
          for (int i=0; i <= 3; i++) {
            UART.setBrakeCurrent(currentBrake);
            delay(50);
          }
          Serial.println("stop");
          delay(50);
        }

        // Check to see if the client request was "GET /R" or "GET /L" to turn Stepper:
        else if (currentLine.endsWith("GET /R")) {
        }
        else if (currentLine.endsWith("GET /L")) {
        }      
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}