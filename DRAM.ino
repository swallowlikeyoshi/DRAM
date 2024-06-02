#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "DRAM.h"

// 뮤텍스 변수 선언
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// 태스크 핸들 선언
TaskHandle_t Refresher;

// 함수 프로토타입 선언
void Refresh(void *pvParams);
bool read();
void write(bool data);

Byte B = Byte(std::vector<int>({ 34, 32, 33, 25 }));

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(ENV::WL_PIN, OUTPUT);
    pinMode(ENV::BIT0_PIN, OUTPUT);
    Serial.begin(115200);

    // 태스크 생성
    xTaskCreate(
        Refresh,
        "Refresher",
        1000,
        NULL,
        1,
        &Refresher
    );
}

void loop() {
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        input.trim(); // 입력 문자열의 앞뒤 공백 제거

        char command = input[0];

        if (command ==  'r') {
            Serial.printf("\n읽기값: %d\n", read());
        } else if (input.startsWith("c ")) {
            input.remove(0, 2);
            String dataStr = input;

            // 문자열을 숫자로 변환
            int data = dataStr.toInt();
            
            if (data == 0) {
                pinMode(ENV::WL_PIN, OUTPUT); // WL_PIN의 핀 모드 설정 추가
                pinMode(ENV::BIT0_PIN, OUTPUT);
                digitalWrite(ENV::BIT0_PIN, LOW);
                digitalWrite(ENV::WL_PIN, HIGH);
                delay(1000);
                digitalWrite(ENV::WL_PIN, LOW);
                digitalWrite(ENV::BIT0_PIN, LOW);
            }
            else {
                pinMode(ENV::WL_PIN, OUTPUT); // WL_PIN의 핀 모드 설정 추가
                pinMode(ENV::BIT0_PIN, OUTPUT);
                digitalWrite(ENV::BIT0_PIN, HIGH);
                digitalWrite(ENV::WL_PIN, HIGH);
                delay(1000);
                digitalWrite(ENV::WL_PIN, LOW);
                digitalWrite(ENV::BIT0_PIN, LOW);
            }

            Serial.println("쓰기 완료.");
        } else if (command ==  't') {
            write(true);
            delay(10);
            bool data = read();
            Serial.printf("\n%d\n", data);
        } else {
            Serial.println("Invalid command");
        }
    }
}

void Refresh(void *pvParams) {
    while (true) {
        portENTER_CRITICAL(&mux); // 임계 구역 시작

        digitalWrite(LED_BUILTIN, HIGH);

        bool data = read();
        pinMode(ENV::BIT0_PIN, OUTPUT);
        digitalWrite(ENV::BIT0_PIN, data);
        digitalWrite(ENV::WL_PIN, HIGH);

        vTaskDelay(pdMS_TO_TICKS(100)); // FreeRTOS의 딜레이 함수 사용
        
        digitalWrite(ENV::WL_PIN, LOW);
        digitalWrite(ENV::BIT0_PIN, LOW);

        digitalWrite(LED_BUILTIN, LOW);

        portEXIT_CRITICAL(&mux); // 임계 구역 종료

        vTaskDelay(pdMS_TO_TICKS(10)); // FreeRTOS의 딜레이 함수 사용
    }
}

void write(bool data) {
    pinMode(ENV::WL_PIN, OUTPUT); // WL_PIN의 핀 모드 설정 추가
    pinMode(ENV::BIT0_PIN, OUTPUT);
    digitalWrite(ENV::BIT0_PIN, data);
    digitalWrite(ENV::WL_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(1000)); // FreeRTOS의 딜레이 함수 사용
    digitalWrite(ENV::WL_PIN, LOW);
    digitalWrite(ENV::BIT0_PIN, LOW);
}

bool read() {
    pinMode(ENV::WL_PIN, OUTPUT); // WL_PIN의 핀 모드 설정 추가
    pinMode(ENV::BIT0_PIN, INPUT); // BIT0_PIN의 핀 모드 설정 추가
    digitalWrite(ENV::WL_PIN, HIGH);
    bool data = digitalRead(ENV::BIT0_PIN);
    digitalWrite(ENV::WL_PIN, LOW);
    return data;
}
