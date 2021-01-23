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

#define ledPin 2
#define buzzerPin A2

AF_DCMotor driveLeft(2, MOTOR12_64KHZ);  // create driveLeft #1, 64KHz pwm
AF_DCMotor driveRight(1, MOTOR12_64KHZ); // create driveLeft #2, 64KHz pwm

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
const int sweeperStep = 45;

// Keep track of the position of the sweeper
int sweeperPos = sweeperOriginalPos;


// Speed the motors should attempt to run at (0 < driveSpeed < 255)
const int driveSpeed = 120;

int driveDelay = 300;

int vecDelta = 1;

double redVec[] = {0,0,0,0};
double blueVec[] = {0,0,0,0};

inline double vecDistance(double [], double []);

// Functions to allow easy testing of individual components
// Using inline to not lose processes to function calls
inline void motorSetup();
inline void colorSensorSetup();

inline void driveLoopF();
inline void driveForward();
inline void driveBackward();
inline void driveRelease();

inline void sweep(int);
inline void detectColor();
inline void spray();

void calibrate(){
    String incommingString = "";
    Serial.println("Enter Option: ");
    while(true){
        if (Serial.available() > 0) {
            incommingString = Serial.readString();

            if (incommingString = "exit") {
                break;
            } else if (incommingString == "blue") {
                int* ret = readColor();

                for (int i = 0; i < 4; i++){
                    blueVec[i] = ret[i];
                } 
            } else if (incommingString == "red") {
                int* ret = readColor();

                for (int i = 0; i < 4; i++){
                    redVec[i] = ret[i];
                } 
            } else if (incommingString == "delta") {
                Serial.println("Enter delta: ");
                int incommingByte = 0;
                while (true) {
                    if (Serial.available() > 0) {
                        vecDelta = Serial.read();
                       break;
                    }
                }
            } else if (incommingString == "delay") {
                Serial.println("Enter delay: ");
                int incommingByte = 0;
                while (true) {
                    if (Serial.available() > 0) {
                        driveDelay = Serial.read();
                       break;
                    }
                }
            }
        }
    }
}


void setup()
{
    Serial.begin(9600); // set up Serial library at 9600 bps
    calibrate();
    colorSensorSetup();
    motorSetup();
}

void loop()
{
    
    detectColor();
    sweep(UP);
    detectColor();
    sweep(UP);
    detectColor();
    driveLoopF();
    detectColor();
    sweep(DOWN);
    detectColor();
    sweep(DOWN);
    detectColor();
    driveLoopF();
    detectColor();
    sweep(UP);
    detectColor();
    sweep(UP);
    detectColor();

    
    
    
}

inline void driveLoopF()
{

    driveForward();

    delay(driveDelay);

    driveRelease();

}

int* readColor() {
    uint16_t r, g, b, c, colorTemp, lux;

    tcs.getRawData(&r, &g, &b, &c); 
    int* val = {r,g,b,c};
    return val;
}

inline void detectColor()
{
    uint16_t r, g, b, c, colorTemp, lux;

    tcs.getRawData(&r, &g, &b, &c);
    // colorTemp = tcs.calculateColorTemperature(r, g, b);
    //colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
    //lux = tcs.calculateLux(r, g, b);
    int colSum = r+g+b+c;
    double colVec[] = {((double)r)/colSum, 
                        ((double)g)/colSum, 
                        ((double)b)/colSum, 
                        ((double)c)/colSum};

    if (vecDistance(colVec, redVec) < vecDelta) {
        analogWrite(ledPin, HIGH);
        analogWrite(buzzerPin, HIGH);
        delay(1500);
        analogWrite(ledPin, LOW);
        analogWrite(buzzerPin, LOW);
    } else if (vecDistance(colVec, blueVec) < vecDelta) {
        analogWrite(ledPin, HIGH);
        delay(1500);
        analogWrite(ledPin, LOW);
    }

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
    sweeperMotor.attach(10);
    collapseMotor.attach(9);
    delay(1000);
    Serial.print(sweeperMotor.read());

    sweeperMotor.write(sweeperOriginalPos);
    //collapseMotor.write(servoPosMin);
    
    driveLeft.run(RELEASE);
    driveRight.run(RELEASE);

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

inline void driveForward(){
    driveLeft.run(FORWARD);
    driveRight.run(BACKWARD);
}

inline void driveBackward(){
    driveLeft.run(BACKWARD);
    driveRight.run(FORWARD);
}

inline void driveRelease(){
    driveRight.run(RELEASE);
    driveLeft.run(RELEASE);
}

inline double vecDistance(double a[], double b[]){
    return sqrt(pow((a[0]-b[0]),2)+pow((a[1]-b[1]),2)+pow((a[2]-b[2]),2) + pow((a[3]-b[3]),2));
}