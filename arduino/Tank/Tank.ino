/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-dc-motor-l298n-motor-driver-control-speed-direction/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.  
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

#include <esp_now.h>
#include <WiFi.h>

typedef struct struct_message {
  int Rx;
  int Ry;
} struct_message;

struct_message myData;

// Motor A
int motor1Pin1 = 3; 
int motor1Pin2 = 5; 
int motor1Pin3 = 7; 
int motor1Pin4 = 9; 

int enableA = 11; //1,2
int enableB = 12; //3,4


// Setting PWM properties
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 255;


// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));

  Serial.print("Rx: ");
  Serial.println(myData.Rx);
  Serial.print("Ry: ");
  Serial.println(myData.Ry);

  Serial.println();
}

 
void setup() {
  // sets the pins as outputs:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor1Pin3, OUTPUT);
  pinMode(motor1Pin4, OUTPUT);
  
  // configure LEDC PWM
  ledcAttachChannel(enableA, freq, resolution, pwmChannel);
  ledcAttachChannel(enableB, freq, resolution, pwmChannel);
  
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

}

void loop() {
  // Move the DC motor forward at maximum speed
  Serial.println("Moving Forward");
  ledcWrite(enableA, dutyCycle);
  ledcWrite(enableB, dutyCycle);
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor1Pin3, LOW);
  digitalWrite(motor1Pin4, HIGH);
  
  delay(2000);

  // Stop the DC motorz
  Serial.println("Motor stopped");
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor1Pin3, HIGH);
  digitalWrite(motor1Pin4, LOW);
  delay(1000);


}
