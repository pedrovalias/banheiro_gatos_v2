#include <Arduino.h>

// Bibliotecas Utilizadas
#include <FastLED.h>                 // FastLed library version 3.3.3 - https://github.com/FastLED/FastLED/wiki/Overview or http://fastled.io/ with NEOPIXEL or WS2812B
#include <NewPing.h>                 // Biblioteca para manipulação do sensor ultrassônico
#include <Wire.h>                    //One Wire library included in Arduino IDE version 1.8.9
#include <Adafruit_SSD1306.h>        //Adafruit_SSD1306 library version 2.2.1 - https://github.com/adafruit/Adafruit_SSD1306

#include <variaveis-globais.h>       // Declaração das variáveis globais do sistema 
#include <inicializar-funcoes.h>     // Declaração de todas as funções utilizadas no sistema
#include <images.h>

void setup() {
  //starting the serial begin for debugging purposes
  Serial.begin(9600);

  inicializaPinagem();
  // display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  //Initialize the display - SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for my oled lcd 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); //Don't proceed, loop forever if the display is not Initializing (check connections or if the address for you oled lcd is correct)
  }
  
  //setting up the pin modes for the rotary encoder and the switch
  pinMode(rotaryOutputA, INPUT);
  pinMode(rotaryOutputB, INPUT);
  pinMode(rotarySwitch, INPUT);

  //using interrupts so we never miss change in rotary encoder
  attachInterrupt(digitalPinToInterrupt(rotaryOutputA), rotaryChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(rotaryOutputB), rotaryChange, CHANGE);
  
  //read the initial value of outputA for the rotary encoder
  rotaryPrevState = digitalRead(rotaryOutputA);

  // //starting up the instaces for the led strips and defining the output pins
  // FastLED.addLeds<NEOPIXEL, 8>(ledsA, NUM_LEDS_PART_A);    //led strip one on pin 8
  // fill_solid(ledsA, NUM_LEDS_PART_A, CRGB::Black);    // some led strips are all on at power on, so let's power them off at boot
  // //sending the data to the leds
  // FastLED.show();
  
  //write the my youtube channel name and my name on the display - you can whatever you want here for your application
  updateDisplay(1,1,0,0,"    Think small,"); // Font size 1 = 8 rows used = 21 characters per row
  delay(500);
  updateDisplay(0,2,0,9,"build big!"); // Font size 2 = 14 rows used = 10 characters per row
  delay(500);
  updateDisplay(0,1,0,25,"  CATCLEAN TESTE"); // Font size 1 = 8 rows used = 21 characters per row
  delay(3000);
}

void loop() {

  switchAndChanges();

  // verificaStatusSistema();

  digitalWrite(LED_VERDE, HIGH);

   // Envia o ping, obtem a distancia em cm e exibe terminal (0 = fora do alcance)
  // for (uint8_t i = 0; i < SONAR_NUM; i++) { // Loop por cada sensor e exibição do valor lido
  //   delay(50);                              // Aguarda 50ms entre pings (aprox 20 pings/segundo). 29ms deve ser o menor delay entre pings.
  //   Serial.print(i);
  //   Serial.print("=");
  //   Serial.print(sonar[i].ping_cm());
  //   Serial.print("cm ");
  // }
  // Serial.println();

  // Função padrão: modo automático
  if(currentFunction == 0){
    // DEFAULT
    // updateDisplay(1,1,0,0,"  MODO CONFIGURACAO  ");
    // updateDisplay(0,1,0,2,"_____________________");

  }else if(currentFunction == 1){

    updateDisplay(1,1,0,0,"     MODO MANUAL     ");
    updateDisplay(0,1,0,2,"_____________________");

    // display.clearDisplay();
    // display.drawBitmap(0,0,bla,128,64,WHITE);
    // display.display();
    
  }else if(currentFunction == 2){
    updateDisplay(1,1,0,0,"   MODO AUTOMATICO   ");
    updateDisplay(0,1,0,2,"_____________________");

    verificaStatusSistema();

    verifica_banheiro(0);
    verifica_banheiro(1);
    delay(delayLoop * 1000);
  };

  //sending the data to the leds
  // FastLED.show();
}

/** Função que inicializa todos os pins utilizados no sistema */
void inicializaPinagem(){
  // setting up the pin modes for the rotary encoder and the switch
  // pinMode(rotaryOutputA, INPUT);
  // pinMode(rotaryOutputB, INPUT);
  // pinMode(rotarySwitch, INPUT);

  // read the initial value of outputA for the rotary encoder
  rotaryPrevState = digitalRead(rotaryOutputA);

  // setting up the pin modes for the solenoyd valves
  pinMode(PINO_VALVULA_1, OUTPUT);
  digitalWrite(PINO_VALVULA_1, HIGH);

  pinMode(PINO_VALVULA_2, OUTPUT);
  digitalWrite(PINO_VALVULA_2, HIGH);

  // setting up the 
  pinMode(LED_VERDE, OUTPUT);
  digitalWrite(LED_VERDE, LOW);

  pinMode(LED_VERMELHO, OUTPUT);
  digitalWrite(LED_VERMELHO, LOW);

  pinMode(LED_AZUL, OUTPUT);
  digitalWrite(LED_AZUL, LOW);
}

/** Void function to read the switch, change the functions and update the display */
void switchAndChanges(){

  //reading the switch
  rotarySwitchState = digitalRead(rotarySwitch);

  //if the switch is pressed, then we react - because the switch on my rotary encoder has a pulldown resistor built in, i need to detect on LOW state
  if(rotarySwitchState == LOW){
    
    //keep track of the current time for the Reset functions
    currentMillisBtn = millis();
    
    //if delayForResetAll time (in this case 1500 milliseconds) has passed, then we reset selectedFunction and currentFunction
    if(previousMillisBtn+delayForResetAll < currentMillisBtn){
      
      //reset selectedFunction and currentFunction
      selectedFunction = 0;
      currentFunction = 0;
      
      //print to serial monitor for debugging
      Serial.println("Reset functions :)");
      
      //update the display
      updateDisplay(1,2,0,0," Function"); // Font size 2 = 14 rows used = 10 characters per row
      updateDisplay(0,2,0,16,"  Reset :)"); // Font size 2 = 14 rows used = 10 characters per row
      //delay to let the message on the display for 1 second
      delay(1000);
      
      //set timesToUpdateTheDisplay to 1 to update the display one time...
      timesToUpdateTheDisplay = 1;
      
      //set rotarySwitchPrevState so nothing its run anymore
      rotarySwitchPrevState = 0;

    }else{
      
      //if the time has not passed, then we set the rotarySwitchPrevState
      rotarySwitchPrevState = 1;
      
    }

  //if rotarySwitchPrevState changed, then we react and in this case we is 1 and we set the new selcted function...
  }else if(rotarySwitchPrevState == 1){
    
    //we set the current function to the new selected function
    currentFunction = selectedFunction;
    
    //print to serial monitor for debugging
    Serial.print("Functions set! "); Serial.println(currentFunction);
    
    //update the display
    // updateDisplay(0,1,0,25,"    Function set!"); // Font size 1 = 8 rows used = 21 characters per row

    updateDisplay(0,1,0,25,"Funcao selhecionada!"); // Font size 1 = 8 rows used = 21 characters per row
    
    //delay to let the message on the display for half of one second    
    delay(1500);
    
    //reset rotarySwitchPrevState so the this function will not run
    rotarySwitchPrevState = 0;
    
    //set how many times to update the display
    timesToUpdateTheDisplay = 1;
  
  }else{
    //here we keep track of the current time and the previousMillis tn time that's its used for resetting selected Function and currentFunction
    currentMillisBtn = previousMillisBtn = millis();
  }

  //if delayForResetSelectedFunction time (in this case 10000 miliseconds) has passed, then we reset selectedFunction that was not applied
  if(previousMillisBtn2+delayForResetSelectedFunction < currentMillisBtn && selectedFunction != currentFunction){
    
    //setting selectedFunction to the currentFunction that is running 
    selectedFunction = currentFunction;
    
    //setting how many time to update the display
    timesToUpdateTheDisplay = 1;
    
    //print to serial monitor for debugging
    Serial.print("Selected function: "); Serial.print(selectedFunction);Serial.print(" Curent function: ");   Serial.println(currentFunction);
  }

  //update the display if timesToUpdateTheDisplay is not equal to 0
  if(timesToUpdateTheDisplay != 0){
    updateDisplay(1,1,0,0,"Gire p/ selhecionar"); // Font size 1 = 8 rows used = 21 characters per row
  
    //updating the display
    // if(selectedFunction == 0 && currentFunction == 0 && timesToUpdateTheDisplay != 0){
    //   updateDisplay(1,1,0,0,"    No function is"); // Font size 1 = 8 rows used = 21 characters per row
    //   updateDisplay(0,1,0,9,"        running"); // Font size 1 = 8 rows used = 21 characters per row
    // }else{
    //   updateDisplay(1,1,0,0,"Current running: " + String(currentFunction)); // Font size 1 = 8 rows used = 21 characters per row
    //   updateDisplay(0,2,0,9,"Select: " + String(selectedFunction)); // Font size 2 = 14 rows used = 10 characters per row
    // }
    if(selectedFunction == 1){
      updateDisplay(0,2,0,9,"  MANUAL");
    } else if(selectedFunction == 2){
      updateDisplay(0,2,0,9,"AUTOMATICO");
    } else {
      updateDisplay(0,2,0,9," CONFIG.");
    }
    //decreasing the number to update the display - timesToUpdateTheDisplay - so we don't run this function/update the display to infinitely
    timesToUpdateTheDisplay--;     
  }
}

/** Function that its run on attachInterrupt on change to detect the rotary encoder changes... */
void rotaryChange(){
  
  //reading the rotaryState
  rotaryState = digitalRead(rotaryOutputA);
  
  //if rotaryState and rotaryPrevState are not equal, then we react
  if(rotaryState != rotaryPrevState){
    
    //keep track of the previousMillisBtn2 time for reseting the selectedFunction that was not applied
    previousMillisBtn2 = millis();

    //determining the direction that the router encoder and increase or decrease
    if(digitalRead(rotaryOutputB) != rotaryState){
      
      //increasing if the rotary encoder its rotating to right
      currentIncrementChange++;
      
      //if rotary encoder has made one step, then we change stuff
      if(currentIncrementChange >= incrementChangesPerStep){
        //here we increment selectedFunction on each step
        selectedFunction++;
        //and if the selectedFunction is greater than the totalNumberFunctions (in this case 10), then we go back to 0
        if(selectedFunction > totalNumberFunctions){
          selectedFunction = 0;
        }
        
        //here we set the currentIncrementChange to 0 after we made a step on the rotary encoder
        currentIncrementChange = 0;
      }
    }else{
      
      //decrease if the rotary encoder its rotating to left
      currentIncrementChange++;
      
      //if rotary encoder has made one step, then we change stuff
      if(currentIncrementChange >= incrementChangesPerStep){
        
        //here we decrease selectedFunction on each step        
        selectedFunction--;
        
        //and if the selectedFunction is smaller than 0, then we go back to totalNumberFunctions (in this case 10)
        if(selectedFunction < 0){
          selectedFunction = totalNumberFunctions;
        }
        
        //here we set the currentIncrementChange to 0 after we made a step on the rotary encoder
        currentIncrementChange = 0;
      }
    }
    
    //after we made a step on the rotary encoder we react...
    if(currentIncrementChange == 0){
      
      //set the timesToUpdateTheDisplay to 2, to update the display two times...
      timesToUpdateTheDisplay = 2;
      
      //and we print to serial monitor for debugging
      Serial.println(selectedFunction);  
    }
    
  }
  
  //here we are setting the rotaryPrevState to be able to determine the direction next time that the rotary encoder moves
  rotaryPrevState = rotaryState;
}

/**
 * With this function it's more easier to update the display with one line of code...
 * - clearOrUpdate - 0 don't clear the display and 1 clear the display
 * - fontSize - represents the font size
 * - columnPixelToStart - represents the pixel on horizontal that the text will start
 * - rowPixelToStart - represents the pixel on vertical that the text will start
 * - textToWrite - represents the text will be displayed and it must be a string...
 */
void updateDisplay(int clearOrUpdate, int fontSize,int columnPixelToStart,int rowPixelToStart,String textToWrite){
  
  if(clearOrUpdate == 1){
    display.clearDisplay(); //clear the display if clearOrUpdate is 1
  }
  
  display.setTextSize(fontSize);      //set the font size - Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  //display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.setCursor(columnPixelToStart, rowPixelToStart);     // Start at left-top corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.print(textToWrite); //set the text that will be printed on the display

  display.display(); //send the data to the display
}

void verifica_banheiro(int numSensor){
   
  distancia = validarDistancia(numSensor);
  numSensor == 0 ? updateDisplay(0,1,0,10,"Banheiro 1: " + String(distancia)) : updateDisplay(0,1,0,19,"Banheiro 2: " + String(distancia));  

  boolean gatoUsandoBanheiro = false ;

  if(distancia <= limite_distancia && distancia != 0){
    gatoUsandoBanheiro = true;
    gato_usou_banheiro = true;

    while(gatoUsandoBanheiro) {

      Serial.println("O Gato está no banheiro");
      
      digitalWrite(LED_VERDE, LOW);
      digitalWrite(LED_VERMELHO, HIGH);

      distancia = validarDistancia(numSensor);

      if(distancia > limite_distancia || distancia == 0) {
        Serial.println("Banheiro liberado");
        Serial.print("Tempo de espera antes de ligar a valvula: ");
        for(int i = 0 ; i < tempoEsperaSegundos; i++){
          delay(1000);
          Serial.print(i);
        }
        // delay(5000);
        gatoUsandoBanheiro = false;
        digitalWrite(LED_VERMELHO, LOW);
      }
    }
  }

  Serial.print("Gato usou o banheiro "); 
  Serial.print(numSensor);
  Serial.print(" : ");
  Serial.println(gato_usou_banheiro);

  executaLimpeza(numSensor);
}

/** Função que obtém 5 leituras do sensor ultrassônico num intervalo de 500ms e calcula uma média */
int validarDistancia(int dv){
  
  int d[5];
  int media = 0;

  Serial.println("Validando distancia do sensor " + String(dv));
  for(uint8_t i = 0; i < 5; i++){
    d[i] = sonar[dv].ping_cm();
    delay(500);
    Serial.println("leitura " + String(i) + " : " + String(d[i]));
//    Serial.print(i);
//    Serial.print(" : ");
//    Serial.println(d[i]);
  }

  media = (d[0] + d[1] + d[2] + d[3] + d[4]) /5;

  Serial.println("Distancia média obtida: " + String(media));
  Serial.println("-------------------------------------------------");


  return media;
}

void executaLimpeza(int banheiro){

  distancia = validarDistancia(banheiro);
  
  if((distancia > limite_distancia || distancia == 0) && gato_usou_banheiro == true){ 
    Serial.print("Limpeza em andamento!");
    digitalWrite(LED_VERMELHO, LOW);
    digitalWrite(LED_AZUL, HIGH);
    banheiro == 0 ? digitalWrite(PINO_VALVULA_1, LOW) : digitalWrite(PINO_VALVULA_2, LOW);
    Serial.print("Valvula "); Serial.print(banheiro); Serial.println(" Ligada");
    delay(8000);
    banheiro == 0 ? digitalWrite(PINO_VALVULA_1, HIGH) : digitalWrite(PINO_VALVULA_2, HIGH);
    digitalWrite(LED_AZUL, LOW);
    gato_usou_banheiro = false;
    // execucoesLimpeza ++;
  }

}

void verificaStatusSistema(){

  int distancia_validada1 = 0;
  int distancia_validada2 = 0;

  Serial.println("######### Verificando Status do Sistema #########");

  boolean controle = true;

  // Enquanto a leitura do sensor trouxer valor zero os LEDs piscam sequenciamente
  while(controle){

    distancia_validada1 = validarDistancia(0);
    distancia_validada2 = validarDistancia(1);

    if(distancia_validada1 == 0){
      updateDisplay(0,1,0,10,"Banheiro 1:ERROR");
      // piscaLEDS();
    }
    if(distancia_validada2 == 0){
      updateDisplay(0,1,0,19,"Banheiro 2:ERROR");
      // piscaLEDS();
    }
    if(distancia_validada1 + distancia_validada2 >= 0){
      controle = false;
    } else {
      piscaLEDS();
      apagaLEDS();
    } 
  }
}

void apagaLEDS(){
  delay(700);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AZUL, LOW);
  digitalWrite(LED_VERMELHO, LOW);
}

void piscaLEDS(){
    digitalWrite(LED_VERDE, HIGH);
    delay(700);
    // digitalWrite(LED_AZUL, HIGH);
    // delay(700);
    digitalWrite(LED_VERMELHO, HIGH);
}
