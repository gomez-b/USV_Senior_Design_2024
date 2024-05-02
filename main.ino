/*
  Title: USV Senior Design 2024
  File name: main.ino
  Library Dependancies: Ping(SONAR), MAVLINK_1, SD, Time
  Purpose: This program is used to log date,time, latitude, longitude, temp, salinity, depth
  on to SD adafruit module. This programs uses ARDUINO MEGA 2560 and Pixhawk 2.4.8.
  Author: Brian Gomez Jimenez & Flor Luna 
  Date: 5/1/24
*/

/*** Ping libraries */
#include <ping-message-all.h>
#include <ping-message-common.h>
#include <ping-message-ping1d.h>
#include <ping-message-ping360.h>
#include <ping-message.h>
#include <ping-parser.h>
#include <ping1d.h>
#include "ping1d.h"
/** SD libraries */
#include <SPI.h>
#include <SD.h>
/** Mavlink Libraries  **/
#include "mavlink.h"
//** Serial Communication Libraries **/
#include <SoftwareSerial.h>
//**Time Library**//
#include <TimeLib.h>
//** Initialize data array size
#define ARRAY_SIZE 11  // Define the size of the array//without V and I its 11. INDEX START @ 0 NOT 1!
//** Insert the size array into our data_array[]  **/
String data_array[ARRAY_SIZE];  //This string will hold our entire data array!!!
/**  UART Serial Communication  **/
///** This uses SOFTWARESERIAL library from Arduino to connect the flight controller
static const uint8_t mavlinkRxPin = 10;                // Rx on PXH -> pin 10 on MCU
static const uint8_t mavlinkTxPin = 11;                // Tx on PXH -> pin 11 on MCU
SoftwareSerial mySerial1(mavlinkRxPin, mavlinkTxPin);  // Create a SoftwareSerial object
//** This uses HARDWARESERIAL library from Arduino to connect the SONAR sensor
static const uint8_t arduinoRxPin = 19;  // Rx on PXH -> pin 10 on MCU
static const uint8_t arduinoTxPin = 18;  // Tx on PXH -> pin 11 on MCU
static Ping1D ping{ Serial1 };           // Create a HardwareSerial object
/* CAN UNCOMMENT TO USE SOFTWARESERIAL, EXPERIENCED ISSUE USING TWO SOFTWARE SERIAL
THATS WHY WE USE HARDWARE AND SOFTWARE
static const uint8_t arduinoRxPin = 12;                 // White Wire on Sonar to pin 12
static const uint8_t arduinoTxPin = 13;                 // Green Wire on Sonar to pin 13
SoftwareSerial pingSerial(arduinoRxPin, arduinoTxPin);  // Create a SoftwareSerial object
static Ping1D ping{ pingSerial };
*/

//declaring floating variables for Salinity
float Salinity_Analog_Pin = A0;
float Salinity_Voltage;
float Two_point_calibration;

//declaring floating variables for Temp
float Temp_Analog_Pin = A2;
float Temp_Voltage;
float Temp;
// starting depth variable at 0
uint32_t depth = 0;
//variable to store the resistance of the thermistor
unsigned long thermistor;
//TIMER initialization
unsigned long previousMillisMAVLink = 0;
unsigned long next_interval_MAVLink = 1000;
const int num_hbs = 30;  // # of heartbeats to wait before activating STREAMS from Pixhawk. 60 = one minute.
int num_hbs_pasados = num_hbs;
//******************************//
// PROGRAM starting as false
bool program = false;
//** LED INDICATOR PINOUT **//
const int button = 5;  //BUTTON connected on pin 5
int LED_R = 7;         // Red LED
int LED_G = 6;         // Green LED
int LED_B = 4;         // Blue LED
int LED_Y = 3;         // Yellow LED
int LED_W = 2;         // White LED
//******************************//
//** Initialize Button Counter **/
int count = 0;
int newcount = 0;
/********************************/
/**Initialize CS pin for SD card **/
const int chipSelect = 53;  // Pin connected to SD card's chip select line
/*********************************/
/**Call Temperature Function **/
unsigned long resistance(unsigned long Temp_Analog);  //function to calculate resistance from Temp
float steinharthart(unsigned long resistance);        //calculates resistance for Steinhart-Hart's equation
/*********************************/

/**SETUP PROGRAM**/
void setup() {
  Serial.begin(57600);     // Sets up the serial monitor bauderate
  Serial1.begin(57600);    // Sets up the serial bauderate for SONAR
  mySerial1.begin(57600);  // Sets up the serial bauderate for Mavlink(pixhawk)
                           /** GPIO Pin Setup**/
  pinMode(Salinity_Analog_Pin, INPUT);
  pinMode(Temp_Analog_Pin, INPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_W, OUTPUT);

  /** Check to see if we have initialized SONAR, SD card, and Mavlink Communication **/
  while (!ping.initialize() || !SD.begin(chipSelect) || !mySerial1.available()) {  // Checks if sonar and SD card are available
    if (!ping.initialize()) {
      Serial.println("\nPing device failed to initialize!");
      Serial.println("Are the Ping RX/TX wired correctly?");
      Serial.print("Ping RX is the green wire, and should be connected to Arduino pin ");
      Serial.print(arduinoTxPin);
      Serial.println(" (Arduino TX)");
      Serial.print("Ping TX is the white wire, and should be connected to Arduino pin ");
      Serial.print(arduinoRxPin);
      Serial.println(" (Arduino RX)");
      Serial.println(" ");
      blinkLedFast(LED_B, 800);  // Toggle LED_B
      delay(1000);
    }

    if (!SD.begin(chipSelect)) {
      Serial.println("SD card initialization failed. Things to check:");
      Serial.println("1. Is a card inserted?");
      Serial.println("2. Is your wiring correct?");
      Serial.println("3. Did you change the chipSelect pin to match your shield or module?");
      Serial.println("Note: Press the reset button on the board and reopen this Serial Monitor after fixing your issue!");
      Serial.println(" ");
      blinkLedFast(LED_Y, 800);  // Toggle LED_Y
      delay(1000);
    }

    if (!mySerial1.available()) {
      Serial.println("Check Pixhawk Wiring");
      blinkLedFast(LED_W, 800);  // Toggle LED_Y
      delay(1000);
    }
  }
  /**If we have all connections, TURN LEDS ON **/
  digitalWrite(LED_B, HIGH);  // Turn on LED_B
  digitalWrite(LED_Y, HIGH);  // Turn on LED_Y
  digitalWrite(LED_W, HIGH);  // Turn on LED_W
  Serial.println(" ");
  Serial.println("All devices initialized successfully. Press button to continue.");
}

/** LOOP FOR CHECKING BUTTON STATE **/
void loop() {
  // Read the button state
  if (digitalRead(button) == HIGH) {
    // Wait until the button is released to avoid rapid toggling
    while (digitalRead(button) == HIGH) {
      delay(50);
    }
    newcount = count + 1;  // Increment counter

    if (newcount != count) {
      Serial.println(newcount); // Print counter state 
      switch (newcount) { // CASE 1
        case 1:           // if button counter == 1 --> START PROGRAM
          startprogram();
          break;
      }
      count = newcount; // update counter
    }
  }
}
/** MAIN LOOP MAIN LOOP MAIN LOOP**/
void startprogram() {
  program = true; // update bool program == true
  while (program) {
//  ALL these variable are related to the PIXHAWK 
//  can be found on QGROUNDCONTROL OR ARDUPILOT
    int sysid = 1;
    int compid = 151;
    int type = 1;
    uint8_t system_type = MAV_TYPE_GENERIC;
    uint8_t autopilot_type = MAV_AUTOPILOT_GENERIC;
    uint8_t system_mode = 0;
    uint32_t custom_mode = 0;
    uint8_t system_state = MAV_STATE_STANDBY;
//**********************************************
    mavlink_message_t msg; // IMPORTTANT 
    //***HELPFULL LINKS : https://mavlink.io/en/mavgen_c/ https://mavlink.io/en/guide/serialization.html
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];
    uint16_t len;
//** USING millis() instead of delay https://www.norwegiancreations.com/2017/09/arduino-tutorial-using-millis-instead-of-delay/
    unsigned long currentMillisMAVLink = millis();
    if (currentMillisMAVLink - previousMillisMAVLink >= next_interval_MAVLink) {
      previousMillisMAVLink = currentMillisMAVLink;
      num_hbs_pasados++; 
      if (num_hbs_pasados >= num_hbs) {
        Serial.println("Streams requested!");
        Mav_Request_Data(); // CALL REQUESTT DATA
        num_hbs_pasados = 0; //resett
      }
    }
    comm_receive(); // RECIEVE DATA
  }
}



void Mav_Request_Data() {
  mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];
  const int maxStreams = 1;
  const uint8_t MAVStreams[maxStreams] = { MAV_DATA_STREAM_ALL }; // refer to common.h, can be edited to requuesting specific streams
  const uint16_t MAVRates[maxStreams] = { 0x06 }; // Rate we receive message i.e 0x06 = 6 hz 

  for (int i = 0; i < maxStreams; i++) {
    mavlink_msg_request_data_stream_pack(1, 151, &msg, 1, 0, MAVStreams[i], MAVRates[i], 1);// refer to mavlink_msg_request_data_stream.h
    uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
    mySerial1.write(buf, len);
  }
}

void comm_receive() {

  static bool ready_to_log = false;  // Flag to indicate if all data is received and ready to log

  mavlink_message_t msg;
  mavlink_status_t status; // look at mavlink_types.h 

  while (mySerial1.available() > 0) { // check received on serial 
    char c = mySerial1.read();
    if (mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {// look at mavlink_helpers.h
      // Handle message and build the received data
      switch (msg.msgid) { ////< ID of message in payload
      /* UNCOMMENT TO REQUEST VOLTAGE AND CURRENT STATUS
        case MAVLINK_MSG_ID_SYS_STATUS:  //https://github.com/gomez-b/mavlink_arduinouno/blob/main/common/mavlink_msg_sys_status.h
          {
            mavlink_sys_status_t sys_status;
            mavlink_msg_sys_status_decode(&msg, &sys_status);
            uint16_t voltage = sys_status.voltage_battery;  ///< Battery voltage, in millivolts (1 = 1 millivolt)
            uint16_t current = sys_status.current_battery;  ///< Battery current, in 10*milliamperes (1 = 10 milliampere), -1: autopilot does not measure the current
            data_array[0] = voltage;
            data_array[1] = current;
          }
          break;
          */
        case MAVLINK_MSG_ID_SYSTEM_TIME: //Look att mavlink_msg_system_time.h
          {
            mavlink_system_time_t system_time;
            mavlink_msg_system_time_decode(&msg, &system_time); //Decode Packet
            uint64_t unixTimeMicroseconds = system_time.time_unix_usec; // Stores time, give in UNIX EPOCH TIME
            time_t unixTimeSeconds = static_cast<time_t>(unixTimeMicroseconds / 1000000);//CONVERT microseconds --> seconds
            setTime(unixTimeSeconds); // using the time library, set the time to what you get from the pixhawk
            // This library is helpful because it take the long time number and is easy to manipulate into date, hour, min,second
            // You can make a new variable for each item and then store itt
            data_array[0] = day(); // store the day
            data_array[1] = month(); //store the month
            data_array[2] = year(); // store the year

            int mav_hour = hour();//YOU MIGHT HAVE TO ADJUST, sometimes there need to be hour adjustments
            int adjust_hour = mav_hour - 7;
            data_array[3] = adjust_hour; // store adjusted hour
            data_array[4] = minute(); // store minute
            data_array[5] = second(); //store second 
            //* YOU CAN MAKE THIS INTO ONE WHOLE ARRAY ASWELL *//
          }
          break;

        case MAVLINK_MSG_ID_GPS_RAW_INT://look at mavlink_msg_gps_raw_int.h
          {
            mavlink_gps_raw_int_t gps_raw_int;
            mavlink_msg_gps_raw_int_decode(&msg, &gps_raw_int);
            data_array[6] = gps_raw_int.lat; //store latitude
            data_array[7] = gps_raw_int.lon; //store longitude
            readTemperatureAndSalinity(); // call temp and sal function
            readPingData();// call depth function
            if (!ready_to_log) { // ONCE WE HAVE ALL DATA, SET ready to log flag TRUE
              ready_to_log = true;
            } else {
              logDataToSD(data_array[0], data_array[1], data_array[2], data_array[3], data_array[4], data_array[5], data_array[6], data_array[7], data_array[8], data_array[9], data_array[10], data_array[11], data_array[12]);
              ready_to_log = false;  // Reset the flag
              depth = 0;
            }
          }
          break;
      }
    }
  }
}
//* FUNCTION TAKES IN ENTIRE ARRAY AND STORES IT ONTO THE SD CARD **/
void logDataToSD(String current_voltage, String current_current, String current_day, String current_month, String current_year, String current_hour, String current_minute, String current_second, String current_latitude, String current_longitude, String current_temp, String current_salinity, String current_depth) {
  // Open file for writing
  File dataFile = SD.open("USV_Data.csv", FILE_WRITE);  //SD file only [13]character!

  if (dataFile) {// if file is open....WRITE TO IT
    String labels[] = { "Voltage", "Current", "Date", "Time", "Latitude", "Longitude", "Temperature", "Salinity", "Depth" }; //labels
    String values[] = { current_voltage, current_current, current_day + "/" + current_month + "/" + current_year, current_hour + ":" + current_minute + ":" + current_second, current_latitude, current_longitude, current_temp, current_salinity, current_depth };// these values come from what is in array

    // Write data to file
    for (int i = 0; i < 9; i++) { // loop label with correlated value
      dataFile.print(labels[i] + ": ");
      dataFile.print(values[i] + " ");
      Serial.print(labels[i]);
      Serial.print(values[i]);
    }

    dataFile.println();


    dataFile.close();// close file 
    Serial.println(" Data logged to SD card.");//print stattement
    blinkLedFast(LED_G, 200); // Green LED indicating successfull log!

  } else {
    Serial.println("Error opening data file."); // if there isn't file Red LED will go off
    while (true) {
      blinkLedFast(LED_R, 800);  // Blink for 500 milliseconds
    }
  }
}


void stopprogram() { // If button counter ==2 --> STOP Program
  program = false;
  Serial.println("BUtton is pressed, program has stopped");
}

void readTemperatureAndSalinity() {
  float Temp_Analog = analogRead(Temp_Analog_Pin);  // reads A2 (Temp)
  thermistor = resistance(Temp_Analog);             // calculate resistance of the thermistor
  Temp = steinharthart(thermistor);                 // calculate temperature using the SteinHart-Hart equation
  Temp_Voltage = Temp_Analog / 1023.0 * 5.0;        // ADC conversion for Temp
                                                    // Serial.println(Temp);
                                                    // delay(1000);
  data_array[8] = String(Temp);//store temp value

  float Salinity_Analog = analogRead(Salinity_Analog_Pin);          // reads A0 (Salinity)
  Salinity_Voltage = Salinity_Analog / 1023.0 * 5.0;                // ADC conversion for Salinity
  Two_point_calibration = (12.7793 * Salinity_Voltage) - 0.236921;  // calibration equation from QuestLab

  data_array[9] = String(Two_point_calibration);//store salinity value
}

void readPingData() {
  //ping.set_ping_interval(100);
  // If there is data in the serial port read the data
  if (ping.update()) { //look at ping1d.h
    int confidence = ping.confidence();//confidence can be from 0-100, SONAR give 100% confidence when itt is fully submerged
    depth = ping.distance();  //store the distance in the depth variable

    if (confidence < 100) {//if confidence is <100 print -1 in the SD card else print the depth
      data_array[10] = -1;
    } else {
      data_array[10] = depth;//store depth data
    }
  }
}


//** Look at existing Temp sensor code /**
unsigned long resistance(unsigned long Temp_Analog) {
  unsigned long temp;
  temp = (Temp_Analog * 15000) / (1023 - Temp_Analog);
  return temp;
}
// ** Gettting tthe temperatture //**
float steinharthart(unsigned long resistance) {
  float temp;
  float logRes = log(resistance);
  float k0 = 0.00102119;
  float k1 = 0.000222468;
  float k2 = 0.000000133342;

  temp = 1 / (k0 + k1 * logRes + k2 * logRes * logRes * logRes);
  temp = temp - 273.15;
  return temp;
}
// Function to toggle the LED state rapidly
void blinkLedFast(int ledPin, unsigned long duration) {
  unsigned long blinkInterval = 100;  // Blink interval in milliseconds
  unsigned long startTime = millis();

  while (millis() - startTime < duration) {
    digitalWrite(ledPin, HIGH);  // Turn on the LED
    delay(blinkInterval);        // Wait for the specified interval
    digitalWrite(ledPin, LOW);   // Turn off the LED
    delay(blinkInterval);        // Wait for the specified interval
  }
}
