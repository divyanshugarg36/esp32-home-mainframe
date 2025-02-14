/**********************************************************************************
 *  TITLE: ESP RainMaker + IR + Manual Switch control 8 Relays using ESP32 AHT10 LDR (Real time feedback + no WiFi control)
 *  Click on the following links to learn more.
 *  YouTube Video: https://youtu.be/7knQaSuEgsU
 *  Related Blog : https://iotcircuithub.com/esp32-projects/
 *  by Tech StudyCell
 *  Preferences--> Aditional boards Manager URLs :
 *  http://arduino.esp8266.com/stable/package_esp8266com_index.json,https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
 *
 *  Download Board ESP32 (2.0.3): https://github.com/espressif/arduino-esp32

 **********************************************************************************/

#include "RMaker.h"
#include "WiFi.h"
#include "WiFiProv.h"
#include <IRremote.h>
#include <SimpleTimer.h>
extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>
#include "/Users/divyanshu/Development/Arduino/include/AHTSensor.hpp"
#include "/Users/divyanshu/Development/Arduino/Environment.hpp"

#define MQTT_HOST MQTT_HOST_IP
#define MQTT_PORT 1883

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

int strToInt(char *str)
{
    int res = 0;
    if (strcmp("ON", str) == 0)
    {
        res = 100;
    }
    return res;
}

float strToFloat(char *str)
{
    return atof(str);
}

bool strToBool(char *str)
{
    bool res = false;
    if (strcmp("ON", str) == 0)
    {
        res = true;
    }
    return res;
}

void MQ_Setup()
{
    mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
    wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onSubscribe(onMqttSubscribe);
    mqttClient.onUnsubscribe(onMqttUnsubscribe);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.onPublish(onMqttPublish);
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    // mqttClient.setCredentials("username", "password");
}

void connectToMqtt()
{
    Serial.println("Connecting to MQTT...");
    mqttClient.connect();
}

void onMqttConnect(bool sessionPresent)
{
    Serial.println("Connected to MQTT.");
    uint16_t packetIdSub = NULL;
    // MQTT Subscription to the status topics
    packetIdSub = mqttClient.subscribe("stat/home/POWER", 1);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
    Serial.println("Disconnected from MQTT.");
    if (WiFi.isConnected())
    {
        xTimerStart(mqttReconnectTimer, 0);
    }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos)
{
    Serial.println("Subscribe acknowledged.");
    Serial.print("  packetId: ");
    Serial.println(packetId);
    Serial.print("  qos: ");
    Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId)
{
    Serial.println("Unsubscribe acknowledged.");
    Serial.print("  packetId: ");
    Serial.println(packetId);
}

void connectToWifi()
{
    Serial.println("Connecting to Wi-Fi...");
}
void onMqttPublish(uint16_t packetId)
{
}

const char *service_name = "Garg_home";
const char *pop = "12345RS8";

// define the Chip Id
uint8_t espChipId = 1;

// define the Node Name
char nodeName[] = "MAIN_HALL";

// define the Device Names
char deviceName_1[] = "Switch1";
char deviceName_2[] = "Switch2";
char deviceName_3[] = "8266 Switch1";

// Update the HEX code of IR Remote buttons 0x<HEX CODE>
#define IR_Button_1 0x80BF49B6
#define IR_Button_2 0x80BFC936
#define IR_All_Off 0x80BF3BC4

// define the GPIO connected with Relays and switches
static uint8_t RelayPin1 = 23;
static uint8_t RelayPin2 = 22;

static uint8_t SwitchPin1 = 13;
static uint8_t SwitchPin2 = 12;

static uint8_t wifiLed = 2;
static uint8_t gpio_reset = 0;
static uint8_t IR_RECV_PIN = 35; // D35 (IR receiver pin)

/* Variable for reading pin status*/
// Relay State
bool toggleState_1 = LOW; // Define integer to remember the toggle state for relay 1
bool toggleState_2 = LOW; // Define integer to remember the toggle state for relay 2

// Switch State
bool SwitchState_1 = LOW;
bool SwitchState_2 = LOW;

IRrecv irrecv(IR_RECV_PIN);
decode_results results;

AHTSensor ahtSensor;
bool isAHTActive = false;

SimpleTimer Timer;

// The framework provides some standard device types like:-
// switch, lightbulb, fan, temperature sensor.
static Switch my_switch1(deviceName_1, &RelayPin1);
static Switch my_switch2(deviceName_2, &RelayPin2);
static Switch my_switch3(deviceName_3, NULL);

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
    Serial.println("Publish received.");
    Serial.print("  topic: ");
    Serial.println(topic);
    // Parse and act on status messages

    if (strcmp(topic, "stat/home/POWER") == 0)
    {
        Serial.println("OutsideLights Matched");
        char msg[50] = "";
        strncpy(msg, payload, len);
        msg[sizeof(payload)] = '\0';
        Serial.printf("msg: %s\n", msg);
        my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, strToBool(msg));
    }
}

static TemperatureSensor temperature("Temperature");
static TemperatureSensor humidity("Humidity");

void sysProvEvent(arduino_event_t *sys_event)
{
    switch (sys_event->event_id)
    {
    case ARDUINO_EVENT_PROV_START:
#if CONFIG_IDF_TARGET_ESP32
        Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
        printQR(service_name, pop, "ble");
#else
        Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on SoftAP\n", service_name, pop);
        printQR(service_name, pop, "softap");
#endif
        break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.printf("\nConnected to Wi-Fi!\n");
        digitalWrite(wifiLed, true);
        break;

    case SYSTEM_EVENT_STA_GOT_IP:
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.print(WiFi.localIP());
        connectToMqtt();
        break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
        Serial.println("WiFi lost connection");
        xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
        xTimerStart(wifiReconnectTimer, 0);
        break;
    }
}

void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx)
{
    const char *device_name = device->getDeviceName();
    const char *param_name = param->getParamName();

    if (strcmp(device_name, deviceName_1) == 0)
    {

        Serial.printf("Lightbulb = %s\n", val.val.b ? "true" : "false");

        if (strcmp(param_name, "Power") == 0)
        {
            Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
            toggleState_1 = val.val.b;
            (toggleState_1 == false) ? digitalWrite(RelayPin1, HIGH) : digitalWrite(RelayPin1, LOW);
            param->updateAndReport(val);
        }
    }
    else if (strcmp(device_name, deviceName_2) == 0)
    {

        Serial.printf("Switch value = %s\n", val.val.b ? "true" : "false");

        if (strcmp(param_name, "Power") == 0)
        {
            Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
            toggleState_2 = val.val.b;
            (toggleState_2 == false) ? digitalWrite(RelayPin2, HIGH) : digitalWrite(RelayPin2, LOW);
            param->updateAndReport(val);
        }
    }
    else if ((strcmp(param_name, "Power") == 0) && (strcmp(device_name, deviceName_3) == 0))
    {
        Serial.printf("Received value = %d for %s - %s\n", val.val.b, device_name, param_name);
        bool state = val.val.b;
        if (state)
        {
            mqttClient.publish("cmnd/home/POWER", 1, true, "1");
            Serial.println("Sent ON");
        }
        else
        {
            mqttClient.publish("cmnd/home/POWER", 1, true, "0");
            Serial.println("Sent OFF");
        }
        param->updateAndReport(val);
    }
}

void sendSensor()
{
    if (isAHTActive)
    {
        AHTData ahtData = ahtSensor.readData();
        Serial.print("Temperature: " + String(ahtData.temperature) + " degrees C");
        Serial.print("Humidity: " + String(ahtData.humidity) + "% rH");
        temperature.updateAndReportParam("Temperature", ahtData.temperature);
        humidity.updateAndReportParam("Humidity", ahtData.humidity);
    }
}

void manual_control()
{
    if (digitalRead(SwitchPin1) == LOW && SwitchState_1 == LOW)
    {
        digitalWrite(RelayPin1, LOW);
        toggleState_1 = 1;
        SwitchState_1 = HIGH;
        my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1);
        Serial.println("Switch-1 on");
    }
    if (digitalRead(SwitchPin1) == HIGH && SwitchState_1 == HIGH)
    {
        digitalWrite(RelayPin1, HIGH);
        toggleState_1 = 0;
        SwitchState_1 = LOW;
        my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1);
        Serial.println("Switch-1 off");
    }
    if (digitalRead(SwitchPin2) == LOW && SwitchState_2 == LOW)
    {
        digitalWrite(RelayPin2, LOW);
        toggleState_2 = 1;
        SwitchState_2 = HIGH;
        my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2);
        Serial.println("Switch-2 on");
    }
    if (digitalRead(SwitchPin2) == HIGH && SwitchState_2 == HIGH)
    {
        digitalWrite(RelayPin2, HIGH);
        toggleState_2 = 0;
        SwitchState_2 = LOW;
        my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2);
        Serial.println("Switch-2 off");
    }
}

void all_SwitchOff()
{
    toggleState_1 = 0;
    digitalWrite(RelayPin1, HIGH);
    my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1);
    delay(100);
    toggleState_2 = 0;
    digitalWrite(RelayPin2, HIGH);
    my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2);
    delay(100);
}

void ir_remote()
{

    if (irrecv.decode(&results))
    {
        switch (results.value)
        {
        case IR_Button_1:
            digitalWrite(RelayPin1, toggleState_1);
            toggleState_1 = !toggleState_1;
            my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1);
            delay(100);
            break;
        case IR_Button_2:
            digitalWrite(RelayPin2, toggleState_2);
            toggleState_2 = !toggleState_2;
            my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2);
            delay(100);
            break;
        case IR_All_Off:
            all_SwitchOff();
            break;
        default:
            break;
        }
        // Serial.println(results.value, HEX);
        irrecv.resume();
    }
}

void setup()
{
    Serial.begin(115200);

    // Set the Relays GPIOs as output mode
    pinMode(RelayPin1, OUTPUT);
    pinMode(RelayPin2, OUTPUT);
    pinMode(wifiLed, OUTPUT);

    // Configure the input GPIOs
    pinMode(SwitchPin1, INPUT_PULLUP);
    pinMode(SwitchPin2, INPUT_PULLUP);
    pinMode(gpio_reset, INPUT);

    // Write to the GPIOs the default state on booting
    digitalWrite(RelayPin1, !toggleState_1);
    digitalWrite(RelayPin2, !toggleState_2);
    digitalWrite(wifiLed, LOW);

    irrecv.enableIRIn(); // Enabling IR sensor

    isAHTActive = ahtSensor.initialize();

    Node my_node;
    my_node = RMaker.initNode(nodeName);

    // Standard switch device
    my_switch1.addCb(write_callback);
    my_switch2.addCb(write_callback);
    my_switch3.addCb(write_callback);

    // Add switch device to the node
    my_node.addDevice(my_switch1);
    my_node.addDevice(my_switch2);
    my_node.addDevice(my_switch3);

    my_node.addDevice(temperature);
    my_node.addDevice(humidity);

    Timer.setInterval(5000);

    RMaker.enableOTA(OTA_USING_PARAMS);
    // If you want to enable scheduling, set time zone for your region using setTimeZone().
    // The list of available values are provided here https://rainmaker.espressif.com/docs/time-service.html
    RMaker.setTimeZone("Asia/Kolkata");
    //  Alternatively, enable the Timezone service and let the phone apps set the appropriate timezone
    RMaker.enableTZService();
    // RMaker.enableSchedule();

    // Service Name
    for (int i = 0; i < 17; i = i + 8)
    {
        espChipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }

    Serial.printf("\nChip ID:  %d Service Name: %s\n", espChipId, service_name);

    Serial.printf("\nStarting ESP-RainMaker\n");

    MQ_Setup();

    RMaker.start();

    WiFi.onEvent(sysProvEvent);
#if CONFIG_IDF_TARGET_ESP32
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);
#else
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_SOFTAP, WIFI_PROV_SCHEME_HANDLER_NONE, WIFI_PROV_SECURITY_1, pop, service_name);
#endif

    my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
    my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
    my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
}

void loop()
{
    // Read GPIO0 (external button to reset device
    if (digitalRead(gpio_reset) == LOW)
    { // Push button pressed
        Serial.printf("Reset Button Pressed!\n");
        // Key debounce handling
        delay(100);
        int startTime = millis();
        while (digitalRead(gpio_reset) == LOW)
            delay(50);
        int endTime = millis();

        if ((endTime - startTime) > 10000)
        {
            // If key pressed for more than 10secs, reset all
            Serial.printf("Reset to factory.\n");
            RMakerFactoryReset(2);
        }
        else if ((endTime - startTime) > 3000)
        {
            Serial.printf("Reset Wi-Fi.\n");
            // If key pressed for more than 3secs, but less than 10, reset Wi-Fi
            RMakerWiFiReset(2);
        }
    }
    delay(100);

    if (WiFi.status() != WL_CONNECTED)
    {
        // Serial.println("WiFi Not Connected");
        digitalWrite(wifiLed, false);
    }
    else
    {
        // Serial.println("WiFi Connected");
        digitalWrite(wifiLed, true);
        if (Timer.isReady())
        {
            // Serial.println("Sending Sensor Data");
            sendSensor();
            Timer.reset(); // Reset a second timer
        }
    }

    ir_remote(); // IR remote Control
    manual_control();
}
