// defines pins numbers
const int stepPin = 5;
const int stepPin2 = 6; 
const int dirPin = 2; 
const int dirPin2 = 3;
const int enPin = 8;
const int enPin2 = 9;
int motor;
int spins;
const int circle = 200;
const int del = 2500;
void setup() {
  
  // Sets the two pins as Outputs
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(enPin2, OUTPUT);
  pinMode(13, OUTPUT);

  pinMode(enPin,OUTPUT);
  digitalWrite(enPin,LOW);
  digitalWrite(enPin, LOW);

  Serial.begin(9600);
  motor = 0;
  spins = 1;
 
  
}
void loop() {
  digitalWrite(dirPin,LOW); // Enables the motor to move in a particular direction
  digitalWrite(dirPin2, LOW);
  digitalWrite(13, LOW);
  if(Serial.available()){
    motor = Serial.read();
  }
    

  if(motor == 97){
    //digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
    // Makes 200 pulses for making one full cycle rotation
    for(int x = 0; x < circle*spins; x++) {
      digitalWrite(13, HIGH);
      digitalWrite(stepPin,HIGH); 
      delayMicroseconds(del);
      digitalWrite(stepPin,LOW); 
      delayMicroseconds(del);  
    }
    Serial.write('D');
  }
  else if(motor == 98){
    for(int x = 0; x < circle*spins; x++) {
      digitalWrite(13, HIGH);
      digitalWrite(stepPin2,HIGH);
      delayMicroseconds(del);
      digitalWrite(stepPin2,LOW); 
      delayMicroseconds(del); 
    }
    Serial.write('D');
  }
  motor = 0;
  
  
}
