
int var = 0;
int varScaled = 0;

void setup() {

  // set output pins
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
 // DDRB = DDRB | B111100; //same as the code above

  // set input pins
  pinMode(A0, INPUT);
  
}


void loop() {

  PORTB = PORTB | B111100;
  //digitalWrite(13, HIGH);
  delay(1000);
  
  PORTB = PORTB & B11000000;
  //digitalWrite(13, LOW);
  delay(1000);


  // --------------------------------------

  // reading and printing pot value
  var = analogRead(A0);
  Serial.println(var);

  // map 10bit value to 4bit value
  varScaled = map(var, 0, 1023, 0, 15);
  Serial.println(varScaled);
  
  varScaled = (varScaled << 2);

  // needed so the two high bits are not changed (they are mapped to the crystal pins) 
  varScaled |= (B11 << 6);

  // set the pins
  PORTB = (varScaled << 2);
}
