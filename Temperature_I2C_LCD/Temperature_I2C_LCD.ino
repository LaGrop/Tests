#include <OneWire.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// The DallasTemperature library can do all this work for you!
// http://milesburton.com/Dallas_Temperature_Control_Library

OneWire  ds(10);  // on pin 10 (a 4.7K resistor is necessary)

byte sensorNbr=0; // Current sensor number

// degree symbol
byte degree[8] = {
  B01100,
  B10010,
  B10010,
  B01100,
  B00000,
  B00000,
  B00000,
};

byte degreex[8] = {
  B00110,
  B01001,
  B01001,
  B00110,
  B00000,
  B00000,
  B00000,
};

void setup(void) {
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, degree); // Create degree symbol
  lcd.setCursor(0,0);
  Serial.begin(9600);
  Serial.println();
}

// Get temperature from sensor with address
float getTemp(byte *addr, byte lowResolution)
{
  byte i;
  byte present = 0;
  byte data[12];
  float celsius;
  
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(770);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

//  Serial.print("  Data = ");
//  Serial.print(present, HEX);
//  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
//    Serial.print(data[i], HEX);
//    Serial.print(" ");
  }
//  Serial.print(" CRC=");
//  Serial.print(OneWire::crc8(data, 8), HEX);
//  Serial.println();

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (lowResolution) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  return celsius;
}

void loop(void) {
  byte addr[8];
  bool lowResolution = false;
  
  if (false) {
    // Find sensors - verbose mode
    byte i;
    if ( !ds.search(addr)) {
      Serial.println("No more addresses.");
      Serial.println();
      ds.reset_search();
      sensorNbr = 0;
      return;
    }
    
    sensorNbr++;
    
    Serial.println("-------------------------------");
    Serial.print("Address =");
    for( i = 0; i < 8; i++) {
      Serial.write(' ');
      Serial.print(addr[i], HEX);
    }
    Serial.println();

    // the first ROM byte indicates which chip
    switch (addr[0]) {
      case 0x10:
        Serial.println("   Chip = DS18S20");  // or old DS1820
        lowResolution = true;
        break;
      case 0x28:
        Serial.println("   Chip = DS18B20 ");
        lowResolution = false;
        break;
      case 0x22:
        Serial.println("   Chip = DS1822");
        lowResolution = false;
        break;
      default:
        Serial.println("Device is not a DS18x20 family device.");
        return;
    } 
  
    if (OneWire::crc8(addr, 7) != addr[7]) {
        Serial.println("CRC is not valid!");
        return;
    }
    Serial.println();
  }
  else {
     if ( !ds.search(addr)) {
      Serial.println("No more sensors.");
      Serial.println();
      ds.reset_search();
      sensorNbr = 0;
      return;
    }
    sensorNbr++;
  }

  // Get and print temp
  lcd.setCursor(0,0);
  lcd.print("Temp C");
  {
    float celsiusf = getTemp(addr, lowResolution);
    String celsius = ((celsiusf<10 && celsiusf>0) ? " ":"") + String(celsiusf, 1); 
    String txt = "Sensor " + String(sensorNbr) + ": " + celsius +" C";
    Serial.println(txt);

    byte r = sensorNbr > 1 ? 1:0;
    byte c = sensorNbr & 1 ? 8:0;
    lcd.setCursor(c,r);
    lcd.print(" " + String(sensorNbr) + " " + celsius);
    lcd.write(byte(0));  // degree
    //lcd.print("C");
  }
}
