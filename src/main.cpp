#include <AFMotor.h>
#include <Arduino.h>
#include <Servo.h>

// Ports 1 & 2 - High speed ports, can run upto 64KHz (For motors 1 and 2 you can choose MOTOR12_64KHZ, MOTOR12_8KHZ, MOTOR12_2KHZ, or MOTOR12_1KHZ)
// Ports 3 & 4 - Can only run at 1KHz (Motors 3 & 4 are only possible to run at 1KHz and will ignore any setting given)

AF_DCMotor driveLeft(1, MOTOR12_64KHZ);  // create driveLeft #1, 64KHz pwm
AF_DCMotor driveRight(2, MOTOR12_64KHZ); // create driveLeft #2, 64KHz pwm
AF_Stepper armMotor(90, 2);
Servo servo1;
Servo servo2;

const int servoPosMax = 170;
const int servoPosMin = 10;

void setup()
{
    Serial.begin(9600); // set up Serial library at 9600 bps
    servo1.attach(9);
    servo2.attach(10);
    servo1.write(servoPosMin);
    servo2.write(servoPosMin);

    driveLeft.setSpeed(200);  // set the speed to 200/255 (max speed is 255)
    driveRight.setSpeed(200); // set the speed to 200/255 (max speed is 255)
}

void loop()
{
    driveLeft.run(FORWARD); // turn it on going forward
    driveRight.run(FORWARD);

    for (int i = servoPosMin; i < servoPosMax; i++)
    {
        servo1.write(i);
        delay(500);
    }

    delay(1000);

    driveLeft.run(BACKWARD); // the other way
    driveRight.run(BACKWARD);

    delay(1000);

    driveLeft.run(RELEASE); // stopped
    driveRight.run(RELEASE);
    
    for (int i = servoPosMin; i < servoPosMax; i++)
    {
        servo2.write(i);
        delay(500);
    }
    delay(1000);
}