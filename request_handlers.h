
const char TTP_HEAD[] PROGMEM            = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";
const char TTP_STYLE[] PROGMEM           = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;}</style>";
const char TTP_SCRIPT[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const char TTP_HEAD_END[] PROGMEM        = "</head><body><div style='text-align:left;display:inline-block;min-width:260px;'>";
const char TTP_PORTAL_OPTIONS[] PROGMEM  = "<form action=\"/wifi\" method=\"get\"><button>Configure WiFi</button></form><br/><form action=\"/0wifi\" method=\"get\"><button>Configure WiFi (No Scan)</button></form><br/><form action=\"/i\" method=\"get\"><button>Info</button></form><br/><form action=\"/r\" method=\"post\"><button>Reset</button></form>";
const char TTP_ITEM[] PROGMEM            = "<div><a href='#p' onclick='c(this)'>{v}</a>&nbsp;<span class='q {i}'>{r}%</span></div>";
const char TTP_FORM_START[] PROGMEM      = "<form method='get' action='wifisave'><input id='s' name='s' length=32 placeholder='SSID'><br/><input id='p' name='p' length=64 type='password' placeholder='password'><br/>";
const char TTP_FORM_PARAM[] PROGMEM      = "<br/><input id='{i}' name='{n}' length={l} placeholder='{p}' value='{v}' {c}>";
const char TTP_FORM_END[] PROGMEM        = "<br/><button type='submit'>save</button></form>";
const char TTP_SCAN_LINK[] PROGMEM       = "<br/><div class=\"c\"><a href=\"/wifi\">Scan</a></div>";
const char TTP_SAVED[] PROGMEM           = "<div>Credentials Saved<br />Trying to connect ESP to network.<br />If it fails reconnect to AP to try again</div>";
const char TTP_END[] PROGMEM             = "</div></body></html>";

// ***************************************************************************
// Request handlers
// ***************************************************************************
void getArgs() {
  if (server.arg("rgb") != "") {
    uint32_t rgb = (uint32_t) strtol(server.arg("rgb").c_str(), NULL, 16);
    main_color.red = ((rgb >> 16) & 0xFF);
    main_color.green = ((rgb >> 8) & 0xFF);
    main_color.blue = ((rgb >> 0) & 0xFF);
  } else {
    if (server.arg("r") != "") main_color.red = server.arg("r").toInt();
    if (server.arg("g") != "") main_color.green = server.arg("g").toInt();
    if (server.arg("b") != "") main_color.blue = server.arg("b").toInt();
  }
  delay_ms = server.arg("d").toInt();
  if(server.arg("mode"))
  {
    int temp = server.arg("mode").toInt();
    if((temp<11)&&(temp>=0)){
    mode = MODE(temp);}
  }
  if(server.arg("h"))
  {
    int temp = server.arg("h").toInt();
    if((temp<4)&&(temp>=0)){
    heartmode = HEARTMODE(temp);}
  }
  if (main_color.red > 255) {
    main_color.red = 255;
  }
  if (main_color.green > 255) {
    main_color.green = 255;
  }
  if (main_color.blue > 255) {
    main_color.blue = 255;
  }

  if (main_color.red < 0) {
    main_color.red = 0;
  }
  if (main_color.green < 0) {
    main_color.green = 0;
  }
  if (main_color.blue < 0) {
    main_color.blue = 0;
  }
 if (server.arg("rain")) {
  Rainbow_color = server.arg("rain").toInt();
  
 }
  if (server.arg("d") == "") {
    if(delay_ms < 20)
    {
      delay_ms = 20;
    }
    else
    {
      delay_ms = delay_ms;
    }
  }
  if (server.arg("t")!=0) {
    DBG_OUTPUT_PORT.println();
    DBG_OUTPUT_PORT.print("arg T: ");
    DBG_OUTPUT_PORT.println(server.arg("t"));
    DBG_OUTPUT_PORT.print("Time: ");
    DBG_OUTPUT_PORT.print(hour());
    DBG_OUTPUT_PORT.print(":");
    if (minute() < 10)
      DBG_OUTPUT_PORT.print('0');
    DBG_OUTPUT_PORT.print(minute());
    if (second() < 10)
      DBG_OUTPUT_PORT.print('0');
    DBG_OUTPUT_PORT.print(":");
    DBG_OUTPUT_PORT.print(second());
    DBG_OUTPUT_PORT.print(" ");
    DBG_OUTPUT_PORT.print(day());
    DBG_OUTPUT_PORT.print(".");
    DBG_OUTPUT_PORT.print(month());
    DBG_OUTPUT_PORT.print(".");
    DBG_OUTPUT_PORT.print(year());
    DBG_OUTPUT_PORT.println();
  }

  DBG_OUTPUT_PORT.print("Mode: ");
  DBG_OUTPUT_PORT.print(mode);
  DBG_OUTPUT_PORT.print(", Color: ");
  DBG_OUTPUT_PORT.print(main_color.red);
  DBG_OUTPUT_PORT.print(", ");
  DBG_OUTPUT_PORT.print(main_color.green);
  DBG_OUTPUT_PORT.print(", ");
  DBG_OUTPUT_PORT.print(main_color.blue);
  DBG_OUTPUT_PORT.print(", Delay:");
  DBG_OUTPUT_PORT.print(delay_ms);
  DBG_OUTPUT_PORT.print(", Brightness:");
  DBG_OUTPUT_PORT.println(brightness);
}

void handleMinimalUpload() {
  char temp[1500];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf ( temp, 1500,
       "<!DOCTYPE html>\
				<html>\
					<head>\
						<title>ESP8266 Upload</title>\
						<meta charset=\"utf-8\">\
						<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\
						<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
					</head>\
					<body>\
						<form action=\"/edit\" method=\"post\" enctype=\"multipart/form-data\">\
							<input type=\"file\" name=\"data\">\
							<input type=\"text\" name=\"path\" value=\"/\">\
							<button>Upload</button>\
						</form>\
					</body>\
				</html>",
             hr, min % 60, sec % 60
           );
  server.send ( 200, "text/html", temp );
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }
  server.send ( 404, "text/plain", message );
}

char* listStatusJSON() {
  char json[255];
  snprintf(json, sizeof(json), "{\"mode\":%d, \"delay_ms\":%d, \"brightness\":%d, \"color\":[%d, %d, %d]}", mode, delay_ms, brightness, main_color.red, main_color.green, main_color.blue);
  return json;
}
void handleRootN() {
  String page = FPSTR(TTP_HEAD);
  page.replace("{v}", "Options");
  page += FPSTR(TTP_SCRIPT);
  page += FPSTR(TTP_STYLE);
  page += "";
  page += FPSTR(TTP_HEAD_END);
  page += "<h1>";
  page += "TEST";
  page += "</h1>";
  page += F("<h3>WiFiManager</h3>");
  page += FPSTR(TTP_PORTAL_OPTIONS);
  page += FPSTR(TTP_END);

  server.send(200, "text/html", page);
}

void getStatusJSON() {
  server.send ( 200, "application/json", listStatusJSON() );
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
  switch (type) {
    case WStype_DISCONNECTED:
      DBG_OUTPUT_PORT.printf("WS: [%u] Disconnected!\n", num);
      break;

    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        DBG_OUTPUT_PORT.printf("WS: [%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        String SendData;
        // send message to client
        webSocket.sendTXT(num, "Connected");
        //webSocket.sendTXT(num, "Time:2009-06-23T13:44:16"); //Format for sending time
        SendData += "Time:";
        SendData += year();
        SendData += '-';
        if (month() < 10)  SendData += '0';
        SendData += month();
        SendData += '-';
        if (day() < 10)  SendData += '0';
        SendData += day();
        SendData += 'T';
        if (hour() < 10)  SendData += '0';
        SendData += hour();
        SendData += ':';
        if (minute() < 10)  SendData += '0';
        SendData += minute();
        SendData += ':';
        if (second() < 10)  SendData += '0';
        SendData += second();
        webSocket.sendTXT(num,SendData);//send time
        
        SendData = "Mod:";
        SendData += mode;
        webSocket.sendTXT(num,SendData); //send mode

        SendData = "HEARTMODE:";
        SendData += heartmode;
        webSocket.sendTXT(num,SendData); //send mode

        SendData = "RAINBOWMODE:";
        SendData += Rainbow_color;
        webSocket.sendTXT(num,SendData); //send mode
        
        SendData = "RGB:";
        if(main_color.red<16) SendData += '0';
        SendData += String(main_color.red,HEX);
        if(main_color.green<16) SendData += '0';
        SendData += String(main_color.green,HEX);
        if(main_color.blue<16) SendData += '0';
        SendData += String(main_color.blue,HEX);
        webSocket.sendTXT(num,SendData);
      }
      break;

    case WStype_TEXT:
      DBG_OUTPUT_PORT.printf("WS: [%u] get Text: %s\n", num, payload);

      // # ==> Set main color
      if (payload[0] == '#') {
        // decode rgb data
        uint32_t rgb = (uint32_t) strtol((const char *) &payload[1], NULL, 16);
        main_color.red = ((rgb >> 16) & 0xFF);
        main_color.green = ((rgb >> 8) & 0xFF);
        main_color.blue = ((rgb >> 0) & 0xFF);
        DBG_OUTPUT_PORT.printf("Set main color to: [%u] [%u] [%u]\n", main_color.red, main_color.green, main_color.blue);
        webSocket.sendTXT(num, "OK");
      }

      // # ==> Set delay
      if (payload[0] == '?') {
        // decode delay data
        uint8_t d = (uint8_t) strtol((const char *) &payload[1], NULL, 10);
        delay_ms = ((d >> 0) & 0xFF);
        DBG_OUTPUT_PORT.printf("WS: Set delay to: [%u]\n", delay_ms);
        webSocket.sendTXT(num, "OK");
      }
      
      // T ==> Set time
      if (payload[0] == 'T') {
        // decode delay data
        String set_time = String((char *) &payload[2]);
        //DBG_OUTPUT_PORT.printf("WS: Set time to: [%s]\n", payload);
        int years = set_time.substring(0,5).toInt();
        int months = set_time.substring(5,8).toInt();
        int days = set_time.substring(8,11).toInt();
        int hours = set_time.substring(11,14).toInt();
        int minutes = set_time.substring(14,17).toInt();
        int seconds = set_time.substring(17,20).toInt();
        setTime(hours,minutes,seconds,days,months,years);
        DBG_OUTPUT_PORT.printf("\nWS: Set time to: [%d-%d-%dT%d:%d:%d]\n", years,months,days,hours,minutes,seconds);
        webSocket.sendTXT(num, "OK");
      }
      // # ==> Set brightness
      if (payload[0] == '%') {
        uint8_t b = (uint8_t) strtol((const char *) &payload[1], NULL, 10);
        brightness = ((b >> 0) & 0xFF);
        DBG_OUTPUT_PORT.printf("WS: Set brightness to: [%u]\n", brightness);
        strip.SetBrightness(brightness);
        webSocket.sendTXT(num, "OK");
      }


      // * ==> Set main color and light all LEDs (Shortcut)
      if (payload[0] == '*') {
        // decode rgb data
        uint32_t rgb = (uint32_t) strtol((const char *) &payload[1], NULL, 16);

        main_color.red = ((rgb >> 16) & 0xFF);
        main_color.green = ((rgb >> 8) & 0xFF);
        main_color.blue = ((rgb >> 0) & 0xFF);

        for (int i = 0; i < strip.PixelCount(); i++) {
          RgbColor color =  RgbColor(main_color.red, main_color.green, main_color.blue);
          strip.SetPixelColor(i, color);
        }
        strip.Show();
        DBG_OUTPUT_PORT.printf("WS: Set all leds to main color: [%u] [%u] [%u]\n", main_color.red, main_color.green, main_color.blue);
        exit_func = true;
        mode = ALL;
        webSocket.sendTXT(num, "OK");
      }

      // ! ==> Set single LED in given color
      if (payload[0] == '!') {
        // decode led index
        uint64_t rgb = (uint64_t) strtol((const char *) &payload[1], NULL, 16);

        uint8_t led =          ((rgb >> 24) & 0xFF);
        if (led < strip.PixelCount()) {
          ledstates[led].red =   ((rgb >> 16) & 0xFF);
          ledstates[led].green = ((rgb >> 8)  & 0xFF);
          ledstates[led].blue =  ((rgb >> 0)  & 0xFF);
          DBG_OUTPUT_PORT.printf("WS: Set single led [%u] to [%u] [%u] [%u]!\n", led, ledstates[led].red, ledstates[led].green, ledstates[led].blue);

          for (uint8_t i = 0; i < strip.PixelCount(); i++) {
            RgbColor color = RgbColor(ledstates[i].red, ledstates[i].green, ledstates[i].blue);
            strip.SetPixelColor(i, color);
            //DBG_OUTPUT_PORT.printf("[%u]--[%u] [%u] [%u] [%u] LED index!\n", rgb, i, ledstates[i].red, ledstates[i].green, ledstates[i].blue);
          }
          strip.Show();
        }
        exit_func = true;
        mode = ALL;
        webSocket.sendTXT(num, "OK");
      }
      if (payload[0] == 'H') {
        // we get mode data
        String str_h_mode = String((char *) &payload[1]);
        if (str_h_mode.startsWith("=0")) {
          heartmode = H_OFF;
        }
        if (str_h_mode.startsWith("=1")) {
          heartmode = H_ON;
        }
        if (str_h_mode.startsWith("=heartbeat")) {
          heartmode = H_HEARTBEAT;
        }
        if (str_h_mode.startsWith("=rainbow")) {
          heartmode = H_RAINBOW;
        }
        DBG_OUTPUT_PORT.printf("Activated heartmode mode [%u]!\n", heartmode);
        webSocket.sendTXT(num, "OK");
      }
      if (payload[0] == 'R') {
        // we get mode data
        String str_r_mode = String((char *) &payload[1]);
        if (str_r_mode.startsWith("=0")) {
          Rainbow_color = 0;
        }
        if (str_r_mode.startsWith("=1")) {
          Rainbow_color = 1;
        }
        DBG_OUTPUT_PORT.printf("Activated rainbow mode [%u]!\n", Rainbow_color);
        webSocket.sendTXT(num, "OK");
      }
      // ! ==> Activate mode
      if (payload[0] == '=') {
        // we get mode data
        String str_mode = String((char *) &payload[0]);

        exit_func = true;
        if (str_mode.startsWith("=off")) {
          mode = OFF;
        }
        if (str_mode.startsWith("=all")) {
          mode = ALL;
        }
        if (str_mode.startsWith("=wipe")) {
          mode = WIPE;
        }
        if (str_mode.startsWith("=rainbow")) {
          mode = RAINBOW;
        }
        if (str_mode.startsWith("=rainbowCycle")) {
          mode = RAINBOWCYCLE;
        }
        if (str_mode.startsWith("=theaterchase")) {
          mode = THEATERCHASE;
        }
        if (str_mode.startsWith("=theaterchaseRainbow")) {
          mode = THEATERCHASERAINBOW;
        }
        if (str_mode.startsWith("=clock")) {
          mode = CLOCK;
        }
        if (str_mode.startsWith("=test")) {
          mode = TEST;
        }
        if (str_mode.startsWith("=heart")) {
          mode = HEART;
        }

        DBG_OUTPUT_PORT.printf("Activated mode [%u]!\n", mode);
        webSocket.sendTXT(num, "OK");
      }

      // $ ==> Get status Info.
      if (payload[0] == '$') {
        DBG_OUTPUT_PORT.printf("Get status info.");

        String json = listStatusJSON();
        DBG_OUTPUT_PORT.println(json);
        webSocket.sendTXT(num, json);
      }
      break;
  }
}

void checkForRequests() {
  webSocket.loop();
  server.handleClient();
}
