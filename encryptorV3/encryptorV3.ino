#include <AES.h>
#include <AES_config.h>
#include "base64.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


#define LED_BUILTIN 2 // ONBOARD LED WORKS OPPOSITE


// Update these with values suitable for your network.
const char* ssid = "dd-wrt";
const char* password = "Clairvoyant";
char* topic = "aes/test"; 
const char* mqtt_server = "broker.datasoft-bd.com";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

// The AES library object.
AES aes;

// Our AES key. Note that is the same that is used on the Node-Js side but as hex bytes(16).
byte key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };//16*8=128bit

// The uninitialized Initialization vector
byte my_iv[N_BLOCK] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};   //8 bit data can be stored in each of the total 16 bytes = 128bit




void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  pinMode(LED_BUILTIN,OUTPUT);
  
  digitalWrite(LED_BUILTIN, LOW);
  
  setup_wifi();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  delay(2000);
  Serial.println("READY"); 

}

void loop() {
  // put your main code here, to run repeatedly:
  
  setPublishTimer();
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}







// Generate a random initialization vector
void gen_iv(byte  *iv) {
    for (int i = 0 ; i < N_BLOCK ; i++ ) {
        iv[i]= (byte) getrnd();
    }
}

uint8_t getrnd() {
    uint8_t really_random = *(volatile uint8_t *)0x3FF20E44;
    return really_random;
}



void setPublishTimer(){
  
    unsigned long now = millis();
  
  if (now - lastMsg > 30000) {
     lastMsg = now;
     Serial.println("Ticking every 30s");
     //publishes every 30 second
     //Write your code here


     
     String payload="{\"current\":";                              // String data = "{\"current\":178.5, \"rpm\":30}";
            payload+=random(0,200);
            payload+=",\"rpm\":";
            payload+=random(0,40);
            payload+="}";

     encrypt(payload);
  

  }
}




void encrypt(String data)  {

    char b64data[200];       //int8_t
    byte cipher[1000];        //can be set globally for decoder access convenience

    //get set keys and iv first
    aes.set_key( key , sizeof(key));  
    gen_iv( my_iv );           // Generate 16 random numbers and fill the 16 blocks of my_iv with it to form INIT VECT


    // STEP 1:  ENCODE INIT VECT AS BASE64
    
    b64_encode( b64data, (char *)my_iv, N_BLOCK);
    Serial.println("IV in b64: " + String(b64data));
    String iv = (char*)b64data;


    

    
    //  STEP 2:   ENCODE MSG_DATA AS BASE64 BEFORE GIVING INPUT TO THE CRYPTO FUNCTION
    
    int b64len = b64_encode(b64data, (char *)data.c_str(),  data.length());
    Serial.println("PlainText Data: " + data );
    Serial.println ("PlainText Data in B64: " + String(b64data) );
//                                                                                                  Serial.println (" The lenght is:  " + String(b64len) );                                                                                                                                                                                            // For data integrity check purpose
//                                                                                                  base64_decode( decoded , b64data , b64len );
//                                                                                                  Serial.println("Decoded: " + String(decoded));
    
    
    //  STEP 3:  ENCRYPT! With AES128, our key and IV, CBC and pkcs7 padding

    Serial.println("Encryption Begins...");
    
    aes.do_aes_encrypt((byte *)b64data, b64len , cipher, key, 128, my_iv);
    
    Serial.println("Encryption done!");    
    Serial.println("Cipher size: " + String(aes.get_size()));
  
    
    //  STEP 4:  ENCODE ENCRYPTED OUTPUT AS BASE64
    
    b64_encode(b64data, (char *)cipher, aes.get_size() );
    Serial.println ("Encrypted data in base64: " + String(b64data) );
    String encrypted_data = (char*)b64data;

    publish(encrypted_data, iv);

}//encoding done


void publish(String data, String iv){
         
     String payload="{\"data\":\"";                        // String data = "{\"current\":178.5, \"rpm\":30}";
            payload+=data;
            payload+="\",\"iv\":\"";
            payload+=iv;
            payload+="\"}";

  
     char message[512];
     payload.toCharArray(message,512);
     Serial.println(message);
     //publish sensor data to MQTT broker
     Serial.println(client.publish(topic, message,512));    
}


void setup_wifi() {
   delay(100);
  // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* payload, unsigned int length) 
{

} //end callback







void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if you MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str(), "iotdatasoft", "brokeriot2o2o"))
    {
      Serial.println("connected");
     //once connected to MQTT broker, subscribe command if any
//      client.subscribe("ds/reset");
      Serial.println("Subscribed to the topic:    ds/reset");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(6000);
    }
  }
} //end reconnect()
