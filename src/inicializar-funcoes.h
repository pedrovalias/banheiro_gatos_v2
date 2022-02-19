#include <Arduino.h>

/**
 * Funções do Display / Menu
 * */
void updateDisplay(int clearOrUpdate, int fontSize,int columnPixelToStart,int rowPixelToStart,String textToWrite);
void switchAndChanges();
void rotaryChange();

/**
 * Funções do banheiro automático
 */
void inicializaPinagem();
void verifica_banheiro(int numSensor);
int validarDistancia(int dv);
void verificaStatusSistema();
void executaLimpeza(int banheiro);
void apagaLEDS();
void piscaLEDS();