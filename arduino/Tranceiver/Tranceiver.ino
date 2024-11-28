/* ESP32S2 ESPNow based Nerf Tank remote
 * By Juntendo
*/

#include <esp_now.h>
#include <WiFi.h> //Use official EPS library (Not arduino)

const int button_pin = 12;
const int Rx_pin = 9; //ADC1_8
const int Ry_pin = 7; //ADC1_6

int Rx_value = 0;
int Ry_value = 0;
bool buttonState;

//Button paramters
boolean buttonActive = false;
boolean longPressActive = false;

unsigned long buttonTimer =0; //Debounce timer
unsigned long longPressTime = 500;  //Long press duration

//State mode paramters
boolean fireCommand = false;
boolean turretMode = false;

void longPressed() {
  //Fire sequence
  fireCommand = 1;
}

void shortPressed() {
  //Mode change sequence
  turretMode = 1 - turretMode;
}

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x70, 0x04, 0x1D, 0xF5, 0x9A, 0xE8};

// Structure example to send data
// Must match the receiver structure

typedef struct struct_message {
  int Rx;
  int Ry;
  bool turretMode;
  bool fireCommand;
} struct_message;


// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  //  Initialise pins
  pinMode(button_pin, INPUT);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
  //Read ADC value
  Rx_value = analogRead(Rx_pin);
  Ry_value = analogRead(Ry_pin);

  //Button
  if (digitalRead(button_pin) == LOW){
    if(buttonActive == false){
      buttonActive = true;
      buttonTimer = millis();
    }
    if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) {
      longPressActive = true;
      longPressed();
    }
  }
  else{
    if (buttonActive == true){
      if (longPressActive == true){
        longPressActive = false;
      }
      else{
        shortPressed();
      }
      buttonActive = false;
    }
  }
  
  // Set values to send
  myData.Rx = Rx_value;
  myData.Ry = Ry_value;
  myData.turretMode = turretMode;
  myData.fireCommand = fireCommand;
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  //Clear fire command after send;
  fireCommand = 0;
  delay(2000);
}
