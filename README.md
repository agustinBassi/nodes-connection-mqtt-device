# Nodes Connection MQTT device

Author: Agustin Bassi - 2020

## 
## Table of Contents

* [Intro](#intro)
* [Install dependencies](#install-dependencies)
* [Run Nodes Connection project](#run-nodes-connection-project)
* [Run MQTT Client](#run-mqtt-client)
* [Usage info](#usage-info)
* [Want to help?](#want-to-help-?)
* [License](#license)

## Intro

This project consists in a embedded MQTT Client that connects to [Nodes Connection](https://github.com/agustinBassi/nodes-connection) project, in order to visualize physical variables in a dashboard.

This project based in Arduino framework using PlatformIO tool. The original board target is ESP32 but it can be easily modified to work with ESP8266 or similar, configuring the `platformio.ini` file in the root folder of the project.

## Install dependencies

The project need the next dependencies:

* [PlatformIO](https://platformio.org/): a full CLI tool to manage libraries, platforms, frameworks and projects for embedded systems. It handles all task needed in background with a easy to use command line interface. Even, it can be installed as an extension for many IDEs, for example Visual Studio Code.
* [Visual Studio Code](https://code.visualstudio.com/) (optional but nice to hace): a popular IDE that can be used to program many languages. It has an extension for PlatformIO. So, if Visual Studio code is installed, the convenient way to install PlatformIO is by adding it as an extension instead of install them separately. 

## Run Nodes Connection project

As described above, this firmware works together with [Nodes Connection](https://github.com/agustinBassi/nodes-connection) project, so, to test it, this project must be running correctly. All the instructions to run it are in its README file.

Once Nodes Connection is running correctly, proceed to run the `MQTT Device` client.

## Run MQTT Client

Begin by downloading this project with the command below.

```sh
git clone https://github.com/agustinBassi/nodes-connection-mqtt-device.git
cd nodes-connection-mqtt-device/
```

Configure WiFi settings accordingly modifying the lines 58-59 of file `src/main.cpp`.

```c
const char* WIFI_SSID = "WIFI_SSID";
const char* WIFI_PASS = "WIFI_PASS";
```

Configure `Nodes Connection MQTT Broker` settings accordingly modifying the lines 61-64 of file `src/main.cpp`.

```c
const String MQTT_SERVER         = "192.168.1.1";
const int    MQTT_PORT           = 1883;
const String MQTT_USER           = "";
const String MQTT_PASS           = "";
```

Compile the firmware, download it to the board and open the serial monitor, all in one, with the next command.

```sh
pio run -t upload && pio device monitor
```

When program starts an output like this must be shown.

```sh
Welcome to Nodes Connection MQTT device!

Connecting to WIFI_SSID...
WiFi connected
IP address: 192.168.1.37
Attempting MQTT connection...connected
Subscribed to topic: device/config/esp32-001
Sending MQTT Topic-Payload: device/up/esp32-001 -> up
Sending MQTT Topic-Payload: device/status/esp32-001 -> {"temperature":"25","humidity":"50"}
```

In the Nodes Connection dashboard, the values 25 & 50 should be shown in the widgets.

## Usage info

By pressing the ONBOARD_BUTTON, the device will change the values of Humidity and Temperature in random fashion. When it occurs, a message saying `Updating the sensor data values` should be shown in the serial terminal. The new values should be shown in the Nodes Connection widget as well.

To change the publish temperature & humidity values time, in the `Nodes Connection Dashboard` change the slider in the `Device Control` section. A message similar to `Publish time will change to 5000 ms` should be shown in the serial terminal.

Each time the device publish a new status topic to the broker, the ONBOARD_LED will blink, in order to testify that a new topic were published.

The publish & subscribe function use `ArduinoJSON` library to parse JSON and create and serialize it. This library are extremly well documented and has a lot of examples and community support.

## Want to help?

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

If you find it useful please helpme following my Github user and give to this project a Star. This will animate me to continue contribuiting with the great open source community.

## License

This project is licensed under the GPLV3 License.
