#ifndef AHT_SENSOR_H
#define AHT_SENSOR_H

#include <Adafruit_AHTX0.h>

struct AHTData
{
    float temperature = 0;
    float humidity = 0;
};

class AHTSensor
{
private:
    Adafruit_AHTX0 aht;

public:
    // Initialize the AHT sensor
    bool initialize()
    {
        if (!aht.begin())
        {
            Serial.println("Could not find AHT? Check wiring");
            return false; // Return false if initialization fails
        }
        return true; // Return true if initialization is successful
    }

    // Read and print temperature and humidity
    AHTData readData()
    {
        sensors_event_t humidityEvent, tempEvent;
        aht.getEvent(&humidityEvent, &tempEvent); // Populate temp and humidity objects with fresh data

        AHTData data;
        data.temperature = tempEvent.temperature;
        data.humidity = humidityEvent.relative_humidity;

        return data;
    }
};

#endif