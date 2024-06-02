#include <Arduino.h>
#include "DRAM.h"

// Task handle declaration
TaskHandle_t Refresher;

// Function prototype declaration
void Refresh(void *pvParams);

Byte B = Byte(27, 14, 12, 13);

void setup() {
    // pinMode(27, INPUT);
    // pinMode(14, INPUT);
    // pinMode(12, INPUT);
    // pinMode(13, INPUT);
    pinMode(ENV::WL_PIN, OUTPUT);
    Serial.begin(115200);

    io_mutex = xSemaphoreCreateMutex();
    if (io_mutex == NULL) {
        Serial.println("Mutex error");
    }

    // Create task
    xTaskCreate(
        Refresh,
        "Refresher",
        4096,
        NULL,
        1,
        &Refresher
    );
}

void loop() {
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        input.trim(); // Remove leading and trailing whitespace

        char command = input[0];

        if (command == 'r') {
            Serial.printf("읽은 값: %d\n", B.read().num);
        } else if (command == 'c') {
            input.remove(0, 2);
            int data = input.toInt();

            half_byte t = half_byte(data);

            // portENTER_CRITICAL(&mux);
            xSemaphoreTake(io_mutex, portMAX_DELAY);
            B.write(false, t);
            xSemaphoreGive(io_mutex);
            // portEXIT_CRITICAL(&mux);

            Serial.println("쓰기 완료.");
        } else {
            Serial.println("Invalid command");
        }
    }
}

void Refresh(void *pvParams) {
    while (true) {
        // portENTER_CRITICAL(&mux); // Start critical section

        xSemaphoreTake(io_mutex, portMAX_DELAY);

        half_byte t = B.read();
        B.write(true, t);
        // portEXIT_CRITICAL(&mux); // End critical section
        
        xSemaphoreGive(io_mutex);

        vTaskDelay(pdMS_TO_TICKS(100)); // Use FreeRTOS delay function
    }
}
