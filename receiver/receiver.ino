#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// =====================================================
// NRF24L01
// =====================================================

#define CE_PIN 48
#define CSN_PIN 49

RF24 radio(CE_PIN, CSN_PIN);

const byte address[6] = "ROVER";


// =====================================================
// OLED
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

bool oledOK = false;


// =====================================================
// LEFT IBT-2
// =====================================================

#define LEFT_RPWM 5
#define LEFT_LPWM 6
#define LEFT_EN   7


// =====================================================
// RIGHT IBT-2
// =====================================================

#define RIGHT_RPWM 8
#define RIGHT_LPWM 9
#define RIGHT_EN   10


// =====================================================
// MOTOR SPEED
// =====================================================

#define MOTOR_SPEED 180


// =====================================================
// SAFETY TIMEOUT
// =====================================================

#define SIGNAL_TIMEOUT 200

unsigned long lastReceived = 0;


// =====================================================
// COMMANDS
// =====================================================

#define STOP_CMD     0
#define FORWARD_CMD  1
#define BACKWARD_CMD 2
#define LEFT_CMD     3
#define RIGHT_CMD    4


// =====================================================
// DATA PACKET
// MUST MATCH UNO EXACTLY
// =====================================================

struct DataPacket
{
  byte command;
  int x;
  int y;
  unsigned long packetNumber;
};

DataPacket data;


// =====================================================
// OLED STATE
// =====================================================

String currentDirection = "";


// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(9600);


  // ===================================================
  // MOTOR PINS
  // ===================================================

  pinMode(LEFT_RPWM, OUTPUT);
  pinMode(LEFT_LPWM, OUTPUT);
  pinMode(LEFT_EN, OUTPUT);

  pinMode(RIGHT_RPWM, OUTPUT);
  pinMode(RIGHT_LPWM, OUTPUT);
  pinMode(RIGHT_EN, OUTPUT);


  // Enable drivers
  digitalWrite(LEFT_EN, HIGH);
  digitalWrite(RIGHT_EN, HIGH);


  // Safety: motors OFF
  stopMotors();


  // ===================================================
  // OLED
  // ===================================================

  Wire.begin();


  if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    oledOK = true;

    Serial.println("OLED FOUND AT 0x3C");
  }
  else if (display.begin(SSD1306_SWITCHCAPVCC, 0x3D))
  {
    oledOK = true;

    Serial.println("OLED FOUND AT 0x3D");
  }
  else
  {
    oledOK = false;

    Serial.println("OLED NOT DETECTED!");
  }


  // ===================================================
  // OLED READY
  // ===================================================

  if (oledOK)
  {
    display.clearDisplay();

    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(2);

    display.setCursor(20, 20);

    display.println("READY");

    display.display();

    delay(500);
  }


  // ===================================================
  // NRF24
  // ===================================================

  Serial.println("Starting NRF24...");


  if (!radio.begin())
  {
    Serial.println("NRF24 NOT DETECTED!");

    if (oledOK)
    {
      display.clearDisplay();

      display.setTextColor(SSD1306_WHITE);

      display.setTextSize(1);

      display.setCursor(10, 20);

      display.println("NRF24 NOT");
      display.println("DETECTED");

      display.display();
    }

    while (1);
  }


  // ===================================================
  // NRF24 SETTINGS
  // MUST MATCH UNO
  // ===================================================

  radio.setChannel(108);

  radio.setDataRate(RF24_250KBPS);

  radio.setPALevel(RF24_PA_LOW);

  // Same as UNO
  radio.setAutoAck(false);


  // ===================================================
  // RECEIVE PIPE
  // ===================================================

  radio.openReadingPipe(0, address);

  radio.startListening();

  radio.powerUp();


  // ===================================================
  // READY
  // ===================================================

  Serial.println("NRF24 FOUND!");
  Serial.println("ROVER RECEIVER READY");
  Serial.println("WAITING FOR DATA...");
  Serial.println();


  data.command = STOP_CMD;

  lastReceived = millis();

  displayDirection("STOP");
}


// =====================================================
// LOOP
// =====================================================

void loop()
{
  // ===================================================
  // RECEIVE DATA
  // ===================================================

  if (radio.available())
  {
    // Read the newest available packet
    while (radio.available())
    {
      radio.read(&data, sizeof(data));
    }


    // Update communication time
    lastReceived = millis();


    // =================================================
    // SERIAL MONITOR
    // =================================================

    Serial.print("PACKET=");
    Serial.print(data.packetNumber);

    Serial.print("  X=");
    Serial.print(data.x);

    Serial.print("  Y=");
    Serial.print(data.y);

    Serial.print("  COMMAND=");


    // =================================================
    // EXECUTE COMMAND
    // =================================================

    switch (data.command)
    {
      // -----------------------------------------------
      // STOP
      // -----------------------------------------------

      case STOP_CMD:

        Serial.println("STOP");

        stopMotors();

        displayDirection("STOP");

        break;


      // -----------------------------------------------
      // FORWARD
      // -----------------------------------------------

      case FORWARD_CMD:

        Serial.println("FORWARD");

        forward();

        displayDirection("FORWARD");

        break;


      // -----------------------------------------------
      // BACKWARD
      // -----------------------------------------------

      case BACKWARD_CMD:

        Serial.println("BACKWARD");

        backward();

        displayDirection("BACKWARD");

        break;


      // -----------------------------------------------
      // LEFT
      // -----------------------------------------------

      case LEFT_CMD:

        Serial.println("LEFT");

        turnLeft();

        displayDirection("LEFT");

        break;


      // -----------------------------------------------
      // RIGHT
      // -----------------------------------------------

      case RIGHT_CMD:

        Serial.println("RIGHT");

        turnRight();

        displayDirection("RIGHT");

        break;


      // -----------------------------------------------
      // INVALID
      // -----------------------------------------------

      default:

        Serial.println("INVALID");

        stopMotors();

        displayDirection("STOP");

        break;
    }
  }


  // ===================================================
  // SIGNAL LOST
  // ===================================================

  if (millis() - lastReceived > SIGNAL_TIMEOUT)
  {
    stopMotors();

    displayDirection("STOP");
  }
}


// =====================================================
// FORWARD
// =====================================================
// THIS IS YOUR CURRENTLY WORKING DIRECTION.
// LEFT UNCHANGED.
// =====================================================

void forward()
{
  // LEFT MOTOR
  analogWrite(LEFT_RPWM, 0);
  analogWrite(LEFT_LPWM, MOTOR_SPEED);

  // RIGHT MOTOR
  analogWrite(RIGHT_RPWM, 0);
  analogWrite(RIGHT_LPWM, MOTOR_SPEED);
}


// =====================================================
// BACKWARD
// =====================================================

void backward()
{
  // LEFT MOTOR
  analogWrite(LEFT_RPWM, MOTOR_SPEED);
  analogWrite(LEFT_LPWM, 0);

  // RIGHT MOTOR
  analogWrite(RIGHT_RPWM, MOTOR_SPEED);
  analogWrite(RIGHT_LPWM, 0);
}


// =====================================================
// TURN LEFT
// =====================================================

void turnLeft()
{
  // LEFT MOTOR = BACKWARD
  analogWrite(LEFT_RPWM, MOTOR_SPEED);
  analogWrite(LEFT_LPWM, 0);

  // RIGHT MOTOR = FORWARD
  analogWrite(RIGHT_RPWM, 0);
  analogWrite(RIGHT_LPWM, MOTOR_SPEED);
}


// =====================================================
// TURN RIGHT
// =====================================================

void turnRight()
{
  // LEFT MOTOR = FORWARD
  analogWrite(LEFT_RPWM, 0);
  analogWrite(LEFT_LPWM, MOTOR_SPEED);

  // RIGHT MOTOR = BACKWARD
  analogWrite(RIGHT_RPWM, MOTOR_SPEED);
  analogWrite(RIGHT_LPWM, 0);
}


// =====================================================
// STOP
// =====================================================

void stopMotors()
{
  analogWrite(LEFT_RPWM, 0);
  analogWrite(LEFT_LPWM, 0);

  analogWrite(RIGHT_RPWM, 0);
  analogWrite(RIGHT_LPWM, 0);
}


// =====================================================
// OLED
// =====================================================

void displayDirection(const char* direction)
{
  if (!oledOK)
  {
    return;
  }


  // Don't redraw same direction
  if (currentDirection == direction)
  {
    return;
  }


  currentDirection = direction;


  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);


  int16_t x1;
  int16_t y1;

  uint16_t width;
  uint16_t height;


  display.getTextBounds(
    direction,
    0,
    0,
    &x1,
    &y1,
    &width,
    &height
  );


  int x = (SCREEN_WIDTH - width) / 2;

  int y = (SCREEN_HEIGHT - height) / 2;


  display.setCursor(x, y);

  display.println(direction);

  display.display();
}
