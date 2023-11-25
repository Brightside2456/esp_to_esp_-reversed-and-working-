#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define LED_PIN 23 // Change to your LED pin
#define BUZZER_PIN 18 // Change to your Buzzer pin

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    char a[32];
    int b;
    float c;
    String d;
    bool e;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Char: ");
  Serial.println(myData.a);
  Serial.print("Int: ");
  Serial.println(myData.b);
  Serial.print("Float: ");
  Serial.println(myData.c);
  Serial.print("Bool: ");
  Serial.println(myData.e);
  Serial.println();

  // Display the received message on the OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("SpherGuard");
  display.setCursor(0, 30);
  display.setTextColor(WHITE);
  display.println("Received message:");
  display.println(myData.a);
  display.display();

  // Blink LED and buzz buzzer twice
  for(int i = 0; i < 2; i++) {
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, 1000); // Send 1KHz sound signal...
    delay(1000); // ...for 1 sec
    noTone(BUZZER_PIN); // Stop sound...
    digitalWrite(LED_PIN, LOW);
    delay(1000); // ...for 1sec
  }

  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("SpherGuard");
  display.setCursor(0, 30);
  display.setTextColor(WHITE);
  display.println("No messages received");
  display.display();
}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize the OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("SpherGuard");
  display.setCursor(0, 30);
  display.setTextColor(WHITE);
  display.println("No messages received");
  display.display();

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

  // Initialize LED and buzzer
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // delay(2000);
  // display.clearDisplay();
  // display.setTextSize(1);
  // display.setCursor(0, 0);
  // display.println("SpherGuard");
  // display.setCursor(0, 30);
  // display.setTextColor(WHITE);
  // display.println("No messages received");
  // display.display();
}
 
void loop() {

}
