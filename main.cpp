#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "driver/ledc.h"

#define number_of_servo 8

int servoPins[number_of_servo] = {13,12,14,27,26,25,33,32};
int pwmChannels[number_of_servo] = {0,1,2,3,4,5,6,7};

int pwmFreq = 50;
int pwmRes = 16;

int minPul = 500;
int maxPul = 2500;


const char* ssid = "wifi_ssid";
const char* password = "wifi_pass";

WebServer server(80);


uint32_t angleToDuty(int angle){
    int pw = map(angle, 0, 180, minPul, maxPul);
    return (pw * ((1 << pwmRes) - 1)) / 20000;
}


void movingFunction(int number, int angle){
    ledcWrite(pwmChannels[number], angleToDuty(angle));
}


String htmlPage(){
    String html = "<!DOCTYPE html><html><body>";
    html += "<h2>Robo Dog Control</h2>";

    for(int i=0;i<number_of_servo;i++){
        html += "Servo " + String(i) + 
        ": <input type='range' min='0' max='180' value='90' ";
        html += "oninput='send(" + String(i) + ", this.value)'><br>";
    }

    html += R"rawliteral(
    <script>
    function send(servo, angle){
        fetch(`/move?servo=` + servo + `&angle=` + angle);
    }
    </script>
    )rawliteral";

    html += "</body></html>";
    return html;
}


void handleRoot(){
    server.send(200, "text/html", htmlPage());
}


void handleMove(){
    int servoNum = server.arg("servo").toInt();
    int angle = server.arg("angle").toInt();

    if(servoNum >= 0 && servoNum < number_of_servo){
        movingFunction(servoNum, angle);
    }

    server.send(200, "text/plain", "OK");
}

void setup(){
    Serial.begin(115200);


    for(int i=0;i<number_of_servo;i++){
        ledcSetup(pwmChannels[i], pwmFreq, pwmRes);
        ledcAttachPin(servoPins[i], pwmChannels[i]);
        ledcWrite(pwmChannels[i], angleToDuty(90));
    }


    WiFi.begin(ssid, password);
    Serial.print("Connecting");

    while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConnected!");
    Serial.println(WiFi.localIP());


    server.on("/", handleRoot);
    server.on("/move", handleMove);

    server.begin();
}

void loop(){
    server.handleClient();
}