int brightness = 5;
int fade_amount = 5;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(LED_BUILTIN, brightness);
  brightness += fade_amount;
  if (brightness <= 0) {
    brightness = 0;
    fade_amount = -fade_amount;
  }
  else if (brightness >= 255){
    brightness = 255;
    fade_amount = -fade_amount;
  }
  delay(30);
}
