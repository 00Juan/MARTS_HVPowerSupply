#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_ADS1X15.h>
#include <esp_now.h>
#include <WiFi.h>

Adafruit_ADS1115 ads;
// Pines
int pinSSR = 19, pinR0 = 18, pinK1 = 17, pinK2 = 16,pinGND=4, pinBot1 = 25, pinBot2 = 26, pinBot3 = 27;
int pinLedR=13,pinLedG=14;
bool cmdSSR, cmdR0, cmdK1, cmdK2,cmdGND,cmdLedR,cmdLedG;
bool stsB1, stsB2, stsB3, stsB1Trigger, stsB2Trigger, stsB3Trigger, stsB1TriggerAUX, stsB2TriggerAUX, stsB3TriggerAUX;
int delayPRG = 1000;
int delayBounce = 30;
// Estado
// int stsOuput=0; //0-ssrOff R0 off, 1-ssrON R0Off, 2.ssrON R0ON k2off k1off
int stepindexPRG = 0;
int stepindexManual = 0;

bool sts300=true, stsOUT, stsIN;

int cont = 0;

//CAL:
float calA0=2.507489;
float calA1=2.497620;
float difTrigger=0.001;

//Timer
unsigned long int tMax=2000,tC=0;


//3C:61:05:3E:25:7C esprec
//C8:C9:A3:D1:7F:10 espenv
uint8_t broadcastAddress[] = {0x3C, 0x61, 0x05, 0x3E, 0x25, 0x7C};
// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    int act;
   
} struct_message;
// Create a struct_message to hold incoming sensor readings
struct_message structdata;

esp_now_peer_info_t peerInfo;

unsigned long int t;
int allow;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Serial.print("\r\nLast Packet Send Status:\t");
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  // if (status ==0){
  //   success = "Delivery Success :)";
  // }
  // else{
  //   success = "Delivery Fail :(";
  // }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

  memcpy(&structdata, incomingData, sizeof(structdata));
  // Serial.print("Bytes received: ");
  // Serial.println(len);
  allow = structdata.act;
  tC=millis();

}

void read()
{
  stsB1 = !digitalRead(pinBot1);
  stsB2 = !digitalRead(pinBot2);
  stsB3 = !digitalRead(pinBot3);

  if (stsB1 && !stsB1TriggerAUX)
  {
    stsB1TriggerAUX = true;
    stsB1Trigger = true;
    delay(delayBounce);
  }
  else if (!stsB1 && stsB1TriggerAUX)
  {
    stsB1TriggerAUX = false;
  }
  else
  {
    stsB1Trigger = false;
  }

  if (stsB2 && !stsB2TriggerAUX)
  {
    stsB2TriggerAUX = true;
    stsB2Trigger = true;
    delay(delayBounce);
  }
  else if (!stsB2 && stsB2TriggerAUX)
  {
    stsB2TriggerAUX = false;
  }
  else
  {
    stsB2Trigger = false;
  }

  if (stsB3 && !stsB3TriggerAUX)
  {
    stsB3TriggerAUX = true;
    stsB3Trigger = true;
    delay(delayBounce);
  }
  else if (!stsB3 && stsB3TriggerAUX)
  {
    stsB3TriggerAUX = false;
  }
  else
  {
    stsB3Trigger = false;
  }
}


 //0.48
void write()
{
  digitalWrite(pinSSR, cmdSSR);
  digitalWrite(pinR0, cmdR0);
  digitalWrite(pinK1, !cmdK1);
  digitalWrite(pinK2, !cmdK2);
  digitalWrite(pinGND, cmdGND);
  digitalWrite(pinLedR, cmdLedR);
  digitalWrite(pinLedG, cmdLedG);
}

void setup()
{
  Serial.begin(115200);
  pinMode(pinSSR, OUTPUT);
  pinMode(pinR0, OUTPUT);
  pinMode(pinK1, OUTPUT);
  pinMode(pinK2, OUTPUT);
  pinMode(pinGND, OUTPUT);
  pinMode(pinBot1, INPUT_PULLUP);
  pinMode(pinBot2, INPUT_PULLUP);
  pinMode(pinBot3, INPUT_PULLUP);

  pinMode(pinLedR, OUTPUT);
  pinMode(pinLedG, OUTPUT);
  ads.setGain(GAIN_ONE); 
  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }


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
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void print()
{
  //Serial.println((String) "B1: " + stsB1 + " B2: " + stsB2 + " B3: " + stsB3);
 // Serial.println(structdata.act);
}

void HVPrgManual()
{
  if (!stsIN && stsB1Trigger)
  {
    stsIN = true;
    cmdSSR = true;
  }
  else if (stsIN && stsB1Trigger)
  {
    stsIN = false;
    cmdSSR = false;
  }

  if (sts300 && stsB2Trigger)
  {
    sts300 = false;
    cmdK2 = true;
    write();
    delay(delayPRG);
    cmdGND = true;
    write();
    delay(delayPRG);
    cmdK1 = true;
    write();
    stsB2Trigger=false;
  }
  else if (!sts300 && stsB2Trigger)
  {
    sts300 = true;
    cmdK1 = false;
    write();
    delay(delayPRG);
    cmdGND = false;
    write();
    delay(delayPRG);
    cmdK2 = false;
    write();
    stsB2Trigger=false;
  }
  if (!stsOUT && stsB3Trigger)
  {
    stsOUT = true;
    cmdR0 = true;
    // write();
    // delay(delayPRG);
  }
  else if (stsOUT && stsB3Trigger)
  {

    stsOUT = false;
    cmdR0 = false;
    // write();
    // delay(delayPRG);
  }



}

void HVPrg()
{
  static int stepindexAux = 0;
  switch (stepindexPRG)
  {
  case 0:
    // SSR off
    cmdSSR = cmdR0 = cmdK1 = cmdK2 = false;
    if (stsB2)
    {
      cmdR0 = cmdK1 = cmdK2 = false;
      cmdSSR = true;
      stepindexPRG += 10;
    }
    break;

  case 10:
    // SSR on but OUT off
    if (stsB1)
    {
      stepindexPRG = 0;
    }
    if (stsB2)
    {
      cmdK1 = cmdK2 = false;
      cmdSSR = cmdR0 = true;
      stepindexPRG += 10;
    }
    break;

  case 20:
    // 300v mode
    cmdK1 = cmdK2 = false;
    cmdSSR = cmdR0 = true;
    if (stsB1)
    {
      stepindexPRG = 0;
    }
    else if (stsB2)
    {
      cmdK2 = cmdR0 = false;
      cmdSSR = cmdK2 = true;
      stepindexPRG += 5;
    }
    break;

  case 25:
    // change
    // wait
    if (stsB1)
    {
      stepindexPRG = 0;
    }
    stepindexPRG += 5;
    break;

  case 30:
    // 600v mode
    cmdK1 = cmdK2 = cmdSSR = cmdR0 = true;
    if (stsB1)
    {
      stepindexPRG = 0;
    }
    else if (stsB2)
    {
      cmdK1 = cmdR0 = false;
      cmdSSR = cmdK2 = true;
      stepindexPRG += 5;
    }
    break;

  case 35:
    // change
    if (stsB1)
    {
      stepindexPRG = 0;
    }
    stepindexPRG = 20;
    break;

  default:
    break;
  }
  if ((stepindexAux != stepindexPRG) && stepindexPRG != 0)
  {
    delay(delayPRG);
    stepindexAux = stepindexPRG;
  }
}


void readADC()
{
  int16_t adc0, adc1, adc2, adc3;
  float volts0, volts1, volts2, volts3;

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  volts0 = ads.computeVolts(adc0);
  volts1 = ads.computeVolts(adc1);

  float avg0=0,avg1=0,numS=3;

  for(int i=0;i<numS;i++)
  {
    adc0 = ads.readADC_SingleEnded(0);
    adc1 = ads.readADC_SingleEnded(1);
    volts0 = ads.computeVolts(adc0);
    volts1 = ads.computeVolts(adc1);
    avg0+=volts0;
    avg1+=volts1;
  }
  avg0=avg0/(numS*1.0);
  avg1=avg1/(numS*1.0);

  Serial.println("-----------------------------------------------------------");
  Serial.print("AIN0: "); Serial.print(adc0); Serial.print("  "); Serial.print(avg0,6); Serial.println("V");
  Serial.print("AIN1: "); Serial.print(adc1); Serial.print("  "); Serial.print(avg1,6); Serial.println("V");
  float dif=abs(abs(calA0-avg0)-abs(calA1-avg1));
  Serial.print("Dif: ");Serial.println(dif,6);

  // if(dif>=difTrigger)
  // {
  //   stsIN=0;
  //   stsOUT=0;
  // }

}


void loop()
{

  read();
  //readADC();
  

  
  HVPrgManual();
  if(((millis()-tC)>=tMax)||structdata.act==0)
  {
    cmdR0=0;
    cmdSSR=0;
  }

  cmdLedG=cmdSSR;
  cmdLedR=cmdR0;
  

  // HVPrg();



  write();
  print();
  // delay(100);
}

