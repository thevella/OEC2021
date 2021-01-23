#include <Arduino.h>
#include <Servo.h>
#include <AFMotor.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>

// Ports 1 & 2 - High speed ports, can run upto 64KHz (For motors 1 and 2 you can choose MOTOR12_64KHZ, MOTOR12_8KHZ, MOTOR12_2KHZ, or MOTOR12_1KHZ)
// Ports 3 & 4 - Can only run at 1KHz (Motors 3 & 4 are only possible to run at 1KHz and will ignore any setting given)
#define DEBUG


// Direction of sweeper motor
#define DOWN 0
#define UP 1

AF_DCMotor driveLeft(1, MOTOR12_64KHZ);  // create driveLeft #1, 64KHz pwm
AF_DCMotor driveRight(2, MOTOR12_64KHZ); // create driveLeft #2, 64KHz pwm

Servo sweeperMotor;
Servo collapseMotor;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

// The motors we are using have reports of being damaged when
//     run to their endstop, these definitions limit the range
const int servoPosMax = 170;
const int servoPosMin = 10;

// First position for the sweeper
const int sweeperOriginalPos = 45;
// Steps the sweeper has to travel between sensor positions
const int sweeperStep = ceil(45 / 2);

// Keep track of the position of the sweeper
int sweeperPos = sweeperOriginalPos;


// Speed the motors should attempt to run at (0 < driveSpeed < 255)
const int driveSpeed = 120;


// Functions to allow easy testing of individual components
// Using inline to not lose processes to function calls
inline void motorSetup();
inline void colorSensorSetup();

inline void driveForward();
inline void sweep(int);
inline void detectColor();
inline void spray();

void setup()
{
    Serial.begin(9600); // set up Serial library at 9600 bps

    colorSensorSetup();
    motorSetup();
}

void loop()
{

    detectColor();
    sweep(UP);
    delay(500);
    driveForward();
}

inline void driveForward()
{
    driveLeft.run(FORWARD);
    driveRight.run(FORWARD);

    delay(1000);

    driveRight.run(RELEASE);
    driveLeft.run(RELEASE);
}

inline void detectColor()
{
    uint16_t r, g, b, c, colorTemp, lux;

    tcs.getRawData(&r, &g, &b, &c);
    // colorTemp = tcs.calculateColorTemperature(r, g, b);
    colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
    lux = tcs.calculateLux(r, g, b);

#ifdef DEBUG
    Serial.print("Color Temp: ");
    Serial.print(colorTemp, DEC);
    Serial.print(" K - ");
    Serial.print("Lux: ");
    Serial.print(lux, DEC);
    Serial.print(" - ");
    Serial.print("R: ");
    Serial.print(r, DEC);
    Serial.print(" ");
    Serial.print("G: ");
    Serial.print(g, DEC);
    Serial.print(" ");
    Serial.print("B: ");
    Serial.print(b, DEC);
    Serial.print(" ");
    Serial.print("C: ");
    Serial.print(c, DEC);
    Serial.print(" ");
    Serial.println(" ");
#endif
}

inline void motorSetup()
{
    sweeperMotor.attach(9);
    collapseMotor.attach(10);

    sweeperMotor.write(sweeperOriginalPos);
    collapseMotor.write(servoPosMin);

    driveLeft.setSpeed(driveSpeed);
    driveRight.setSpeed(driveSpeed);
}

inline void colorSensorSetup()
{
    if (tcs.begin())
    {
        Serial.println("Found sensor");
    }
    else
    {
        Serial.println("No TCS34725 found ... check your connections");
        while (1)
            ;
    }
}

inline void sweep(int direction)
{
    if (direction == UP)
    {
#ifdef DEBUG
        Serial.print("Moving arm up");
#endif
        delay(10);
        sweeperMotor.write(sweeperPos + sweeperStep);
        sweeperPos += sweeperStep;
    }
    else if (direction == DOWN)
    {
#ifdef DEBUG
        Serial.print("Moving arm down");
#endif
        delay(10);
        sweeperMotor.write(sweeperPos - sweeperStep);
        sweeperPos -= sweeperStep;
    }
}