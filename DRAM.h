#include <vector>

namespace ENV {
    const int WL_PIN = 18;
    const int BIT0_PIN = 33;
};

class half_byte {
public:
    int num;

    half_byte(int num) {
        if (num >= pow(2, 4)) {
            this->num = pow(2, 4);
        }
        else {
            this->num = num;
        }
    }

    void set(int num) {
        if (num >= pow(2, 4)) {
            this->num = pow(2, 4);
        }
        else {
            this->num = num;
        }
    }

    static half_byte vec2int(std::vector<bool>& v) {
        int t = 0;
        for (int i = 0; i < 4; i++) {
            t += v[i] * pow(2, i);
        }
        return half_byte(t);
    }
};

class Bit {
    private:
        const int pin;
    public:
        Bit(int pin) : pin(pin) {}

        bool read() {
            pinMode(pin, INPUT);
            digitalWrite(ENV::WL_PIN, HIGH);
            bool data = digitalRead(pin);
            digitalWrite(ENV::WL_PIN, LOW);
            return data;
        }

        template<bool isTask>
        bool write(bool data) {
            pinMode(ENV::WL_PIN, OUTPUT); // WL_PIN의 핀 모드 설정 추가
            pinMode(pin, OUTPUT);
            digitalWrite(pin, data);
            digitalWrite(ENV::WL_PIN, HIGH);
            if (isTask) {
                vTaskDelay(pdMS_TO_TICKS(1000)); // FreeRTOS의 딜레이 함수 사용
            }
            else {
                delay(1000);
            }
            digitalWrite(ENV::WL_PIN, LOW);
            digitalWrite(pin, LOW);
        }
};

class Byte {
private:
    std::vector<Bit> V;
public:
    Byte(std::vector<int> pins) {
        for (int pin : pins) {
            V.push_back(Bit(pin));
        }
    }

    half_byte read() {
        std::vector<bool> T;
        for (Bit B : V) {
            T.push_back(B.read());
        }
        return half_byte::vec2int(T);
    }

    void write(half_byte& hb) {

    }
};