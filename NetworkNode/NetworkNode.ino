
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <EEPROM.h> //EEPROM functions

RF24 radio(10, 9);                 // nRF24L01 (CE,CSN)
RF24Network network(radio);        // Include the radio in the network
int this_node;                     // Octal format ( 04,031, etc), read from settings
const uint16_t coordinator = 00;   // Address of the other node in Octal format

int led = 2;

typedef struct
{
  double pres;
  float temp;
  float humi;
}
Payload;

Payload payload;

const unsigned long interval = 1000; // How often to send data to the other unit in ms
unsigned long last_sent;            // When did we last send?

void setup() {
   pinMode(4, OUTPUT); 
  checkNodeAddress();
  Serial.begin(57600);
  SPI.begin();
  radio.begin();
  network.begin(90, this_node);  //(channel, node address)
  radio.setDataRate(RF24_2MBPS);
  Serial.println("RF24 initialized"); 
  pinMode(2, OUTPUT); 

  //initial data
  payload.pres = 200;
  payload.temp = 200;
  payload.humi = 200;
 
}

void loop() {
     network.update();
    // Serial.println("Update network"); 
  
     //===== Sending =====//
    unsigned long now = millis();
    if (now - last_sent >= interval) {   // If it's time to send a data, send it!
       PIND |= (1<<PIND7); //this toggles the status LED at pin seven on interval
       last_sent = now;
       RF24NetworkHeader header(coordinator);   // (Address where the data is going)
       if (network.write(header, &payload, sizeof(payload))){
           digitalWrite(led,HIGH);
           Serial.println("Send OK");
            payload.pres = payload.pres + 1;
            payload.temp = payload.temp + 1;
            payload.humi = payload.humi  + 1;
       }
       else {
           digitalWrite(led,LOW);
           Serial.println("Send FAIL");
       }
   } 
}

void checkNodeAddress() {
   pinMode(3, INPUT_PULLUP); //pin 3 to ground and we heve configure mode 
   int val; //variable to store node address
   byte cAddr = 128; //this variable is used to see if this is the first time the module is being used
   
   String enter = F("Enter 'Y' for yes or any other character for no:"); //F() macro tells IDE to store string in flash memory and not SRAM
   String invalid = F("Invalid entry, default to ");
   String would = F("Would you like to update the ");
   
   if(EEPROM.get(0,cAddr)!= 128 || !digitalRead(3)) {
         digitalWrite(4,HIGH); //in settings mode so turn on status LED
        Serial.begin(57600); //start serial communication, need to turn off before using sleep and WDT
        //the following code reads the current settings from EEPROM and puts them in local variables
        Serial.println(F("Current settings in EEPROM"));
         Serial.print("Node address: ");
        Serial.println(EEPROM.get(1,val),OCT);
        
        //Change Node address
        Serial.print(would);
        Serial.print("Node Address? ");
        Serial.println(enter);
        if(getAnswer()) {
          Serial.println(F("Enter Node address to store in EEPROM"));
          while (!Serial.available()) { }
          val = Serial.parseInt();
          if(val >= 0) {
            EEPROM.put(1, val);
          }
          else { //if zero is entered it is invalid since coordinator is zero
            Serial.print(invalid);
            Serial.println("01");
            val = 01;
            EEPROM.put(1, val);
          }
        }
        getEEPROMValues(); //gets settings from EEPROM and stor in global variables
        //the following code prints out current settings from global variables
        Serial.print("Node address: ");
        Serial.println(this_node,OCT);
        
        cAddr = 128; //write '128' to EEPROM to show that settings have been entered at least once
        EEPROM.put(0, cAddr);
        Serial.end();
   }
   else
    {
     //not in settings mode so just get settings from EEPROM and store in global variables
    getEEPROMValues();
  }
}

void getEEPROMValues() {
  EEPROM.get(1,this_node);
}
bool getAnswer() {
   while (!Serial.available()) { }
   if(Serial.read() == 'Y') return true;
   else return false;
}
