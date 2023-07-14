#include <Arduino.h>

const int ECHO = 0;
const int TRIG = 1;
#define SOUND_SPEED 340
#define TRIG_PULSE_DURATION_US 10

const int BUZZER = 6;
const int SW1 = 3;
const int RED=7;
const int BLUE=11;
const int GREEN=10;
char sentence='d';
int switch_state = 0; // door is initially CLOSED
int parcel = 0; //indicates parcel exists or not

#include <Arduino.h>
#include <SPI.h>

void setup() {
  // put your setup code here, to run once:

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(SW1, INPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  
  switch_state = 0; // door is initially CLOSED
  parcel = 0; //indicates parcel exists or not

  Serial.begin(115200);
/*
  Serial2.setTX(8);
  Serial2.setRX(9);*/
  Serial2.begin(115200);

  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, HIGH);
  digitalWrite(BLUE, HIGH);
  parcel = 0;
  switch_state = 0; 
}

void loop() {
  // put your main code here, to run repeatedly:

  int switch_in = digitalRead(SW1); //check if the door is open

  //Determining if SW1 is on or not

  if(switch_in == LOW){
    switch_state = 1;
    //indicates whether receiver is ready to get the parcel(=DOOR OPEN)
  }
  else{
    switch_state = 0;
    //(=DOOR closed)
  }

  //Making a Pulse and calculating distance

  digitalWrite(TRIG,LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG,LOW);
  
  float T_ultrason = pulseIn(ECHO, HIGH);
  float dist = T_ultrason * 343* 0.0001/2;
  Serial.println("----------");
  Serial.print(dist);
  Serial.println(" cm  ");
  Serial.println("----------");

  if(dist < 30 ){ //DELIVERY ARRIVED
    parcel = 1;
    sentence='Your package has arrived!';
    Serial.println("Your package has arrived!");
    digitalWrite(GREEN, LOW);
    delay(100);
    digitalWrite(GREEN, HIGH);
    delay(100);
    digitalWrite(GREEN, LOW);
    delay(100);
    digitalWrite(GREEN, HIGH);
    

    //send message to phone
  }

  // CHECKING STATUS
  
  Serial.print("Your package"); //Shows if package is exists
  if(parcel ==1){Serial.println(" is available");}
  else{Serial.println(" don't exists");}

  Serial.print("Door: "); //Show if switch is on(=DOOR OPEN)
  if (switch_state == 1) {Serial.println(" Open"); }
  else {Serial.println(" Closed"); }
  
  
  //CHECKING THEFT

  int theft_happened = 0; //Show if theft happened

  if(switch_state ==0){ //when door is closed
    if(parcel ==1 && dist > 10 ) {//THEFT!
      
      //send message to phone

      sentence='Someone stole your package!!';
      Serial.println("Someone stole your package!!");
      theft_happened =1;
      digitalWrite(RED, LOW);
    }
  }

  if(  theft_happened ==1){ //Theft message stays until door is OPEN
    Serial.println("Someone stole your package!!");

    
    if(switch_state ==1){ //Owner has arrived
      theft_happened = 0; //Reset to normal
      parcel =0;
      digitalWrite(RED, HIGH);
    
    }
    
  }


  if(theft_happened ==0){ //when theft didn't happened
    if(switch_state ==1){ //when door is OPEN
      if(dist>10 && parcel==1)
      { //parcel is safely received
        parcel=0;
        switch_state=0;
        sentence='You receievd package.';
        Serial.println("You receievd package.");
        digitalWrite(BLUE, LOW);
        delay(500);
        digitalWrite(BLUE, HIGH);
        delay(500);
        digitalWrite(BLUE, LOW);
        delay(500);
        digitalWrite(BLUE, HIGH);

        digitalWrite(RED, HIGH);

      }
   }  
  }

  delay(2500);

  if(Serial.available())
  {
    char c = Serial.read();
    Serial2.write(c);
    Serial.print(c);
  }
  if(Serial2.available())
  {
    char c2 = Serial2.read();
    Serial.write(c2);
  }

  
  /*
  if(Serial.available() > 0)
  {
    char c; 
    c = Serial.read();
    Serial2.write(c);
    Serial.print(c);
  }
  if(Serial2.available() > 0)
  {
    char c2;
    c2 = Serial2.read();
    Serial.write(c2);
  }*/
}
