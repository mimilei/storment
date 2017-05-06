#include <Wire.h>

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

const int doorLatch = 13;
const int airPump = 11;

long startTime; // Time door was closed
int pos = 0;    // variable to store the servo position
int prevDoorState;

void setup() {
  Serial.begin(9600);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  myservo.write(0);
  pinMode(doorLatch, INPUT_PULLUP);
  pinMode(airPump, OUTPUT);
  Wire.begin(); // for I2C

  digitalWrite(airPump, LOW);
//  digitalWrite(commToLightning, LOW);
}

void pressVape(int delayFreq) {
  Serial.println("In PressVape");
  Serial.println(delayFreq);
//  analogWrite(airPump, 255);
  for (pos = 0; pos <= 40; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(3);                       // waits 15ms for the servo to reach the position
  }
  delay(5000);
  for (pos = 40; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(3);                       // waits 15ms for the servo to reach the position
  }
//  analogWrite(airPump, 0);
  delay(delayFreq);
}

int mode(int arr[], int num_vals) {
  int counts[num_vals] = {0};
  for (int i = 0; i < sizeof(arr)/sizeof(arr[0]); i++) {
    counts[arr[i]]++;
  }
  int curr_mode = 0;
  for (int i = 0; i < num_vals; i++) {
    if (counts[i] > counts[curr_mode]) {
      curr_mode = i;
    }
  }
  return curr_mode;
}

void loop() {
  int sensorInfo[3];
  sensorInfo[0] = digitalRead(doorLatch);
  sensorInfo[1] = digitalRead(doorLatch);
  sensorInfo[2] = digitalRead(doorLatch);
  int doorState = mode(sensorInfo, 3);

  if (doorState == HIGH) {
    Serial.println("DOOR OPEN");
  }

  if (doorState != prevDoorState) {
    if (doorState == LOW) {
      startTime = millis() / 60000.;
      Serial.println("Open to closed!");
//      digitalWrite(commToLightning, HIGH);
      Wire.beginTransmission(9);
      Wire.write(1);
      Wire.endTransmission();
    } 
    else {
//      digitalWrite(commToLighting, LOW);
      Serial.println("Closed to open");
      Wire.beginTransmission(9);
      Wire.write(0);
      Wire.endTransmission();
    }
  }

  prevDoorState = doorState;

  if (doorState == LOW) {
//    int hoursElapsed = (int) ((millis() - startTime) / 3.6e+6);
//    digitalWrite(commToLightning, HIGH);
    Wire.beginTransmission(9);
    Wire.write(1);
    Wire.endTransmission();
    Serial.println("DOOR CLOSED");
    int currTime = millis() / 60000.;
//    Serial.print("currTime = ");
//    Serial.println(currTime);
//    Serial.print("starttime = ");
//    Serial.println(startTime);
    
    int minutesElapsed = (int) (currTime - startTime);
//    Serial.print(minutesElapsed);
//    Serial.println(" minutes elapsed");
//    Serial.print(hoursElapsed);
//    Serial.println(" hours elapsed");
    analogWrite(airPump, 255); // Speed is value between 0 (stop) and 255 (full speed)
//    pressVape((int) (5000 / (hoursElapsed / 8.))); 
    int delayTime = map(minutesElapsed, 0, 15, 10000, 2000);
//    pressVape(3000);
    pressVape(delayTime);
    analogWrite(airPump, 0);
  }

  delay(500);
}

