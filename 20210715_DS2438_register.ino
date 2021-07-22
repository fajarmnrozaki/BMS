//Register DS2438
//title           :ReadingDS2438.ino
//description     :DS2438 Register.
//author          :Fajar Muhammad Noor Rozaqi
//date            :2021/07/08
//version         :0.3
//usage           :BMS
//notes           :DS2438 Registers (Monitoring V,I,T,ICA)

//library
#include <OneWire.h>

//pin connection ds2438 to microcontroller
OneWire ds(4);

void setup(void) {
  Serial.begin(9600);
}

void loop(void) {
  byte i;
  byte data[12];
  byte addr[8];

  //Sensor ID DS2438
  while (!ds.search(addr)) {
    ds.reset_search();
//    Serial.println("No more DS2438 sensors");
    delay(1000);
  }

 for ( i = 0; i < 8; i++) { // we need 9 bytes, last one for CRC
   Serial.print(addr[i], HEX);
   Serial.print(":");
 }
 Serial.print(" ");

   //==================================================================================================== 
  // Reading Page 0
  // Configure chip to read VAD and not VDD
  ds.reset();
  ds.select(addr);
  ds.write(0x4E); //write scratchpad
  ds.write(0x00); //opening page 0 
  ds.write(0x00); //opening page 0

  // Convert voltage (0XB4)
  ds.reset();
  ds.select(addr);
  ds.write(0xB4);//Voltage Conversion
  delay(20);

  // Start temperature conversion
  ds.reset();
  ds.select(addr);
  ds.write(0x44); //Temperature Conversion
  delay(20);
  
  // Current readings are updated automatically
  
  // Recall 0xB8
  ds.reset();
  ds.select(addr);
  ds.write(0xB8);//Recalling
  ds.write(0x00);

  // Read Scratchpad 0xBE
  ds.reset();
  ds.select(addr);
  ds.write(0xBE); //Read Scratchpad
  ds.write(0x00);
  
  for ( i = 0; i < 9; i++) { // we need 9 bytes, last one for CRC
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }

  // CRC
  if ( OneWire::crc8(data, 8) != data[8]) {
    Serial.println("CRC is not valid!");
    return;
  }
  
  Serial.print("|");
 
  // Shit Register Union Method
  Serial.print(" Temperature =");
  float temp = (((((int16_t) data[2]) << 8) | data[1]) >> 3) * 0.03125;
  Serial.print(temp);
  Serial.print(" |");

  Serial.print(" Voltage VAD =");
  float vad = ((((int16_t)data[4]) << 8) | data[3]) * 0.01;
  Serial.print(vad);
  Serial.print(" |");

  Serial.print(" Current =");
  float iad = ((((int16_t)data[6]) << 8) | data[5] << 2) * 0.2441 / (4096 * 0.0005);
  Serial.print(iad);
  Serial.print(" |");

  // Configure chip to read VDD and not VAD
  ds.reset();
  ds.select(addr);
  ds.write(0x4E);
  ds.write(0x00);
  ds.write(0x0F); //VDD

  // Convert voltage 0xB4
  ds.reset();
  ds.select(addr);
  ds.write(0xB4);
  delay(20);

  // Recall 0xB8
  ds.reset();
  ds.select(addr);
  ds.write(0xB8);
  ds.write(0x00);

  // Read Scratchpad 0xBE
  ds.reset();
  ds.select(addr);
  ds.write(0xBE);
  ds.write(0x00);
  
  //  Serial.print(" ");
    for ( i = 0; i < 9; i++) { // we need 9 bytes, last one for CRC
    data[i] = ds.read();
  }

  // CRC
  if ( OneWire::crc8(data, 8) != data[8]) {
    Serial.println("CRC is not valid!");
    return;
  }
  
  Serial.print(" VDD=");
  float vdd = ((((int16_t)data[4]) << 8) | data[3]) * 0.01;
  Serial.print(vdd);
  Serial.println(".");
  Serial.println("~~~~~~~~~~~~~");
  delay(3000);

  //==================================================================================================== 
  //ENABLING ICA, CCA, AND DCA ON SINGLE DS2438 (BUS MASTER COMMAND)  
  // Reading Page 1
  ds.reset();
  ds.select(addr);
  ds.write(0x4E); //write scratchpad
  ds.write(0x00); //page 0
  ds.write(0x0F); //Sets ICA, CA, EE, AD BITS ACTIVE

  // Read Scratchpad 0xBE
  ds.reset();
  ds.select(addr);
  ds.write(0xBE);
  ds.write(0x00); //Still not sure

  // Activate DS2438 BECOME 1
  ds.reset();
  ds.select(addr);
  ds.write(0x48);
  ds.write(0x00);
  ds.reset();
  
  //==================================================================================================== 
  // Reading Page 1
  ds.reset();
  ds.select(addr);
  ds.write(0x4E); //write scratchpad
  ds.write(0x01); //Page 1
  ds.write(0x01); //Page 1

  // Recall 0xB8
  ds.reset();
  ds.select(addr);
  ds.write(0xB8); // Recall 0xB8
  ds.write(0x01); // Page 1

  // Read Scratchpad 0xBE
  ds.reset();
  ds.select(addr);
  ds.write(0xBE);
  ds.write(0x01);
  
  for ( i = 0; i < 9; i++) { // we need 9 bytes, last one for CRC
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }

  // CRC
  if ( OneWire::crc8(data, 8) != data[8]) {
    Serial.println("CRC is not valid!");
    return;
  }

  Serial.print("|");
  
  Serial.print(" ICA=");
  float ica = (data[4] * 0.4882 / (2048 * 0.0005));
  Serial.print(ica);
  Serial.print(" |");
  //  Serial.println("~~~~~~~~~~~~~");

  //==================================================================================================== 
  // Reading Page 7
  ds.reset();
  ds.select(addr);
  ds.write(0x4E); //write scratchpad
  ds.write(0x07); //Page 7
  ds.write(0x07); //Page 7

  // Recall 0xB8
  ds.reset();
  ds.select(addr);
  ds.write(0xB8); // Recall 0xB8
  ds.write(0x07); // Page 7
  
  // Read Scratchpad 0xB8
  ds.reset();
  ds.select(addr);
  ds.write(0xB8);
  ds.write(0x07);
  
  for ( i = 0; i < 9; i++) { // we need 9 bytes, last one for CRC
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }

  // CRC
  if ( OneWire::crc8(data, 8) != data[8]) {
    Serial.println("CRC is not valid!");
    return;
  }

  Serial.print("|");
  
  Serial.print(" CCA=");
  float cca = (data[5]| data[4]) * 15.625;
  Serial.print(ica);
  Serial.print(" |");

  Serial.print(" DCA=");
  float dca = (( data[7]| data[6]) * 15.625;)
  Serial.print(dca);
  Serial.println(".");
  Serial.println("~~~~~~~~~~~~~");
 
  delay(3000);
}  
