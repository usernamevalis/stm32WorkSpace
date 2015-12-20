
#include <PCD8544.h>

static PCD8544 lcd;
byte slide[505];
int counterwoohoo;

//PINS
const int carouselTrigger = 8;

//LCD pins 2-7

void setup() {
  //initialize pins
  pinMode(carouselTrigger,OUTPUT);
  //intialize LCD
  lcd.begin(84,48);
  lcd.setCursor(0,0);
  // initialize serial communication:
  Serial.begin(115200);
  Serial.flush();
  lcd.clear();

}


//////  SUPERDUPER INTERESTING /////////

/*the screen has a limit to how fast it can refresh (this is a given).
 As a result displaying video, 24fps, is not possible. you can create a 
 very jumping movie maybe say 5 frames a second.
 
 here is the interesting part:
 it seems this screen refreshing from the inside out, 
 so by ramping up the refresh rate you manage to get from video
 frame rate(+-15fps) only in the center of the screen. 
 the periphery gets very glitchy. if you speed upp the refresh
 even faster theimage actually is bitshifted of the screen. 
 the process and manner in which the screen updates indivudual pixels
 is made clear.
 
 the other possible factor slowing down the refresh rate, 
 i.e adding to the glitchyness, is the serial buffer on the arduino
 which is 128 bytes. im sending 504 for a single frame, processing all of this 
 is what could be slowing it down but also repsonislbe for the glitches.
 where before it can write all the info to the screen, it losses some and take 
 new frame info, or alternatively is reading all 504 byte but only writing 
 the first 128 bytes. Which means the library is fucked up if you can continue 
 your next operation before it has completed displaying all the contents on the screen
 
 */
void loop() {

  Serial.print('A');

  if(Serial.available() > 0)   {   
    //serial.read == "header" (if statement) jumps out and reads image and dsiplays
    char buffer [504];
    Serial.readBytes(buffer,504);

    for(int i =0;i < 504; i++){

      slide[i] = char(buffer[i]);

    }
    digitalWrite(carouselTrigger,HIGH);
    lcd.drawBitmap(slide,84,48);
    delay(1);
    // counterwoohoo = 0; 

    Serial.print('A');  
    digitalWrite(carouselTrigger,LOW);  

  }  
}






//total of 504 charater














