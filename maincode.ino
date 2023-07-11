//Pin 번호별로 부여

const int ECHO = 0;
const int TRIG = 1;
const int BUZZER = 6;
const int SW1 = 3;


// the setup function runs once when you press reset or power the board
void setup() {
// initialize digital pin LED_BUILTIN as an output.
  
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(SW1, INPUT);
  
  Serial.begin(9600);

}

// the loop function runs over and over again forever
void loop() {

  int parcel = 0; //indicates parcel exists or not

  int switch_in = digitalRead(SW1);
  int switch_state = 0;

  //Making a Pulse and calculating distance

  delay(500);
  digitalWrite(TRIG, HIGH);
  delay(500);
  digitalWrite(TRIG, LOW);
  
  float T_ultrason = pulseIn(ECHO, HIGH);
  float dist = T_ultrason * 343* 0.0001/2;

  //Determining if SW1 is on or not

  if(switch_in == LOW){
    switch_state = 1;
    //indicates whether receiver is ready to get the parcel
  }
  
  if(dist < 30){ //DELIVERY ARRIVED
    parcel = 1;
    //send message to phone
  }


  if(parcel ==1 && dist > 30 && switch_state ==0){//THEFT!
    //send message to phone
  }



  Serial.print(dist);
  Serial.println(" cm  ");
  
  delay(10000);
}
