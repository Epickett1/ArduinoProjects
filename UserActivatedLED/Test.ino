int brightness = 5;
int fade_amount = 5;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(LED_BUILTIN, brightness);
  if (brightness < 5 || brightness > 250) fade_amount = -fade_amount;
  brightness += fade_amount;
  delay(30);
}
