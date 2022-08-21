/***************************************************
  Adafruit MQTT Library ESP8266 Example

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino

  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821


  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
//#include <NewPing.h>
/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "WIFI-NAME"
#define WLAN_PASS       "WIFI-PASSWORD"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "ADAFRUIT-USERNAME"
#define AIO_KEY         "ADAFRUIT-KEY"

#define TRIGGER D7
#define ECHO D6
/************ Global State (you don't need to change this!) ******************/
//NewPing ultrasonic(TRIGGER,ECHO,400);
int dist=0;
// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
/*
  Adafruit_MQTT_Publish temp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Temp");
*/
// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe forward = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/bruhh");
Adafruit_MQTT_Subscribe right = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/r2");
Adafruit_MQTT_Subscribe left = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/left");
Adafruit_MQTT_Subscribe backward = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/back1");
Adafruit_MQTT_Subscribe pump = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/pump");
/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).

#define m1f D0
#define m1b D1
#define m2f D2
#define m2b D3
#define pump_pin  D8
unsigned char stop_arr[4] = {0};
void motorforward();
void motorbackward();
void motorright();
void motorleft();
void motorstop();
void MQTT_connect();
void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println(F("Adafruit MQTT demo"));
  pinMode(m1f, OUTPUT);
  pinMode(m1b, OUTPUT);
  pinMode(m2f, OUTPUT);
  pinMode(m2b, OUTPUT);
  pinMode(pump_pin, OUTPUT);
  
  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&forward);
  mqtt.subscribe(&backward);
  mqtt.subscribe(&right);
  mqtt.subscribe(&left);
  mqtt.subscribe(&pump);
  
}



void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();
//dist=ultrasonic.ping_cm();
Serial.println(dist);
  // this is our 'wait for incoming subscription packets' busy subloop

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(500))) {
    if (subscription == &forward) {
      // Serial.print(F("Got: "));
      //Serial.println((char *)forward.lastread);
    }
  }
  Adafruit_MQTT_Subscribe *subscription2;
  while ((subscription2 = mqtt.readSubscription(500))) {
    if (subscription2 == &backward) {
      //Serial.print(F("Got: "));
      //Serial.println((char *)backward.lastread);
    }
  }
  Adafruit_MQTT_Subscribe *subscription3;
  while ((subscription3 = mqtt.readSubscription(500))) {
    if (subscription3 == &right) {
      //Serial.print(F("Got: "));
      //Serial.println((char *)right.lastread);
    }
  }
  
  Adafruit_MQTT_Subscribe *subscription4;
  while ((subscription4 = mqtt.readSubscription(500))) {
    if (subscription4 == &left) {
      //Serial.print(F("Got: "));
      //Serial.println((char *)left.lastread);
    }
  }
    Adafruit_MQTT_Subscribe *subscription5;
  while ((subscription5 = mqtt.readSubscription(500))) {
    if (subscription5 == &pump) {
      // Serial.print(F("Got: "));
      //Serial.println((char *)forward.lastread);
    }
  }
 if(strcmp( (char *)pump.lastread, "ON") == 0)
  {
    digitalWrite(pump_pin,0);
    delay(3000);
    digitalWrite(pump_pin,1);
  }
  else
  {
    digitalWrite(pump_pin,1);
   }
if (strcmp( (char *)forward.lastread, "ON") == 0/*&&dist>30&&dist!=0*/)
  {
    motorforward();
  }
  else if (strcmp( (char *)backward.lastread, "ON") == 0/*&&dist>30&&dist!=0*/)
  {
    motorbackward();
    
  }
  else if (strcmp( (char *)right.lastread, "ON") == 0/*&&dist>30&&dist!=0*/)
  {
    motorright();
    delay(1000);
    motorstop();
  }
  else if (strcmp( (char *)left.lastread, "ON") == 0/*&&dist>30&&dist!=0*/)
  {
    motorleft();
    delay(1000);
    motorstop();
  }
  else
  {
    motorstop();
  }
 // delay(500);
  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
    if(! mqtt.ping()) {
    mqtt.disconnect();
    }
  */
}

void motorforward()
{
  digitalWrite(m1f, 0);
  digitalWrite(m1b, 1);
  digitalWrite(m2f, 0);
  digitalWrite(m2b, 1);
}
void motorbackward()
{
  digitalWrite(m1f, 1);
  digitalWrite(m1b, 0);
  digitalWrite(m2f, 1);
  digitalWrite(m2b, 0);
}

void motorleft()
{
  digitalWrite(m1f, 0);
  digitalWrite(m1b, 1);
  digitalWrite(m2f, 1);
  digitalWrite(m2b, 0);
}

void motorright()
{
  digitalWrite(m1f, 1);
  digitalWrite(m1b, 0);
  digitalWrite(m2f, 0);
  digitalWrite(m2b, 1);
}
void motorstop() {
  digitalWrite(m1f, 1);
  digitalWrite(m1b, 1);
  digitalWrite(m2f, 1);
  digitalWrite(m2b, 1);

}
// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(500);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
