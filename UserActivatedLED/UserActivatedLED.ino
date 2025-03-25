"""
Estevan Pickett

This program allows users to control an LED through serial commands. 
It supports simple commands such as turning the LED on or off, 
as well as more complex behaviors like blinking, strobing, and 
fading. Commands are entered via the serial monitor and must be 
separated by semicolons (;).
"""

#include <Arduino.h>
#include <limits.h> // Include this to ensure the constants are defined correctly

// Define LONG_MAX and LONG_MIN if they are not already defined
#ifndef LONG_MAX
#define LONG_MAX 2147483647L
#endif

const int MIN_FADE = 0;
const int MAX_FADE = 255;

int led = LED_BUILTIN; // = 13;
int fade_increment = 5;
int command_count = 0;
bool activated = false;

// time variables
long blink_interval = 500;
long blink_duration = 3000;
long strobe_interval = 50;
long strobe_duration = 3000;

// single word commands
String valid_commands[] = {"on", "off", "blink", "strobe"};

void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW); // light starts off
  Serial.print("\nEnter a command (or '0' for more information):  ");
}

void displayCommandInfo(){
  Serial.println("\n                   *** Command Procedures ***");
  Serial.println("Commands must be separated by a semicolon (;). The last command must also");
  Serial.println("end with a semicolon. Commands can be entered in either upper or lower case"); 
  Serial.println("fashion. Some commands require two inputs, which must be separated by a space.");
  Serial.println("If the commands start with a semicolon or if there are consecutive semicolons");
  Serial.println("throughout the command, the execution will continue as expected.");
  Serial.println("\nPossible Commands:");
  Serial.println("  ON                   - Turns light on");
  Serial.println("  OFF                  - Turns light off");
  Serial.println("  BLINK                - Turns light on and off in blinking fashion");
  Serial.println("  STROBE               - Turns light on and off in strobing fashion");
  Serial.println("  FADE ON/OFF          - Fades light on or off depending on state specified");
  Serial.println("  WAIT *TIME(milli)*   - Delays the system a certain amount of milliseconds before continuing");
  Serial.println("\nExample Commands:");
  Serial.println("  ON;WAIT 500;OFF;FADE ON;");
  Serial.println("  strobe; blink; off; fade off;");
  Serial.println("  ;On; fade on; wait 30;; strobe ;");
}

bool parseInput(String input, String commands[]){
  if (input == "0"){  // user guide
    displayCommandInfo();
    return false;
  }
  if (input.length() < 1 || input[input.length()-1] != ';') { // command too short or doesn't end with semicolon
    Serial.println("\nERROR: invalid command");
    return false;
  }
  // split input at semi-colons
  int start = 0; 
  command_count = 0;
  for (int i = 0; i < input.length(); i++){
    if (input[i] == ';'){
      if (start != i){    // skip over semicolons only
        commands[command_count] = input.substring(start, i);
        commands[command_count].trim();
        command_count++;
      }
      start = i + 1;
    }
  }
  bool good_commands = true;
  for (int x = 0; x < command_count; x++){  // split commands into words
    String comm = commands[x];
    String temp[30];
    int word_count = 0;
    int start_word = 0;
    for (int l = 0; l < comm.length(); l++){
      if (comm[l] == ' '){
        if (start_word !=  l){ // only count word if not space only
          temp[word_count] = comm.substring(start_word, l);
          word_count++;
        }
        start_word = l + 1;
      }
      if (l + 1 == comm.length() && start_word != l){  // end of string
        temp[word_count] = comm.substring(start_word, comm.length());
        word_count++;
      }
    }
    if (word_count == 1){     // commands with one word
      bool found = false;
      for (String com : valid_commands){
        if (commands[x] == com)
          found = true;
      }
      if (!found)
        good_commands = false;
    }
    else if (word_count == 2){  // commands with two words
      if (temp[0] != "fade" && temp[0] != "wait")
        good_commands = false;
      else if (temp[0] == "fade"){
        if (temp[1] != "on" && temp[1] != "off"){
          good_commands = false;
        }
      }
      else if (temp[0] == "wait"){
        if (!isValidLong(temp[1])){
          good_commands = false;
        }
      }
    }
    else {                  // fauty commands
      good_commands = false;
    }
  }
  if (command_count < 1)  // check for empty input
    good_commands = false;

  if (good_commands)
    Serial.println("\nProcessing Commands...");
  else
    Serial.println("\nERROR: invalid command");
  return good_commands;
}

bool isValidLong(const String& str) {
  char* end;
  long value = strtol(str.c_str(), &end, 10);

  // Check if the entire string was converted and if the value is within the range of a long
  return (*end == '\0') && (value >= 0) && (value <= LONG_MAX);
}

void lightOnOff(unsigned long interval, unsigned long duration){
  unsigned long start = millis();
  while (millis() - start < duration){
    digitalWrite(led, HIGH);
    delay(interval);
    digitalWrite(led, LOW);
    delay(interval);
  }
}

void wait(String command){
  String num = "";
  String numbers = "0123456789";
  for (char c : command){
    if (numbers.indexOf(c) != -1){
      num.concat(c);
    }
  }
  delay(num.toInt());
}

void fade(String command){
    int fade;
    int start;
    if (command.indexOf("on") != -1){
      fade = fade_increment;
      start = MIN_FADE;
    } else {
      fade = -fade_increment;
      start = MAX_FADE;
    }
    for (int brightness = start; MIN_FADE <= brightness && brightness <= MAX_FADE; brightness += fade){
      analogWrite(led, brightness);
      delay(30);
    }
}

void readCommand(String command){
  if (command == "on") 
    digitalWrite(led, HIGH);
  else if (command == "off") 
    digitalWrite(led, LOW);
  else if (command == "blink")
    lightOnOff(blink_interval, blink_duration);
  else if (command == "strobe")
    lightOnOff(strobe_interval, strobe_duration);
  else if (command.indexOf("wait") != -1) {
    wait(command);
  } else if (command.indexOf("fade") != -1){
    fade(command);
  }
}

void loop() {
  // Check if data is available to read from the serial port
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    input.toLowerCase();
    Serial.println(input);
    String commands[100];
    if (parseInput(input, commands)){
      for (int i = 0; i < command_count; i++){
        readCommand(commands[i]);
      }
      Serial.println("Commands Successfully Executed!");
    }
    delay(30);  
    Serial.print("\nEnter a command (or '0' for more information):  ");
  }
}
