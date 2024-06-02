#include <Arduino.h>
#include <cmath> // Include cmath for pow()
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

// Mutex variable declaration
// portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
SemaphoreHandle_t io_mutex;

namespace ENV {
    const int WL_PIN = 18;
};

class half_byte {
public:
    int num;

    half_byte(int num) {
        if (num >= pow(2, 4)) {
            this->num = pow(2, 4) - 1; // Corrected value to be within 4 bits
        }
        else {
            this->num = num;
        }
    }

    void set(int num) {
        if (num >= pow(2, 4)) {
            this->num = pow(2, 4) - 1; // Corrected value to be within 4 bits
        }
        else {
            this->num = num;
        }
    }
};

class Byte {
    private:
        int* V;
    public:
        Byte(int p1, int p2, int p3, int p4) {
            V = new int[4]{p1, p2, p3, p4};
        }

        half_byte read() {
            for (int i = 0; i < 4; i++) {
                pinMode(V[i], INPUT);
            }
            digitalWrite(ENV::WL_PIN, HIGH);
            int t = 0;
            for (int i = 0; i < 4; i++) {
                t += digitalRead(V[i]) * (int)pow(2, i);
            }
            return half_byte(t);
        }

        void write(bool isTask, half_byte& hb) {
            for (int i = 0; i < 4; i++) {
                pinMode(V[i], OUTPUT); 
            }
            digitalWrite(ENV::WL_PIN, LOW);
            
            for (int i = 0; i < 4; i++) {
                digitalWrite(V[i], (hb.num & (int)pow(2, i)) >> i);
            }
            
            digitalWrite(ENV::WL_PIN, HIGH);
            vTaskDelay(pdMS_TO_TICKS(100));
            digitalWrite(ENV::WL_PIN, LOW);
        }
};
