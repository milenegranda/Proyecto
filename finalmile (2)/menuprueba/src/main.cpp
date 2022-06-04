#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <arduinoFFT.h>


#include "WiFi.h"
#include "Audio.h"
#include "SD.h"
#include "FS.h"

//#include "SSD1306Wire.h"
#include "SparkFunHTU21D.h"

// Digital I/O used
#define SD_CS          5
#define SPI_MOSI      23
#define SPI_MISO      19
#define SPI_SCK       18
#define I2S_DOUT      17
#define I2S_BCLK      27
#define I2S_LRC       26



#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Audio audio;

String ssid = "Mi Ada";
String password = "telefono4ada";


HTU21D myHumidity;

TaskHandle_t Task0;
 
 int selected= 0;
  int entered=-1; 
 
  int total_options = 3;


  float temp = 0;

void ShowMenu(char **options,int num_options,char *opcion){
      char title[200];
      sprintf(title,"%s (%f)",opcion, temp );
      total_options = num_options;
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println(F(title));
      display.setTextSize(0);
      display.println("");
      for (int i = 0; i < num_options; i++) {
        if (i == selected) {
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.println(options[i]);
        } else if (i != selected) {
          display.setTextColor(SSD1306_WHITE);
          display.println(options[i]);
        }
      }

  }


void displaySubMenu()
{

const char *options[3] = {
    " 1.- RADIO",
    " 2.- MUSICA SD",
    " 3.- VOLUMEN",
  };

  const char *options_radio[4] = {
    " 1.- RADIO 1",
    " 2.- RADIO 2",
    " 3.- RADIO 3",
    " 4.- RADIO 4",
  };

  const char *options_volumen[4] = {
    " 1.- BAJO",
    " 2.- MEDIO",
    " 3.- ALTO",
    " 4.- MUY ALTO",
    
  };
  const char *options_SD[2] = {
    "N95 - Kendrick Lamar",
    "Nights - Frank Ocean",
    
  };

  if (entered == -1) {
    
        ShowMenu((char **)options,3,"MENU");

  } 
  else if (entered == 0) {

 ShowMenu((char **)options_radio,4,"RADIO");


  } 

  else if (entered == 1) {
   ShowMenu((char **)options_SD,2,"SD");
  }

  else if (entered == 2) {
    ShowMenu((char **)options_volumen,4,"VOLUMEN");

  }



   display.display();

}




void displaymenu() {

  int button1 = digitalRead(25); //down
 // int button2 = digitalRead(33);  //up
  int button3 = digitalRead(32); //enter
  int button4 = digitalRead(33); //back

  

  if (button1 == LOW) {
    if(selected<total_options-1){
      selected = selected + 1;
      delay(200);
    }
    else{
      selected=0;
      delay(200);
    }
     displaySubMenu();
  };

  if (button3 == LOW) {
    
  
      if(entered==-1){
          entered = selected;

          if(entered==0){
             // En radio seleccionamos por defecto según la temperatura
             if(temp>30){
               selected = 0;
               
             }
             else{
                selected = 1;
             }
          }
          else {

            selected = 0;
          }

          
          displaySubMenu();
      }
      else if (entered==0){

          if(selected==0){
             // PLay radio  0
              Serial.println("1980s (London, UK)");
               
            audio.connecttohost("http://19353.live.streamtheworld.com/977_80_SC");
          }
          else if(selected==1){
             // PLay radio 1
               Serial.println("http://mp3.ffh.de/radioffh/hqlivestream.aac");
              audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.aac");
           }
            else if(selected==2){
             // PLay radio 2
               Serial.println("AB Classic Rock Live");
              audio.connecttohost("http://s3-webradio.antenne.de/classic-rock-live.aac");
           }
            else if(selected==3){
             // PLay radio 2
               Serial.println("The Big 80s Station");
             // audio.connecttohost("http://s5.nexuscast.com:8065/;");
              audio.connecttohost("http://158.69.114.190:8024/stream");
           }


      }
      else if (entered==1){
          if(selected==0){
             // PLay file SD  0
              audio.connecttoFS(SD, "/N9532kbps.mp3");
          }
          else if(selected==1){
             // PLay file SD 1
              audio.connecttoFS(SD, "/ocean32kbps.mp3");
           }

      }
       else if (entered==2){
          if(selected==0){
            audio.setVolume(5);
          }
          else if(selected==1){
              audio.setVolume(10);
           }
            else if(selected==2){
              audio.setVolume(15);
           }
            else if(selected==3){
              audio.setVolume(21);
           }

      }

      
     
      
  };



  if (button4 == LOW) {
    audio.stopSong();
    entered = -1;
    selected = 0;
     displaySubMenu();
  };



 
}

void Task0code( void * pvParameters ){
  delay(100);
  for (;;){  //create an infinate loop

     temp = myHumidity.readTemperature();
      delay(100);
    }
  }


  void setupTemp() {
    Serial.begin(115200);
    Serial.println();
    Serial.println();

    Serial.println("HTU21D Example!");

    myHumidity.begin();
}


void setupSD(){
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    Serial.begin(115200);
    SD.begin(SD_CS);
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(10); // 0...21
   
   
}

void setupRadio() {
    pinMode(SD_CS, OUTPUT);      digitalWrite(SD_CS, HIGH);
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    Serial.begin(115200);
    SD.begin(SD_CS);
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED) delay(1500);
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(21); // 0...21
}



void setup() {

 setupTemp();

 //setupSD();
  setupRadio();

  pinMode(25, INPUT_PULLUP);
 // pinMode(33, INPUT_PULLUP);
  pinMode(32, INPUT_PULLUP);
  pinMode(33, INPUT_PULLUP);


  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);
  display.display();
  delay(2000); // Pause for 2 seconds

 displaySubMenu();

xTaskCreatePinnedToCore(
                    Task0code,   
                    "Task0",    
                    10000,      
                    NULL,      
                    2,          
                    &Task0,     
                    1);               


}


 



void loop() {
  // put your main code here, to run repeatedly:


  displaymenu();
  
  audio.loop();
  
}


