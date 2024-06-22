#include <WiFiS3.h>

WiFiServer server(80);

void wifi_ap_setup() {
  DEBUG_SERIAL.println();
  DEBUG_SERIAL.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  // a valid password must have more than 7 characters
  WiFi.config(IPAddress(192,48,56,2));
  if (!WiFi.beginAP(AVANDI_SSID, AVANDI_PASS)) {
    DEBUG_SERIAL.println("Soft AP creation failed.");
    while(1);
  }
  IPAddress myIP = WiFi.softAPIP();
  DEBUG_SERIAL.print("AP IP address: ");
  DEBUG_SERIAL.println(myIP);
  server.begin();

  DEBUG_SERIAL.println("Server started");
}

void protocol_to_qr_location(String currentLine) {
  // http://192.48.56.2/qr?text={}&delta_x={}&timestamp={}
  currentLine.replace("GET /qr?", "");
  currentLine.replace("HTTP/1.1", "");
  currentLine.replace(" ", "");
  int local_index = -1;
  int local_dx = 0;
  long local_timestamp = 0;
  while (true) {
    int char_index = currentLine.indexOf("&");
    int subline_end = currentLine.length();
    if (char_index != -1) {
      subline_end = char_index;
    }
    String substr1 = currentLine.substring(0, currentLine.indexOf("="));
    String substr2 = currentLine.substring(currentLine.indexOf("=") + 1, subline_end);
    if (substr1 == "text") {
      if (substr2 == "book_c") {
        local_index = 0;
      } else if (substr2 == "book_d") {
        local_index = 1;
      } else if (substr2 == "book_e") {
        local_index = 2;
      }
    } else if (substr1 == "delta_x") {
      local_dx = substr2.toInt();
    } else if (substr1 == "timestamp") {
      local_timestamp = substr2.toInt();
    }
    if (char_index == -1) {
      break;
    } else {
      currentLine = currentLine.substring(char_index + 1);
    }
  }
  if (local_index != -1) {
    qr_dx[local_index] = local_dx;
    qr_timestamp[local_index] = local_timestamp;
  }
  DEBUG_SERIAL.println((String) "got qr: " + local_index + ", " + local_dx + ", " + local_timestamp);
}

void protocol_to_timestamp(String currentLine) {
  currentLine.replace("GET /timestamp?timestamp=", "");
  currentLine.replace("HTTP/1.1", "");
  currentLine.replace(" ", "");
  timestamp = currentLine.toInt();
  DEBUG_SERIAL.println((String) "got timestamp: " + timestamp);
}

String book_names[3] = {
  "1. [553] ZANMANG LOOPY - kimyemin",
  "2. [554] 종강하는 날 - 박현우",
  "3. [555] MONKEY D TAESOO - kimtaesu"
};

#define Fprint(x) client.print(F(x))

void wifi_ap_client_event() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (!client) {
    return;
  }                             // if you get a client,
  //DEBUG_SERIAL.println("New Client.");           // print a message out the serial port
  String currentLine = "";                // make a String to hold incoming data from the client
  int get_type = -1;
  bool menu_borrow = false;
  while (client.connected()) {            // loop while the client's connected
    if (!client.available()) {             // if there's bytes to read from the client,
      continue;
    }
    char c = client.read();             // read a byte, then
    //DEBUG_SERIAL.write(c);                    // print it out the serial monitor
    if (c == '\n') {                    // if the byte is a newline character
      // if the current line is blank, you got two newline characters in a row.
      // that's the end of the client HTTP request, so send a response:
      if (currentLine.length() == 0) {
        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
        // and a content-type so the client knows what's coming, then a blank line:
        // break out of the while loop:
        
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println();
        // the content of the HTTP response follows the header:
        Fprint("<head>");
        Fprint("<meta http-equiv=\"Content-Type\" content=\"charset=utf-8\">");
        if (get_type != -1) {
          Fprint("<meta http-equiv=\"refresh\" content=\"1; url=/\" />");
          Fprint("</head>");
          Fprint("<body>");
          Fprint("</body>");
          client.println();
          break;
        }
        if (robot_act != 0) {
          Fprint("<meta http-equiv=\"refresh\" content=\"2; url=/\" />");
          Fprint("</head>");
          Fprint("<body>");
          Fprint("로봇이 작동중입니다.<br>");
          client.print(error_code);
          Fprint("</body>");
          client.println();
          break;
        }
        Fprint("</head>");
        Fprint("<body style=\"font-size:24px\">");
        Fprint("- 로봇 도서관 -<br><br>");
        Fprint("<a href=\"/borrow\">책 대출</a><br>");
        if (menu_borrow) {
          Fprint("대출할 책을 선택하세요.<br>");
          for (int i = 0; i < 3; i ++) {
            char book_char = 'A' + i;
            if (has_book[i]) client.print((String) "<a href=\"/" + book_char + "\">" + book_names[i] + "</a><br>");
            else client.print(book_names[i] + "(대여중)<br>");
          }
          Fprint("<a href=\"/\">메뉴 닫기</a><br><br>");
        }
        Fprint("<a href=\"/return\">책 반납</a><br>");
        Fprint("<br><br>- 관리자 모드 -<br><br>");
        Fprint("<a href=\"/sort\">책 정렬</a><br>");
        Fprint("</body>");
        // The HTTP response ends with another blank line:
        client.println();
        break;
      } else {    // if you got a newline, then clear currentLine:
        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.startsWith("GET /A")) {
          get_type = BOOK_A;
        } else if (currentLine.startsWith("GET /B")) {
          get_type = BOOK_B;
        } else if (currentLine.startsWith("GET /C")) {
          get_type = BOOK_C;
        } else if (currentLine.startsWith("GET /qr")) {
          protocol_to_qr_location(currentLine);
        } else if (currentLine.startsWith("GET /timestamp")) {
          protocol_to_timestamp(currentLine);
        } else if (currentLine.startsWith("GET /borrow")) {
          menu_borrow = true;
        } else if (currentLine.startsWith("GET /return")) {
          get_type = BOOK_RETURN;
        } else if (currentLine.startsWith("GET /sort")) {
          get_type = SORT;
        }
        currentLine = "";
      }
    } else if (c != '\r') {  // if you got anything else but a carriage return character,
      currentLine += c;      // add it to the end of the currentLine
    }
  }
  // close the connection:
  client.stop();
  //DEBUG_SERIAL.println("Client Disconnected.");

  if (robot_act != 0) {
    return;
  }
  if (get_type == BOOK_RETURN) {
    robot_act = 1;
    DEBUG_SERIAL.println("Starting to return book...");
  } else if (BOOK_A <= get_type && get_type <= BOOK_C) {
    robot_act = 2;
    robot_act_param = get_type;
    DEBUG_SERIAL.println("Starting to borrow book...");
  } else if (get_type == SORT) {
    robot_act = 3;
    DEBUG_SERIAL.println("Starting to sort books...");
  }
}