/*
 ============================================================
 INVERNADERO INTELIGENTE
 Subsistema de Iluminacion LED + Monitoreo LDR
 Simulacion: Proteus 8 (Arduino Simulino UNO)
 ============================================================
 PINES:
   A0  -> LDR con divisor de voltaje (10 kOhm a GND)
   D2  -> LED azul  (Configuracion 1 - fotosintesis)
   D3  -> LED rojo  (Configuracion 2 - fotosintesis)
   D4  -> Pulsador inicio/paro (INPUT_PULLUP)
   D5..D10 -> LCD en modo 4 bits
     RS = D10
     E  = D9
     D4 = D8
     D5 = D7
     D6 = D6
     D7 = D5

 SECUENCIA DE ILUMINACION:
   C1 -> C2 -> C2 -> C1 (se repite hasta apagar)

   Configuracion 1 (LED Azul):
     ON 500ms | OFF 100ms | ON 500ms | OFF 1000ms

   Configuracion 2 (LED Rojo):
     ON 1000ms | OFF 100ms

 MONITOREO LDR -> LCD:
   < 25%  : Ilum. baja
   25-75% : Ilum. media
   >= 75% : Ilum. alta
 ============================================================
*/

#include <LiquidCrystal.h>

// LCD: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(10, 9, 8, 7, 6, 5);

const int PIN_LED_AZUL = 2;
const int PIN_LED_ROJO = 3;
const int PIN_BTN_LUZ  = 4;
const int PIN_LDR      = A0;

bool sistemaActivo = false;

unsigned long tUltimoDeb = 0;
unsigned long tUltimoLuz = 0;

const unsigned long DEBOUNCE_MS   = 200;
const unsigned long INTERVALO_LUZ = 2000UL;

void setup() {
  lcd.begin(16, 2);
  pinMode(PIN_LED_AZUL, OUTPUT);
  pinMode(PIN_LED_ROJO, OUTPUT);
  pinMode(PIN_BTN_LUZ,  INPUT_PULLUP);
  digitalWrite(PIN_LED_AZUL, LOW);
  digitalWrite(PIN_LED_ROJO, LOW);
  tUltimoLuz = millis();
  lcd.print("Sistema listo");
  lcd.setCursor(0, 1);
  lcd.print("Presiona boton");
}

void loop() {
  unsigned long ahora = millis();

  // Boton con anti-rebote
  if (digitalRead(PIN_BTN_LUZ) == LOW) {
    if ((ahora - tUltimoDeb) > DEBOUNCE_MS) {
      tUltimoDeb = ahora;
      sistemaActivo = !sistemaActivo;
      if (!sistemaActivo) {
        digitalWrite(PIN_LED_AZUL, LOW);
        digitalWrite(PIN_LED_ROJO, LOW);
        lcd.clear();
        lcd.print("Iluminacion OFF");
      }
    }
  }

  // Ejecutar secuencia de iluminacion
  if (sistemaActivo) {
    ejecutarSecuencia();
  }

  // Monitorear LDR y actualizar LCD
  ahora = millis();
  if (ahora - tUltimoLuz >= INTERVALO_LUZ) {
    tUltimoLuz = ahora;
    monitorearLuz();
  }
}

void monitorearLuz() {
  int adcLDR = analogRead(PIN_LDR);
  int porcentaje = map(adcLDR, 0, 1023, 0, 100);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Luz: ");
  lcd.print(porcentaje);
  lcd.print("%");
  lcd.setCursor(0, 1);

  if (porcentaje < 25) {
    lcd.print("Ilum. baja");
  } else if (porcentaje >= 75) {
    lcd.print("Ilum. alta");
  } else {
    lcd.print("Ilum. media");
  }
}

void ejecutarSecuencia() {
  configuracion1();
  if (!sistemaActivo) return;
  configuracion2();
  if (!sistemaActivo) return;
  configuracion2();
  if (!sistemaActivo) return;
  configuracion1();
}

// Configuracion 1: LED Azul
// ON 500ms | OFF 100ms | ON 500ms | OFF 1000ms
void configuracion1() {
  if (!sistemaActivo) return;
  digitalWrite(PIN_LED_AZUL, HIGH);
  delay(500);
  digitalWrite(PIN_LED_AZUL, LOW);
  delay(100);
  if (!sistemaActivo) return;
  digitalWrite(PIN_LED_AZUL, HIGH);
  delay(500);
  digitalWrite(PIN_LED_AZUL, LOW);
  delay(1000);
}

// Configuracion 2: LED Rojo
// ON 1000ms | OFF 100ms
void configuracion2() {
  if (!sistemaActivo) return;
  digitalWrite(PIN_LED_ROJO, HIGH);
  delay(1000);
  digitalWrite(PIN_LED_ROJO, LOW);
  delay(100);
}
