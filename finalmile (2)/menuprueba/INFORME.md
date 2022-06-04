
# PROYECTO PD: La música  a través de la temperatura 
## Objetivos
Para este proyecto nuestros objetivos han sido:
Encontrar funciones diferentes a las que hemos aplicado a los dispositivos en clase.
Regular la música que se transmitirá por el altavoz dependiendo de la temperatura a la que esté el ambiente en el momento en que se ejecute el programa. Por ejemplo, si en ese momento el ambiente es caluroso por el altavoz sonará una emisora de radio de una zona con temperaturas elevadas y asimismo si la temperatura es baja.

## Materiales
En este proyecto hemos utilizado los siguientes materiales:
- ESP32
- 3 pulsadores
- Altavoz 
- Amplificador de Audio Módulo decodificador I2S Dac sin filtrar
- SPI Reader Lector de Tarjeta SD
- OLED Display I2C
- KYYKA HTU21D I2C - Módulo de sensor de humedad
- Protoboard
- Cables M-H
- Cables M-M

## Funcionamiento
A través del menú que presenta el display tendremos 3 opciones:
- La opción Radio: 
Con esta primera opción, el ESP32 detecta a través del sensor de temperatura esta misma para poder clasificarla a través del código (main.cpp) y así poder transmitir la radio más acertada a la temperatura que esté haciendo en ese momento.


- La opción Archivo SD:
Si el usuario no quiere escuchar una radio aleatoria regulada por la temperatura, tiene esta segunda opción en donde puede escuchar por el altavoz un archivo guardado en la microSD que leerá por ende el lector de Tarjetas SD
- La opción volumen: 
El usuario tiene la opción de poder regular el volumen al que quiere escuchar tanto el archivo .wav o .mp3 como la radio. 


## Explicación del código

```
#include <Arduino.h>
//Libreria para SPI
#include <SPI.h>
#include <Wire.h>
//Libreria para los dispositivos I2C
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <arduinoFFT.h>
//Libreria Wi-Fi
#include "WiFi.h"
//Libreria de Audio
#include "Audio.h"
//Libreria para la microSD
#include "SD.h"
#include "FS.h"
//#include "SSD1306Wire.h"
#include "SparkFunHTU21D.h"
///Definimos los Pines: 
// Digital I/O used
#define SD_CS          5
#define SPI_MOSI      23
#define SPI_MISO      19
#define SPI_SCK       18
#define I2S_DOUT      17
#define I2S_BCLK      27
#define I2S_LRC       26
#define SCREEN_WIDTH 128 // OLED display width, en pixels
#define SCREEN_HEIGHT 64 // OLED display height, en pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Audio audio;
///Inicializamos los datos wifi para poder encontrar las emisoras.
String ssid = "Mi Ada";
String password = "telefono4ada";
HTU21D myHumidity;
TaskHandle_t Task0;
 
 int selected= 0;
  int entered=-1; 
 
  int total_options = 3;
  float temp = 0;
/// Se muestra un menu concreto del usuario. options es un vector de opciones, num_opciones:numero de opciones que tiene el menú.Option: es el titulo del menu
//Se mira la variable global selected para cambiar el color d ela opcion seleccionada
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
//Estos vectores definen las diferentes opciones de los menús y se pasará como parámetro a la función anterior
///El menú tiene las opciones siguientes:
const char *options[3] = {
    " 1.- RADIO",
    " 2.- MUSICA SD",
    " 3.- VOLUMEN",
  };
///La opción 1 nos muestra:
  const char *options_radio[4] = {
    " 1.- RADIO 1",
    " 2.- RADIO 2",
    " 3.- RADIO 3",
    " 4.- RADIO 4",
  };
///La opción 3 nos muestra:
  const char *options_volumen[4] = {
    " 1.- BAJO",
    " 2.- MEDIO",
    " 3.- ALTO",
    " 4.- MUY ALTO",
    
  };
  ///La opción 2 nos muestra:
  const char *options_SD[2] = {
    "N95 - Kendrick Lamar",
    "Nights - Frank Ocean",
    
  };
  
  
  //La variable global entered indica en que submenú estamos. Entered =-1 es el menú principal
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
// declaramos los botones para poder controlar el menú
void displaymenu() {
  int button1 = digitalRead(25); //down
 // int button2 = digitalRead(33);  //up
  int button3 = digitalRead(32); //enter
  int button4 = digitalRead(33); //back
  
// dependiendo de las condiciones de los botones: 
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
  
  //Han pulsado en entrar
  if (button3 == LOW) {
    
  //estamos en el menú principal
      if(entered==-1){
          entered = selected;
          if(entered==0){
          //han seleccionado entrar en el menú radio
             // En radio seleccionamos por defecto según la temperatura usando la variable global temp
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
///Se conectan las radios a través de los siguientes enlaces y a través de la opción escogida
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
             // PLay radio 3
               Serial.println("The Big 80s Station");
             // audio.connecttohost("http://s5.nexuscast.com:8065/;");
              audio.connecttohost("http://158.69.114.190:8024/stream");
           }
      }
      //Se selcciona el archivo de la microSD
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
      //Se selecciona el volumen que quiera el usuario
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

//esta es la tarea que lee todo el rato la temperatura y la guarda en una variable global que se llama temp.
// la variable temp es global porque será consultada desde la otra tarea para que cuando se entre en el menú radio en lugar de seleccionar por defecto la 0 
// seleccione una dependiendo de esta variable global
void Task0code( void * pvParameters ){
  delay(100);
  for (;;){  //create an infinate loop
     temp = myHumidity.readTemperature();
      delay(100);
    }
  }
  void setupTemp() {
    Serial.begin(115200);//pone max 115200bps
    Serial.println();
    Serial.println();
    Serial.println("HTU21D Example!"); //escribe en la terminal
    myHumidity.begin(); //inicializa el sensor de humedad y temperatura
}
void setupSD(){ //setup SD
    pinMode(SD_CS, OUTPUT); //conecta los pines
    digitalWrite(SD_CS, HIGH);
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI); //inicializa el SPI
    Serial.begin(115200);//pone max 115200bps
    SD.begin(SD_CS); //inicializa SD
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(10); // pone el volumen 0 a 21 
   
   
}
void setupRadio() {//setup radio
    pinMode(SD_CS, OUTPUT);  //conecta los pines
    digitalWrite(SD_CS, HIGH);
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI); //inicializa el SPI
    Serial.begin(115200); //pone max 115200bps
    SD.begin(SD_CS); //inicializa SD
    WiFi.disconnect(); //DESCONECTA EL WI-FI
    WiFi.mode(WIFI_STA); //llama al mode
    WiFi.begin(ssid.c_str(), password.c_str()); //pone el SSID y la contraseña del wi-fi
    while (WiFi.status() != WL_CONNECTED) delay(1500); //si no se conecta tiene un delay
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT); 
    audio.setVolume(21); // pone el volumen 0 a 21 
}
void setup() {
 setupTemp(); //llamada al setupTemp()
 //setupSD();
  setupRadio();//llamada al setupRadio()
  pinMode(25, INPUT_PULLUP); //pone los pines como INPUT
 // pinMode(33, INPUT_PULLUP);
  pinMode(32, INPUT_PULLUP);
  pinMode(33, INPUT_PULLUP);
  Serial.begin(115200);//pone max 115200bps
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { //si no puede inicializar el display 
    Serial.println(F("SSD1306 allocation failed"));//escribirá SSD1306 allocation failed
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  // dibuja u pixel en blanco
  display.drawPixel(10, 10, SSD1306_WHITE);
  display.display();
  delay(2000); // Pausa por 2 seconds
 displaySubMenu();
 //crea la tarea ask0code y la pone en el core 1
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

  displaymenu();
  
  audio.loop();
  
}
```
Ver vídeo para entender más claramente las salidas.
