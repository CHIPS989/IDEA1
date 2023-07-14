//Pin 번호별로 부여

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


#include <SPI.h>
#include <Ethernet.h>    // W5500부터 Ethernet2 라이브러리를 사용.
// http://www.arduino.org/learning/reference/Ethernet-two-Library 참조

#define SOCK_PORT        80


byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };    // MAC ADDRESS.

IPAddress local_ip(192,168, 11, 170);    // IP Address
IPAddress gateway(192,168, 11, 1);    // Gateway Address
IPAddress subnet(255, 255, 255, 0);    // Subnet Mask
IPAddress dns_addr(8, 8, 8, 8);    // DNS Address

EthernetServer server(SOCK_PORT);    // 소켓 생성할 때 80(Http)번 포트로 생성
// 잘 알려진 포트번호 정보 : https://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers

byte linefeed_cnt;


// the setup function runs once when you press reset or power the board
void setup() {
// initialize digital pin LED_BUILTIN as an output.
  
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(SW1, INPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  
  
  Serial.begin(9600);

  Serial.println("Booting up....");

  Ethernet.init(17);



  if (Ethernet.begin(mac) == 0)    // DHCP서버에 IP할당 요청  
  {
    // IP할당에 실패하면 Static으로 설정 
    Serial.println("Failed to configure Ethernet using DHCP");
    Serial.println("Configure Static Network Information");
    // initialize the ethernet device not using DHCP:
    Ethernet.begin(mac, local_ip, dns_addr, gateway, subnet);
  }

  Serial.println("***** W5100S-EVB-Pico WebServer (LED Control) *****");
  Serial.print("Device IP address : ");
  Serial.print(Ethernet.localIP());    // IP주소 출력 
  Serial.print(" : ");    
  Serial.println(SOCK_PORT);    // Port 번호 출력 
  
  server.begin();    // 서버 Listen 시작 (Socket open -> Listen 상태 돌입)

}

// the loop
//opfunction runs over and over again forever
void loop() {
  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, HIGH);
  digitalWrite(BLUE, HIGH);

  EthernetClient client = server.available();    // 클라이언트가 서버에 연결되었다면 
  
  if (client.connected()) 
  { 
    String http_request = "";
    while (client.available())
    {
       char request_char = client.read(); // 데이터를 1byte 읽어서 request_char 변수에 저장
       http_request += request_char;
       Serial.print(request_char);   // 수신한 데이터를 1byte씩 시리얼 메세지로 출력

      if(checkRequestEnd(request_char))
      {  
        sendHTTPResponse();
        client.stop();
      }
    }
  }

  int parcel = 0; //indicates parcel exists or not
  int switch_in = digitalRead(SW1); //check if the door is open
  int switch_state = 0; // door is initially CLOSED


  //Making a Pulse and calculating distance

  digitalWrite(TRIG,LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG,LOW);
  
  float T_ultrason = pulseIn(ECHO, HIGH);
  float dist = T_ultrason * 343* 0.0001/2;
  
  Serial.print(dist);
  Serial.println(" cm  ");
  

  //Determining if SW1 is on or not

  if(switch_in == LOW){
    switch_state = 1;
    //indicates whether receiver is ready to get the parcel(=DOOR OPEN)
  }
  else{
    switch_state = 0;
    //(=DOOR closed)
  }
  
  if(dist < 30 ){ //DELIVERY ARRIVED
    parcel = 1;
    sentence='Your package has arrived!';
    Serial.println("Your package has arrived!");
    digitalWrite(GREEN, low);
    delay(100);
    digitalWrite(GREEN, high);
    delay(100);
    digitalWrite(GREEN, low);
    delay(100);
    digitalWrite(GREEN, high);
    

    //send message to phone
  }


  // CHECKING STATUS
  Serial.print("Your package"); //Shows if package is exists
  if(parcel ==1){Serial.println(" is available");}
  else{Serial.println(" don't exists");}

  Serial.print("Door: "); //Show if switch is on(=DOOR OPEN)
  if (switch_state == 1) {Serial.print(" Open"); }
  else {Serial.print(" Closed"); }
  
  Serial.print("      Motion: ");
  if (pirStat == LOW) {Serial.println(" O"); }
  else {Serial.println(" X"); }

  //CHECKING THEFT

  int theft_happened = 0; //Show if theft happened

  if(switch_state ==0){ //when door is closed
    if(parcel ==1 && (dist > 30 ) {//THEFT!
      
      //send message to phone

      sentence='Someone stole your package!!';
      Serial.println("Someone stole your package!!");
      theft_happened =1;
    }
  }

  if(  theft_happened ==1){ //Theft message stays until door is OPEN
    Serial.println("Someone stole your package!!");
    digitalWrite(RED, low);
    
    if(switch_state ==1){ //Owner has arrived
      theft_happened = 0; //Reset to normal
      parcel =0;
      digitalWrite(RED, high);
    
    }
    
  }


  if(theft_happened ==0){ //when theft didn't happened
    if(switch_state ==1){ //when door is OPEN
      if(dist>30 && parcel==1)
      { //parcel is safely received
        parcel=0;
        switch_state=0;
        sentence='You receievd package.';
        Serial.println("You receievd package.");
        digitalWrite(BLUE, low);
        delay(500);
        digitalWrite(BLUE, High);
        delay(500);
        digitalWrite(BLUE, low);
        delay(500);
        digitalWrite(BLUE, high);

        

      }
   }  
  }
  //Serial.print(dist);
  //Serial.println(" cm  ");
  
  delay(1000);
}




bool checkRequestEnd(char request_ch)
{
      bool ret = 0;
      if(linefeed_cnt == 1)
      {
        if(request_ch == '\r') linefeed_cnt++;
        else linefeed_cnt = 0;
      }
      if(request_ch == '\n') linefeed_cnt++;

      if(linefeed_cnt >= 2)
      {
        linefeed_cnt = 0;
        ret = 1;
      }
      return ret;
}

void sendHTTPResponse(void)
{
  // HTTP Message
  server.println("HTTP/1.1 200 OK");
  server.println("Content-Type: text/html");
  server.println("Connection: close");  // the connection will be closed after completion of the response
  server.println("Refresh: 5");  // refresh the page automatically every 5 sec
  server.println();
  // HTML Message
  server.println("<!DOCTYPE HTML>");
  server.println("<html>");
  server.println("<head>");
  server.println("<title>W5100S-EVB-Pico WebServer (Post Data)</title>");
  server.println("</head>");

  server.println("<body>");
  server.println("<br /> ***  W5100S-EVB-Pico WebServer (Display Sensors Value)  ***  <br /><br />");
  
  server.print("Package : ");
  server.print(sentence);
  server.println("&nbsp(0~1023 value)<br />");

  
  server.println("</body>");
  server.println("</html>");
  // HTML Message Done
}
