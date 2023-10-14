
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
//  #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

//#include <WiFi.h>

//#include <ArduinoBLE.h> // Not compatible with nRF52 arch
#include <bluefruit.h> // Use this instead - need to change code below !!
// Examples: https://github.com/adafruit/Adafruit_nRF52_Arduino/tree/master/libraries/Bluefruit52Lib

//#include "arduinoFFT.h"
#include <arduinoFFT.h>

//arduinoFFT FFT;

#include <Dictionary.h>

// Peripheral uart service
BLEUart bleuart;


// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN   22
#define LED_COUNT 48

const int VIBRATION_SENSOR = 3;
const uint16_t samples = 64; //This value MUST ALWAYS be a power of 2
const double signalFrequency = 1000;
const double samplingFrequency = 5000;
const uint8_t amplitude = 100;
double vReal[samples];
double vImag[samples];
int bitTime = 0;


arduinoFFT FFT;

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// RED BLUE CYAN YELLOW ORANGE GREEN VIOLET
//  C .  D  .  E    F  .   G   .  A  .  B     C D EF G A BC

// default in C
const char sA[] = {'b', '', 'm', 'r', '', 'p', '', 'v', 'y', '', 'o', ''};
const char sE[] = {'v', 'y', '', 'o', '', 'b', '', 'm', 'r', '', 'p', ''};
const char sC[] = {'r', '', 'p', '', 'v', 'y', '', 'o', '' , 'b', '', 'm'};
const char sG[] = {'o', '', 'b', '', 'm', 'r', '', 'p', '' , 'v', 'y', ''};

const int KeyOffSet [] = {};

const int KeyC_CStringRed [] =   {0, 0,  0, 0, 127,0,127, 0,0, 0, 64,127,  0,0,0,0,127,0,127,0,0,0,64,127}; //12 to 24
const int KeyC_CStringGreen [] = {0, 0,  0,127,127,0,64,  0,0,127, 0, 0,    0,0,0,127,127,0,64,0,0,127,0,0}; //12 to 24
const int KeyC_CStringBlue [] =  {0,127, 0,127, 0, 0, 0,  0,0, 0, 127,0,   0,127,0,127,0,0,0,0,0,0,127,0}; //12 to 24
//  .                             .   B    C  . D   .  E      F   .  G   .  A  . 
const int KeyC_AStringRed [] =   {0, 127, 127,0,0,  0, 0,    127, 0, 127, 0, 0,  0, 127, 127,0,0,  0, 0,   127,  0, 127, 0, 0}; //12 to 24
const int KeyC_AStringGreen [] = {0, 0,   0,  0,0,  0, 127,  127, 0, 64,  0,127, 0, 0,   0,  0,0,  0, 127, 127,  0, 64,  0, 127}; //12 to 24
const int KeyC_AStringBlue [] =  {0, 127, 0,  0,127,0, 127,  0,   0, 0,   0, 0 ,  0, 127, 0,  0,127,0, 127,  0,   0, 0,   0, 0};
//  F . G . A . B C . D . E 
const int KeyC_EStringRed [] =   {127, 0,127,0, 0,  0, 127, 127, 0, 0,   0, 0,   127, 0,127,0, 0,  0, 127, 127, 0, 0,   0, 0}; //12 to 24
const int KeyC_EStringGreen [] = {127, 0,64, 0, 127, 0, 0,   0 , 0, 0,   0, 127, 127, 0,64, 0, 127, 0, 0,   0 , 0, 0,   0, 127}; //12 to 24
const int KeyC_EStringBlue [] =  {0,  0, 0,  0, 0,  0, 127,  0,  0, 127, 0, 127,  0,  0, 0,  0, 0,  0, 127,  0,  0, 127, 0, 127};
//  . A . B C . D . E F . G
const int KeyC_GStringRed [] =   {0, 0,  0, 127, 127, 0, 0, 0, 0,  127, 0, 127, 0, 0,  0, 127, 127, 0, 0, 0, 0,  127, 0, 127}; //12 to 24
const int KeyC_GStringGreen [] = {0, 127,0, 0,  0,  0,  0, 0, 127, 127, 0, 64, 0, 127,0, 0,  0,  0,  0, 0, 127, 127, 0, 64}; //12 to 24
const int KeyC_GStringBlue [] =  {0, 0,  0, 127, 0, 0, 127, 0, 127, 0,  0, 0, 0, 0,  0, 127, 0, 0, 127, 0, 127, 0,  0, 0};

int    HTTP_PORT   = 80;
String HTTP_METHOD = "GET"; // or "POST"
char   HOST_NAME[] = "example.phpoc.com"; // hostname of web server:
String PATH_NAME   = "";

int keyA = 3;
int keyBb = 2;
int keyB = 1;
int keyD = 10;
int keyEb = 9;
int keyE = 8;
int keyF = 7;
int keyG = 5;

int number;

//BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy LED Service

// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
//BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

const int ledPin = LED_BUILTIN; // pin to use for the LED
void setup() {

  // 
  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb

  Serial.println("Bluefruit52 Dual Role BLEUART Example");
  Serial.println("-------------------------------------\n");
  
  // Initialize Bluefruit with max concurrent connections as Peripheral = 1, Central = 1
  // SRAM usage required by SoftDevice will increase with number of connections
  Bluefruit.begin(1, 1);
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values

  // Callbacks for Peripheral
  Bluefruit.Periph.setConnectCallback(prph_connect_callback);
  Bluefruit.Periph.setDisconnectCallback(prph_disconnect_callback);

  // Callbacks for Central
  //Bluefruit.Central.setConnectCallback(cent_connect_callback);
  //Bluefruit.Central.setDisconnectCallback(cent_disconnect_callback);

  // To be consistent OTA DFU should be added first if it exists
  //bledfu.begin();

  // Configure and Start BLE Uart Service
  bleuart.begin();
  bleuart.setRxCallback(prph_bleuart_rx_callback);

  // Init BLE Central Uart Serivce
  //clientUart.begin();
  //clientUart.setRxCallback(cent_bleuart_rx_callback);

  /* Start Central Scanning
   * - Enable auto scan if disconnected
   * - Interval = 100 ms, window = 80 ms
   * - Filter only accept bleuart service
   * - Don't use active scan
   * - Start(timeout) with timeout = 0 will scan forever (until connected)
   */

  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.setInterval(160, 80); // in unit of 0.625 ms
  Bluefruit.Scanner.filterUuid(bleuart.uuid);
  Bluefruit.Scanner.useActiveScan(false);
  Bluefruit.Scanner.start(0);                   // 0 = Don't stop scanning after n seconds

  // Set up and start advertising
  startAdv();
  // put your setup code here, to run once:
  //Dictionary<String, int> dict = Dictionary<String, int>();
  Dictionary* d_ptr = new Dictionary(6);
  Dictionary& dic = *d_ptr;


  dic("C","0");
  dic("G","7");
  dic("D","2");
  dic("A","9");
  dic("F","5");
  dic("Bb","10");

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  number = sizeof(myarray) / sizeof(int);

}

int currentShift = 0; 
// colors r g b y c v o
int getRed(char c){
  int v = 0;
  switch(c){
    case 'r':
      v = 128;
    break;
    case 'b':
      v = 0;
    break;
    case 'g':
      v = 0;
    break;
    case 'y':
      v = 64;
    break;
    case 'c':
      v = 0;
    break;
    case 'v':
      v = 0;
    case 'o':
      v = 128;
    break;
  }
  return v;
}

int getGreen(char c){
  int v = 0;
  switch(c){
    case 'r':
    case 'b':
      v = 0;
    break;
    case 'g':
      v = 128;
    break;
    case 'y':
      v = 64;
    break;
    case 'c':
      v = 64;
    break;
    case 'v':
      v = 0;
    case 'o':
      v = 32;
    break;
  }
  return v;
}

int getBlue(char c){
  int v = 0;
  switch(c){
    case 'r':
      v = 0;
    break;
    case 'b':
      v = 128;
    break;
    case 'g':
      v = 0;
    break;
    case 'y':
      v = 0;
    break;
    case 'c':
      v = 64;
    break;
    case 'v':
      v = 64;
    case 'o':
      v = 0;
    break;
  }
  return v;
}

int sAR[12];
int sAG[12];
int sAB[12];

int sER[12];
int sEG[12];
int sEB[12];

int sCR[12];
int sCG[12];
int sCB[12];

int sGR[12];
int sGG[12];
int sGB[12];

void setStringColors(int* sR, int* sG, int* sB, char* colors){
  for(int i=0; i<12; i++){
    char c = colors[i];
    sR[i] = getRed(c);
    sG[i] = getGreen(c);
    sB[i] = getBlue(c);
  }
}

void shiftKey(int shiftNx){
    shift = shiftNx - currentShift;
    if(shift < 0){
      shift = shift + 12;
    };
    currentShift = shiftNx;
    for(int j = 0;j<shift;j++){
      int tempo = sG[number - 1];
      for (int i = number - 1; i > 0; i--) sG[i] = sG[i - 1];
      sG[0] = tempo;
      tempo = sC[number - 1];
      for (int i = number - 1; i > 0; i--) sC[i] = sC[i - 1];
      sC[0] = tempo;
      tempo = sE[number - 1];
      for (int i = number - 1; i > 0; i--) sE[i] = sE[i - 1];
      sE[0] = tempo;
      tempo = sA[number - 1];
      for (int i = number - 1; i > 0; i--) sA[i] = sA[i - 1];
      sA[0] = tempo;
    }
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();

  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   *
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html
   */

  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

// void switchCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
//   // central wrote new value to characteristic, update LED
//   Serial.print("Characteristic event, written: ");
//   if (switchCharacteristic.value()) {
//     Serial.println("LED on");
//     digitalWrite(ledPin, HIGH);
//   } else {
//     Serial.println("LED off");
//     digitalWrite(ledPin, LOW);
//   }
// }


/*------------------------------------------------------------------*/
/* Central
 *------------------------------------------------------------------*/
void scan_callback(ble_gap_evt_adv_report_t* report)
{
  // Since we configure the scanner with filterUuid()
  // Scan callback only invoked for device with bleuart service advertised  
  // Connect to the device with bleuart service in advertising packet  
  Bluefruit.Central.connect(report);
}

/*------------------------------------------------------------------*/
/* Peripheral
 *------------------------------------------------------------------*/
void prph_connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char peer_name[32] = { 0 };
  connection->getPeerName(peer_name, sizeof(peer_name));

  Serial.print("[Prph] Connected to ");
  Serial.println(peer_name);
}


void prph_disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.println();
  Serial.println("[Prph] Disconnected");
}


void prph_bleuart_rx_callback(uint16_t conn_handle)
{
  (void) conn_handle;
  
  // Forward data from Mobile to our peripheral
  char str[20+1] = { 0 };
  bleuart.read(str, 20);

  Serial.print("[Prph] RX: ");
  Serial.println(str);  

  switch(str){
    case 'C':
      shiftKey(0);
      
    break;
    case 'D':
      shiftKey(2);
    break;
    case 'Eb':
      shiftKey(3);
    break;
    case 'E':
      shiftKey(4);
    break;
    case 'F':
      shiftKey(5);
    break;
    case 'G':
      shiftKey(7);
    break;
    case 'A':
      shiftKey(9);
    break;
    case 'Bb':
      shiftKey(10);
    break;
    case 'B':
      shiftKey(11);
    break;
  }

  setStringColors(sAR, sAG, sAB, sA);
  setStringColors(sER, sEG, sEB, sE);
  setStringColors(sCR, sCG, sCB, sC);
  setStringColors(sGR, sGG, sGB, sC);

  //strip.setPixelColor(i+36,strip.Color(r, g, b))  // TODO 

  // if ( clientUart.discovered() )
  // {
  //   clientUart.print(str);
  // }else
  // {
  //   bleuart.println("[Prph] Central role not connected");
  // }
}


void loop() {
  // put your main code here, to run repeatedly:
  strip.clear();

  int value = analogRead(A0);
  bitTime = bitTime + 1;
  vReal[bitTime] = value;
  vImag[bitTime] = value;
  if(bitTime == 1024){
    double peak = doFFT();
    String str = String(peak,5);
    bleuart.print( str );
    bitTime = 0;
  }

  value = analogRead(A1);
  //bitTime = bitTime + 1;
  vReal[bitTime] = value;
  vImag[bitTime] = value;
  if(bitTime == 1024){
    double peak = doFFT();
    String str = String(peak,5);
    bleuart.print( str );
    bitTime = 0;
  }

  value = analogRead(A2);
  //bitTime = bitTime + 1;
  vReal[bitTime] = value;
  vImag[bitTime] = value;
  if(bitTime == 1024){
    double peak = doFFT();
    String str = String(peak,5);
    bleuart.print( str );
    bitTime = 0;
  }

  value = analogRead(A3);
  //bitTime = bitTime + 1;
  vReal[bitTime] = value;
  vImag[bitTime] = value;
  if(bitTime == 1024){
    double peak = doFFT();
    String str = String(peak,5);
    bleuart.print( str );
    bitTime = 0;
  }
  
}

double doFFT(){
  FFT = arduinoFFT(vReal, vImag, samples, samplingFrequency); /* Create FFT object */
  FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(FFT_FORWARD); /* Compute FFT */
  FFT.ComplexToMagnitude(); /* Compute magnitudes */
  double x;
  double v;
  FFT.MajorPeak(&x, &v);
  return v;
}

void updateKeyDisplay(){
  //char[] val = ledCharacteristic.value();
}

// void chromaKeyC(){
//   // C String pixels 12 to 24
//   for (int i =0;i<12;i++) { 
//     int r = KeyC_AStringRed[i];
//     int g = KeyC_AStringGreen[i];
//     int b = KeyC_AStringBlue[i];
//     strip.setPixelColor(i,strip.Color(r, g, b));
//     r = KeyC_EStringRed[i];
//     g = KeyC_EStringGreen[i];
//     b = KeyC_EStringBlue[i];
//     strip.setPixelColor(i+12,strip.Color(r, g, b));
//     r = KeyC_CStringRed[i];
//     g = KeyC_CStringGreen[i];
//     b = KeyC_CStringBlue[i];
//     strip.setPixelColor(i+24,strip.Color(r, g, b)); 
//     r = KeyC_GStringRed[i];
//     g = KeyC_GStringGreen[i];
//     b = KeyC_GStringBlue[i];
//     strip.setPixelColor(i+36,strip.Color(r, g, b));
    
//   }
// }

// void chromaKeyD(){
//   // C String pixels 12 to 24
//   int start = keyD;
//   int end = 12 + keyD;
//   for (int i =start; i<end; i++) { 
//     int r = KeyC_CStringRed[i];
//     int g = KeyC_CStringGreen[i];
//     int b = KeyC_CStringBlue[i];
//     strip.setPixelColor(i,strip.Color(r, g, b)); 
//   }
// }
