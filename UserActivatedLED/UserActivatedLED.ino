int led = LED_BUILTIN; // = 13;
bool activated = false;

void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW); // light starts off
  Serial.print("\nEnter '1' to activate the light or '0' to deactivate it:  ");
}

void loop() {
  // Check if data is available to read from the serial port
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    Serial.println(input);
    if (isValid(input)){

      if (input.toInt()){ // '1' entered
        if (!activated){    // turn on light if not already on
          digitalWrite(led, HIGH);
          Serial.println("Light has been activated!");
          activated = true;
        } else {
          Serial.println("Light is already on!");
        }

      } else {  // '0' entered     
        if (activated){   // turn off light if not already off
          digitalWrite(led, LOW);
          Serial.println("Light has been deactivated!");
          activated = false;
        } else {
          Serial.println("Light is already off!");
        }
      }
    }
    delay(30);  
    Serial.print("\nEnter '1' to activate the light or '0' to deactivate it:  ");
  }
}

bool isValid(String input){
  if (input == "1" || input == "0") return true;
  Serial.println("** Invalid value entered **");
  return false;
}

void test(){
  
}
