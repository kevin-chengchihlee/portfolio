/*
Cooperative IOT Self Organizing Example
SwitchDoc Labs, August 2015

 */
#include <Wire.h>
#include <WiFi.h>
#include <Network.h>
#include <NetworkUdp.h>
#undef DEBUG

char ssid[] = "iPhoneKL";  //  your network SSID (name)
char pass[] = "123456789"; // your network password

unsigned int localPort = 1996; // local port to listen for UDP packets
IPAddress serverAddress = IPAddress(255, 255, 255, 255); // default no IP Address

#define LED_BUILTIN 2
#define PHOTO_SNS_PIN 34

#define VERSIONNUMBER 28
#define LOGGERIPINC 20
#define SWARMSIZE 5
// 30 seconds is too old - it must be dead
#define SWARMTOOOLD 30000

int mySwarmID = 0;

// Packet Types

#define LIGHT_UPDATE_PACKET 0
#define RESET_SWARM_PACKET 1
#define CHANGE_TEST_PACKET 2
#define RESET_ME_PACKET 3
#define DEFINE_SERVER_LOGGER_PACKET 4
#define LOG_TO_SERVER_PACKET 5
#define MASTER_CHANGE_PACKET 6
#define BLINK_BRIGHT_LED 7

// master variables
boolean masterState = false;   // True if master, False if not
int swarmClear[SWARMSIZE];
int swarmVersion[SWARMSIZE];
int swarmState[SWARMSIZE];
long swarmTimeStamp[SWARMSIZE];   // for aging

int swarmAddresses[SWARMSIZE];  // Swarm addresses

// variables for light sensor

int clearColor;
int redColor;
int blueColor;
int greenColor;

const int PACKET_SIZE = 14; // Light Update Packet
const int BUFFERSIZE = 1024;

byte packetBuffer[BUFFERSIZE]; //buffer to hold incoming and outgoing packets
char photo_sns_buffer[10];  /*temporary buffer for text*/
/*for movavg*/
uint16_t PHOTO_READINGS[5] = {0, 0, 0, 0, 0};/*executed in a one second task with 2 seconds MovAvg*/
// A UDP instance to let us send and receive packets over UDP
NetworkUDP udp;
IPAddress localIP;

int data_cnt = 0;
uint16_t c = 0;

TaskHandle_t xHandle_Task_UDP_Receive;
TaskHandle_t xHandle_Task_UDP_Send;
TaskHandle_t xHandle_Task_PhotoSns;
TaskHandle_t xHandle_Task_Swarm;
TaskHandle_t xHandle_Task_LED_Blink;
SemaphoreHandle_t swarmMutex;

void setup()
{
  /*Start*/
  Serial.begin(19200);
  Serial.println();
  Serial.println();
  Serial.println("");
  Serial.println("--------------------------");
  Serial.println("LightSwarm");
  Serial.print("Version ");
  Serial.println(VERSIONNUMBER);
  Serial.println("--------------------------");

  Serial.println(F(" 09/03/2015"));
  Serial.print(F("Compiled at:"));
  Serial.print (F(__TIME__));
  Serial.print(F(" "));
  Serial.println(F(__DATE__));
  Serial.println();
  pinMode(LED_BUILTIN, OUTPUT);
  /*Start*/
  
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  /*Get a random seed*/
  randomSeed(analogRead(35));
  Serial.print("analogRead(35)=");
  Serial.println(analogRead(35));

  // everybody starts at 0 and changes from there
  mySwarmID = 0;

  // We start by connecting to a WiFi network
  Serial.print("LightSwarm Instance: ");
  Serial.println(mySwarmID);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
 
  // initialize Swarm Address - we start out as swarmID of 0

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");

  udp.begin(localPort);       // bind & listen on port
  //udp.setBroadcast(true);     // allow broadcast TX
  Serial.print("Local port: ");
  Serial.println(localPort);

  // initialize light sensor and arrays
  int i;
  for (i = 0; i < SWARMSIZE; i++)
  {
    swarmAddresses[i] = 0;
    swarmClear[i] = 0;
    swarmTimeStamp[i] = -1;
  }
  swarmClear[mySwarmID] = 0;
  swarmTimeStamp[mySwarmID] = 1;   // I am always in time to myself
  clearColor = swarmClear[mySwarmID];
  swarmVersion[mySwarmID] = VERSIONNUMBER;
  swarmState[mySwarmID] = masterState;
  Serial.print("clearColor =");
  Serial.println(clearColor);
  // set SwarmID based on IP address 
  localIP = WiFi.localIP();
  swarmAddresses[0] =  localIP[3];
  mySwarmID = 0;
  Serial.print("MySwarmID=");
  Serial.println(mySwarmID);

  /*Mutesx to protect shared swarm data field*/
  swarmMutex = xSemaphoreCreateMutex();
  /*Task Creation*/
  xTaskCreatePinnedToCore(Task_PhotoSns, "Task_PhotoSns", 4096, NULL, 1, &xHandle_Task_PhotoSns, 1);
  xTaskCreatePinnedToCore(Task_UDP_Receive, "Task_UDP_Receive", 4096, NULL, 2, &xHandle_Task_UDP_Receive, 1);
  xTaskCreatePinnedToCore(Task_UDP_Send, "Task_UDP_Send", 4096, NULL, 1, &xHandle_Task_UDP_Send, 1);
  xTaskCreatePinnedToCore(Task_Swarm, "Task_Swarm", 4096, NULL, 1, &xHandle_Task_Swarm, 1);
  xTaskCreatePinnedToCore(Task_LED_Blink, "Task_LED_Blink", 4096, NULL, 1, &xHandle_Task_LED_Blink, 1);
  xTaskCreatePinnedToCore(Task_Monitor, "Task_Monitor", 4096, NULL, 1, NULL, 1);
  
  vTaskDelete(NULL);
}

void loop()
{
 /**/
}

// send an LIGHT Packet request to the swarms at the given address
void sendLightUpdatePacket(IPAddress & address)
{
  memset(packetBuffer, 0, PACKET_SIZE);
  xSemaphoreTake(swarmMutex, portMAX_DELAY);
// update swarm arrays and clearColor
  packetBuffer[0] = 0xF0;   
  packetBuffer[1] = LIGHT_UPDATE_PACKET;
  packetBuffer[2] = localIP[3];
  packetBuffer[3] = masterState;
  packetBuffer[4] = VERSIONNUMBER;
  packetBuffer[5] = (clearColor >> 8) & 0xFF;
  packetBuffer[6] = (clearColor & 0xFF);
  packetBuffer[7] = (redColor >> 8) & 0xFF;
  packetBuffer[8] = (redColor & 0xFF);
  packetBuffer[9] = (greenColor >> 8) & 0xFF;
  packetBuffer[10] = (greenColor & 0xFF);
  packetBuffer[11] = (blueColor >> 8) & 0xFF;
  packetBuffer[12] = (blueColor & 0xFF);
  packetBuffer[13] = 0x0F;
  xSemaphoreGive(swarmMutex);
  // === ESP32 core 3.x correct send path ===
  udp.beginPacket(address, localPort);
  udp.write(packetBuffer, PACKET_SIZE);
  udp.endPacket();
}

// delay 0-MAXDELAY seconds
#define MAXDELAY 500
void broadcastARandomUpdatePacket()
{
  int sendToLightSwarm = 255;
  int randomDelay;
  randomDelay = random(0, MAXDELAY);
  Serial.print("Delay = ");
  Serial.print(randomDelay);
  Serial.print("ms : ");

  vTaskDelay(randomDelay / portTICK_PERIOD_MS);

  IPAddress sendSwarmAddress(255, 255, 255, sendToLightSwarm); // my Swarm Address
  sendLightUpdatePacket(sendSwarmAddress);
}

void checkAndSetIfMaster()
{
  int i;
  int howLongAgo;
  for (i = 0; i < SWARMSIZE; i++)
  {

#ifdef DEBUG
    Serial.print("swarmClear[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.print(swarmClear[i]);
    Serial.print("  swarmTimeStamp[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(swarmTimeStamp[i]);
#endif
    Serial.print("#");
    Serial.print(i);
    Serial.print("/");
    Serial.print(swarmState[i]);
    Serial.print("/");
    Serial.print(swarmVersion[i]);
    Serial.print(":");
    // age data
    howLongAgo = millis() - swarmTimeStamp[i] ;

    if (swarmTimeStamp[i] == 0)
    {
      Serial.print("TO ");
    }
    else if (swarmTimeStamp[i] == -1)
    {
      Serial.print("NP ");
    }
    else if (swarmTimeStamp[i] == 1)
    {
      Serial.print("ME ");
    }
    else if (howLongAgo > SWARMTOOOLD)
    {
      Serial.print("TO ");
      swarmTimeStamp[i] = 0;
      swarmClear[i] = 0;

    }
    else
    {
      Serial.print("PR ");
    }
  }

  Serial.println();
  boolean setMaster = true;

  for (i = 0; i < SWARMSIZE; i++)
  {

    if (swarmClear[mySwarmID] >= swarmClear[i])
    {
      // I might be master!
    }
    else
    {
      // nope, not master
      setMaster = false;
      break;
    }

  }
  if (setMaster == true)
  {
    if (masterState == false)
    {
      Serial.println("I just BECAME Master");
    }
    masterState = true;
    vTaskSuspend(xHandle_Task_LED_Blink);
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else //setMaster == false
  {
    if (masterState == true)
    {
      Serial.println("I just LOST Master");
    }
    masterState = false;
    vTaskResume(xHandle_Task_LED_Blink);
  }
  swarmState[mySwarmID] = masterState;
}

int setAndReturnMySwarmIndex(int incomingID)
{
  int i;
  for (i = 0; i< SWARMSIZE; i++)
  {
    if (swarmAddresses[i] == incomingID)
    {
       return i;
    } 
    else
    if (swarmAddresses[i] == 0)  // not in the system, so put it in
    {
      swarmAddresses[i] = incomingID;
      Serial.print("incomingID ");
      Serial.print(incomingID);
      Serial.print("  assigned #");
      Serial.println(i);
      return i;
    }
  }  
  
  // if we get here, then we have a new swarm member.   
  // Delete the oldest swarm member and add the new one in 
  // (this will probably be the one that dropped out)
  
  int oldSwarmID;
  long oldTime;
  oldTime = millis();
  for (i = 0;  i < SWARMSIZE; i++)
 {
  if (oldTime > swarmTimeStamp[i])
  {
    oldTime = swarmTimeStamp[i];
    oldSwarmID = i;
  }
 } 
 
 // remove the old one and put this one in....
 swarmAddresses[oldSwarmID] = incomingID;
 // the rest will be filled in by Light Packet Receive
 
}

void Task_UDP_Receive(void *pvParameters){

  for(;;){
    int cb = udp.parsePacket();
    if (cb) {
      udp.read(packetBuffer, PACKET_SIZE);
      xSemaphoreTake(swarmMutex, portMAX_DELAY);
// update swarm arrays and clearColor
      Incoming_Packet_Helper(packetBuffer);
      xSemaphoreGive(swarmMutex);
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void Task_UDP_Send(void *pvParameters){
  for(;;){
    vTaskDelay(1);                 // let RX run first
    broadcastARandomUpdatePacket();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void Task_Swarm(void *pvParameters){
  for(;;){
    // Check to see if I am master!
    checkAndSetIfMaster();
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void Task_PhotoSns(void *pvParameters){
  uint8_t cnt = 0;
  uint16_t val = 0;
  for(;;){
    val = Photo_Sns(); /*Doing Photo Cell Sensing*/
    if ((data_cnt>=5)) { /*Wait for 5 sec worth data before sending*/
        c = val;
        Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
        Serial.println(" ");
        clearColor = c;
        swarmClear[mySwarmID] = clearColor;
    }
    cnt++;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void Task_LED_Blink(void *pvParameters){
  for(;;){
    uint16_t BLNK_MS;
    if(swarmClear[mySwarmID]<=500){BLNK_MS = 2000;}
    else if(swarmClear[mySwarmID]<=1500){BLNK_MS = 1500;}
    else if(swarmClear[mySwarmID]<=2500){BLNK_MS = 1000;}
    else if(swarmClear[mySwarmID]<=3500){BLNK_MS = 800;}
    else if(swarmClear[mySwarmID]> 3500){BLNK_MS = 300;}
    Serial.print("BLNK_MS Value = ");
    Serial.print(BLNK_MS);
    Serial.print('\n');
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    vTaskDelay(BLNK_MS / portTICK_PERIOD_MS);
  }
}

void Incoming_Packet_Helper(byte *buf){

  if (packetBuffer[1] == LIGHT_UPDATE_PACKET)
    {
      Serial.print("LIGHT_UPDATE_PACKET received from LightSwarm #");
      Serial.println(packetBuffer[2]);
      setAndReturnMySwarmIndex(packetBuffer[2]);

      Serial.print("LS Packet Recieved from #");
      Serial.print(packetBuffer[2]);
      Serial.print(" SwarmState:");
      if (packetBuffer[3] == 0)
        Serial.print("SLAVE");
      else
        Serial.print("MASTER");

      Serial.print(" CC:");
      Serial.print(packetBuffer[5] * 256 + packetBuffer[6]);
      Serial.print(" RC:");
      Serial.print(packetBuffer[7] * 256 + packetBuffer[8]);
      Serial.print(" GC:");
      Serial.print(packetBuffer[9] * 256 + packetBuffer[10]);
      Serial.print(" BC:");
      Serial.print(packetBuffer[11] * 256 + packetBuffer[12]);
      Serial.print(" Version=");
      Serial.println(packetBuffer[4]);

      // record the incoming clear color
      swarmClear[setAndReturnMySwarmIndex(packetBuffer[2])] = packetBuffer[5] * 256 + packetBuffer[6];
      swarmVersion[setAndReturnMySwarmIndex(packetBuffer[2])] = packetBuffer[4];
      swarmState[setAndReturnMySwarmIndex(packetBuffer[2])] = packetBuffer[3];
      swarmTimeStamp[setAndReturnMySwarmIndex(packetBuffer[2])] = millis();
    }

  if (packetBuffer[1] == RESET_SWARM_PACKET)
  {
    Serial.println(">>>>>>>>>>>>>RESET_SWARM_PACKETPacket Recieved<<<<<<<<<<<<<<<\n");
    masterState = false;
    Serial.println("Reset Swarm:  Resetting Swarm!! (and everybody else!)");
    vTaskSuspend(xHandle_Task_LED_Blink);
    digitalWrite(LED_BUILTIN, LOW);
  }
/*
  if (packetBuffer[1] == RESET_ME_PACKET)
  {
    Serial.println(">>>>>>>>>RESET_ME_PACKET Packet Recieved");
    if (packetBuffer[2] == swarmAddresses[mySwarmID])
    {
      masterState = false;
      Serial.println("Reset Me:  I just BECAME Master");
      digitalWrite(LED_BUILTIN, LOW);
    }
    else
    {
      Serial.print("Wanted #");
      Serial.print(packetBuffer[2]);
      Serial.println(" Not me - reset ignored");
    }
  }
  */
  /*
  if (packetBuffer[1] ==  DEFINE_SERVER_LOGGER_PACKET)
  {
    Serial.println(">>>>>>>>>DEFINE_SERVER_LOGGER_PACKET Packet Recieved");
    serverAddress = IPAddress(packetBuffer[4], packetBuffer[5], packetBuffer[6], packetBuffer[7]);
    Serial.print("Server address received: ");
    Serial.println(serverAddress);
  }
*/
/*
  if (packetBuffer[1] ==  BLINK_BRIGHT_LED)
  {
    Serial.println(">>>>>>>>>BLINK_BRIGHT_LED Packet Recieved");
    if (packetBuffer[2] == swarmAddresses[mySwarmID])
    {
      vTaskDelay((packetBuffer[4] * 100) / portTICK_PERIOD_MS);
    }
    else
    {
      Serial.print("Wanted #");
      Serial.print(packetBuffer[2]);
      Serial.println(" Not me - reset ignored");
    }
  }
*/
  Serial.print("MasterStatus:");
  if (masterState == true)
  {
    Serial.print("MASTER");
  }
  else
  {
    Serial.print("SLAVE");
  }
  Serial.print("/cc=");
  Serial.print(clearColor);
  Serial.print("/KS:");
  Serial.println(serverAddress);
  
  Serial.println("--------");
  int i;
  for (i = 0; i < SWARMSIZE; i++)
  {
    Serial.print("swarmAddress[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(swarmAddresses[i]); 
  }
  Serial.println("--------");
}

int Photo_Sns(void){
  int val = 0u;

  val = analogRead(PHOTO_SNS_PIN);
  val = MovAvg(val, &PHOTO_READINGS[0]);/*Moving average*/
  data_cnt+=1;
  sprintf(photo_sns_buffer, "%d", val);  /*convert int → text*/
  Serial.print("Photo Sns Read=");
  Serial.print(photo_sns_buffer);
  Serial.println(" ");
  return val;
}

uint16_t MovAvg(uint16_t input, uint16_t *buffer){
  /*Buffer with 5 elements*/
  float avg = 0.0f;

  buffer[4] = buffer[3];
  buffer[3] = buffer[2];
  buffer[2] = buffer[1];
  buffer[1] = buffer[0]; /*pop out old element and push new*/
  buffer[0] = input;

  avg = (buffer[0]+buffer[1]+buffer[2]+buffer[3]+buffer[4])/5;

  return avg;
}

void Task_Monitor(void *pv) {
  for (;;) {
    Serial.print("[Stack] UDP send free min: ");
    Serial.println(uxTaskGetStackHighWaterMark(xHandle_Task_UDP_Send));
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}
