#include <Arduino.h>

#define NUM_LEDS_PART_A 10        //setting up the number of leds on the strips

//librarys and setting for the display OLED 0.91” - 128x32 - SSD1306 chipset
#define SCREEN_WIDTH 128          // OLED display width, in pixels
#define SCREEN_HEIGHT 32          // OLED display height, in pixels

CRGB ledsA[NUM_LEDS_PART_A];      //setting up the instaces for the led strips

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// By default on arduino uno SDA is A4 and SCL is A5
#define OLED_RESET     -1         // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int timesToUpdateTheDisplay = 1;  //used for knowing how many times to update the display, because in the rotary encoder function the display needs two updates...

//setting up the input pins
const int rotaryOutputA = 2;      //used for the output a of the rotary encoder
const int rotaryOutputB = 3;      //used for the output b of the rotary encoder
const int rotarySwitch = 4;       //used for the switch on the rotary encoder

//setting up the variables for holding the button states
int rotaryState = 0;              //to keep track of the current state for the rotary encoder
int rotaryPrevState = 0;          //to keep track of the previous state for the rotary encoder to determin the direction
int rotarySwitchState = 0;        //to keep track of the current state of the switch
int rotarySwitchPrevState = 0;    //to keep track of the previous state of the switch and react to the change
int currentIncrementChange = 0;   //to keep track of changes in each step of the rotary encoder
int incrementChangesPerStep = 2;  //each step of the rotary encoder generates two changes, so we keep count of them to increment only once... for easy use...
int delayForResetAll = 1500;      //delay for holding the buttons to reset currentFunction and selectedFunction
int delayForResetSelectedFunction = 10000; //delay for reseting selectedFunction if it was not applied

//setting up the variables to keep track of the current and selected functions
int totalNumberFunctions = 2;    //the total number of functions that you wanna use
int currentFunction = 0;          //the current function that was applied and is running - you can set a default function to run on power up from here
int selectedFunction = 0;         //the selected function with the rotary encoder that is waiting to be applied with switch

//variables for keeping track of the current and previous time for buttons
unsigned long currentMillisBtn = 0;   //current time
unsigned long previousMillisBtn = 0;  //previous time used to reset options
unsigned long previousMillisBtn2 = 0; //previous time used to reset selectedFunction if the modifications are not applied


/**
 * Variáveis do banheiro automático
 */
#define MAX_DISTANCE    55      // Distância máxima que queremos o ping(em centimetros). O máximo possível é 400-500cm.
#define SONAR_NUM       2       // Número de sensores
#define PINO_VALVULA_1  9       // Válvula Solenoide 1
#define PINO_VALVULA_2  10      // Válvula Solenoide 2

NewPing sonar[SONAR_NUM] = {         // Sensor object array.
  NewPing(6, 5, MAX_DISTANCE),       // Cada sensor possui um trigger pin, um echo pin e a maxima distancia para o ping  
  NewPing(8, 7, MAX_DISTANCE)
};

#define LED_VERDE    11 
#define LED_AZUL     12
#define LED_VERMELHO 13

int distancia = 0;
int delayLoop = 3;
const int limite_distancia = 32;
boolean gato_usou_banheiro = false;
int tempoEsperaSegundos = 3;    // Tempo determinado para espera é de 120 segundos   

// int execucoesLimpeza = 0;