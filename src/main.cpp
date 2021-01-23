#include <Arduino.h>
#include <Servo.h>
#include <AFMotor.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>

// Ports 1 & 2 - High speed ports, can run upto 64KHz (For motors 1 and 2 you can choose MOTOR12_64KHZ, MOTOR12_8KHZ, MOTOR12_2KHZ, or MOTOR12_1KHZ)
// Ports 3 & 4 - Can only run at 1KHz (Motors 3 & 4 are only possible to run at 1KHz and will ignore any setting given)

AF_DCMotor driveLeft(1, MOTOR12_64KHZ);  // create driveLeft #1, 64KHz pwm
AF_DCMotor driveRight(2, MOTOR12_64KHZ); // create driveLeft #2, 64KHz pwm

AF_Stepper armMotor(48, 2);
Servo servo1;
Servo servo2;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

const int servoPosMax = 170;
const int servoPosMin = 10;

void setup()
{
    Serial.begin(9600); // set up Serial library at 9600 bps

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

    //servo1.attach(9);
    //servo2.attach(10);
    //servo1.write(servoPosMin);
    //servo2.write(servoPosMin);

    driveLeft.setSpeed(0); // set the speed to 200/255 (max speed is 255)

    driveRight.setSpeed(0); // set the speed to 200/255 (max speed is 255)

    armMotor.setSpeed(350);
    armMotor.onestep(FORWARD, DOUBLE);

    armMotor.release();
}

void loop()
{

    uint16_t r, g, b, c, colorTemp, lux;

    tcs.getRawData(&r, &g, &b, &c);
    // colorTemp = tcs.calculateColorTemperature(r, g, b);
    colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
    lux = tcs.calculateLux(r, g, b);

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
    delay(500);
    /*
    driveLeft.run(FORWARD); // turn it on going forward
    driveRight.run(FORWARD);
    armMotor.step(1000, FORWARD, DOUBLE);

    for (int i = servoPosMin; i < servoPosMax; i++)
    {
        //servo1.write(i);
        delay(10);
    }

    delay(15000);

    driveLeft.run(BACKWARD); // the other way
    driveRight.run(BACKWARD);

    delay(1000);

    driveLeft.run(RELEASE); // stopped
    driveRight.run(RELEASE);

    for (int i = servoPosMin; i < servoPosMax; i++)
    {
        //servo2.write(i);
        delay(10);
    }
    delay(1000);*/
}