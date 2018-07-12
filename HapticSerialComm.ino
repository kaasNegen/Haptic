/*
  Developer: Ruben Sponselee
  Serial communication code is inspired by examples given on: http://forum.arduino.cc/index.php?topic=396450.0
  Project on which the program is intergrated: SenseShift - Porject B
*/

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];      // temporary array for use when parsing

// variables to hold the parsed data
char messageFromPC[numChars] = {0};
int degreesFromPC;
int distanceFromPC;

boolean newData = false;

// PWM Pins with the haptic modules connected (left to right)
int pwmPins[] = {3, 5, 6, 9};
int usedPinCount = 4;

// Module configurable data
int degreesPerModule = 60; // one module is placed every x degrees.
int minDistance = 300; // in milimeters
int maxDistance = 10000;
int maxDegree = 180;
int updateInterval = 1000; // interval between processing data and getting ready for new data.

int usableDistance = maxDistance - minDistance;
int hapticSpeed;

//============

void setup() {
  for (int thisPin = 0; thisPin < usedPinCount; thisPin++) {
    pinMode(pwmPins[thisPin], OUTPUT);
  }
  Serial.begin(9600);
  // Enter data in this style: <Degrees[Int], Distance[Int]>
  Serial.println("A"); // Let python know that arduino is ready for data.
}

//============

void loop() {
  recvWithStartEndMarkers();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    // this temporary -local- copy is necessary to protect the original data
    // because strtok() used in parseData() replaces the commas with \0
    parseData();
    if (degreesFromPC == -1 || distanceFromPC == -1) {
      notifyInop(); // Notify user by sending specific pattern to haptic motors.
    } else {
      //showParsedData(); // Use only with Arduino Serial Monitor.
      for (int thisPin = 0; thisPin < usedPinCount; thisPin++) { // reset analogWrites
        analogWrite(pwmPins[thisPin], 0);
      }
      controlHapticModules(); // alter Haptic Modules only when new data is received
      newData = false;
      delay(updateInterval);
      Serial.println('A'); // lets python know that Arduino is Available for new data
    }
  }
}

//============

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      } else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    } else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

//============

void parseData() {      // split the data into its parts

  char * strtokIndx;    // this is used by strtok() as an index

  strtokIndx = strtok(tempChars, ",");       // get the first part - the string
  degreesFromPC = atoi(strtokIndx);          // convert this part to an integer

  strtokIndx = strtok(NULL, ",");         // this continues where the previous call left off
  distanceFromPC = atoi(strtokIndx);      // convert this part to an integer

}

//============

void showParsedData() {
  Serial.print("Degrees ");
  Serial.println(degreesFromPC);
  Serial.print("Distance ");
  Serial.println(distanceFromPC);
}

//============

void controlHapticModules() {
  // calculate hapticspeed as given by input
  if (distanceFromPC < minDistance) {
    return;
  } else {
    distanceFromPC -= minDistance;
  }

  //hapticSpeed = (distanceFromPC / usableDistance) * 255; // calculate a value between 0 and 255 depending on input
  hapticSpeed = map(distanceFromPC, 0, usableDistance, 0, 255);
  // control the modules
  // degree thresholds are dependent on amount of modules; a quarter of degree per module is start threshold,
  //    after that increments with half of the degrees per module and alternates between using 1 or 2 modules.
  if (degreesFromPC <= 15) {
    analogWrite(pwmPins[0], hapticSpeed);
  } else if (degreesFromPC <= 45) {
    analogWrite(pwmPins[0], hapticSpeed);
    analogWrite(pwmPins[1], hapticSpeed);
  } else if (degreesFromPC <= 75) {
    analogWrite(pwmPins[1], hapticSpeed);
  } else if (degreesFromPC <= 105) {
    analogWrite(pwmPins[1], hapticSpeed);
    analogWrite(pwmPins[2], hapticSpeed);
  } else if (degreesFromPC <= 135) {
    analogWrite(pwmPins[2], hapticSpeed);
  } else if (degreesFromPC <= 165) {
    analogWrite(pwmPins[2], hapticSpeed);
    analogWrite(pwmPins[3], hapticSpeed);
  } else {
    analogWrite(pwmPins[3], hapticSpeed);
  }
}

//============

void notifyInop() {
  for (i = 0; i < 255; i++) {
    analogWrite(pwmPins[0], i);
    analogWrite(pwmPins[1], i);
    analogWrite(pwmPins[2], i);
    analogWrite(pwmPins[3], i);
    delay(10);
  }
}
