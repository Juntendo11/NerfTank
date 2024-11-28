/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-dc-motor-l298n-motor-driver-control-speed-direction/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

#include <esp_now.h>
#include <WiFi.h>

/*
  ~5000 = idle
  Rx => 6000 = right
  Rx => 4000 => left

  Ry => 6000 => foward
  Ry => 4000 => backwards
*/

#define LEFT_THRESHOLD  4000
#define RIGHT_THRESHOLD 6000
#define UP_THRESHOLD    4000
#define DOWN_THRESHOLD  6000


typedef struct struct_message {
  int Rx;
  int Ry;
  bool turretMode;
  bool fireCommand;
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

void idle() {
  //Stop
  Serial.println("Stop");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor1Pin3, LOW);
  digitalWrite(motor1Pin4, LOW);
}

void move_forwards() {
  Serial.println("Forwards");
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor1Pin3, HIGH);
  digitalWrite(motor1Pin4, LOW);
}

void move_backwards() {
  Serial.println("Backwards");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor1Pin3, LOW);
  digitalWrite(motor1Pin4, HIGH);
}

void turn_right() {
  Serial.println("Right");
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor1Pin3, LOW);
  digitalWrite(motor1Pin4, HIGH);
}

void turn_left() {
  Serial.println("Left");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor1Pin3, HIGH);
  digitalWrite(motor1Pin4, LOW);
}


// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  /*
    Serial.print("Rx: ");
    Serial.println(myData.Rx);
    Serial.print("Ry: ");
    Serial.println(myData.Ry);
    Serial.print("turretMode: ");
    Serial.println(myData.turretMode);
    Serial.print("fireCommand: ");
    Serial.println(myData.fireCommand);
    Serial.println();
  */
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
  Serial.println("Initialized ESP-NOW");
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  //Wont be in the loop from here

}

void loop() {
  // Move the DC motor forward at maximum speed
  ledcWrite(enableA, dutyCycle);
  ledcWrite(enableB, dutyCycle);

  // check left/right commands
  if (myData.Rx < LEFT_THRESHOLD) {
    Serial.println(myData.Rx);
    turn_left();
  }
  else if (myData.Rx > RIGHT_THRESHOLD) {
    turn_right();
  }
  // check up/down commands
  else if (myData.Ry < UP_THRESHOLD) {
    move_forwards();
  }
  else if (myData.Ry > DOWN_THRESHOLD) {
    move_backwards();
  }
  else {
    idle();
  }

  //if ((LEFT_THRESHOLD < myData.Rx <RIGHT_THRESHOLD)&&(DOWN_THRESHOLD < myData.Ry <UP_THRESHOLD)){
  //idle();
  //}
  delay(100);


}
