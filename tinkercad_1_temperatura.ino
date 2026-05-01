/*
 ============================================================
 INVERNADERO INTELIGENTE
 Subsistema 1: Monitoreo y Control de Temperatura
 Simulacion: Tinkercad
 ============================================================
 PINES:
   A0  -> Sensor TMP36
   D5  -> LED verde  (Temperatura media  20-30 C)
   D6  -> LED azul   (Temperatura baja   < 20 C)
   D7  -> LED rojo   (Temperatura alta   >= 30 C)
   D8  -> Relevador ventilador (activa cuando T > 32 C)
   D9  -> Pulsador inicio/paro (INPUT_PULLUP)

 SENSOR TMP36:
   V   = (ADC * 5.0) / 1023.0
   T_C = (V - 0.5) * 100.0
   T_F = (T_C * 9.0 / 5.0) + 32.0

 FRECUENCIA DE MUESTREO: 1 medicion por minuto (60000 ms)
 ============================================================
*/

const int PIN_TMP        = A0;
const int PIN_LED_TMEDIA = 5;
const int PIN_LED_TBAJA  = 6;
const int PIN_LED_TALTA  = 7;
const int PIN_RELAY_VENT = 8;
const int PIN_BTN_TEMP   = 9;

bool sistemaActivo = false;

unsigned long tUltimoDebTemp = 0;
unsigned long tUltimoTemp    = 0;

const unsigned long DEBOUNCE_MS  = 200;
const unsigned long INTERVALO_MS = 60000UL;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_LED_TBAJA,   OUTPUT);
  pinMode(PIN_LED_TMEDIA,  OUTPUT);
  pinMode(PIN_LED_TALTA,   OUTPUT);
  pinMode(PIN_RELAY_VENT,  OUTPUT);
  pinMode(PIN_BTN_TEMP,    INPUT_PULLUP);
  digitalWrite(PIN_RELAY_VENT, LOW);
  apagarLEDs();
  Serial.println("Sistema de temperatura listo.");
  Serial.println("Presiona el boton para iniciar.");
}

void loop() {
  unsigned long ahora = millis();

  if (digitalRead(PIN_BTN_TEMP) == LOW) {
    if ((ahora - tUltimoDebTemp) > DEBOUNCE_MS) {
      tUltimoDebTemp = ahora;
      sistemaActivo = !sistemaActivo;
      tUltimoTemp = ahora - INTERVALO_MS;
      Serial.print("Sistema: ");
      Serial.println(sistemaActivo ? "ON" : "OFF");
      if (!sistemaActivo) {
        apagarLEDs();
        digitalWrite(PIN_RELAY_VENT, LOW);
      }
    }
  }

  if (sistemaActivo && (ahora - tUltimoTemp) >= INTERVALO_MS) {
    tUltimoTemp = ahora;
    medirTemperatura();
  }
}

void medirTemperatura() {
  int adcVal = analogRead(PIN_TMP);
  float voltaje = (adcVal * 5.0) / 1023.0;
  float tempC   = (voltaje - 0.5) * 100.0;
  float tempF   = (tempC * 9.0 / 5.0) + 32.0;

  Serial.print("Temperatura en centigrados: ");
  Serial.print(tempC, 1);
  Serial.print(" C; en Fahrenheit: ");
  Serial.print(tempF, 1);
  Serial.println(" F.");

  apagarLEDs();

  if (tempC < 20.0) {
    digitalWrite(PIN_LED_TBAJA, HIGH);
    Serial.println("Estado: Temperatura baja");
  } else if (tempC >= 30.0) {
    digitalWrite(PIN_LED_TALTA, HIGH);
    Serial.println("Estado: Temperatura alta");
  } else {
    digitalWrite(PIN_LED_TMEDIA, HIGH);
    Serial.println("Estado: Temperatura media");
  }

  if (tempC > 32.0) {
    digitalWrite(PIN_RELAY_VENT, HIGH);
    Serial.println("ALERTA: Ventilador activado (T > 32 C)");
  } else {
    digitalWrite(PIN_RELAY_VENT, LOW);
  }
}

void apagarLEDs() {
  digitalWrite(PIN_LED_TBAJA,  LOW);
  digitalWrite(PIN_LED_TMEDIA, LOW);
  digitalWrite(PIN_LED_TALTA,  LOW);
}
