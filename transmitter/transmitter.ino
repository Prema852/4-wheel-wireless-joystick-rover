#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// =====================================================
// NRF24L01
// =====================================================

#define CE_PIN 9
#define CSN_PIN 10

RF24 radio(CE_PIN, CSN_PIN);

const byte address[6] = "ROVER";


// =====================================================
// JOYSTICK
// =====================================================

#define JOY_X A0
#define JOY_Y A1

// Your joystick center
#define CENTER_X 515
#define CENTER_Y 513

// Joystick dead zone
#define DEAD_ZONE 150


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
// This MUST be identical on Mega
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
// PACKET COUNTER
// =====================================================

unsigned long packetCounter = 0;


// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(9600);

  // Start NRF24
  if (!radio.begin())
  {
    Serial.println("NRF24 NOT DETECTED!");
    while (1);
  }

  // NRF24 settings
  radio.setChannel(108);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);

  // Disable ACK/retries for fast joystick control
  radio.setAutoAck(false);

  radio.openWritingPipe(address);
  radio.stopListening();

  radio.powerUp();

  Serial.println();
  Serial.println("================================");
  Serial.println("     ROVER TRANSMITTER READY");
  Serial.println("================================");
}


// =====================================================
// LOOP
// =====================================================

void loop()
{
  // ===================================================
  // READ JOYSTICK
  // ===================================================

  data.x = analogRead(JOY_X);
  data.y = analogRead(JOY_Y);


  // Calculate movement from center
  int dx = data.x - CENTER_X;
  int dy = data.y - CENTER_Y;

  int absX = abs(dx);
  int absY = abs(dy);


  // ===================================================
  // DETERMINE COMMAND
  // ===================================================

  // CENTER = STOP
  if (absX <= DEAD_ZONE && absY <= DEAD_ZONE)
  {
    data.command = STOP_CMD;
  }

  // Y AXIS DOMINANT
  else if (absY > absX)
  {
    // FORWARD
    if (dy < -DEAD_ZONE)
    {
      data.command = FORWARD_CMD;
    }

    // BACKWARD
    else if (dy > DEAD_ZONE)
    {
      data.command = BACKWARD_CMD;
    }

    else
    {
      data.command = STOP_CMD;
    }
  }

  // X AXIS DOMINANT
  else
  {
    // LEFT
    if (dx < -DEAD_ZONE)
    {
      data.command = LEFT_CMD;
    }

    // RIGHT
    else if (dx > DEAD_ZONE)
    {
      data.command = RIGHT_CMD;
    }

    else
    {
      data.command = STOP_CMD;
    }
  }


  // ===================================================
  // PACKET NUMBER
  // ===================================================

  data.packetNumber = packetCounter++;


  // ===================================================
  // TRANSMIT
  // ===================================================

  radio.write(&data, sizeof(data));


  // ===================================================
  // SERIAL MONITOR
  // ===================================================

  Serial.print("PACKET=");
  Serial.print(data.packetNumber);

  Serial.print("  X=");
  Serial.print(data.x);

  Serial.print("  Y=");
  Serial.print(data.y);

  Serial.print("  COMMAND=");

  switch (data.command)
  {
    case STOP_CMD:
      Serial.println("STOP");
      break;

    case FORWARD_CMD:
      Serial.println("FORWARD");
      break;

    case BACKWARD_CMD:
      Serial.println("BACKWARD");
      break;

    case LEFT_CMD:
      Serial.println("LEFT");
      break;

    case RIGHT_CMD:
      Serial.println("RIGHT");
      break;

    default:
      Serial.println("INVALID");
      break;
  }


  // Send 50 packets per second
  delay(20);
}
