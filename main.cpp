
#include <Arduino.h>
#include "driver/ledc.h"

#define number_of_servo 8

int servo[number_of_servo] = {13,12,14,27,26,25,33,32};

int pwnChannels[number_of_servo] = {0,1,2,3,4,5,6,7};
int pwmFreq = 50;
int pwmRes = 16;

// reange in ms
int minPul = 500;
int maxPul = 2500;

// angle -> duty cycle

uint32_t angleToDuty(int angle){
    int pw = map(angle, 0, 180, minPul, maxPul);
    uint32_t duty = (pw * ((1 << pwmRes) - 1)) / 20000;
    return duty;
}

void setup(){

    Serial.begin(115200);

    for (int i = 0; i< number_of_servo; i++){
        ledcAttach(servo[i], pwmFreq, pwmRes);
        ledcWrite(servo[i], angleToDuty(90));
    }

}

void loop(){
    // now we have to write the code to controll

    // this will move all servo from 0 to 180

    for (int angle = 0; angle <= 180; angle += 5){
        for (int i = 0; i < number_of_servo; i++){
            ledcWrite(pwnChannels[i],angleToDuty(angle));
        } 

        delay(20);
    } 

    delay(1000);


    // now we'll move all servo from 180 to 0

    for (int angle = 180; angle >= 0; angle -= 5){
        for (int i = 0; i < number_of_servo; i++){
            ledcWrite(pwnChannels[i], angleToDuty(angle));
        }

        delay(20);
    }


    delay(1000);

}