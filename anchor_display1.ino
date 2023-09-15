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

/*

For ESP32 UWB Pro with Display

*/

#include <SPI.h>
#include "DW1000Ranging.h"


#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

char ANCHOR_ADD[18];

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23

#define UWB_RST 27 // reset pin
#define UWB_IRQ 34 // irq pin
#define UWB_SS 21  // spi select pin

#define I2C_SDA 4
#define I2C_SCL 5

Adafruit_SSD1306 display(128, 64, &Wire, -1);

char* ssid = "TP-Link_E1EF";
const char* password = "golfcdf76r";
const char* mqtt_server = "192.168.0.107";
const int mqtt_port = 1883;
const char* mqtt_topic = "Distance";

uint16_t Adelay = 16550;

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
    Serial.begin(115200);
    MAC_Address();
    
    Wire.begin(I2C_SDA, I2C_SCL);
    delay(1000);
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    { // Address 0x3C for 128x32
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }
    display.clearDisplay();

    logoshow();

    // init the configuration
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    DW1000Ranging.initCommunication(UWB_RST, UWB_SS, UWB_IRQ); // Reset, CS, IRQ pin
    
    DW1000Ranging.attachNewRange(newRange);
    DW1000Ranging.attachBlinkDevice(newBlink);
    DW1000Ranging.attachInactiveDevice(inactiveDevice);

    DW1000.setAntennaDelay(Adelay);

    DW1000Ranging.startAsAnchor(ANCHOR_ADD, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
        WiFi.begin(ssid , password);
    while (WiFi.status() != WL_CONNECTED){
      delay(1000);
      Serial.println("connecting to wifi");
    }
    Serial.println("connected to wifi");

    
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

void loop()
{
    DW1000Ranging.loop();
    
    
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
    float Data = DW1000Ranging.getDistantDevice()->getRange();
    String payload = String(Data);
    client.publish(mqtt_topic, payload.c_str());
    delay(2000);
}

void newBlink(DW1000Device *device)
{
   
     Serial.print("blink; 1 device added ! -> ");
     Serial.print(" short:");
     Serial.println(device->getShortAddress(), HEX);
    
}

void inactiveDevice(DW1000Device *device)
{
  
    Serial.print("delete inactive device: ");
    Serial.println(device->getShortAddress(), HEX);
  
}

void logoshow(void)
{
    display.clearDisplay();

    display.setTextSize(2);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);             // Start at top-left corner
    display.println(F("Makerfabs"));
    display.println(F("UWB Anchor"));

    display.setTextSize(1);
    display.setCursor(0, 40); // Start at top-left corner
    display.println(ANCHOR_ADD);
    display.display();
}



void MAC_Address()
{
  delay(1000);
  uint8_t mac[6];
  WiFi.macAddress(mac);

  // Format and store the MAC address in the ANCHOR_ADD variable
  sprintf(ANCHOR_ADD, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  Serial.print("MAC Address: ");
  Serial.println(ANCHOR_ADD);
}
