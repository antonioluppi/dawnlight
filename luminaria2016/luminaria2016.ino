#include <Wire.h>
#include "RTClib.h"
#include "luminaria2016.h"

RTC_DS3231 rtc;
int dailymin, v_azul, v_amarelo;

int irqpin = 2;  // Digital 2
boolean touchStates[12]; //to keep track of the previous touch states

int Afraco = 9;
int Bfraco = 6;
int Aforte = 5;
int Bforte = 3;

// senoid 1440
int waveform[] = {
  0, 0, 0, 1, 2, 3, 4, 6, 8, 10, 12, 14, 17, 20, 23, 26, 30, 33, 37, 41,
  46, 50, 54, 59, 64, 69, 74, 79, 84, 89, 95, 100, 105, 111, 116, 122,
  128, 133, 139, 144, 150, 155, 160, 166, 171, 176, 181, 186, 191, 196, 201, 205,
  209, 214, 218, 222, 225, 229, 232, 235, 238, 241, 243, 245, 247, 249, 251, 252,
  253, 254, 255, 255, 255, 255, 255, 254, 253, 252, 251, 249, 247, 245, 243, 241,
  238, 235, 232, 229, 225, 222, 218, 214, 209, 205, 201, 196, 191, 186, 181, 176,
  171, 166, 160, 155, 150, 144, 139, 133, 128, 122, 116, 111, 105, 100, 95, 89,
  84, 79, 74, 69, 64, 59, 54, 50, 46, 41, 37, 33, 30, 26, 23, 20,
  17, 14, 12, 10, 8, 6, 4, 3, 2, 1, 0, 0,
};

void setup () {

  Serial.begin(9600);
  delay(1000); // wait for console opening

  pinMode(irqpin, INPUT);
  digitalWrite(irqpin, HIGH); //enable pullup resistor

  Wire.begin();
  mpr121_setup();

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

}

void loop () {
 //int intensidade = readTouchInputs();

  DateTime now = rtc.now();
  dailymin = now.minute() + 24 * now.hour();
  v_azul = waveform[dailymin];
  v_amarelo = 256 - v_azul;

/*
  Serial.print("Hora: ");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print("\t Produto horaminuto: ");
  Serial.println(dailymin, DEC);
  Serial.print("waveform: ");
  Serial.println(waveform[dailymin], DEC);

  Serial.print("v_azul = ");
  Serial.println(v_azul, DEC);
  Serial.print("v_amarelo = ");
  Serial.println(v_amarelo, DEC);

  Serial.println();
  */

  
  delay(100);
}


int readTouchInputs(int anterior) {
  int intensidade = anterior;
  int potencia[] = { 0, 25, 52, 77, 105, 130, 160, 190, 230, 255};
  if (!checkInterrupt()) {

    //read the touch state from the MPR121
    Wire.requestFrom(0x5A, 2);

    byte LSB = Wire.read();
    byte MSB = Wire.read();

    uint16_t touched = ((MSB << 8) | LSB); //16bits that make up the touch states


    for (int i = 0; i < 12; i++) { // Check what electrodes were pressed
      if (touched & (1 << i)) {

        if (touchStates[i] == 0) {
          //pin i was just touched
          Serial.print("pin ");
          Serial.print(i);
          Serial.println(" was just touched");
          intensidade = potencia[i];
        }
        else if (touchStates[i] == 1) {
          //pin i is still being touched
        }

        touchStates[i] = 1;
      }
      else {
        if (touchStates[i] == 1) {
          Serial.print("pin ");
          Serial.print(i);
          Serial.println(" is no longer being touched");

          //pin i is no longer being touched
        }

        touchStates[i] = 0;
      }

    }

  }
  return intensidade;
}


void mpr121_setup(void) {

  set_register(0x5A, ELE_CFG, 0x00);

  // Section A - Controls filtering when data is > baseline.
  set_register(0x5A, MHD_R, 0x01);
  set_register(0x5A, NHD_R, 0x01);
  set_register(0x5A, NCL_R, 0x00);
  set_register(0x5A, FDL_R, 0x00);

  // Section B - Controls filtering when data is < baseline.
  set_register(0x5A, MHD_F, 0x01);
  set_register(0x5A, NHD_F, 0x01);
  set_register(0x5A, NCL_F, 0xFF);
  set_register(0x5A, FDL_F, 0x02);

  // Section C - Sets touch and release thresholds for each electrode
  set_register(0x5A, ELE0_T, TOU_THRESH);
  set_register(0x5A, ELE0_R, REL_THRESH);

  set_register(0x5A, ELE1_T, TOU_THRESH);
  set_register(0x5A, ELE1_R, REL_THRESH);

  set_register(0x5A, ELE2_T, TOU_THRESH);
  set_register(0x5A, ELE2_R, REL_THRESH);

  set_register(0x5A, ELE3_T, TOU_THRESH);
  set_register(0x5A, ELE3_R, REL_THRESH);

  set_register(0x5A, ELE4_T, TOU_THRESH);
  set_register(0x5A, ELE4_R, REL_THRESH);

  set_register(0x5A, ELE5_T, TOU_THRESH);
  set_register(0x5A, ELE5_R, REL_THRESH);

  set_register(0x5A, ELE6_T, TOU_THRESH);
  set_register(0x5A, ELE6_R, REL_THRESH);

  set_register(0x5A, ELE7_T, TOU_THRESH);
  set_register(0x5A, ELE7_R, REL_THRESH);

  set_register(0x5A, ELE8_T, TOU_THRESH);
  set_register(0x5A, ELE8_R, REL_THRESH);

  set_register(0x5A, ELE9_T, TOU_THRESH);
  set_register(0x5A, ELE9_R, REL_THRESH);

  set_register(0x5A, ELE10_T, TOU_THRESH);
  set_register(0x5A, ELE10_R, REL_THRESH);

  set_register(0x5A, ELE11_T, TOU_THRESH);
  set_register(0x5A, ELE11_R, REL_THRESH);

  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(0x5A, FIL_CFG, 0x04);

  // Section E
  // Electrode Configuration
  // Set ELE_CFG to 0x00 to return to standby mode
  set_register(0x5A, ELE_CFG, 0x0C);  // Enables all 12 Electrodes


  // Section F
  // Enable Auto Config and auto Reconfig
  /*set_register(0x5A, ATO_CFG0, 0x0B);
    set_register(0x5A, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   set_register(0x5A, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
    set_register(0x5A, ATO_CFGT, 0xB5);*/  // Target = 0.9*USL = 0xB5 @3.3V

  set_register(0x5A, ELE_CFG, 0x0C);

}

boolean checkInterrupt(void) {
  return digitalRead(irqpin);
}

void set_register(int address, unsigned char r, unsigned char v) {
  Wire.beginTransmission(address);
  Wire.write(r);
  Wire.write(v);
  Wire.endTransmission();
}



