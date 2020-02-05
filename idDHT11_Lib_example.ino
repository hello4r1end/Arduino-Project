/*
  Board	          int.0	  int.1	  int.2	  int.3	  int.4	  int.5
 Uno, Ethernet	  2	  3
 Mega2560	  2	  3	  21	  20	  19	  18
 Leonardo	  3	  2	  0	  1
 Due	          (any pin, more info http://arduino.cc/en/Reference/AttachInterrupt)
 */
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <idDHT11.h>
// size of buffer used to capture HTTP requests
#define REQ_BUF_SZ   60


int idDHT11pin = 2; //Digital pin for comunications
int idDHT11intNumber = 0; //interrupt number (must be the one that use the previus defined pin (see table above)

//declaration
void dht11_wrapper(); // must be declared before the lib initialization

// Lib instantiate
idDHT11 DHT11(idDHT11pin,idDHT11intNumber,dht11_wrapper);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177); // IP address, may need to change depending on network
EthernetServer server(80);  // create a server at port 80
File webFile;               // the web page file on the SD card
char HTTP_req[REQ_BUF_SZ] = {0}; // buffered HTTP request stored as null terminated string
char req_index = 0;              // index into HTTP_req buffer
boolean LED_state[2] = {0}; // stores the states of the LEDs


void setup()
{
  // disable Ethernet chip
    pinMode(10, OUTPUT);
    digitalWrite(10, HIGH);
   
    Serial.begin(9600);       // for debugging
   
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

    // initialize SD card
    Serial.println("Initializing SD card...");
    if (!SD.begin(4)) {
        Serial.println("ERROR - SD card initialization failed!");
        return;    // init failed
    }
    Serial.println("SUCCESS - SD card initialized.");
    // check for index.htm file
    if (!SD.exists("index.htm")) {
        Serial.println("ERROR - Can't find index.htm file!");
        return;  // can't find index file
    }
    Serial.println("SUCCESS - Found index.htm file.");

    // LEDs
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
    
    Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void dht11_wrapper() {
  DHT11.isrCallback();
} 
void loop()
{
  
 /* */

  // TELOS KWDIKA UGRASIAS
   EthernetClient client = server.available();  // try to get client

    if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                // limit the size of the stored received HTTP request
                // buffer first part of HTTP request in HTTP_req array (string)
                // leave last element in array as 0 to null terminate string (REQ_BUF_SZ - 1)
                if (req_index < (REQ_BUF_SZ - 1)) {
                    HTTP_req[req_index] = c;          // save HTTP request character
                    req_index++;
                }
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    // remainder of header follows below, depending on if
                    // web page or XML page is requested
                    // Ajax request - send XML file
                    if (StrContains(HTTP_req, "ajax_inputs")) {
                        // send rest of HTTP header
                        client.println("Content-Type: text/xml");
                        client.println("Connection: keep-alive");
                        client.println();
                        SetLEDs();
                        // send XML file containing input states
                        
                        XML_response(client);
                        
                    }
                    else {  // web page request
                        // send rest of HTTP header
                        client.println("Content-Type: text/html");
                        client.println("Connection: keep-alive");
                        client.println();
                        
                        // send web page
                        webFile = SD.open("index.htm");        // open web page file
                        if (webFile) {
                            while(webFile.available()) {
                                client.write(webFile.read()); // send web page to client
                            }
                            webFile.close();
                        }
                    }
                    // display received HTTP request on serial port
                    Serial.print(HTTP_req);
                    // reset buffer index and all buffer elements to 0
                    req_index = 0;
                    StrClear(HTTP_req, REQ_BUF_SZ);
                    break;
                }
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
 
}




// checks if received HTTP request is switching on/off LEDs
// also saves the state of the LEDs
void SetLEDs(void)
{
    // LED 1 (pin 8)
    if (StrContains(HTTP_req, "LED1=1")) {
        LED_state[0] = 1;  // save LED state
        digitalWrite(8, HIGH);
    }
    else if (StrContains(HTTP_req, "LED1=0")) {
        LED_state[0] = 0;  // save LED state
        digitalWrite(8, LOW);
    }
    // LED 2 (pin 9)
    if (StrContains(HTTP_req, "LED2=1")) {
        LED_state[1] = 1;  // save LED state
        digitalWrite(9, HIGH);
    }
    else if (StrContains(HTTP_req, "LED2=0")) {
        LED_state[1] = 0;  // save LED state
        digitalWrite(9, LOW);
    }
}


// send the XML file with analog values, switch status
//  and LED status
void XML_response(EthernetClient cl)
{
 //KWDIKAS 8ERMOKRASIAS
    int analog_val;            
    int count;                 
   
    const int inPin = 0 ;
    int value = analogRead(inPin);
    float millivolts = (value / 1024.0) * 5000;
    float celsius = (millivolts / 10) - 273.15 - 12 - 36 -4 ;


 DHT11.acquire();
    cl.print("<?xml version = \"1.0\" ?>");
    cl.print("<inputs>");
    // checkbox LED states
    // LED1
    cl.print("<LED>");
    if (LED_state[0]) {
        cl.print("checked");
    }
    else {
        cl.print("unchecked");
    }
    cl.println("</LED>");
    // button LED states
    // LED3
    cl.print("<LED>");
    if (LED_state[1]) {
        cl.print("on");
    }
    else {
        cl.print("off");
    }
    cl.println("</LED>");
// TAG GIA 8ERMOKRASIA-YGRASIA
   
         cl.println("<DHT11-LM335>");

    cl.println("Humidity (%): ");
    cl.println(DHT11.getHumidity(), 2);
   cl.println("......");

    cl.println("Celsius (C): ");
     cl.print(celsius);
    cl.println("</DHT11-LM335>");
    cl.print("</inputs>");
}

// sets every element of str to 0 (clears array)
void StrClear(char *str, char length)
{
    for (int i = 0; i < length; i++) {
        str[i] = 0;
    }
}

// searches for the string sfind in the string str
// returns 1 if string found
// returns 0 if string not found
char StrContains(char *str, char *sfind)
{
    char found = 0;
    char index = 0;
    char len;

    len = strlen(str);
    
    if (strlen(sfind) > len) {
        return 0;
    }
    while (index < len) {
        if (str[index] == sfind[found]) {
            found++;
            if (strlen(sfind) == found) {
                return 1;
            }
        }
        else {
            found = 0;
        }
        index++;
    }

    return 0;
}
  
