#include <Servo.h>

// Servo setup
Servo servoX;
Servo servoY;
bool dataChanged = false; // Cờ để theo dõi thay đổi
String data1;

int servoPinX = 9; // Pin for X servo
int servoPinY = 10; // Pin for Y servo
const int stepSize = 2; // Tốc độ tối đa mỗi bước (độ)

const int buttonPin1 = 2;     // Chân kết nối nút biến running
const int buttonPin2 = 3;     // Chân kết nối nút biến running_inference
const int outputPin = 4;      // Chân xuất dữ liệu để cảnh báo đèn còi
const int buttonPin3 = 5;     // Chân kết nối nút tắt đèn còi thủ công

unsigned long lastPressTime1 = 0;  // Thời gian nhấn nút 1 lần cuối
unsigned long lastPressTime2 = 0;  // Thời gian nhấn nút 2 lần cuối
const unsigned long debounceDelay = 700;  // Thời gian debounce (200 ms)

volatile bool button1Pressed = false; // Cờ để nhận biết nút 1 đã nhấn
volatile bool button2Pressed = false; // Cờ để nhận biết nút 2 đã nhấn

int currentAngleX = 90; // Góc hiện tại của servoX
int currentAngleY = 90; // Góc hiện tại của servoY
int targetAngleX = 90;  // Góc mục tiêu của servoX
int targetAngleY = 90;  // Góc mục tiêu của servoY

void setup() {
  pinMode(buttonPin1, INPUT_PULLUP);   // Thiết lập chế độ INPUT với PULLUP cho nút 1
  pinMode(buttonPin2, INPUT_PULLUP);   // Thiết lập chế độ INPUT với PULLUP cho nút 2
  pinMode(outputPin, OUTPUT);          // Thiết lập chân xuất dữ liệu
  digitalWrite(outputPin, LOW);        // Đặt trạng thái ban đầu cho outputPin
  pinMode(buttonPin3, INPUT_PULLUP); 

  // Cấu hình ngắt ngoài
  attachInterrupt(digitalPinToInterrupt(buttonPin1), handleButton1Press, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPin2), handleButton2Press, FALLING);

  servoX.attach(servoPinX);
  servoY.attach(servoPinY);

  servoX.write(currentAngleX); // Set initial position
  servoY.write(currentAngleY);

  Serial.begin(9600); // Match baud rate with Python
}

void loop() {

  // Kiểm tra tín hiệu từ Python qua Serial
  if (Serial.available() > 0) {
    data1 = String(currentAngleX) + "," + String(currentAngleY); // Servo
    Serial.println(data1);

    String data = Serial.readStringUntil('\n'); 
    int commaIndex1 = data.indexOf(','); 
    int commaIndex2 = data.indexOf(',', commaIndex1 + 1);

    int num1 = data.substring(0, commaIndex1).toInt(); // Số đầu tiên
    targetAngleX = data.substring(commaIndex1 + 1, commaIndex2).toInt(); // Góc mục tiêu servoX
    targetAngleY = data.substring(commaIndex2 + 1).toInt(); // Góc mục tiêu servoY

    if (num1 == 1) {
      digitalWrite(outputPin, HIGH);   // Đặt chân outputPin là HIGH khi nhận giá trị '1'
    } else {
      digitalWrite(outputPin, LOW);    // Đặt lại LOW nếu giá trị khác '1'
    }

  }

  // Điều chỉnh góc của servoX
  if (currentAngleX < targetAngleX) {
    currentAngleX++;
  } else if (currentAngleX > targetAngleX) {
    currentAngleX--;
  }

  // Điều chỉnh góc của servoY
  if (currentAngleY < targetAngleY) {
    currentAngleY++;
  } else if (currentAngleY > targetAngleY) {
    currentAngleY--;
  }

  // Gửi lệnh đến servo
  servoX.write(currentAngleX);
  servoY.write(currentAngleY);


  // Kiểm tra nút tắt đèn còi
  if (digitalRead(buttonPin3) == LOW) {
    digitalWrite(outputPin, LOW);
    delay(5000);
  }

  delay(30); // Thêm độ trễ để làm mượt chuyển động
}

// Xử lý ngắt ngoài khi nhấn nút 1
// Xử lý ngắt ngoài khi nhấn nút 1
void handleButton1Press() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastPressTime1 > debounceDelay) {
    Serial.println("10");  // Gửi tín hiệu '10' ngay khi nút 1 được nhấn
    lastPressTime1 = currentMillis;  // Cập nhật thời gian nhấn nút 1
  }
}

// Xử lý ngắt ngoài khi nhấn nút 2
void handleButton2Press() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastPressTime2 > debounceDelay) {
    Serial.println("01");  // Gửi tín hiệu '01' ngay khi nút 2 được nhấn
    lastPressTime2 = currentMillis;  // Cập nhật thời gian nhấn nút 2
  }
}
