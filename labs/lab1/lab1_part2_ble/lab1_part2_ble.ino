
String str = "";
String return_str = "XX";
void setup() {
  
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  str = Serial.readString();

  for (int i = 0; i < str.length(); i++) {
    return_str[0] = str[i];
    Serial.println(return_str);
  }
  
}
