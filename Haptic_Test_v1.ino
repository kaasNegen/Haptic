// PWM pins on UNO: 3, 5, 6, 9, 10, and 11.
int pwmPins[] = {3, 5, 6, 9, 10, 11};
int pinCount = 6;


void setup() { 
  for (int thisPin = 0; thisPin < pinCount; thisPin++) {
    pinMode(pwmPins[thisPin], OUTPUT);
  }
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Insert connected PWM Pin for Haptic Sensor");
} 

 
void loop() { 
  if (Serial.available()) {    
    int hapticPin = Serial.parseInt();
    
    Serial.println("Running tests on pin: ");
    Serial.println(hapticPin);
    Serial.println("\nIncreasing vibration");
    for (int i=0; i <= 255; i++){
      analogWrite(hapticPin, i);
      delay(10);
    }
    Serial.println("Decreasing vibration");
    for (int i=255; i >= 0; i--){
      analogWrite(hapticPin, i);
      delay(10);
    }

   Serial.println("Testing finsihed, feel free to select another PWM Pin. \n \n");
  }
} 
