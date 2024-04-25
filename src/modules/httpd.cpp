#include <Arduino.h>
#include "globals.h"
#include "player.h"

#include <WebServer.h>

static WebServer server(80); // listen on port 80

// this is the webserver root-Document (like a index.html) 
void handleRoot() {
  char itoa_buf[5];
  char* HTML = (char*)malloc(8000*sizeof(char));
  if (HTML==NULL) 
   return;
      
  if ( server.argName(0).compareTo("command") == 0 ) {
    if (server.arg(0).compareTo("Next") == 0) {
   	  next_station();
    }
       
    if (server.arg(0).compareTo("Prev") == 0) {
      prev_station();
    }
      
    if (server.arg(0).compareTo("Stop") == 0) {
      stop_player();
    }
      
    if (server.arg(0).compareTo("Play") == 0) {
      start_player();
    }
    
    if (server.arg(0).compareTo("Vol+") == 0) {
      volume_up();
    }
    
    if (server.arg(0).compareTo("Vol-") == 0) {
      volume_down();
    }
    
  } // if ( server.argName(0).compare("command") == 0 ) { 
    
  if ( server.argName(0).compareTo("gotoStation") == 0 ) {
    goto_station(server.arg(0).toInt());
  }
  
  
   //server.send(200, "text/plain", "hello from pico w!\r\n");
   /* Send a simple response */
   HTML[0]='\0'; // empty HTML response
   strcat(HTML,"<!DOCTYPE html>\
<html> \
<head> \
<style> \
.button { \
  background-color: #4CAF50; \
  border: none; \
  color: white; \
  padding: 5px; \
  text-align: center; \
  text-decoration: none; \
  display: inline-block; \
  font-size: 16px; \
  margin: 1px; 1px; \
  cursor: pointer; \
  width: 80px; height: 50px; \
}  \
.button_eqn { \
 background-color: #4CAF50; \
  border: none; \
  color: white; \
  padding: 5px; \
  text-align: center; \
  text-decoration: none; \
  display: inline-block; \
  font-size: 16px; \
  margin: 1px; 1px; \
  cursor: pointer; \
  width: 200px; height: 40px; \
} \
input[type=range][orient=vertical] \
{ \
    writing-mode: bt-lr; /* IE */ \
    -webkit-appearance: slider-vertical; /* Chromium */ \
    width: 8px; \
    height: 175px; \
    padding: 0 5px; \
} \
.animated-text { \
  font-family: monospace; \
  overflow: hidden; \
  height:1.1em; \
  word-wrap: break-word; \
  white-space: nowrap; \
  animation: typing 4s steps(100) forwards; \
} \
@keyframes typing { \
   from { \
      width: 0; \
   } \
   to { \
      width: 100ch; \
   } \
} \
</style> \
</head> \
<body style=\"background-color:powderblue;\"> \
<center> ");


// now playing ?
strcat(HTML,"<p class=\"animated-text\"> iRadioPico on RP2040 now playing: <span id=\"nowPlaying\">0</span>");
strcat(HTML," </p> ");

// player controls
strcat(HTML,"<form action=\"\" method=\"get\"> \
<button name=\"command\" type=\"submit\"  value=\"Prev\"class=\"button\">Prev</button> \
<button name=\"command\" type=\"submit\"  value=\"Next\"class=\"button\">Next</button> \
<button name=\"command\" type=\"submit\"  value=\"Stop\"class=\"button\">Stop</button> \
<button name=\"command\" type=\"submit\"  value=\"Play\"class=\"button\">Play</button> \
<button name=\"command\" type=\"submit\"  value=\"Vol-\"class=\"button\">Vol-</button> \
<button name=\"command\" type=\"submit\"  value=\"Vol+\"class=\"button\">Vol+</button> \
</form>");

strcat(HTML,"<br> <form action=\"\" method=\"GET\"> <select name=\"gotoStation\" id=\"Stations\" onchange=\"this.form.submit()\" STYLE=\"width: 52%\" size=\"20\" > ");

// build station list
for (int i=0;i<channels_in_list;i++) {
    strcat(HTML,"<option value=\"");
    itoa(i, itoa_buf, 10); 
    strcat(HTML,itoa_buf);
    strcat(HTML,"\"> ");
    strcat(HTML,playlist[i]);
    strcat(HTML," </option> ");
}
strcat(HTML,"</select></form>");
	
// XHR req/response for "nowPlaying"	
strcat(HTML,"</center> \
<script> \
setInterval(function() { \
  getData(); \
}, 2000); \
function getData() { \
  var xhttp = new XMLHttpRequest(); \
  xhttp.onreadystatechange = function() { \
    if (this.readyState == 4 && this.status == 200) { \
      document.getElementById(\"nowPlaying\").innerHTML = \
      this.responseText; \
    } \
  }; \
  xhttp.open(\"GET\", \"nowPlaying\", true); \
  xhttp.send(); \
} \
</script> \
</body> \
</html> \
");


   server.send(200, "text/html", HTML);
   free(HTML);
}


// send response for xhr req "nowPlaying" -> just a ip/nowPlaying - site
void handleNowPlaying() {
    char response[512] ="\0"; 
    strcat(response,playlist[actual_channel_or_file_ID]);
    
    server.send(200, "text/plain", response);
}

// 404 case
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


void httpd_init() {
  
  server.addHook([](const String & method, const String & url, WiFiClient * client, WebServer::ContentTypeFunction contentType) {
    (void)method;       // GET, PUT, ...
    (void)url;          // example: /root/myfile.html
    (void)client;       // the webserver tcp client connection
    (void)contentType;  // contentType(".html") => "text/html"
    
    //Serial.printf(method.c_str());
    //Serial.printf(url.c_str());
    for (uint8_t i = 0; i < server.args(); i++) {
     //Serial.printf(server.argName(i).c_str());   Serial.printf(server.arg(i).c_str());
    }
 
    return WebServer::CLIENT_REQUEST_CAN_CONTINUE;
  });

  server.on("/", handleRoot);
  server.on("/nowPlaying", handleNowPlaying);
  server.onNotFound(handleNotFound);
 
  server.begin();
  SERIAL_PORT.println("HTTPD : HTTP server started");
  SERIAL_PORT.flush();
  
}

void httpd_run() {
  server.handleClient();
}
