#include <WiFi.h>
#include <esp_now.h>

// REPLACE WITH THE MAC Address of your receiver
uint8_t broadcastAddress[] = {0x24, 0x4C, 0xAB, 0x51, 0xAB, 0x1C};

typedef struct struct_message
{
    float temp;
    float hum;
} struct_message;

struct_message readings;

struct_message incomingReadings;

// Define variables to store incoming readings
float incomingTemp;
float incomingHum;

// Variable to store if sending data was successful
String success;

// Create a struct_message to hold incoming sensor readings

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    if (status == 0)
    {
        success = "Delivery Success :)";
    }
    else
    {
        success = "Delivery Fail :(";
    }
}

// Callback when data is received
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
    Serial.print("Bytes received: ");
    Serial.println(len);
    incomingTemp = incomingReadings.temp;
    incomingHum = incomingReadings.hum;
    Serial.print(incomingReadings.temp);
}

void setup()
{
    // Init Serial Monitor
    Serial.begin(115200);

    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);

    // Init ESP-NOW
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Once ESPNow is successfully Init, we will register for Send CB to
    // get the status of Trasnmitted packet
    esp_now_register_send_cb(onDataSent);

    // Register peer
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    // Add peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        Serial.println("Failed to add peer");
        return;
    }
    // Register for a callback function that will be called when data is received
    esp_now_register_recv_cb(esp_now_recv_cb_t(onDataRecv));
}

void loop()
{
    // Set values to send
    readings.temp = 1;
    readings.hum = 1;

    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&readings, sizeof(readings));

    if (result == ESP_OK)
    {
        Serial.println("Sent with success");
    }
    else
    {
        Serial.println("Error sending the data");
    }
    delay(5000);
}
