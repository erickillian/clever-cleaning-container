/*
  Clever Cleaning Container (C^3)

  This code processes input from a 4x4 keypad matrix
  (Teyleten Robot 4 x 4 Matrix Array 16 Key Membrane Switch Keypad Keyboard for Arduino AVR PI C)
  https://www.amazon.com/Teyleten-Robot-Membrane-Keyboard-Arduino/dp/B0993D2VLR/ref=sr_1_23?dchild=1&keywords=keypad+for+arduino&qid=1631044015&s=industrial&sr=1-23
  
  The code then processes the input to control 4 DC Pumps through a motor sheild
  (HiLetgo TB6612 Mosfet for Arduino Motor Shield Standard IIC I2C TB6612FNG Stepper Motor PCA9685 PWM Servo Driver Shield V2 Robot PWM Uno Mega R3 Replace L293D)
  https://www.amazon.com/HiLetgo-TB6612FNG-Stepper-Motor-Replace/dp/B073SP76MC/ref=sr_1_3?dchild=1&keywords=ARDUINO+V2&qid=1630451092&s=industrial&sr=1-3


  Pinouts:
  * list the components attached to each input
  * list the components attached to each output

  Created 10/12/2021
  By Eric Killian
  
  Modified 10/12/2021
  Eric Killian
*/


#include <EEPROM.h>
#include <Keypad.h>
#include <Adafruit_MotorShield.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *motor1 = AFMS.getMotor(1);
Adafruit_DCMotor *motor2 = AFMS.getMotor(2);
Adafruit_DCMotor *motor3 = AFMS.getMotor(3);
Adafruit_DCMotor *motor4 = AFMS.getMotor(4);

// These pins might need to change based on what pins are needed for the motor shield outputs
const byte row_pins[ROWS] = {9,2,3,4}; // Pins used for the rows of the keypad
const byte col_pins[COLS] = {5,6,7,8}; // Pins for the columns of the keypad

// Initialise the Keypad
Keypad keypadMatrix = Keypad(makeKeymap(keys), row_pins, col_pins, ROWS, COLS);

int selected_motor = 1;

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);   // Initialise the serial monitor

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
//    while (1);
  }
  
//  if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
//    Serial.println("Could not find Motor Shield. Check wiring.");
////    while (1);
//  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Set the speed to start, from 0 (off) to 255 (max speed)
//  motor1->run(FORWARD);
//  motor2->run(FORWARD);
//  motor3->run(FORWARD);
//  motor4->run(FORWARD);
//  motor1->setSpeed(0);
//  motor2->setSpeed(0);
//  motor3->setSpeed(0);
//  motor4->setSpeed(0);

}

void displayText(String s) {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0,24);
  display.setTextSize(2);
  display.println(s);
  display.display();
}


void runMotor(int motor_num, int time_ms) {
  switch(motor_num) {
    case 1:
      Serial.println("Dispense A");
//      motor1->setSpeed(255);
      break;
    case 2:
      Serial.println("Dispense B");
//      motor2->setSpeed(255);
    break;
    case 3:
//      motor3->setSpeed(255);
      Serial.println("Dispense C");
    break;
    case 4:
//      motor4->setSpeed(255);
      Serial.println("Dispense D");
    break;
  }
  delay(time_ms);
//  motor1->setSpeed(0);
//  motor2->setSpeed(0);
//  motor3->setSpeed(0);
//  motor4->setSpeed(0);
}

void loop() {
  // put your main code here, to run repeatedly:
  char button = keypadMatrix.getKey();

  if (button) {
     switch(button) {
       case 'A':
          Serial.println("Select A");
          selected_motor = 1;
          displayText("A");
          break;
       case 'B':
          Serial.println("Select B");
          selected_motor = 2;
          displayText("B");
          break;
       case 'C':
          Serial.println("Select C");
          selected_motor = 3;
          displayText("C");
          break;
       case 'D':
          Serial.println("Select D");
          selected_motor = 4;
          displayText("D");
          break;
       case '#':
          Serial.println("Dispense");
          runMotor(selected_motor, 1000);
          break;
  
       default: //Optional
          Serial.println(button);
     }
  }
}
