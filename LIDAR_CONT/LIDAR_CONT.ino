/* 
What this does:

This sketch controls a LIDAR-Lite/pan and tilt aparatus and sends data to a companion plotting script written in Python. 
This sketch assumes servos with rotation > 180 degrees, but not a continuous rotation servo. Indeed, a better solution 
might be to use a continuous rotation servo (as with RPLidar/Neato/etc); however, precise servo position sensing was a little 
tricky in my attempts. To get around this, I suggest using an additional positioning mechanism, of which there are probably many. 
A switch that gets triggered once per rotation at a fixed direction, for example. A physical switch would work, but it might 
better to add a barrier that is nearer the LIDAR optics than the defined minimum range. Position could then be very closely 
approximated through dead reckoning. 

If everything works you should have a little "Johnny 5" looking thing that does a very slow "nodding" of its "head", while rapidly 
shaking its "head" back and forth. 

It utilizes the 'Arduino I2C Master Library' from DSS Circuits:
http://www.dsscircuits.com/index.php/articles/66-arduino-i2c-master-library 



*/
#include <I2C.h>
#include <Wire.h>
#include <LIDARLite.h>
#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.
#include <Servo.h> 
//TODO: Dynamic range control
#define MIN_RANGE 200   //Minimum range to be transmitted to companion script
#define MAX_RANGE 1000  //Maximum range to be transmitted to companion script
LIDARLite myLidarLite;

int reading = 0;
Servo myservo;  // create servo object to control a servo 
Servo myservo2;                // a maximum of eight servo objects can be created 
 
int pos = 0;    // variable to store the servo position 
int posy = 0;
void setup(){
   Wire.begin(); // join i2c bus
  Serial.begin(9600); // start serial communication at 9600bps
  delay(100); // Waits to make sure everything is powered up before sending or receiving data  
  myservo.attach(10);  // attaches the servo on pin 9 to the servo object 
  myservo2.attach(9);
  //myservo.writeMicroseconds(1500);
 // delay(15);
}

void loop()
{ 
 
     myservo2.write(1700);
    for(pos = 80; pos < 160; pos += 1)  // goes from 0 degrees to 180 degrees 
    {                                  // in steps of 1 degree 
     //myservo.writeMicroseconds(1700);
      //myservo.write(0);
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      pollLidar(pos); 
      //delay(15);
      //myservo.writeMicroseconds(1500);
      //delay(1000);                       // waits 15ms for the servo to reach the position 
    } 
      posy += 1;
    //  myservo2.write(posy);
   for(pos = 160; pos>80; pos-=1)     // goes from 180 degrees to 0 degrees 
    {                              
      //myservo.writeMicroseconds(1300);
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      pollLidar(pos);
      //myservo.writeMicroseconds(1500);
      //delay(15);  
      //myservo.writeMicroseconds(1500);// waits 15ms for the servo to reach the position 
      //delay(1000);  
  }
    
     
    for(pos = 80; pos < 160; pos += 1)  // goes from 0 degrees to 180 degrees 
    {                                  // in steps of 1 degree 
     //myservo.writeMicroseconds(1700);
      //myservo.write(0);
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      pollLidar(pos); 
      //delay(15);
      //myservo.writeMicroseconds(1500);
      //delay(1000);                       // waits 15ms for the servo to reach the position 
    } 
      posy-=1;
      
   for(pos = 160; pos>80; pos-=1)     // goes from 180 degrees to 0 degrees 
    {                               
      //myservo.writeMicroseconds(1300);
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      pollLidar(pos);
      //myservo.writeMicroseconds(1500);
      //delay(15);  
      //myservo.writeMicroseconds(1500);// waits 15ms for the servo to reach the position 
      //delay(1000);  
    } 
}


void pollLidar(int pos){
 Wire.beginTransmission((int)LIDARLite_ADDRESS); // transmit to LIDAR-Lite
  Wire.write((int)RegisterMeasure); // sets register pointer to  (0x00)  
  Wire.write((int)MeasureValue); // sets register pointer to  (0x00)  
  Wire.endTransmission(); // stop transmitting

  delay(20); // Wait 20ms for transmit

  Wire.beginTransmission((int)LIDARLite_ADDRESS); // transmit to LIDAR-Lite
  Wire.write((int)RegisterHighLowB); // sets register pointer to (0x8f)
  Wire.endTransmission(); // stop transmitting

  delay(20); // Wait 20ms for transmit

  Wire.requestFrom((int)LIDARLite_ADDRESS, 2); // request 2 bytes from LIDAR-Lite

  if(2 <= Wire.available()) // if two bytes were received
  {
    
    reading = Wire.read(); // receive high byte (overwrites previous reading)
    reading = reading << 8; // shift high byte to be high 8 bits
    reading |= Wire.read(); // receive low byte as lower 8 bits
    if (reading>=MIN_RANGE && reading<=MAX_RANGE){
    Serial.print((pos-100));
    Serial.print(",");
    //Serial.print((posy-40));
    Serial.print(",");
    Serial.println(reading);
    //Serial.println("\n");// print the reading
  }
  }
}
