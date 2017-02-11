#include <Servo.h> 

// Servo objects
Servo spray1Servo;
Servo spray2Servo;

// Servo pins
int spray1Pin = 10;
int spray2Pin = 11;

// Stepper pins
int iDirA = 6;
int iStepA = 7;
int iDirB = 8;
int iStepB = 9;

// Stepper step delay (less delay = faster)
int iStepDelay = 2;
// Serial parsing string
String txtMsg;
// Stepper motor move positions
int pos [] = {0,0};    
int cnt;

int iH;
int iV;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  // initialize digital pin 13 as an output.

  pinMode(iDirA, OUTPUT);
  pinMode(iStepA, OUTPUT);
  pinMode(iDirB, OUTPUT);
  pinMode(iStepB, OUTPUT);
  // initSteppers();
  // manually home steppers 
 // init servos
 spray1Servo.attach(spray1Pin);
 spray2Servo.attach(spray2Pin);
 // switch off
 servoOff(spray1Servo);
 servoOff(spray2Servo);
}

void initSteppers() {
  setHigh(iDirA);
  setHigh(iDirB);
}

void setHigh(int iPin) {
  digitalWrite(iPin, HIGH);
}

void setLow(int iPin) {
  digitalWrite(iPin, LOW);
}

void turnStepper(int iStep) {
  for(int i = 0; i <= 400; i++) {
    digitalWrite(iStep, HIGH);
    delay(iStepDelay);
    digitalWrite(iStep, LOW);
    delay(iStepDelay); 
  }    
}

// the loop function runs over and over again forever
void loop() {
  /*
  while(true){
    setHigh(iDirA);
    turnStepper(iStepA); 
    setHigh(iDirB);
    turnStepper(iStepB);
    
    setLow(iDirA);
    turnStepper(iStepA); 
    setLow(iDirB);
    turnStepper(iStepB);    
  }
  */
  while (Serial.available() > 0) {
    char inChar = Serial.read();
    txtMsg += inChar;
  }
  // Delay used for human readability so we don't split messages
  // typed in Serial Monitor. 
  // If application at the other end can identify beginning and end
  // of message such as delimiters, delay is not required.
  delay(200);
  //Serial.println(analogRead(0));
  if (txtMsg.length() > 0) {
    
    Serial.print(txtMsg);
    Serial.println("*");
    processMsg(txtMsg);  
  }  
}

void processMsg(String &txtMsg) {
   txtMsg.trim();
   if(txtMsg == "S1ON") {
    servoOn(spray1Servo);
    txtMsg = "";
    return;
   }  
   if(txtMsg == "S1OFF") {
    servoOff(spray1Servo);
    txtMsg = "";
    return;
   } 
   if(txtMsg == "S2ON") {
    servoOn(spray2Servo);
    txtMsg = "";
    return;
   }  
   if(txtMsg == "S2OFF") {
    servoOff(spray2Servo);
    txtMsg = "";
    return;   
   }  
   if(txtMsg.substring(0,1) == "H" || txtMsg.substring(0,1) == "V") {
     setval(txtMsg);
     txtMsg = "";
     return;
   }
   txtMsg = ""; 
}

void setval(String txtMsg)
{
    String strStepper = txtMsg.substring(0,1);
    String pos = txtMsg.substring(1);
    if (strStepper == "H") {
      //myservoH.write(data);
      iH = pos.toInt();    
      cnt ++;
    }
    if (strStepper == "V"){
    //myservoV.write(data);
    iV = pos.toInt();
    cnt ++;
    }
    if (cnt % 2 == 0)
    {
    setPos(iH, iV);
    delay(15);
    }
}

// Move steppers so spray can goes to required spot
void setPos(int x, int y) {
  // Origin (0,0) set at top left
  Serial.print("x = ");
  Serial.println(x);
  Serial.print("y = ");
  Serial.println(y);  
}

void servoOn(Servo myServo) {
  /*
  int pos;
  for(pos = 0; pos <= 90; pos += 1) // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myServo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  */
  int pos = 90;
  myServo.write(pos);
  delay(15);
}

void servoOff(Servo myServo) {
  /*
  int pos;
  for(pos = 90; pos>=0; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    myServo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  */
  int pos = 0;
  myServo.write(pos);
  delay(15);
}



