/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "dht11.h"

const int LM35 = 0;//LM35 pin

dht11 DHT11;
#define DHT11PIN 5
#define BEEPPIN 15
#define LEDB 14
#define LEDR 12
#define LEDG 13

// Update these with values suitable for your network.

const char* ssid = "NETGEAR66";
const char* password = "strongbanana453";
const char* mqtt_server = "link.tlink.io";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[200];
char msg1[100];
int value = 0;
byte gbvalue = 0;
byte gbrgb = 0;

void setup_beep(){
  pinMode(BEEPPIN, OUTPUT);
}

void setup_led(){
  pinMode(LEDB, OUTPUT);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    //digitalWrite(6, HIGH);
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "8UWQ4C60QC69GOQMZXW";
    //clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),"rainfeng@163.com","1234,abcdEFG")) { //
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("8UWQ4C60QC69GOQM", "hello world");
      // ... and resubscribe
      client.subscribe("8XD8Z4QRPVPD8OWF"); //8XD8Z4QRPVPD8OWF
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  //setup_beep();
  setup_led();
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int iIndex = 0;
  if(Serial.available()){
    size_t len = Serial.available();
    //uint8_t sbuf[len];
    Serial.readBytes(msg1, len);
    msg1[len] = '\0';

    if( len > 0 )
    {
          snprintf (msg, 75, "{\"sensorDatas\":[{\"sensorsID\":200017105,\"value\":\"%s\"}]}", msg1); 
          Serial.println(msg);
        client.publish("8XD8Z4QRPVPD8OWF", msg);   
    }
    else
    {
       //Serial.println("serial no data !!!");
    }
  }
  
  long now = millis();
  
  if (now - lastMsg > 2000) {
    //snprintf( msg, 30, " now=%d, lastMsg=%d", now, lastMsg );
    //Serial.println(msg); 
    lastMsg = now;

    gbrgb = gbrgb + 1;
    if( gbrgb > 3 )
    {
      gbrgb = 0;  
    }

    if( gbrgb == 0 )
    {
      digitalWrite( LEDB, HIGH );
      digitalWrite( LEDR, LOW );
      digitalWrite( LEDG, LOW );     
    }
    else if( gbrgb == 1 )
    {
      digitalWrite( LEDB, LOW );
      digitalWrite( LEDR, HIGH );
      digitalWrite( LEDG, LOW );
    }
    else if( gbrgb == 2 )
    {
      digitalWrite( LEDB, LOW );
      digitalWrite( LEDR, LOW );
      digitalWrite( LEDG, HIGH );      
    }
    else
    {
      digitalWrite( LEDB, HIGH );
      digitalWrite( LEDR, HIGH );
      digitalWrite( LEDG, HIGH );      
    }

    if( gbvalue == 0 )
    {
      //digitalWrite(32, HIGH);
      //digitalWrite(35,HIGH);

      //digitalWrite( LEDB, HIGH );
      //digitalWrite( LEDR, LOW );
      //digitalWrite( LEDG, HIGH );

      gbvalue=1;
    }
     else
     {
      //digitalWrite(32,LOW);
      //digitalWrite(35,LOW);

      //digitalWrite( LEDB, LOW );
      //digitalWrite( LEDR, HIGH );
      //digitalWrite( LEDG, LOW );

      gbvalue=0;
     }

     //! 读取温度和湿度传感器
  int chk = DHT11.read(DHT11PIN);
  
    //Serial.print("Read sensor: ");
    switch (chk)
    {
      case DHTLIB_OK:
        //Serial.println("OK");
        break;
      case DHTLIB_ERROR_CHECKSUM:
        Serial.println("Checksum error");
        break;
      case DHTLIB_ERROR_TIMEOUT:
        Serial.println("Time out error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
    //Serial.print("Humidity (%): ");
    //Serial.println((float)DHT11.humidity, 2);
  
    //Serial.print("Temperature (°C): ");
    //Serial.println((float)DHT11.temperature, 2);     


    //200001787
    
    //snprintf (msg, 180, "{\"sensorDatas\":[{\"sensorsID\":200016327,\"value\":\"%ld\"},{\"sensorsID\":200016440,\"value\":\"%ld\"}]}", DHT11.temperature,DHT11.humidity);
    //snprintf (msg, 75, "{\"sensorDatas\":[{\"sensorsID\":200016327,\"value\":\"%ld\"}]}", value);
    snprintf (msg, 180, "{\"sensorDatas\":[{\"sensorsID\":200016440,\"value\":\"%ld\"},{\"sensorsID\":200016327,\"value\":\"%ld\"}]}", DHT11.temperature,DHT11.humidity);
    //Serial.print("Publish message: ");
    //Serial.println(msg);

    if( gbvalue == 1 )
    {
      snprintf (msg, 75, "{\"sensorDatas\":[{\"sensorsID\":200016327,\"value\":\"%ld\"}]}", DHT11.temperature); 
      client.publish("8UWQ4C60QC69GOQM", msg);
    }
    else
    {
      snprintf (msg, 75, "{\"sensorDatas\":[{\"sensorsID\":200016441,\"value\":\"%ld\"}]}", DHT11.humidity); 
      client.publish("2BPUB77AE336611B", msg);   
    }
  }
}
