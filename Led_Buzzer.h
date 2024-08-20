//#include <BlynkSimpleEsp8266.h> // Include Blynk if needed for functions

#define LED_GREEN D1
#define LED_RED D2
bool ledState = LOW;
bool ledGreenState = LOW;
bool ledRedState = LOW;

void led_init(void){
    pinMode(LED_GREEN, OUTPUT);  // LED đỏ
    pinMode(LED_RED, OUTPUT);  // LED xanh lá
}

void led_off(void){
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);
  ledGreenState = LOW;
  ledRedState = LOW;
}

void led_blink(void) {
  ledState = !ledState;
  digitalWrite(D1, ledState); // Thay đổi trạng thái của đèn xanh
}

bool led_begin(int data) {
    // Xử lý dữ liệu nhận được
    if (data > 20) {
      // Bật đèn đỏ nếu dữ liệu lớn hơn 10
      digitalWrite(LED_RED, HIGH);
      delay(2000);
      digitalWrite(LED_RED, LOW);
      ledRedState = HIGH;
      return ledRedState;

    } else {
      // Tắt đèn đỏ và bật đèn xanh nếu dữ liệu không lớn hơn 10
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GREEN, HIGH);
      delay(2000);
      digitalWrite(LED_GREEN, LOW);
      ledGreenState = HIGH;
      return ledGreenState;
    }
}