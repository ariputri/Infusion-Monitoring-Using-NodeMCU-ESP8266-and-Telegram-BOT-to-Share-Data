//Infusion Monitoring program code declaration for library section
#include <Q2HX711.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

//the program code for the HX711 module, where this program code explains the determination of the weight variable, the location of the data and clock placement on which pin, then determines the right calibration and mapping, and prints out the Telegram bot.
const byte hx711_data_pin = 5;
const byte hx711_clock_pin = 4;
char ssid[] = "";     // your network SSID (name)
char password[] = ""; // your network key
int berat=0;
int a=0;

//the program code to connect the sensor results to the Telegram bot by including the Telegram bot token, then there is code to include the Telegram user ID who acts as the user who receives the looping sensor results from the infusion weight
#define BOTtoken ""  // your Bot Token (Get from Botfather)
Q2HX711 hx711(hx711_data_pin, hx711_clock_pin);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done
bool Start = false;
boolean kirim=false;
int sementara=0;
void setup() {
  
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }
  for(int k=0;k<10; k++){
  berat=hx711.read();
 
  berat/=1000;//81
  
  berat=berat-8409;
  
  a+=berat;
  delay(1000);
  }
  berat=a/10;
  
  a=0;
  if(berat<0)berat=0;//1920=1kg 8228

  
  berat=map(berat, 0, 444, 0, 1000);
  Serial.println(berat);

  if(berat<450){
      String welcome = "Infusion fluid is decreased to: "; 
      welcome += berat;
      welcome += " ml.";
      if(bot.sendMessage("70567293", welcome))Serial.println("Succeed");
  }
  else{
    
    
 
      String welcome = "Infusion fluid volume: ";
      welcome += berat;
      welcome += " ml.";
      if(bot.sendMessage("70567293", welcome))Serial.println("Succeed");
  
  
  }
  
 
 // Serial.println(berat);
  //delay(500);
}

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if (text == "/send_test_action") {
        bot.sendChatAction(chat_id, "typing");
        delay(4000);
        bot.sendMessage(chat_id, "Did you see the action message?");

        // You can't use own message, just choose from one of bellow

        //typing for text messages
        //upload_photo for photos
        //record_video or upload_video for videos
        //record_aud io or upload_audio for audio files
        //upload_document for general files
        //find_location for location data

        //more info here - https://core.telegram.org/bots/api#sendchataction
    }
     if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "This is a bot to monitor your patient's infusion.\n\n";
      welcome += "/send_test_action : to send test chat action message\n";
      bot.sendMessage(chat_id, welcome);
    }
  }
}
