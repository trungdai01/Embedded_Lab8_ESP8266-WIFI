#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// Wifi name
#define WLAN_SSID "netfpga"
// Wifi password
#define WLAN_PASS "ktmt201_c5"

// setup Adafruit
#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883
// fill your username
#define AIO_USERNAME "trungdai"
// fill your key
#define AIO_KEY "aio_usXm74IS0AlU4ro0TdIPaxPtVIIb"

// setup MQTT
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// setup publish
Adafruit_MQTT_Publish light_pub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/led");
Adafruit_MQTT_Publish temp_pub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temp");

// setup subcribe
Adafruit_MQTT_Subscribe light_sub = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/led", MQTT_QOS_1);
Adafruit_MQTT_Subscribe temp_sub = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/temp", MQTT_QOS_1);

int led_counter = 0;
int led_status = HIGH;
float temp = 0;
int tempPubCount = 0;

void lightcallback(char *value, uint16_t len)
{
  if (value[0] == '0')
    Serial.print("Ada_a\n");
  if (value[0] == '1')
    Serial.print("Ada_A\n");
}

void setup()
{
  // put your setup code here, to run once:
  // set pin 2,5 as OUTPUT
  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);
  // set busy pin HIGH
  digitalWrite(5, HIGH);

  Serial.begin(115200);

  // connect Wifi
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
  }

  // subscribe light feed
  light_sub.setCallback(lightcallback);
  mqtt.subscribe(&light_sub);
  mqtt.subscribe(&temp_sub);

  // connect MQTT
  while (mqtt.connect() != 0)
  {
    mqtt.disconnect();
    delay(100);
  }

  // finish setup, set busy pin LOW
  digitalWrite(5, LOW);
}

void loop()
{
  // put your main code here, to run repeatedly:

  // receive packet
  mqtt.processPackets(1);

  // read serial
  if (Serial.available())
  {
    String msg = Serial.readString();
    if (msg == "o")
      Serial.print("O");
    else if (msg == "a")
    {
      Serial.print("STM32_a\n");
      light_pub.publish(0);
    }
    else if (msg == "A")
    {
      Serial.print("STM32_A\n");
      light_pub.publish(1);
    }
    else
      temp = msg.toFloat();
      temp_pub.publish(temp);
  }

  led_counter++;
  // tempPubCount++;
  if (led_counter == 100)
  {
    // every 1s
    led_counter = 0;
    // toggle LED
    if (led_status == HIGH)
      led_status = LOW;
    else
      led_status = HIGH;

    digitalWrite(2, led_status);
  }
  // if (tempPubCount == 1000)
  // {
  //   tempPubCount = 0;
  //   temp_pub.publish(temp);
  // }
  delay(10);
}
