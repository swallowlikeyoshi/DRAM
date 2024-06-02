#include <Arduino.h>
#include "DRAM.h"

// Task handle declaration
TaskHandle_t Refresher;

// Function prototype declaration
void Refresh(void *pvParams);

Byte B = Byte(27, 14, 12, 13);

void setup()
{
    pinMode(ENV::WL_PIN, OUTPUT);
    Serial.begin(115200);

    io_mutex = xSemaphoreCreateMutex();
    if (io_mutex == NULL)
    {
        Serial.println("Mutex error");
    }

    // Create task
    xTaskCreate(
        Refresh,
        "Refresher",
        4096,
        NULL,
        1,
        &Refresher);

    B.write(false, half_byte(0));
}

void loop()
{
    if (Serial.available())
    {
        String input = Serial.readStringUntil('\n');
        input.trim();
        char command = input[0];

        if (command == 'r')
        {
            Serial.printf("읽은 값: %d\n", B.read().num);
        }
        else if (command == 'c')
        {
            input.remove(0, 2);
            int data = input.toInt();

            half_byte t = half_byte(data);

            xSemaphoreTake(io_mutex, portMAX_DELAY);
            B.write(false, t);
            xSemaphoreGive(io_mutex);

            Serial.printf("값 %d 쓰기 완료.\n", t.num);
        }
        else
        {
            Serial.println("Invalid command");
        }
    }
}

void Refresh(void *pvParams)
{
    while (true)
    {
        xSemaphoreTake(io_mutex, portMAX_DELAY);

        half_byte t = B.read();
        B.write(true, t);

        xSemaphoreGive(io_mutex);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
