//pins for stepper drivers
const int stepPin = 5;
const int stepPin2 = 6; 
const int dirPin = 2; 
const int dirPin2 = 3;
const int enPin = 8;
const int enPin2 = 9;

//Integers for which motor to spin and how many times to spin it
int motor;
int spins;

//Constants for speed and duration of spin, currently about 360 degrees
const int circle = 200;
const int del = 2500;
void setup() {
  

  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(enPin2, OUTPUT);
  pinMode(enPin,OUTPUT);

  //led for debugging
  pinMode(13, OUTPUT);

  //enable the motors to turn
  digitalWrite(enPin,LOW);
  digitalWrite(enPin2, LOW);

  Serial.begin(9600);
  motor = 0;
  spins = 1;
 
  
}
void loop() {
  //set the motors to turn clockwise
  digitalWrite(dirPin,LOW); 
  digitalWrite(dirPin2, LOW);
  
  digitalWrite(13, LOW);
  
  //listen for incoming serial
  if(Serial.available()){
    motor = Serial.read();
  }
    
  //spins motor a
  if(motor == 97){
    for(int x = 0; x < circle*spins; x++) {
      digitalWrite(13, HIGH);
      digitalWrite(stepPin,HIGH); 
      delayMicroseconds(del);
      digitalWrite(stepPin,LOW); 
      delayMicroseconds(del);  
    }
    //Tell the DE1 there was a succesful rotation
    Serial.write('D');
  }
  //spin motor b
  else if(motor == 98){
    for(int x = 0; x < circle*spins; x++) {
      digitalWrite(13, HIGH);
      digitalWrite(stepPin2,HIGH);
      delayMicroseconds(del);
      digitalWrite(stepPin2,LOW); 
      delayMicroseconds(del); 
    }
    //Tell the DE1 there was a succesful roation
    Serial.write('D');
  }
  
  //Test startup sequence to confirm that serial works
  else if(motor == 'T'){
    Serial.write('S');
  }

  motor = 0;
  
}
