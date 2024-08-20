#define BLYNK_TEMPLATE_ID "TMPL6nKD1ScLU"
#define BLYNK_TEMPLATE_NAME "DIEU KHIEN LED"

#define BLYNK_FIRMWARE_VERSION "0.1.0"

#define BLYNK_PRINT Serial
#define BLYNK_DEBUG
#define USE_NODE_MCU_BOARD
#define APP_DEBUG

#include "BlynkEdgent.h"
#include <stdlib.h>

WidgetLED LedConnect(V0);
unsigned long timeUpdate = millis();

void setup() {
  Serial.begin(9600);  // Khởi tạo UART với tốc độ 9600 baud
  //uart_init(9600);
  delay(100);
  BlynkEdgent.begin();
  led_init();
}

void loop() {
  BlynkEdgent.run();
  // if (uart_available()) {
  //   char data = uart_read();
  //   Serial.print(data); // In dữ liệu nhận được lên Serial Monitor
  //  }

  if (uart_available()) {
    String receivedData = readString(); // Đọc chuỗi đến khi gặp ký tự xuống dòng
    delay(10);
    uart_write_bytes("Received: ", 10); // Gửi chuỗi "Received: "
    uart_write_bytes(receivedData.c_str(), receivedData.length()); // In ra chuỗi nhận được
    uart_write('\n'); // Gửi ký tự xuống dòng
    int data = stringToInt(receivedData);

    // Hiển thị giá trị nhận được trên V3
    Blynk.virtualWrite(V3, data);

    led_off();
    if (led_begin(data) == ledRedState) {
      Blynk.virtualWrite(V2, HIGH);
      delay(1000);
      Blynk.virtualWrite(V2, LOW);
    } else {
      Blynk.virtualWrite(V1, HIGH);
      delay(1000);
      Blynk.virtualWrite(V1, LOW);
    }
  } else if(millis() - timeUpdate > 1000) {
    led_blink();
    // if (LedConnect.getValue()) {
    //   LedConnect.off();
    // } else {
    //   LedConnect.on();
    // }
    timeUpdate = millis();
  }
}

BLYNK_CONNECTED() {
  Blynk.syncAll();
}

BLYNK_WRITE(V1) {
  int p = param.asInt();
  digitalWrite(D1, p);
  ledGreenState = p; // Cập nhật trạng thái LED xanh
  Blynk.virtualWrite(V1, ledGreenState); // Đồng bộ với Blynk Cloud
}

BLYNK_WRITE(V2) {
  int p = param.asInt();
  digitalWrite(D2, p);
  ledRedState = p; // Cập nhật trạng thái LED đỏ
  Blynk.virtualWrite(V2, ledRedState); // Đồng bộ với Blynk Cloud
}

BLYNK_WRITE(V3) {
  int value = param.asInt(); // Nhận giá trị từ V3
  Serial.print("Received value from V3: ");
  Serial.println(value);
  // Thực hiện hành động tùy ý với giá trị này
}
