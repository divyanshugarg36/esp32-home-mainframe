#include <ESP8266WiFi.h>
#include <espnow.h>

// REPLACE WITH THE MAC Address of your receiver
uint8_t broadcastAddress[] = {0x08, 0xD1, 0xF9, 0x35, 0x64, 0x20};

typedef struct struct_message
{
    float temp;
    float hum;
} struct_message;

struct_message readings;

struct_message incomingReadings;

// Callback when data is sent
void onDataSent(uint8_t *mac_addr, uint8_t sendStatus)
{
    Serial.print("Last Packet Send Status: ");
    if (sendStatus == 0)
    {
        Serial.println("Delivery success");
    }
    else
    {
        Serial.println("Delivery fail");
    }
}

// Callback when data is received
// Callback when data is received
void onDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len)
{
    memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print(incomingReadings.temp);
    Serial.print(incomingReadings.hum);
}

void setup()
{
    // Init Serial Monitor
    Serial.begin(115200);

    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // Init ESP-NOW
    if (esp_now_init() != 0)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Set ESP-NOW Role
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

    // Once ESPNow is successfully Init, we will register for Send CB to
    // get the status of Trasnmitted packet
    esp_now_register_send_cb(onDataSent);

    // Register peer
    esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);

    // Register for a callback function that will be called when data is received
    esp_now_register_recv_cb(onDataRecv);
    delay(2000);
}

void loop()
{
    readings.hum = 0;
    readings.temp = 0;
    esp_now_send(broadcastAddress, (uint8_t *)&readings, sizeof(readings));
    delay(5000);
}
