//hola

// /*
//   Rui Santos & Sara Santos - Random Nerd Tutorials
//   Complete project details at https://RandomNerdTutorials.com/esp-now-two-way-communication-esp32/
//   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
//   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// */
// #include <esp_now.h>
// #include <WiFi.h>


// // REPLACE WITH THE MAC Address of your receiver 
// //3C:61:05:3E:25:7C

// uint8_t broadcastAddress[] = {0xC8, 0xC9, 0xA3, 0xD1, 0x7F, 0x10};
// uint8_t broadcastAddress[] = {0x3C, 0x61, 0x05, 0x3E, 0x25, 0x7C};


// // Variable to store if sending data was successful
// String success;

// //Structure example to send data
// //Must match the receiver structure
// typedef struct struct_message {
//     unsigned long int t;
//     int act;
   
// } struct_message;


// // Create a struct_message to hold incoming sensor readings
// struct_message structdata;

// esp_now_peer_info_t peerInfo;

// unsigned long int t;
// int allow;

// // Callback when data is sent
// void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
//   Serial.print("\r\nLast Packet Send Status:\t");
//   Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
//   if (status ==0){
//     success = "Delivery Success :)";
//   }
//   else{
//     success = "Delivery Fail :(";
//   }
// }

// // Callback when data is received
// void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
//   memcpy(&structdata, incomingData, sizeof(structdata));
//   Serial.print("Bytes received: ");
//   Serial.println(len);
//   t= structdata.t;
//   allow = structdata.act;

// }
 
// void setup() {
//   // Init Serial Monitor
//   Serial.begin(115200);

//   // Init BME280 sensor

 
//   // Set device as a Wi-Fi Station
//   WiFi.mode(WIFI_STA);

//   // Init ESP-NOW
//   if (esp_now_init() != ESP_OK) {
//     Serial.println("Error initializing ESP-NOW");
//     return;
//   }

//   // Once ESPNow is successfully Init, we will register for Send CB to
//   // get the status of Trasnmitted packet
//   esp_now_register_send_cb(OnDataSent);
  
//   // Register peer
//   memcpy(peerInfo.peer_addr, broadcastAddress, 6);
//   peerInfo.channel = 0;  
//   peerInfo.encrypt = false;
  
//   // Add peer        
//   if (esp_now_add_peer(&peerInfo) != ESP_OK){
//     Serial.println("Failed to add peer");
//     return;
//   }
//   // Register for a callback function that will be called when data is received
//   esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
// }
 
// void loop() {
  
//   // Send message via ESP-NOW
//   esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &structdata, sizeof(structdata));
   
//   if (result == ESP_OK) {
//     Serial.println("Sent with success");
//   }
//   else {
//     Serial.println("Error sending the data");
//   }
//   delay(10000);
// }
