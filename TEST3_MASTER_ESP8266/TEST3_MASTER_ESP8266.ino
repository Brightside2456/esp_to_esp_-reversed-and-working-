#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Wire.h"
#include <MPU6050_light.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x08, 0xD1, 0xF9, 0xD9, 0x60, 0xB0};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  String d;
  bool e;
} struct_message;

// Create a struct_message called myData
struct_message myData;

unsigned long lastTime = 0;  
unsigned long timerDelay = 100;  // send readings timer

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
//Declaration of mpu6050 instance
MPU6050 mpu(Wire);

long timer = 0;
float prevRoll = 0.0;
float prevPitch = 0.0;
float gyroThreshold = 100.0;  // Adjust this threshold based on your project requirements

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
 
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  // Initialize the display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();

  // Initialize the MPU6050
  Wire.begin();
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.print("MPU6050 status: ");
  display.println(status);
  while(status!=0){ } // Stop everything and loop forever if could not connect to MPU6050

  // Calibrate the MPU6050
  Serial.println("Calculating Offsets, do not move MPU6050");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.println("Calculating Offsets, do not move MPU6050");
  delay(3000);
  mpu.calcOffsets(true, true); // gyro, accelerometer
  delay(3000);
  Serial.println("Done\n");
  display.setCursor(0, 10);
  display.println("Done\n");
  display.display();
}

void loop() {
  mpu.update();

  if(millis() - timer > 1000){
    display.clearDisplay();
    Serial.print("Roll: ");Serial.println(mpu.getAngleX());
    display.setCursor(0, 0);
    display.print("Roll: ");
    display.println(mpu.getAngleX());
    Serial.print("Pitch: ");Serial.println(mpu.getAngleY());
    display.setCursor(0, 10);
    display.print("Pitch: ");
    display.println(mpu.getAngleY());
    Serial.print("Gyro X: ");Serial.println(mpu.getGyroX());
    Serial.print("Gyro Y: ");Serial.println(mpu.getGyroY());

    // Calculate the change in roll and pitch angles
    float rollChange = abs(mpu.getAngleX() - prevRoll);
    float pitchChange = abs(mpu.getAngleY() - prevPitch);

    // Display the gyro changes
    display.setCursor(0, 20);
    display.print("Gyro Change X: ");
    display.println(rollChange);
    display.setCursor(0, 30);
    display.print("Gyro Change Y: ");
    display.println(pitchChange);

    // Check if the gyro changes are too rapid or too slow
    if (rollChange > gyroThreshold || pitchChange > gyroThreshold) {
      display.setCursor(0, 40);
      display.println("Unusual movement detected!");
      display.setCursor(0, 50);
      display.println("Please attend to the person");
      delay(700);

      // Set values to send
      strcpy(myData.a, "Please Help");
      myData.b = 0;
      myData.c = 0.0;
      myData.d = "";
      myData.e = false;

      // Send message via ESP-NOW
      esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    }
   
    display.display();
    timer = millis();
    prevRoll = mpu.getAngleX();
    prevPitch = mpu.getAngleY();
  }
}