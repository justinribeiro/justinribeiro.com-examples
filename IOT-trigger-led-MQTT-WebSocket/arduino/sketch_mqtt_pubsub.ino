#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// Set the MAC address
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x01 };
 
// Set fallback IP address if DHCP fails
IPAddress ip(192,168,0,201);

// Set the broker server IP
byte server[] = { XXX, XXX, XXX, XXX };

// Set what PINs our Led's are connected to
int redPin = 5;                
int greenPin = 6;
int bluePin = 7;

// Set a generic code that will trigger our Blue Led
// think of this as a set of codes for automation you might write
byte triggerBlue[6] = "12345";

// handles messages that are returned from the broker on our subscribed channel
void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("New message from broker on topic:");
  Serial.println(topic);
  
  Serial.print("Payload:");
  Serial.write(payload, length);
  
  // This will blink our green LED
  blink(greenPin);
  
  // Check and see if our payload matches our simple trigger test
  if ((length == 5) & (memcmp(payload, triggerBlue, 5) == 0) )
  {
    blink(bluePin);
  }

}

// Fire up our PubSub client
PubSubClient client(server, 1883, callback);

void setup()
{
  
  // Open serial communications
  Serial.begin(9600);
  
  // Setup our Leds
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // attempt a DHCP connection
  Serial.println("Attempting to get an IP address using DHCP:");
  if (!Ethernet.begin(mac)) 
  {
    // if DHCP fails, start with a hard-coded address:
    Serial.println("failed to get an IP address using DHCP, trying manually");
    Ethernet.begin(mac, ip);
  }
  
  Serial.print("My address:");
  Serial.println(Ethernet.localIP());
  
  // Connect to Broker, give it arduino as the name
  if (client.connect("arduino")) {
    
    // Good, we connected turn on the red led
    digitalWrite(redPin, HIGH);
    
    // Publish a message to the status topic
    client.publish("status","Arduino is now online");
    
    // Listen for messages on the control topic
    client.subscribe("control");
  }
  
}

void loop()
{
  client.loop();
}

// Anything with flashing lights.
void blink(int targetLed) 
{
 static boolean led = HIGH;
 static int count = 0;
 
 Serial.print("Starting to blink...");
 
 while (count < 10) 
 {
   digitalWrite(targetLed, led);
   
   count++;
   
   if (led == HIGH) 
   {
     led = LOW;
   } 
   else 
   {
     led = HIGH;
   }
   
   delay(200);
 }
 
 count = 0;
 
 Serial.print("done.");
}
