#include <Servo.h> 

// Serial debug. Set to 1 to debug.
#define SERIAL_DEBUG 1

// Servo objects
Servo spray1Servo;
Servo spray2Servo;

// Servo pins
int spray1Pin = 10;
int spray2Pin = 11;

// Stepper pins
int iDirX = 6;
int iStepX = 7;
int iDirY = 8;
int iStepY = 9;

// Stepper step delay (less delay = faster)
int iStepDelay = 2;
// Serial parsing string
String txtMsg;
// Stepper motor move positions
int cnt;
int iH;
int iV;
int iNextPosX = 0; // or initial position e.g. 50
int iNextPosY = 0; // or initial position e.g. 50

// set origin
int iCurrPosX = 64; //cm
int iCurrPosY = 64; //cm

// multiplier ~ turn cm into steps
int iCoordMult = 47; // (46.94836 steps move 1cm)


// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  // initialize digital pin 13 as an output.

  pinMode(iDirX, OUTPUT);
  pinMode(iStepX, OUTPUT);
  pinMode(iDirY, OUTPUT);
  pinMode(iStepY, OUTPUT);
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
  setHigh(iDirX);
  setHigh(iDirY);
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
    setHigh(iDirX);
    turnStepper(iStepX); 
    setHigh(iDirY);
    turnStepper(iStepY);
    
    setLow(iDirX);
    turnStepper(iStepX); 
    setLow(iDirY);
    turnStepper(iStepY);    
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
   // x,y coordinates
   if(txtMsg.indexOf(",") > 0) { // we have a coordinate pair, move stepper
     parseXY(txtMsg);
     txtMsg = "";
   }
   txtMsg = ""; 
}

void parseXY(String txtMsg) {
  String strDelim = ","; // TODO move to config file
  int iPos = txtMsg.indexOf(strDelim);
  String strCoord = txtMsg.substring(0, iPos);
  int iX = strCoord.toInt();
  strCoord =  txtMsg.substring(iPos+1);
  int iY = strCoord.toInt();
  setPos(iX, iY);
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
void setPos(int iX, int iY) {
  // calculate new hypotenuse (length of timing belt
  // Origin (0,0) set at top left
  if(SERIAL_DEBUG) {
    Serial.print("x = ");
    Serial.println(iX);
    Serial.print("y = ");
    Serial.println(iY); 
  }
  windX(iX);
  windY(iY);
}

void moveX(int iX) {
  // Subtract the required position from the current position
  // e.g. current = 25, required = 10 ~ move = required - current = -15
  windX(iX - iCurrPosX);
  iCurrPosX = abs(iX); // e.g. current position after move ~ 
}

void windX(int iX) {
  if(iX > 0) {
    setHigh(iDirX);
  } else {
    setLow(iDirX);
  }
  int iTotalSteps = abs(iX - iCurrPosX) * iCoordMult;
  for(int i = 0; i <= iCoordMult; i++) {
    oneStep(iStepX);
  }
  // keep track of where we are
  iCurrPosX = abs(iX);
}

void windY(int iY) {
  if(iY > 0) {
    setHigh(iDirY);
  } else {
    setLow(iDirY);
  }
  int iTotalSteps = abs(iY - iCurrPosY) * iCoordMult;
  for(int i = 0; i <= iCoordMult; i++) {
    oneStep(iStepY);
  }
  // keep track of where we are
  iCurrPosY = abs(iY);
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

void unWindX(int iDistance) {
  setLow(iDirX);
  oneStep(iStepX);
}

void unWindY(int iDistance) {
  setHigh(iDirY);
  oneStep(iStepY);
}

void oneStep(int iStepPin) {
    digitalWrite(iStepPin, HIGH);
    delay(iStepDelay);
    digitalWrite(iStepPin, LOW);
    delay(iStepDelay);  
}
