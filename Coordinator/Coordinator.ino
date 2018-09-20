/*
  Arduino Wireless Network - Multiple NRF24L01 Tutorial
          == Base/ Master Node 00==
  by Dejan, www.HowToMechatronics.com
  Libraries:
  nRF24/RF24, https://github.com/nRF24/RF24
  nRF24/RF24Network, https://github.com/nRF24/RF24Network
*/

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

#define ledNode1 2
#define ledNode2 3

RF24 radio(10, 9);               // nRF24L01 (CE,CSN)
RF24Network network(radio);      // Include the radio in the network
const uint16_t this_node = 00;   // Address of this node in Octal format ( 04,031, etc)
const uint16_t node01 = 01;      // Address of the other node in Octal format
const uint16_t node02 = 02;      // Address of the other node in Octal format

const unsigned long interval = 1000;
unsigned long last_sent; 

struct Payload
{
  double press;
  float temp;
  float humi;
};

void setup() {
  Serial.begin(9600);
  Serial.println("Start setup");
  
  SPI.begin();
  radio.begin();
  network.begin(90, this_node);  //(channel, node address)
  radio.setDataRate(RF24_2MBPS);

  
  pinMode(ledNode1, OUTPUT);
  pinMode(ledNode2, OUTPUT);
  Serial.println("Setup done...");

}

void loop() {
  network.update();
 // Serial.println("network update");

  //===== Receiving =====//
  while ( network.available() ) {     // Is there any incoming data?
    Serial.println("Data received");
    RF24NetworkHeader header;

    Payload payload;
    network.read(header, &payload, sizeof(payload)); // Read the incoming data

    if ((header.from_node == 1))
        PIND |= (1<<PIND3);
    
    if ((header.from_node == 2))
        PIND |= (1<<PIND4);
     printNodeData(header.from_node,&payload);
  }
}

void printNodeData(int nodeId, Payload *payload)
{
        Serial.print("Node: ");
        Serial.println(nodeId);

        Serial.print("Pressure: ");
        Serial.print(payload->press);
        Serial.print("; Temperature: ");
        Serial.print(payload->temp);
        Serial.print("; Humidity: ");
        Serial.println(payload->humi);

}
