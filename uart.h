// Định nghĩa các địa chỉ thanh ghi UART
#define UART0_BASE 0x60000000
#define UART_CLKDIV(uart) (UART0_BASE + 0x14)
#define UART_CONF0(uart) (UART0_BASE + 0x20)
//#define UART_CONF1(uart ) (UART0_BASE + 0x24)
#define UART_STATUS(uart) (UART0_BASE + 0x1C)
#define UART_FIFO(uart) (UART0_BASE + 0x00)

// Kiểm tra nếu các macro đã được định nghĩa
#ifndef WRITE_PERI_REG
#define WRITE_PERI_REG(addr, val) (*(volatile uint32_t *)(addr) = (val))
#endif

#ifndef READ_PERI_REG
#define READ_PERI_REG(addr) (*(volatile uint32_t *)(addr))
#endif

#ifndef SET_PERI_REG_MASK
#define SET_PERI_REG_MASK(reg, mask) WRITE_PERI_REG((reg), (READ_PERI_REG(reg) | (mask)))
#endif

#ifndef CLEAR_PERI_REG_MASK
#define CLEAR_PERI_REG_MASK(reg, mask) WRITE_PERI_REG((reg), (READ_PERI_REG(reg) & (~(mask))))
#endif

#define UART_TX_EN (1 << 0)
#define UART_RX_EN (1 << 1)
#define UART_BIT_NUM_S 0
#define UART_PARITY_EN (1 << 3)
#define UART_STOP_BIT_NUM_S 4
#define UART_RXFIFO_CNT_S 0
#define UART_TXFIFO_CNT_S 16

void uart_init(unsigned long baudrate) {
    // Tính toán giá trị chia tần số để cấu hình tốc độ baud
    uint32_t clk_div = (80000000 / (baudrate * 16)) - 1; // ESP8266 chạy ở 80 MHz
    WRITE_PERI_REG(UART_CLKDIV(UART0_BASE), clk_div);

    // Cấu hình UART: 8 bits dữ liệu, không parity, 1 bit stop
    WRITE_PERI_REG(UART_CONF0(UART0_BASE), (3 << UART_BIT_NUM_S) | (0 << UART_PARITY_EN) | (1 << UART_STOP_BIT_NUM_S));

    // Bật chức năng truyền (TX) và nhận (RX) của UART
    SET_PERI_REG_MASK(UART_CONF0(UART0_BASE), UART_TX_EN | UART_RX_EN);

    // Xóa FIFO (Reset RX FIFO và TX FIFO)
    SET_PERI_REG_MASK(UART_STATUS(UART0_BASE), (1 << 18) | (1 << 22));  // Giả sử bit 18 và 22 là các bit reset FIFO
    CLEAR_PERI_REG_MASK(UART_STATUS(UART0_BASE), (1 << 18) | (1 << 22)); // Clear các cờ reset FIFO sau khi reset
}

// Hàm kiểm tra xem có dữ liệu nhận được không
bool uart_available() {
  return (READ_PERI_REG(UART_STATUS(UART0_BASE)) & (0xFF << UART_RXFIFO_CNT_S)) > 0;
}

// Hàm đọc một byte dữ liệu từ UART
char uart_read() {
  while (!uart_available());
  return (char)(READ_PERI_REG(UART_FIFO(UART0_BASE)) & 0xFF);
}

// Hàm gửi một byte dữ liệu qua UART
void uart_write(char data) {
  while (READ_PERI_REG(UART_STATUS(UART0_BASE)) & (0xFF << UART_TXFIFO_CNT_S));
  WRITE_PERI_REG(UART_FIFO(UART0_BASE), data);
}

// Hàm gửi nhiều byte dữ liệu qua UART
void uart_write_bytes(const char *data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        while (READ_PERI_REG(UART_STATUS(UART0_BASE)) & (0xFF << UART_TXFIFO_CNT_S)); // Đợi cho đến khi có chỗ trống trong FIFO
        WRITE_PERI_REG(UART_FIFO(UART0_BASE), data[i]); // Gửi dữ liệu
    }
}
String readString() {
  String result = ""; // Chuỗi kết quả
  char buffer[50]; // Buffer để lưu dữ liệu đọc được
  int index = 0;

  // Đọc dữ liệu từ Serial cho đến khi gặp ký tự xuống dòng hoặc hết bộ đệm
  while (true) {
    while (uart_available() == 0);// Đợi cho đến khi có dữ liệu sẵn sàng
    char c = uart_read();// Đọc một ký tự từ Serial
    // Nếu gặp ký tự xuống dòng hoặc ký tự carriage return (CR), dừng lại
    if (c == '\n' || c == '\r') {
      break;}
    if (index < sizeof(buffer) - 1) { // Nếu buffer chưa đầy, thêm ký tự vào kết quả
      buffer[index++] = c;
    }
  }

  buffer[index] = '\0'; // Kết thúc chuỗi bằng ký tự null
  result = String(buffer); // Chuyển đổi buffer thành chuỗi
  result.trim(); // Loại bỏ ký tự xuống dòng và khoảng trắng không mong muốn

  return result; // Trả về chuỗi kết quả
}

// Hàm chuyển chuỗi số thành số nguyên (int)
int stringToInt( String &str) {
  int result = 0;
  int index = 0;

  char* ptr = reinterpret_cast< char*>(&str[0]); // Sử dụng pointer đến dữ liệu bên trong đối tượng String
  while (ptr[index] != '\0') { // Duyệt từng ký tự cho đến khi gặp ký tự kết thúc chuỗi
    char c = ptr[index];
    if (c >= '0' && c <= '9') { // Kiểm tra xem ký tự có phải là chữ số không
      result = result * 10 + (c - '0'); // Chuyển ký tự thành số và cộng vào kết quả
    } else {
      // Nếu gặp ký tự không phải số, có thể xử lý lỗi ở đây nếu cần
      return 0; // Trả về 0 hoặc giá trị lỗi nếu chuỗi chứa ký tự không hợp lệ
    }
    index++;
  }
  return result;
}

int uart_read_int() {
  String resultString = readString(); // Đọc chuỗi từ UART
  return stringToInt(resultString); // Chuyển đổi chuỗi thành số nguyên
}