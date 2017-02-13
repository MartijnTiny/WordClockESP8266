// ***************************************************************************
// Color modes
// ***************************************************************************
#include "clockface.h"
int dipInterval = 10;
int darkTime = 250;
unsigned long currentDipTime;
unsigned long dipStartTime;
unsigned long currentMillis;
int ledState = LOW;
long previousMillis = 0;
int led = 5;
int interval = 2000;
int twitch = 50;
int dipCount = 0;
int analogLevel = 100;
boolean timeToDip = false;
int ledStates[NUMLEDS];

void hsb2rgbAN1(uint16_t index, uint8_t sat, uint8_t bright, uint8_t myled) {
  // Source: https://blog.adafruit.com/2012/03/14/constant-brightness-hsb-to-rgb-algorithm/
  uint8_t temp[5], n = (index >> 8) % 3;
  temp[0] = temp[3] = (uint8_t)((                                        (sat ^ 255)  * bright) / 255);
  temp[1] = temp[4] = (uint8_t)((((( (index & 255)        * sat) / 255) + (sat ^ 255)) * bright) / 255);
  temp[2] =          (uint8_t)(((((((index & 255) ^ 255) * sat) / 255) + (sat ^ 255)) * bright) / 255);

  RgbColor color = RgbColor(temp[n + 2], temp[n + 1], temp[n]);
  color = colorGamma.Correct(color);
  strip.SetPixelColor(myled, color);
}


void updateLed (int led, int brightness) {
  ledStates[led] = brightness;

  for (int i = 0; i < NUMLEDS; i++)
  {
    uint16_t index = (i % 3 == 0) ? 400 : random(0, 767);
    hsb2rgbAN1(index, 200, ledStates[i], i);
  }
  strip.Show();
}

void clearEdge(void)
{
  strip.SetPixelColor(0,0);
  strip.SetPixelColor(1,0);
  strip.SetPixelColor(122,0);
  strip.SetPixelColor(123,0);
  strip.SetPixelColor(124,0);
  strip.SetPixelColor(125,0);
  strip.SetPixelColor(126,0);
}
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
RgbColor Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    RgbColor color = RgbColor(((255 - WheelPos * 3) / 2), 0, ((WheelPos * 3) / 2));
    // color = colorGamma.Correct(color);
    return color;
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    RgbColor color = RgbColor(0, ((WheelPos * 3) / 2), ((255 - WheelPos * 3) / 2));
    // color = colorGamma.Correct(color);
    return color;
  }
  WheelPos -= 170;
  RgbColor color = RgbColor(((WheelPos * 3) / 2), ((255 - WheelPos * 3) / 2), 0);
  // color = colorGamma.Correct(color);
  return color;
}

/*
  // See: http://forum.mysensors.org/topic/85/phoneytv-for-vera-is-here/13
  void tv() {
	checkForRequests();
	if (exit_func) {
		exit_func = false;
		return;
	}

	if (timeToDip == false)
	{
		currentMillis = millis();
		if(currentMillis-previousMillis > interval)
		{
			previousMillis = currentMillis;
			interval = random(750,4001);//Adjusts the interval for more/less frequent random light changes
			twitch = random(40,100);// Twitch provides motion effect but can be a bit much if too high
			dipCount = dipCount++;
		}
		if(currentMillis-previousMillis<twitch)
		{
			led=random(0, (strip.PixelCount()-1));
			analogLevel=random(50,255);// set the range of the 3 pwm leds
			ledState = ledState == LOW ? HIGH: LOW; // if the LED is off turn it on and vice-versa:

			updateLed(led, (ledState) ? 255 : 0);

			if (dipCount > dipInterval)
			{
				DBG_OUTPUT_PORT.println("dip");
				timeToDip = true;
				dipCount = 0;
				dipStartTime = millis();
				darkTime = random(50,150);
				dipInterval = random(5,250);// cycles of flicker
			}
			//strip.Show();
		}
	}
	else
	{
		DBG_OUTPUT_PORT.println("Dip Time");
		currentDipTime = millis();
		if (currentDipTime - dipStartTime < darkTime)
		{
			for (int i=3; i<strip.PixelCount(); i++)
			{
				updateLed(i, 0);
			}
		}
		else
		{
			timeToDip = false;
		}
		strip.Show();
	}
  }

*/
// Fill the dots one after the other with a color
void colorWipe(RgbColor c, uint8_t wait) {
  uint16_t i;
  for (i = 0; i < strip.PixelCount(); i++) {
    RgbColor color = RgbColor(0, 0, 0);
    strip.SetPixelColor(i, color);
  }
  strip.Show();
  c = colorGamma.Correct(c);
  for (uint16_t i = 2; i < strip.PixelCount()-4; i++) {
    strip.SetPixelColor(i, c);
    strip.Show();

    checkForRequests();
    if (exit_func) {
      exit_func = false;
      return;
    }

    delay(wait);
  }
  mode = HOLD;
}

void rainbow(uint8_t wait) {
  uint16_t i, j;
  clearEdge();
  for (j = 0; j < 256; j++) {
    for (i = 2; i < strip.PixelCount()-4; i++) {
      strip.SetPixelColor(i, Wheel((i + j) & 255));
    }
    strip.Show();

    checkForRequests();
    if (exit_func) {
      exit_func = false;
      return;
    }

    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  
  for (j = 0; j < 256; j++) { // 1 cycle of all colors on wheel
    for (i = 2; i < strip.PixelCount()-4; i++) {
      strip.SetPixelColor(i, Wheel(((i * 256 / strip.PixelCount()) + j) & 255));
    }
    strip.Show();

    checkForRequests();
    if (exit_func) {
      exit_func = false;
      return;
    }

    delay(wait);
  }
}

// Theatre-style crawling lights.
void theaterChase(RgbColor c, uint8_t wait) {
  clearEdge();
  c = colorGamma.Correct(c);
  for (int q = 0; q < 3; q++) {
    for (int i = 2; i < strip.PixelCount()-5; i = i + 3) {
      strip.SetPixelColor(i + q, c);  //turn every third pixel on
    }
    strip.Show();

    checkForRequests();
    if (exit_func) {
      exit_func = false;
      return;
    }
    delay(wait);

    for (int i = 2; i < strip.PixelCount()-5; i = i + 3) {
      strip.SetPixelColor(i + q, 0);      //turn every third pixel off
    }
  }
}

// Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  clearEdge();
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (int i = 2; i < strip.PixelCount()-5; i = i + 3) {
        strip.SetPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      strip.Show();

      checkForRequests();
      if (exit_func) {
        exit_func = false;
        return;
      }
      delay(wait);

      for (int i = 2; i < strip.PixelCount()-5; i = i + 3) {
        strip.SetPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}


void printDigits(int digits)
{
  // utility for digital clock display: prints preceding colon and leading 0
  DBG_OUTPUT_PORT.print(":");
  if (digits < 10)
    DBG_OUTPUT_PORT.print('0');
  DBG_OUTPUT_PORT.print(digits);
}

void digitalClockDisplay()
{
  // digital clock display of the time
  DBG_OUTPUT_PORT.print(hour());
  printDigits(minute());
  printDigits(second());
  DBG_OUTPUT_PORT.print(" ");
  DBG_OUTPUT_PORT.print(day());
  DBG_OUTPUT_PORT.print(".");
  DBG_OUTPUT_PORT.print(month());
  DBG_OUTPUT_PORT.print(".");
  DBG_OUTPUT_PORT.print(year());
  DBG_OUTPUT_PORT.println();
}

void resetStrip() {
  stackptr = 0;
  for(int i = 0; i<strip.PixelCount(); i++) {
    strip_ptr[i] =  0;
  }
}

void resetAndBlack() {
  resetStrip();
  for(int i = 0; i<strip.PixelCount(); i++) {
    
    strip.SetPixelColor(i, RgbColor(0,0,0));
  }
}

void displayStrip(RgbColor colorCode) {
  colorCode = colorGamma.Correct(colorCode);
  for(int i = 0; i<stackptr; i++) {
    strip.SetPixelColor(strip_ptr[i], colorCode);
  }
  strip.Show();
}

void Clock()
{
  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
      digitalClockDisplay();
      resetAndBlack();
      timeToStrip(hour(),minute());
      displayStrip(RgbColor(main_color.red,main_color.green,main_color.blue));
    }
  }

}

void fastTest() {
  if(millis() >= waitUntilFastTest) {
    DBG_OUTPUT_PORT.print("TESTMODE");
    waitUntilFastTest = millis();
    if(testMinutes >= 60) {
      testMinutes = 0;
      testHours++;
    }
    if(testHours >= 24) {
      testHours = 0;
    }
    
    //Array empty
    resetAndBlack();
    timeToStrip(testHours, testMinutes);
    displayStrip(RgbColor(main_color.red,main_color.green,main_color.blue));
    testMinutes++;
    waitUntilFastTest += oneSecondDelay;
  }
}

void displayHeart()
{
  //DBG_OUTPUT_PORT.print("Heart");
  resetAndBlack();
  W_HART();
  displayStrip(RgbColor(main_color.red,main_color.green,main_color.blue));
}


