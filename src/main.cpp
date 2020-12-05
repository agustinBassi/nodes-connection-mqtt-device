/*==================[ file header ]==========================================*/
// File:    main.cpp
// Author:  Agustin Bassi (https://github.com/agustinBassi)
// Licence: GPLV3+
// Version: 1.0.0
// Date:    December 2020
/*==================[inclusions]=============================================*/

#include "secrets.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"

/*==================[macros and definitions]=================================*/

// APPLICATION SETTINGS
#define LED_ONBOARD                 2
#define BUTTON_ONBOARD              0
#define INIT_DELAY                  3000
#define LOOP_DELAY                  1000
#define SERIAL_BAURDATE             115200
#define WIFI_CONNECTION_DELAY       500
#define MQTT_RETRY_CONNECTION_DELAY 100
#define BLINK_TIME                  100
#define DEFAULT_PUBLISH_TIME        5000
#define MIN_PUBLISH_TIME            1000
#define MAX_PUBLISH_TIME            30000
#define DEFAULT_TEMPERATURE_VALUE   25
#define DEFAULT_HUMIDITY_VALUE      50
#define MQTT_BUFFER_SIZE            256
// MQTT TOPICS
#define AWS_IOT_PUBLISH_TOPIC       "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC     "esp32/sub"

/*==================[internal data declaration]==============================*/

WiFiClientSecure WiFiSecureClient = WiFiClientSecure();
MQTTClient MqttClient = MQTTClient(MQTT_BUFFER_SIZE);

/*==================[internal functions declaration]=========================*/

void Gpio_BlinkLed              (uint8_t led, uint32_t milliseconds);
void Wifi_EstablishConnection   (void);
void Wifi_SetSecureCertificates (void);
void Mqtt_ConnectToBroker       (void);
void Mqtt_PublishTopic          (const char topic[], const char payload[]);
void Mqtt_SubscribeCallback     (String &topic, String &payload);
void App_Init                   (void);
void App_PublishStatus          (void);
void App_Loop                   (void);

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

void Gpio_BlinkLed(uint8_t led, uint32_t milliseconds){
    // Blink on board led when topic is sended
    digitalWrite(led, true);
    delay(milliseconds);
    digitalWrite(led, false);
}

void Wifi_EstablishConnection(){
    // Print network SSID
    Serial.println();
    Serial.print("Connecting to ");
    Serial.print(WIFI_SSID);
    // Try to connect
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    // Wait until connection is established
    while (WiFi.status() != WL_CONNECTED) {
        delay(WIFI_CONNECTION_DELAY);
        Serial.print(".");
    }
    // Report IP address
    Serial.println("\n\rWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void Wifi_SetSecureCertificates(){
    // Configure WiFiClientSecure to use the AWS IoT device credentials
    WiFiSecureClient.setCACert(AWS_CERT_CA);
    WiFiSecureClient.setCertificate(AWS_CERT_CRT);
    WiFiSecureClient.setPrivateKey(AWS_CERT_PRIVATE);
}

void Mqtt_ConnectToBroker(){
    // Connect to the MQTT broker on the AWS endpoint we defined earlier
    MqttClient.begin(AWS_IOT_ENDPOINT, 8883, WiFiSecureClient);
    // Create a message handler
    MqttClient.onMessage(Mqtt_SubscribeCallback);
    Serial.print("Attempting MQTT connection...");
    while (!MqttClient.connect(THINGNAME)){
        Serial.print(".");
        Gpio_BlinkLed(LED_ONBOARD, 100);
        delay(MQTT_RETRY_CONNECTION_DELAY);
    }
    if (!MqttClient.connected()){
        Serial.println("\n\rAWS IoT Timeout!");
        return;
    }
    // Subscribe to topic
    MqttClient.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
    Serial.print("\n\rSubscribed to topic: ");
    Serial.println(AWS_IOT_SUBSCRIBE_TOPIC);
    Serial.println("Correctly connected to Broker!");
}

void Mqtt_PublishTopic(const char topic[], const char payload[]){
    // Print in console the topic-payload that will be sent
    Serial.print("Sending MQTT Topic-Payload: ");
    Serial.print(topic);
    Serial.print(" -> ");
    Serial.println(payload);
    // Publish message
    MqttClient.publish(topic, payload);
}

void Mqtt_SubscribeCallback(String &topic, String &payload){
    Serial.println("[Mqtt_SubscribeCallback] " + topic + " -> " + payload);
}

void App_Init(){
    // wait a moment before start
    delay(INIT_DELAY);
    // Configure serial port at 115200 baudrios
    Serial.begin(SERIAL_BAURDATE);
    // Configure pins of buttons and leds
    pinMode(LED_ONBOARD, OUTPUT);
    pinMode(BUTTON_ONBOARD, INPUT);
    // print to console Init message
    Serial.println("Welcome to AWS Connection MQTT device!");
    // Connect to WiFi
    Wifi_EstablishConnection();
    // Configure WiFiClientSecure to use the AWS IoT device credentials
    Wifi_SetSecureCertificates();
    // Establish a connection to the broker
    Mqtt_ConnectToBroker();
    // Leave built in led on
    digitalWrite(LED_ONBOARD, false);
}

void App_PublishStatus(){
    StaticJsonDocument<200> doc;
    doc["time"] = millis();
    doc["sensor_a0"] = random(0, 255);
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer);
    Mqtt_PublishTopic(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void App_Loop(){
    App_PublishStatus();
    MqttClient.loop();
    delay(LOOP_DELAY);
}
    
/*==================[external functions definition]==========================*/

void setup(){
    App_Init();
}

void loop(){
    App_Loop();
}

/*==================[end of file]============================================*/