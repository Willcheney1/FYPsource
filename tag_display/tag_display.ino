/*Will Cheney Tag Display*/


#include <DW1000.h>
#include <DW1000CompileOptions.h>
#include <DW1000Constants.h>
#include <DW1000Device.h>
#include <DW1000Mac.h>
#include <DW1000Ranging.h>
#include <DW1000Time.h>
#include <require_cpp11.h>



#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>

#include <PubSubClient.h>

#include <SPI.h>
#include <DFRobot_ADXL345.h>
#include <DFRobot_Heartrate.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_SPIDevice.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>




//Tag MAC Address Variable
char TAG_ADDR[18];


//Define Pins on Makerfabs PCB
//Define Pins for SPI and I2C
#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23

#define UWB_RST 27 // reset pin
#define UWB_IRQ 34 // irq pin
#define UWB_SS 21   // spi select pin

#define I2C_SDA 4
#define I2C_SCL 5

#define cs_pin 10

//// Heartrate defines
//const int SensorPin = 12;
//DFRobot_Heartrate heartrate(DIGITAL_MODE);

//Variables for UWB Distance 
struct Link
{
    uint16_t anchor_addr;
    float range;
    float dbm;
    struct Link *next;
};
 
struct Link *uwb_data;



//Assign Display Screen
Adafruit_SSD1306 display(128, 64, &Wire, -1);

//Assign unique IDs to each Accelerometer
//0x53 for SDO connected to ground
//0x1D for SDO connected to VCC
Adafruit_ADXL345_Unified accel1 = Adafruit_ADXL345_Unified(0x53); // Sensor with address 0x53
Adafruit_ADXL345_Unified accel2 = Adafruit_ADXL345_Unified(0x1D); // Sensor with address 0x1D
DFRobot_ADXL345_I2C ADXL1(&Wire,0x53);
DFRobot_ADXL345_I2C ADXL2(&Wire,0x1D);

//WiFi Login Variables
//const char* ssid = "TP-Link_E1EF";
//const char* password = "golfcdf76r";
const char* ssid = "WillsiPhone";
const char* password = "will1234";

//MQTT Connection Variables
const char* mqtt_server = "172.20.10.2";
const int mqtt_port = 1883;

//Accelerometer Event Assignment
sensors_event_t prevEvent1, prevEvent2;

//Threshold Variables 
float Fallthreshold1 = 1.0; // Set your threshold for sensor 1
float Fallthreshold2 = 2.0; // Set your threshold for sensor 2
float Lightthreshold1;
float Lightthreshold2;
float Moderatethreshold1;
float Moderatethreshold2;
float Vigorousthreshold1;
float Vigorousthreshold2;
float Running_Threshold = 10;
float Walking_Threshold = 2.5;
float Still_Threshold = 0.5;

//Change in Accelerometer Values (Delta)
float deltaX1;
float deltaY1;
float deltaZ1;

float deltaX2;
float deltaY2;
float deltaZ2;

float deltaPitch1;
float deltaRoll1;
float deltaPitch2;
float deltaRoll2;

float InitialPitch1;
float InitialRoll1;
float InitialPitch2;
float InitialRoll2;

float CurrentRoll1;
float CurrentPitch1;
float CurrentRoll2;
float CurrentPitch2;


int count1 = 0; // Counter for sensor 1 events
int count2 = 0; // Counter for sensor 2 events

// Calculate sensor statuses
int Movement_Status1 = 0;
int Movement_Status2 = 0;

//Delay Replacement
unsigned long previousMillis = 0;
const unsigned long interval = 1000; // Interval in milliseconds

unsigned long startFallDetection = 0; // Variable to store the start time
unsigned long FallDetectionduration = 60000; // 60 seconds in milliseconds

WiFiClient espClient;
PubSubClient client(espClient);

//topics

const char* T_Event1_X = "ADXL1/Event1_X";
const char* T_Event1_Y = "ADXL1/Event1_Y";
const char* T_Event1_Z = "ADXL1/Event1_Z";

const char* T_Event2_X = "ADXL2/Event2_X";
const char* T_Event2_Y = "ADXL2/Event2_Y";
const char* T_Event2_Z = "ADXL2/Event2_Z";

const char* T_Fall_Detected = "Fall_Detected";
const char* T_FD_Tag_MAC_Address = "Fall_Detected/Tag_MAC_Address";
const char* T_FD_Fall_ID = "Fall_Detected/Fall_ID";
const char* T_FD_Orientation_ID = "Fall_Detected/Orientation_ID";

const char* T_Heartrate = "Heartrate";
const char* T_HR_Tag_MAC_Address = "Heartrate/Tag_MAC_Address";
const char* T_HR_Motion_ID = "Heartrate/Motion_ID";

const char* T_Tag_MAC_Address = "Tag_MAC_Address";
const char* T_Tag_Anchor_MAC_Address1 = "Tag_MAC_Address/Anchor_MAC_Address1";
const char* T_Tag_Anchor_MAC_Address2 = "Tag_MAC_Address/Anchor_MAC_Address2";
const char* T_TAG_Anchor1_Distance = "Tag_MAC_Address/Anchor_MAC_Address1/Distance";
const char* T_TAG_Anchor2_Distance = "Tag_MAC_Address/Anchor_MAC_Address2/Distance";

// Random Variables

int RandomFall;
int RandomHR;
int RandomDistance; 
int RandomMotionID;
int RandomFallID;
int RandomOrientationID;

float DistanceValue;
String Distance;
String AnchorMAC1;
long int runtime = 0;
int accval1[3];
int accval2[3];

void setup()
{
    Wire.begin(I2C_SDA, I2C_SCL);
    ADXL1.begin();
    ADXL1.powerOn();
    ADXL2.begin();
    ADXL2.powerOn();
    Serial.println("Wire Begin");
    
    Serial.begin(115200);
    //Serial.begin(9600);
    
    MAC_Address();
    
    Accelerometer_Start();
    Display_Start();
    logoshow();
    UWB_Initialise();

    Wifi_Connect();
    MQTT_Connect();
    InitialiseADXL();    
}



void loop()
{ 
    DW1000Ranging.loop();
    DW1000_Timer();
    Fall_Sampler();
    HR_Sampler();  
    Movement_Status();
    fall_detected(); 
}

void newRange()
{
    Serial.print("from: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
    Serial.print("\t Range: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getRange());
    Serial.print(" m");
    Serial.print("\t RX power: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getRXPower());
    Serial.println(" dBm");
    DistanceValue = DW1000Ranging.getDistantDevice() ->getRange();
    Distance = String(DistanceValue);
    AnchorMAC1 = String(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
    fresh_link(uwb_data, DW1000Ranging.getDistantDevice()->getShortAddress(), DW1000Ranging.getDistantDevice()->getRange(), DW1000Ranging.getDistantDevice()->getRXPower());
    // print_link(uwb_data);
}

void newDevice(DW1000Device *device)
{
    //Serial.print("ranging init; 1 device added ! -> ");
    //Serial.print(" short:");
    //Serial.println(device->getShortAddress(), HEX);

    add_link(uwb_data, device->getShortAddress());
}

void inactiveDevice(DW1000Device *device)
{
    //Serial.print("delete inactive device: ");
    //Serial.println(device->getShortAddress(), HEX);

    delete_link(uwb_data, device->getShortAddress());
}

// Data Link

struct Link *init_link()
{
#ifdef DEBUG
    //Serial.println("init_link");
#endif
    struct Link *p = (struct Link *)malloc(sizeof(struct Link));
    p->next = NULL;
    p->anchor_addr = 0;
    p->range = 0.0;

    return p;
}

void add_link(struct Link *p, uint16_t addr)
{
#ifdef DEBUG
    //Serial.println("add_link");
#endif
    struct Link *temp = p;
    // Find struct Link end
    while (temp->next != NULL)
    {
        temp = temp->next;
    }

    //Serial.println("add_link:find struct Link end");
    // Create a anchor
    struct Link *a = (struct Link *)malloc(sizeof(struct Link));
    a->anchor_addr = addr;
    a->range = 0.0;
    a->dbm = 0.0;
    a->next = NULL;

    // Add anchor to end of struct Link
    temp->next = a;

    return;
}

struct Link *find_link(struct Link *p, uint16_t addr)
{
#ifdef DEBUG
    //Serial.println("find_link");
#endif
    if (addr == 0)
    {
        //Serial.println("find_link:Input addr is 0");
        return NULL;
    }

    if (p->next == NULL)
    {
        //Serial.println("find_link:Link is empty");
        return NULL;
    }

    struct Link *temp = p;
    // Find target struct Link or struct Link end
    while (temp->next != NULL)
    {
        temp = temp->next;
        if (temp->anchor_addr == addr)
        {
            // Serial.println("find_link:Find addr");
            return temp;
        }
    }

    //Serial.println("find_link:Can't find addr");
    return NULL;
}

void fresh_link(struct Link *p, uint16_t addr, float range, float dbm)
{
#ifdef DEBUG
    //Serial.println("fresh_link");
#endif
    struct Link *temp = find_link(p, addr);
    if (temp != NULL)
    {

        temp->range = range;
        temp->dbm = dbm;
        return;
    }
    else
    {
       // Serial.println("fresh_link:Fresh fail");
        return;
    }
}

void print_link(struct Link *p)
{
#ifdef DEBUG
    //Serial.println("print_link");
#endif
    struct Link *temp = p;

    while (temp->next != NULL)
    {
        // Serial.println("Dev %d:%d m", temp->next->anchor_addr, temp->next->range);
        //Serial.println(temp->next->anchor_addr, HEX);
        //Serial.println(temp->next->range);
        //Serial.println(temp->next->dbm);
        temp = temp->next;
    }

    return;
}

void delete_link(struct Link *p, uint16_t addr)
{
#ifdef DEBUG
    //Serial.println("delete_link");
#endif
    if (addr == 0)
        return;

    struct Link *temp = p;
    while (temp->next != NULL)
    {
        if (temp->next->anchor_addr == addr)
        {
            struct Link *del = temp->next;
            temp->next = del->next;
            free(del);
            return;
        }
        temp = temp->next;
    }
    return;
}

// SSD1306

void logoshow(void)
{
    display.clearDisplay();

    display.setTextSize(2);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);             // Start at top-left corner
    display.println(F("Makerfabs"));

    display.setTextSize(1);
    display.setCursor(0, 20); // Start at top-left corner
    display.println(F("DW1000 DEMO"));
    display.display();
    delay(2000);
}

void display_uwb(struct Link *p)
{
    struct Link *temp = p;
    int row = 0;

    //display.clearDisplay();

    display.setTextColor(SSD1306_WHITE);

    if (temp->next == NULL)
    {
        display.setTextSize(2);
        display.setCursor(0, 0);
        //display.println("No Anchor");
        display.display();
        return;
    }

    while (temp->next != NULL)
    {
        temp = temp->next;

        // Serial.println("Dev %d:%d m", temp->next->anchor_addr, temp->next->range);
        Serial.println(temp->anchor_addr, HEX);
        Serial.println(temp->range);

        char c[30];

        // sprintf(c, "%X:%.1f m %.1f", temp->anchor_addr, temp->range, temp->dbm);
        // sprintf(c, "%X:%.1f m", temp->anchor_addr, temp->range);
        sprintf(c, "%.1f m", temp->range);
        display.setTextSize(2);
        display.setCursor(0, row++ * 32); // Start at top-left corner
        display.println(c);

        display.println("");

        sprintf(c, "%.2f dbm", temp->dbm);
        display.setTextSize(2);
        display.println(c);

        if (row >= 1)
        {
            break;
        }
    }
    delay(100);
    display.display();
    return;
}

void Wifi_Connect()
{
  //initialise ssid, password
  //WiFi.begin(ssid, password)
   WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED){
    }
    Serial.println("connected to wifi"); 
  
}


void MQTT_Connect()
{
  //initialise mqtt_server, mqtt_port, mqtt_topic
  //client.setServer(mqtt_server, mqtt_port)
  //if (client.connect("ESP32Client", "mqtt", "willcheney"){}
  client.setServer(mqtt_server, mqtt_port);
  while (!client.connected()){
    Serial.println("connecting to MQTT broker");
    if (client.connect("ESP32Client", "mqtt", "willcheney")){
      Serial.println("connected to mqtt broker");
    }
    else{
      Serial.println("failed mqtt");
      Serial.print(client.state());
      delay(5000);
    }
  }
  
}


void Fall_Sampler()
{
  //Get all raw Accelerometer data

  unsigned long currentMillis = millis();

  // Check if it's time to read accelerometer data
  if (currentMillis - previousMillis >= interval * 2)
  {
    previousMillis = currentMillis;

    //Read data from the first sensor
    sensors_event_t event1;
    accel1.getEvent(&event1);
    
    //Read data from the second sensor 
    sensors_event_t event2;
    accel2.getEvent(&event2);
  
    
    
    display.clearDisplay();
  
    deltaX1 = event1.acceleration.x - prevEvent1.acceleration.x;
    deltaY1 = event1.acceleration.y - prevEvent1.acceleration.y;
    deltaZ1 = event1.acceleration.z - prevEvent1.acceleration.z;
    
    deltaX2 = event2.acceleration.x - prevEvent2.acceleration.x;
    deltaY2 = event2.acceleration.y - prevEvent2.acceleration.y;
    deltaZ2 = event2.acceleration.z - prevEvent2.acceleration.z;
    
    // Calculate absolute values
    int absDeltaX1 = abs(deltaX1);
    int absDeltaY1 = abs(deltaY1);
    int absDeltaZ1 = abs(deltaZ1);
    
    int absDeltaX2 = abs(deltaX2);
    int absDeltaY2 = abs(deltaY2);
    int absDeltaZ2 = abs(deltaZ2);
    
    // Store the current readings as previous for the next iteration
    prevEvent1 = event1;
    prevEvent2 = event2;
    
//Publish both accelerometer X, Y, Z values to MQTT Server
    String event1x = String(event1.acceleration.x);
    client.publish(T_Event1_X, event1x.c_str());
    String event1y = String(event1.acceleration.y);
    client.publish(T_Event1_Y, event1y.c_str());
    String event1z = String(event1.acceleration.z);
    client.publish(T_Event1_Z, event1z.c_str());
    
    String event2x = String(event2.acceleration.x);
    client.publish(T_Event2_X, event2x.c_str());
    String event2y = String(event2.acceleration.y);
    client.publish(T_Event2_Y, event2y.c_str());
    String event2z = String(event2.acceleration.z);
    client.publish(T_Event2_Z, event2z.c_str());

    if (absDeltaX1 > Running_Threshold || absDeltaY1 > Running_Threshold || absDeltaZ1 > Running_Threshold || absDeltaX2 > Running_Threshold || absDeltaY2 > Running_Threshold || absDeltaZ2 > Running_Threshold) 
    {
      Movement_Status1 = 3;
    }
       
    // Use the sensor statuses in the switch statements
   
      
      /*Calculate the difference in accelerometer values
      deltaX1 = event1.acceleration.x - prevEvent1.acceleration.x;
      deltaY1 = event1.acceleration.y - prevEvent1.acceleration.y;
      deltaZ1 = event1.acceleration.z - prevEvent1.acceleration.z;
    
      deltaX2 = event2.acceleration.x - prevEvent2.acceleration.x;
      deltaY2 = event2.acceleration.y - prevEvent2.acceleration.y;
      deltaZ2 = event2.acceleration.z - prevEvent2.acceleration.z;
    
      //Store the current readings as previous for the next iteration
      prevEvent1 = event1;
      prevEvent2 = event2;
    
      //Check if the deltas exceed the thresholds
      if (abs(deltaX1) > threshold1 || abs(deltaY1) > threshold1 || abs(deltaZ1) > threshold1) 
      {
        Serial.println("Sensor 1 - Activity detected!");
      }
      if (abs(deltaX2) > threshold2 || abs(deltaY2) > threshold2 || abs(deltaZ2) > threshold2) 
      {
        Serial.println("Sensor 2 - Activity detected!");
      }*/
  }
}


void InitialiseADXL()
{
  //using DFRobot library get ADXL values and use them to determine the pitch and roll of the ADXLs.
  
  ADXL1.readAccel(accval1);
  ADXL2.readAccel(accval2);
  
  ADXL1.RPCalculate(accval1);
  Serial.print("Roll1:"); Serial.println( ADXL1.RP.roll );
  Serial.print("Pitch1:"); Serial.println( ADXL1.RP.pitch );
  InitialRoll1 = ADXL1.RP.roll;
  InitialPitch1 = ADXL1.RP.pitch;
  ADXL2.RPCalculate(accval2);
  Serial.print("Roll2:"); Serial.println( ADXL2.RP.roll );
  Serial.print("Pitch2:"); Serial.println( ADXL2.RP.pitch );
  InitialRoll1 = ADXL2.RP.roll;
  InitialPitch1 = ADXL2.RP.pitch;
}

void Movement_Status()
{
  //Function to determine the user's activity level from the ADXLs 
   switch (Movement_Status1) 
    {
      case 1:
        
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.println("Walking Detected");
        display.display();
      case 2:
        
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.println("Running Detected");
        display.display();
        break;
      case 3:
        
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.println("Fall Detected");
        display.display();
        break;
      case 4:
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.println("Still Detected");
        display.display();
        break;
    }
    
    switch (Movement_Status2) 
    {
      case 1:
        
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(0, 20);
        display.println("Walking Detected");
        display.display();
        break;
      case 2:
        
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(0, 20);
        display.println("Running Detected");
        display.display();
        break;
      case 3:
        
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(0, 20);
        display.println("Fall Detected");
        display.display();
        break;
      case 4:
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(0, 20);
        display.println("Still Detected");
        display.display();
        break;
    }
}

void (fall_detected)
{

  //Once a fall has occurred monitor the data received from ADXLs to determine orientation and activity level
  if (Movement_Status1 || Movement_Status2 == 3)
  {
    unsigned long currentTime = millis();

    if (startFallDetection == 0)
    {
      startFallDetection = currentTime;
    }
    
    unsigned long elapsedTime = currentTime - startFallDetection;

    if (elapsedTime < FallDetectionduration)
    {
      //real current roll and pitch and compare to initial roll and pitch
      ADXL1.readAccel(accval1);
      ADXL2.readAccel(accval2);
      
      ADXL1.RPCalculate(accval1);
      ADXL2.RPCalculate(accval2);
  
      CurrentRoll1 = ADXL1.RP.roll;
      CurrentPitch1 = ADXL1.RP.pitch;
      CurrentRoll2 = ADXL2.RP.roll;
      CurrentPitch2 = ADXL2.RP.pitch;
  
      deltaRoll1 = abs(currentRoll1 - initialRoll1);
      deltaRoll2 = abs(currentRoll2 - initialRoll2);
      deltaPitch1 = abs(currentPitch1 - initialPitch1);
      deltaPitch2 = abs(currentPitch2 - initialPitch2);
  
      if (deltaRoll1 > 10 || deltaRoll2 > 10 || deltaPitch1 > 10 || deltaPitch2 > 10)
      {
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(0, 30);
        display.println("Not Upright");
        display.display();
      }

      if (absDeltaX1 > Walking_Threshold || absDeltaY1 > Walking_Threshold || absDeltaZ1 > Walking_Threshold || absDeltaX2 > Walking_Threshold || absDeltaY2 > Walking_Threshold || absDeltaZ2 > Walking_Threshold) 
      {
        Movement_Status1 = 2;
      }
      else if (absDeltaX1 > Still_Threshold || absDeltaY1 > Still_Threshold || absDeltaZ1 > Still_Threshold || absDeltaX2 > Still_Threshold || absDeltaY2 > Still_Threshold || absDeltaZ2 > Still_Threshold) 
      {
        Movement_Status1 = 1;
      }
      else if (absDeltaX1 < Still_Threshold || absDeltaY1 < Still_Threshold || absDeltaZ1 < Still_Threshold || absDeltaX2 < Still_Threshold || absDeltaY2 < Still_Threshold || absDeltaZ2 < Still_Threshold) 
      {
        Movement_Status1 = 4;
      }
     
      
    }
    else
    {
      startFallDetection = 0;
    }
  }
}


void HR_Sampler()
{
  /*heartrate loop
  int rateValue;
  heartrate.getValue(SensorPin);
  rateValue = heartrate.getRate();
  if(rateValue){
    Serial.println(rateValue);
  }*/
}


void HR_Threshold()
{
  if (RandomHR > 500)
  {
    MQTT_upload();
  }
  else 
  RandomHR = 500;
}


void MQTT_upload()
{
  //called by Fall_detected()
  //call NewRange() which gets the address of the anchor and the distance to said anchor  
  //create variables for each value, convert to compatible data types and send to "Topic" which can be assigned any string topic name
  
 

  /*
  String HR = String(RandomHR);
  client.publish(T_Heartrate, HR.c_str());
  String esp32MAC = string(TAG_ADDR);
  client.publish(T_FD_Tag_MAC_Address, esp32MAC.c_str());
  client.publish(T_HR_Tag_MAC_Address, esp32MAC.c_str());
  String MotionID = String(RandomMotionID);
  client.publish(T_HR_Motion_ID, MotionID.c_str());
  String FallID = String(RandomFallID);
  client.publish(T_FD_Fall_ID, FallID.c_str());
  String OrientationID = String(RandomOrientationID);
  client.publish(T_FD_Orientation_ID, OrientationID.c_str());

  client.publish(T_Tag_MAC_Address,esp32MAC.c_str());
  client.publish(T_Tag_Anchor_MAC_Address1, AnchorMAC1.c_str());
  client.publish(T_TAG_Anchor1_Distance, Distance.c_str());
  */

  
}

void MAC_Address() 
{
  delay(1000); 
  uint8_t mac[6];
  WiFi.macAddress(mac);

  // Format and store the MAC address in the ANCHOR_ADD variable
  sprintf(TAG_ADDR, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  Serial.print("MAC Address: ");
  Serial.println(TAG_ADDR);
}


/*
void writeTo(int device, byte address, byte val) 
{
  Wire.beginTransmission(device); //start transmission to device
  Wire.write(address);        // send register address
  Wire.write(val);        // send value to write
  Wire.endTransmission(); //end transmission
}

//reads num bytes starting from address register on device in to buff array
void readFrom(int device, byte address, int num, byte buff[]) {
  Wire.beginTransmission(device); //start transmission to device
  Wire.write(address);        //sends address to read from
  Wire.endTransmission(); //end transmission

    Wire.beginTransmission(device); //start transmission to device
  Wire.requestFrom(device, num);    // request 6 bytes from device

  int i = 0;
  while(Wire.available())    //device may send less than requested (abnormal)
  {
    buff[i] = Wire.read(); // receive a byte
    i++;
  }
  Wire.endTransmission(); //end transmission
}

calculate the Roll&Pitch
void RP_calculate(){
  double x_Buff = float(x);
  double y_Buff = float(y);
  double z_Buff = float(z);
  roll = atan2(y_Buff , z_Buff) * 57.3;
  pitch = atan2((- x_Buff) , sqrt(y_Buff * y_Buff + z_Buff * z_Buff)) * 57.3;
}*/


void DW1000_Timer()
{
  if ((millis() - runtime) > 1000)
    {
        display_uwb(uwb_data);
        runtime = millis();
    }
}

void Accelerometer_Start()
{
  // Make sure both Accelerometers are connected
    if (!accel1.begin() || !accel2.begin()) 
    {
      Serial.println("Ooops, no ADXL345 detected!");
      while (1);
    }

    //Set Accelerometer Sensitivity
    accel1.setRange(ADXL345_RANGE_4_G);
    accel2.setRange(ADXL345_RANGE_16_G);
}

void Display_Start()
{
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    { // Address 0x3C for 128x32
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }
    display.clearDisplay();
}

void UWB_Initialise()
{
    // init the configuration
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    DW1000Ranging.initCommunication(UWB_RST, UWB_SS, UWB_IRQ); // Reset, CS, IRQ pin
    // define the sketch as anchor. It will be great to dynamically change the type of module
    DW1000Ranging.attachNewRange(newRange);
    DW1000Ranging.attachNewDevice(newDevice);
    DW1000Ranging.attachInactiveDevice(inactiveDevice);
    // Enable the filter to smooth the distance
    //DW1000Ranging.useRangeFilter(false);

    // we start the module as a tag
    DW1000Ranging.startAsTag(TAG_ADDR, DW1000.MODE_LONGDATA_RANGE_LOWPOWER);
    // DW1000Ranging.startAsTag(TAG_ADDR, DW1000.MODE_SHORTDATA_FAST_LOWPOWER);
    // DW1000Ranging.startAsTag(TAG_ADDR, DW1000.MODE_LONGDATA_FAST_LOWPOWER);
    // DW1000Ranging.startAsTag(TAG_ADDR, DW1000.MODE_SHORTDATA_FAST_ACCURACY);
    // DW1000Ranging.startAsTag(TAG_ADDR, DW1000.MODE_LONGDATA_FAST_ACCURACY);
    // DW1000Ranging.startAsTag(TAG_ADDR, DW1000.MODE_LONGDATA_RANGE_ACCURACY);


    
    uwb_data = init_link();
}
