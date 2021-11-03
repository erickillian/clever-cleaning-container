/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <EEPROM.h>
#include <Keypad.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MotorShield.h>

#define SCREEN_WIDTH      128
#define SCREEN_HEIGHT     64
#define OLED_RESET        4
#define SCREEN_ADDRESS    0x3C
#define MAX_DIGITS        3

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
const byte row_pins[ROWS] = {2, 3, 4, 5}; // Pins used for the rows of the keypad
const byte col_pins[COLS] = {6, 7, 8, 9}; // Pins for the columns of the keypad

// Motor Sheild Initialization, uses default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *motors[4] = {AFMS.getMotor(1), AFMS.getMotor(2), AFMS.getMotor(3), AFMS.getMotor(4)};

// Keypad Initialization
Keypad keypadMatrix = Keypad(makeKeymap(keys), row_pins, col_pins, ROWS, COLS);

// motor selection variables
int selected_motor_addr = 0;
int selected_motor = 1;

// program feature variables
bool program_mode = false;
char program_number[MAX_DIGITS+1];
int program_index = 0;

// time in ms for each motor
int amounts[4] = {100, 100, 100, 100};

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
  }

  for (int i=0; i<4; i++) {
    motors[i]->run(FORWARD);
    motors[i]->setSpeed(0);
  }

  display.clearDisplay();
  print_selected_motor();
}

void loop() {
  // put your main code here, to run repeatedly:
  char button = keypadMatrix.getKey();
  
  if (button)
  {
    if (program_mode) {
      if (isdigit(button) && program_index < MAX_DIGITS) {
        program_number[program_index] = button;
        program_number[program_index+1] = '\0';
        program_index++;
      }
      print_program_num();
      if (button == '*') {
        program_mode = false;
        int amount = atoi(program_number);
        if (amount != 0) {
          amounts[selected_motor] = amount;
        }
        
        print_selected_motor();
      }
      
    } else {
      if (button >= 'A' && button <= 'D') {
        selected_motor = ((int) button) - 65;
        print_selected_motor();
      } else if (button == '#') {
        // runs the motors
        motors[selected_motor]->setSpeed(255);
        delay(600*amounts[selected_motor]);
        motors[selected_motor]->setSpeed(0);
      }
      else if (button == '*') {
        program_mode = true;
        program_index = 0;
        print_program_num();
        program_number[0] = '\0';
        program_number[1] = '\0';
        program_number[2] = '\0';
        program_number[3] = '\0';
        program_number[4] = '\0';
      }
    }
  }
}

void print_selected_motor() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(32, 0);
  display.print(amounts[selected_motor]);
  display.print("mL");
  display.setCursor(54, 24);
  display.setTextSize(4);
  display.print((char) (selected_motor+65));
  display.display();
}

void print_program_num() {
  display.clearDisplay();
  display.cp437(true);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("Program");
  display.setTextColor(WHITE);
  display.setCursor(0, 24);
  display.setTextSize(2);
  display.print((char) (selected_motor + 65));
  display.print(": ");
  display.print(program_number);
  display.print("mL");
  display.display();
}
