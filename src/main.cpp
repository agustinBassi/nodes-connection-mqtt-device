/*==================[ file header ]==========================================*/
// File:    main.cpp
// Author:  Agustin Bassi (https://github.com/agustinBassi)
// Licence: GPLV3+
// Version: 1.0.0
// Date:    November 2020
/*==================[inclusions]=============================================*/

#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>

/*==================[macros and definitions]=================================*/

// APPLICATION SETTINGS
#define LED_ONBOARD                 2
#define BUTTON_ONBOARD              0
#define INIT_DELAY                  3000
#define SERIAL_BAURDATE             115200
#define WIFI_CONNECTION_DELAY       500
#define MQTT_RETRY_CONNECTION_DELAY 5000
#define BLINK_TIME                  100
#define DEFAULT_PUBLISH_TIME        5000
#define MIN_PUBLISH_TIME            1000
#define MAX_PUBLISH_TIME            30000
#define DEFAULT_TEMPERATURE_VALUE   25
#define DEFAULT_HUMIDITY_VALUE      50

/*==================[internal data declaration]==============================*/

// Client to establish WiFi connection
WiFiClient WifiClient;
// Client MQTT, needs a WiFi connection
PubSubClient MqttClient(WifiClient);
// Variable to change report time
static uint32_t PublishTime = DEFAULT_PUBLISH_TIME;
// Variables to store sensor reads values
static uint16_t TemperatureValue = DEFAULT_TEMPERATURE_VALUE;
static uint16_t HumidityValue = DEFAULT_HUMIDITY_VALUE;

/*==================[internal functions declaration]=========================*/

void Gpio_BlinkLed           (uint8_t led, uint32_t milliseconds);
void Wifi_EstablishConnection(void);
void Mqtt_ConnectToBroker    (void);
void Mqtt_PublishTopic       (String topic, String payload);
void Mqtt_SubscribeCallback  (char* topic, byte* payload, unsigned int length);
void App_Init                (void);
void App_PublishStatus       (void);
void App_UpdateSensorData    (void);
void App_Loop                (void);

/*==================[internal data definition]===============================*/

// Device indentification
const String DEVICE_ID           = "esp32-001";
// Wifi settings
const String WIFI_SSID           = "Speedy-068C76";
const String WIFI_PASS           = "1842910270";
// Mqtt server settings
const String MQTT_SERVER         = "192.168.1.33";
const int    MQTT_PORT           = 1883;
const String MQTT_USER           = "";
const String MQTT_PASS           = "";
// Mqtt message settings
const String MQTT_TOPIC_INIT     = "device/up/" + DEVICE_ID;
const String MQTT_TOPIC_STATUS   = "device/status/" + DEVICE_ID;
const String MQTT_TOPIC_CONFIG   = "device/config/" + DEVICE_ID;

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
    WiFi.begin(WIFI_SSID.c_str(), WIFI_PASS.c_str());
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

void Mqtt_ConnectToBroker(){
    // Loop until we're reconnected
    while (!MqttClient.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (MqttClient.connect(DEVICE_ID.c_str())) {
            Serial.println("connected");
            // Subscribe to topic
            MqttClient.subscribe(MQTT_TOPIC_CONFIG.c_str());
            Serial.print("Subscribed to topic: ");
            Serial.println(MQTT_TOPIC_CONFIG.c_str());
            // Advice to broker that device is running correctly
            Mqtt_PublishTopic(MQTT_TOPIC_INIT, "up");
        } else {
            Serial.print("failed, rc = ");
            Serial.print(MqttClient.state());
            Serial.println(". Try again in MQTT_RETRY_CONNECTION_TIME ms.");
            // Wait 5 seconds before retrying
            delay(MQTT_RETRY_CONNECTION_DELAY);
        }
    }
}

void Mqtt_PublishTopic(String topic, String payload){
    // Print in console the topic-payload that will be sent
    Serial.print("Sending MQTT Topic-Payload: ");
    Serial.print(topic);
    Serial.print(" -> ");
    Serial.println(payload);
    // Publish message
    MqttClient.publish( topic.c_str(), payload.c_str(), true );
}

void Mqtt_SubscribeCallback(char* topic, byte* payload, unsigned int length){
    if (strcmp(topic, MQTT_TOPIC_CONFIG.c_str()) == 0){
        StaticJsonDocument<100> doc;
        // put null char to payload buffer
        payload[length] = '\0';
        // Test if parsing succeeds.
        DeserializationError error = deserializeJson(doc, payload);
        if (error) {
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.f_str());
            return;
        }
        // obtain the value from the received JSON
        uint32_t publishTime = doc["publish_secs"];
        // convert seconds to milliseconds
        publishTime *= 1000;
        // Check if value received is correct.
        if (publishTime >= MIN_PUBLISH_TIME && publishTime <= MAX_PUBLISH_TIME){
            Serial.print("Publish time will change to ");
            Serial.print(publishTime);
            Serial.println(" ms");
            // Change publish time
            PublishTime = publishTime;
        } else {
            Serial.println("Invalid publish time, must be between MIN_REPORT_TIME and MAX_REPORT_TIME ms.");
        }
    } else {
        Serial.println("Unknown topic received!");
    }
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
    Serial.println("Welcome to Nodes Connection MQTT device!");
    // Set MQTT Server
    MqttClient.setServer(MQTT_SERVER.c_str(), MQTT_PORT);
    // Configure a callback to receive topics
    MqttClient.setCallback(Mqtt_SubscribeCallback);
    // Connect to WiFi
    Wifi_EstablishConnection();
    // Leave built in led on
    digitalWrite(LED_ONBOARD, false);
}

void App_PublishStatus(void){
    StaticJsonDocument<100> doc;
    char sensorValueStr [6];
    char payloadBuffer [100];
    // Convert int TemperatureValue to str
    itoa(TemperatureValue, sensorValueStr, 10);
    doc["temperature"] = sensorValueStr;
    // Convert int TemperatureValue to str
    itoa(HumidityValue, sensorValueStr, 10);
    doc["humidity"] = sensorValueStr;
    // serialize JSON in order to send it as MQTT payload
    serializeJson(doc, payloadBuffer);
    Mqtt_PublishTopic(MQTT_TOPIC_STATUS, payloadBuffer);
    // Blink LED to testify the topic sent
    Gpio_BlinkLed(LED_ONBOARD, BLINK_TIME);
}

void App_UpdateSensorData(void){
    Serial.println("Updating the sensor data values");
    TemperatureValue = random(0, 50);
    HumidityValue = random(0, 100);
}

void App_Loop(){
    // Create a variable to publish topic
    static uint32_t tickCounter = 0;
    // Check if MQTT client is not connected to server.
    if (!MqttClient.connected()) {
        // Try to connect with MQTT Server.
        Mqtt_ConnectToBroker();
    }
    // Loop for incoming messages.
    MqttClient.loop();
    // Use this variable to unblock the loop
    if (++tickCounter >= PublishTime){
        // Reset counter
        tickCounter = 0;
        // send the current status to the broker MQTT
        App_PublishStatus();
    }
    // if button is pressed change the current fake sensor values
    if(!digitalRead(BUTTON_ONBOARD)){
        App_UpdateSensorData();
        delay(1000);
    }
    // delay 1 MS to start loop again
    delay(1);
}
    
/*==================[external functions definition]==========================*/

void setup(){
    App_Init();
}

void loop(){
    App_Loop();
}

/*==================[end of file]============================================*/
