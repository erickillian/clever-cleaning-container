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
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MotorShield.h>

#define SCREEN_WIDTH          128
#define SCREEN_HEIGHT         64
#define OLED_RESET            4
#define SCREEN_ADDRESS        0x3C
#define MAX_DIGITS            3
#define SELECTED_MOTOR_ADDR   0
#define AMOUNTS_ADDR          2

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
int selected_motor;

// program feature variables
bool program_mode = false;
char program_number[MAX_DIGITS+1];
int program_index = 0;

// time in ms for each motor
unsigned int amounts[4] = {100, 100, 100, 100};

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
    amounts[i] = (int) readIntFromEEPROM(AMOUNTS_ADDR+i*2);
  }

  selected_motor = (int) EEPROM.read(SELECTED_MOTOR_ADDR);

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
          writeIntIntoEEPROM(AMOUNTS_ADDR+selected_motor*2, amount);
        }
        
        print_selected_motor();
      }
      
    } else {
      if (button >= 'A' && button <= 'D') {
        selected_motor = ((int) button) - 65;
        EEPROM.write(SELECTED_MOTOR_ADDR, (byte) selected_motor);
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
        program_number[0] = '\0';
        program_number[1] = '\0';
        program_number[2] = '\0';
        program_number[3] = '\0';
        program_number[4] = '\0';
        print_program_num();
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

void writeIntIntoEEPROM(int address, unsigned int number) { 
  EEPROM.write(address, (byte)(number >> 8));
  EEPROM.write(address + 1, (byte)((number & 0xFF)));
}

unsigned int readIntFromEEPROM(int address) {
  unsigned int byte1 = EEPROM.read(address);
  unsigned int byte2 = EEPROM.read(address + 1);
  return ((byte1 << 8) + byte2);
}
