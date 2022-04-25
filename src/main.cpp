#include <Arduino.h>
#include <WiFi.h>


const char* ssid="zombieIncubator";
const char* password="1BottleOfGina";

WiFiServer server(80);
//inputs


//outputs
const int accessoryPin= 26;
const int ignitionPin =27;
const int starterPin =39;


String header;
// Auxiliar variables to store the current output state
String accessoryState = "on";
String ignitionState = "on";
String starterState="on";
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;


void setup() {
pinMode (accessoryPin,OUTPUT);
pinMode (ignitionPin,OUTPUT);
pinMode (starterPin,OUTPUT);

 
digitalWrite(accessoryPin,HIGH);
digitalWrite(ignitionPin,HIGH);
digitalWrite(starterPin,HIGH);
delay(1000);
digitalWrite(accessoryPin,LOW);
digitalWrite(ignitionPin,LOW);
digitalWrite(starterPin,LOW);
 Serial.begin(115200);
WiFi.mode(WIFI_STA);
WiFi.begin(ssid,password);
while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
 WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /accessory/on") >= 0) {
              Serial.println("accessory on");
              accessoryState = "on";
              digitalWrite(accessoryPin, HIGH);
            } else if (header.indexOf("GET /accessory/off") >= 0) {
                if(ignitionState=="on"){
                  Serial.println("Cannot turn off accessory while ignition is on");
                  client.println("<script>document.getElementById(\"starterStatus\").innerHTML = \"Cannot turn off accessory while ignition is on\"setTimeout(myTimeout,2000) function myTimeout(){window.location.href = \"WiFi.localIP()\"};</script>");
                } else {
              Serial.println("accessory off");
              accessoryState = "off";
              digitalWrite(accessoryPin, LOW);
            }} else if (header.indexOf("GET /ignition/on") >= 0) {
              if (accessoryState=="off"){
                 Serial.println("Cannot turn on ignition while accessory is off");
                 client.println("<script>document.getElementById(\"starterStatus\").innerHTML = \"Cannot turn on ignition while accessory is off\"</script>");
              } else {
              Serial.println("ignition on");
              ignitionState = "on";
              digitalWrite(ignitionPin, HIGH);
            } }else if (header.indexOf("GET /ignition/off") >= 0) {
              Serial.println("ignition off");
              ignitionState = "off";
              digitalWrite(ignitionPin, LOW);
            }
             if (header.indexOf("GET /starter/on") >= 0) {
               if(accessoryState=="on" && ignitionState=="on"){
                Serial.println("starting crank");
                starterState="on";
                digitalWrite(starterPin,HIGH);
               
             }
             
             
             }
            if (header.indexOf("GET /GPS/on") >=0){
              Serial.println("AT+GPS=1");
            }
            if (header.indexOf("GET /GPS/off") >=0){
              Serial.println("AT+GPS=0");
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>autoNinja</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p id=\"starterStatus\">accessoryPin=26 - State " + accessoryState + "</p>");
            // If the output26State is off, it displays the ON button       
            if (accessoryState=="off") {
              client.println("<p><a href=\"/accessory/on\"><button class=\"button button2\">Accessory</button></a></p>");
            } else {
              client.println("<p><a href=\"/accessory/off\"><button class=\"button\">Accessory</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p id=\"starterStatus\">ignitionPin=27 - State " + ignitionState + "</p>");
            // If the output27State is off, it displays the ON button       
            if (ignitionState=="off") {
              client.println("<p><a href=\"/ignition/on\"><button class=\"button button2\">Ignition</button></a></p>");
            } else {
              client.println("<p><a href=\"/ignition/off\"><button class=\"button\">Ignition</button></a></p>");
            }
 client.println("<p id=\"statusBar\"></p>");
            client.println("</body></html>");
            if(accessoryState=="on" && ignitionState=="on"){
               client.println("<script>setTimeout(myTimeout,2000) function myTimeout(){document.getElementById(\"starterStatus\").innerHTML = \"Ready to start car\"}</script>");

            } else {
              client.println("<script>setTimeout(myTimeout,2000) function myTimeout(){document.getElementById(\"starterStatus\").innerHTML = \"turn on ac and ign to prep\"}</script>");
             
            }
            
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }}
