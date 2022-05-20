// MASSPUNKT 6CM FRÅN BOTTEN
// Meter per steg 0.075*3.14/200 = 0.0011775;

#include "pid.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include <AccelStepper.h>

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// To recieve data
const byte numChars = 32;
char receivedChars[numChars];

bool GUI = true;
bool logging = false;

float controller_delay = 0;
float Kp = 25*pow(10, -2);
float Ki = 0*pow(10, -2);
float Kd = 20*pow(10, -2);

float acc;
int elapsed_time = 1;
int new_time = 1; 
int last_time = 0;
float vel;
String input_GUI; 

int new_time_print = 0;
int last_time_print = 0;
int elapsed_time_print = 0;

int step_pin = 4;
int dir_pin = 5;

float input;

MPU6050 mpu;
AccelStepper stepper(AccelStepper::DRIVER, step_pin, dir_pin);
PID pid_obj(Kp, Ki, Kd);

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
 
//timers
long time1Hz = 0;
long time5Hz = 0;

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady()
{
    mpuInterrupt = true;
}

void setup()
{
    stepper.setMaxSpeed(3200);
    stepper.setSpeed(0);
    
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize serial communication
    // (115200 chosen because it is required for Teapot Demo output, but it's
    // really up to you depending on your project)
    Serial.begin(115200);
    while (!Serial); // wait for Leonardo enumeration, others continue immediately

    // initialize device
    Serial.println(F("Initializing I2C devices..."));
    Serial.println("Time, Input, Acc, Vel");
     
    mpu.initialize();

    // verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (devStatus == 0)
    {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    }
    else
    {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }
}

void loop()
{
    // Run stepper motor
    stepper.runSpeed();

    // Logic for logging data for analysis
    if(logging == true){
      new_time_print = millis();
      elapsed_time_print = (new_time_print - last_time_print);
      if(elapsed_time_print > 2){
        Serial.print(new_time_print);
        Serial.print(",");
        Serial.print(input);
        Serial.print(",");
        Serial.print(acc);
        Serial.print(",");
        Serial.print(vel);
        Serial.print("\n");
        last_time_print = new_time_print;
      } 
    }

    // Logic for comunication with Processing GUI
    if(GUI == true){
      new_time_print = millis();
      elapsed_time_print = (new_time_print - last_time_print);
      if(elapsed_time_print > 100){
        if(Serial.available()){          
          input_GUI = Serial.readStringUntil(" ");    
          char Buf[5];
          input_GUI.toCharArray(Buf, 50);        
          char id = Buf[0];          
          int x = (int)Buf[1] - 48;
          int y = (int)Buf[2] - 48;
          int val = x*10+y;
          
          if(String(id) == "P"){
            Serial.print("Kp changed to: ");
            Serial.print(val);
            Serial.print("\n");
            pid_obj.Kp = val*pow(10, -2);
          }
          else if(String(id) == "I"){
            Serial.print("Ki changed to: ");
            Serial.print(val);
            Serial.print("\n");
            pid_obj.Ki = val*pow(10, -2);
          }else if(String(Buf[0]) == "D"){
            Serial.print("Kd changed to: ");
            Serial.print(val);
            Serial.print("\n");
            pid_obj.Kd = val*pow(10, -5);
          }
          else if(String(Buf[0]) == "T"){
            Serial.print("T changed to: ");
            Serial.print(val*10);
            Serial.print("\n");
            controller_delay = val*pow(10, -2);
          }
        }                             
        last_time_print = new_time_print;
        };
     };
    
    // if programming failed, don't try to do anything
    if (!dmpReady) return;

    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize)
    {
        //Update PID and set new speed accordingly
        acc = pid_obj.update(input);
       
        new_time = micros();        
        elapsed_time = (new_time - last_time);

        if(abs(elapsed_time) > controller_delay){
          //vel = vel + acc * elapsed_time;
          vel = vel + acc * elapsed_time/1000;
          vel = constrain(vel, -1500, 1500);

          // Transform from meter/second to steps/second
          //float meter_per_step = 0.0011775;
          //float motor_vel = -vel/meter_per_step;

          stepper.setSpeed(-vel);  
          last_time = micros();
          // Run stepper motor
          stepper.runSpeed();
        }       
    }
      
    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024)
    {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    }
    else if (mpuIntStatus & 0x02)
    {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;

        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

        input = ypr[1] * 180/M_PI * 0.6; // Adjusted manually
   }
}
