# AWS Connection MQTT device

Author: Agustin Bassi - 2020

## 
## Table of Contents

* [Intro](#intro)
* [Install dependencies](#install-dependencies)
* [Confiure AWS Platform](#configure-aws-platform)
* [Run MQTT Client](#run-mqtt-client)
* [Usage info](#usage-info)
* [Want to help?](#want-to-help-?)
* [License](#license)

## Intro

This project consists in a embedded MQTT Client that connects to [AWS IoT Core](https://aws.amazon.com/iot-core/) platform, to send and receive MQTT topics from AWS suite.

This project based in Arduino framework using PlatformIO tool. The original board target is ESP32 but it can be easily modified to work with ESP8266 or similar, configuring the `platformio.ini` file in the root folder of the project.

## Install dependencies

The project need the next dependencies:

* [PlatformIO](https://platformio.org/): a full CLI tool to manage libraries, platforms, frameworks and projects for embedded systems. It handles all task needed in background with a easy to use command line interface. Even, it can be installed as an extension for many IDEs, for example Visual Studio Code.
* [Visual Studio Code](https://code.visualstudio.com/) (optional but nice to hace): a popular IDE that can be used to program many languages. It has an extension for PlatformIO. So, if Visual Studio code is installed, the convenient way to install PlatformIO is by adding it as an extension instead of install them separately. 

## Configure AWS Platform

In order to connect to AWS IoT Core some steps must be done:

* Create account (can be Free Tier).
* Register a Thing into AWS IoT Core.
* Create a policy to enable Things to connect to AWS IoT Core, publish and subscribe to MQTT topics.
* Associate the policy created with the Thing.
* Deploy an application from AWS Serverless Application Repository to connect a physical device to the AWS cloud (optional).

> **_NOTE:_**: All details to connect to AWS, create an account, register a Thing, configure a policy and instance an application from AWS SAR can be found in [helloiot.net](https://helloiot.net) site.

## Run MQTT Client

Begin by downloading this project with the command below.

```sh
git clone https://github.com/agustinBassi/aws-connection-mqtt-device.git
cd aws-connection-mqtt-device/
```

Open the file `src/secrets.h` and configure accordingly settings for WiFi, AWS_IOT_ENDPOINT and thing certificates.

> **_NOTE:_**: All information about get the required settings can be found at [helloiot.net](https://helloiot.net) site.

Compile the firmware, download it to the board and open the serial monitor, all in one, with the next command.

```sh
pio run -t upload && pio device monitor
```

When program starts an output like this must be shown.

```sh
Welcome to AWS Connection MQTT device!
Connecting to WIFI_SSID...
WiFi connected
IP address: 192.168.1.37
Attempting MQTT connection...connected
Subscribed to topic: esp32/sub
Correctly connected to Broker!
Sending MQTT Topic-Payload: esp32/pub -> {"time":8343,"sensor_a0":138}
Sending MQTT Topic-Payload: esp32/pub -> {"time":1035,"sensor_a0":72}
```

## Usage info

Once device is connected correctly to `AWS IoT Core`, go to `Test` section, and subscribe to all topics (#), the message published by device should be shown in the messages section.

To test communication from `AWS IoT Core` to device, go to `Publish` section, and set a topic `esp32/sub` with value `{"message": "Hello from AWS IoT console"}`. That message should be shown in the serial terminal of device like the next output.


```sh
...
[Mqtt_SubscribeCallback] esp32/sub -> {"message": "Hello from AWS IoT console"}
...
```

## Want to help?

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

If you find it useful please helpme following my Github user and give to this project a Star. This will animate me to continue contribuiting with the great open source community.

## License

This project is licensed under the GPLV3 License.
