#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

// Version 0.0.2a

const char* ssid = "foo";
const char* password = "bar";

WebServer server(80);

//-----------------------------------------------------
  //HTML und Daten für das Eingabe-/Ausgabefeld
  //einfügen im globalen Scope
  //-----------------------------------------------------
  //erstellt von (c): Moppi    @roboternetz.de
  //-----------------------------------------------------
  //Breite und Höhe des ges. Feldes
    const int tableWidth = 10;
    const int tableHeight = 10;
  //Füllzeichen für Felder
    const String fillChar = "X";
  //Parameter-Array enthält den Status aller Felder
  //der Ein-/Ausgabematrix (0= AUS, 1= AN)
    bool matrix[tableHeight*tableWidth]={0};
  //Status, ob Änderungen vorliegen
    volatile static bool toChange = 0;
    volatile static bool toLearn = 1;
    volatile static bool toDetect = 0;
    volatile static bool toReset = 0;
    volatile static bool toSave = 0;
    volatile static bool toClear = 0;
  //HTML
    String body = "";
    String s1="<style>\nform{margin:0}\n#un{width:0;height:0;visibility:hidden}\n#bu{width:10em;height:2em;}\n#rw{display:table-row;}\n#ce{display:table-cell;}\n#ip{width:1.5em;text-align:center;font-weight:bold}\n</style>\n<script>dge=document.getElementById; function oc(ob){if(ob.value=='')ob.value='"+fillChar+"';else ob.value='';}</script>\n<div role=\"presentation\" style=\"display:table\"><div id='rw'><div id='ce' style=\"padding-right:1em\"><form method=\"get\"><div role=\"presentation\" style=\"margin-bottom:1em;display:table\">";
    String s2="<div id=\"rw\">";
    String s3="<div id=\"ce\"><input onclick=\"oc(this);\" type=\"text\" name=\"b";
    String s3_1="\" value=\"";
    String s3_2="\" maxlength=\"1\" id=\"ip\"></div>";
    String s4="</div>";
    String s5="</div><div id='ce'><input id=\"bu\" type=\"submit\" value=\"Change:Ok\"><p></form>";
    String s5_0="<form method=\"get\"><input id=\"bu\" type=\"submit\" value=\"Clear\"><input id=\"un\" type=\"text\" name=\"bn\" value=\"clear\"></form>"; 
    String s5_1="<form method=\"get\"><input id=\"bu\" type=\"submit\" value=\"Learn\"><input id=\"un\" type=\"text\" name=\"bn\" value=\"learn\"></form>";
    String s5_2="<form method=\"get\"><input id=\"bu\" type=\"submit\" value=\"Detect\"><input id=\"un\" type=\"text\" name=\"bn\" value=\"detect\"></form>";
    String s5_3="<form method=\"get\"><input id=\"bu\" type=\"submit\" value=\"Reset\"><input id=\"un\" type=\"text\" name=\"bn\" value=\"reset\"></form>";
    String s5_4="<form method=\"get\"><input id=\"bu\" type=\"submit\" value=\"Save\"><input id=\"un\" type=\"text\" name=\"bn\" value=\"save\"></form></div></div></div>";
String par="b";
  //-----------------------------------------------------


float bufferArray[100];


//-----------------------------------------------------
//-----------------------------------------------------
void setup() {

  Serial.begin(115200);
  delay(1000);
  Serial.println();

  //-----------------------------------------------------
  //Anmeldung am Netzwerk
  //-----------------------------------------------------
  Serial.println("connecting by IPAddress(192,168,2,99)");
  WiFi.mode(WIFI_STA);
  

  WiFi.config(IPAddress(192, 168, 2, 99), IPAddress(192, 168, 2, 1), IPAddress(192, 168, 2, 1), IPAddress(255, 255, 255, 0));
  WiFi.begin(ssid, password); //begin WiFi connection // edited

  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


  //-----------------------------------------------------
  //Auswertung und Erstellung des Ein-/Ausgabefeldes
  //-----------------------------------------------------
  //Einbinden in setup() und in loop() nutzen durch
  //Aufruf von "server.handleClient();"
  //benötigt Objekt "server", erstellt im globalen Scope
  //mittels:       ESP8266WebServer server(80);
  //benötigt auch: #include <ESP8266WebServer.h>
  //-----------------------------------------------------
  //erstellt von (c): Moppi    @roboternetz.de
  //-----------------------------------------------------

  memset(matrix , 0, sizeof(matrix));
  server.on("/",[]()
   {
    //Auswerten des Query-String vom Browser
    int n = 0; 
    if(server.hasArg("bn")){
      if(server.arg("bn") == "clear") toClear=1; else toClear=0;
      if(server.arg("bn") == "save") toSave=1; else toSave=0;
      if(server.arg("bn") == "reset") toReset=1; else toReset=0;
      if(server.arg("bn") == "detect") toDetect=1; else toDetect=0;
      if(server.arg("bn") == "learn") toLearn=1; else toLearn=0;      
    }
    for(int x=0; x < tableHeight; x++){
      for(int j=0; j < tableWidth; j++){
        if(server.hasArg(par+String(n))){
          String a = server.arg(par+String(n));
          bool ic = matrix[n];
          if (a == "") matrix[n] = 0; else matrix[n] = 1; 
          if (ic != matrix[n]) toChange = 1;
          n++;
        }
      }
    }
    //Aufbau des Feldes
    String v; body = s1; n = 0;
    for(int x=0; x < tableHeight; x++){
      body += s2;
      for(int j=0; j < tableWidth; j++){ 
        if (matrix[n]) v = fillChar; else v = "";
        body += s3 + String(n) + s3_1 + v + s3_2;
        n++;
      }
      body += s4;
    }
    body += s4+s5+s5_0+s5_1+s5_2+s5_3+s5_4;
    //Senden der Seite an den Browser
    server.send(200, "text/html", body);
   }
  );
  //-----------------------------------------------------



  //-----------------------------------------------------
  //Webserver starten
  //-----------------------------------------------------
  server.begin();
  Serial.println("Web server started!");

}


uint32_t timestamp=0, sec=0;
uint8_t modeLearn=0, modeDetect=0;

//-----------------------------------------------------
//-----------------------------------------------------
void loop() {  
  server.handleClient();
  // Aktionen nach ButtonClick:
  if(toClear) {
    Serial.println((String)"toClear="+toClear);
    memset(matrix , 0, sizeof(matrix));
    toClear=0;
  }
  if (toChange) {
    Serial.println("new data:");
    for (int i = 0; i < 100; i++) {
       bufferArray[i] = (float)matrix[i];
       Serial.println((String)i + ": " + bufferArray[i]);
    }
    toChange=0;
  }
  if(toLearn){       
    modeLearn=1;
    modeDetect=0;
    Serial.println((String)"modeLearn="+modeLearn);
    toLearn=0; 
  }
  else 
  if(toDetect) {      
    modeLearn=0;
    modeDetect=1;
    Serial.println((String)"modeDetect="+modeDetect);
    toDetect=0;  
  }
  if(toReset) {
    toLearn=0;
    toDetect=1;
    Serial.println((String)"toReset="+toReset);
    toReset=0;
  }
  if(toSave) {
    toLearn=0;
    toDetect=1;
    Serial.println((String)"toSave="+toSave);
    toSave=0;
  }
  if(millis()-timestamp >= 1000) {
    sec++;
    timestamp +=1000;
    Serial.println(sec);
  }
}
