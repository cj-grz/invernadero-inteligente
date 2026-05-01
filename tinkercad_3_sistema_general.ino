/*
 ============================================================
 INVERNADERO INTELIGENTE
 Sistema General Integrado: Temperatura + Humedad/Riego
 Simulacion: Tinkercad
 ============================================================
 PINES TEMPERATURA:
   A0  -> Sensor TMP36
   D5  -> LED verde  (Temperatura media  20-30 C)
   D6  -> LED azul   (Temperatura baja   < 20 C)
   D7  -> LED rojo   (Temperatura alta   >= 30 C)
   D8  -> Relevador ventilador (activa cuando T > 32 C)
   D9  -> Pulsador inicio/paro temperatura (INPUT_PULLUP)

 PINES HUMEDAD / RIEGO:
   A1  -> Sensor de humedad de suelo
   A2  -> Potenciometro (simula sensor ultrasonico nivel agua)
   D2  -> Relevador bomba de riego
   D11 -> Pulsador inicio/paro humedad (INPUT_PULLUP)

 FRECUENCIA DE MUESTREO: 1 medicion por minuto (60000 ms)
 ============================================================
*/

// ── Pines temperatura ──────────────────────────────────
const int PIN_TMP        = A0;
const int PIN_LED_TMEDIA = 5;
const int PIN_LED_TBAJA  = 6;
const int PIN_LED_TALTA  = 7;
const int PIN_RELAY_VENT = 8;
const int PIN_BTN_TEMP   = 9;

// ── Pines humedad / riego ──────────────────────────────
const int PIN_HUM_SUELO   = A1;
const int PIN_NIVEL_AGUA  = A2;
const int PIN_RELAY_BOMBA = 2;
const int PIN_BTN_HUM     = 11;

// ── Calibracion sensor suelo ───────────────────────────
const int HUM_SECO   = 880;
const int HUM_HUMEDO = 20;

// ── Estado de subsistemas ──────────────────────────────
bool sistemaTempActivo = false;
bool sistemaHumActivo  = false;

unsigned long tUltimoDebTemp = 0;
unsigned long tUltimoDebHum  = 0;
unsigned long tUltimoTemp    = 0;
unsigned long tUltimaHum     = 0;

const unsigned long DEBOUNCE_MS  = 200;
const unsigned long INTERVALO_MS = 60000UL;

void setup() {
  Serial.begin(9600);

  pinMode(PIN_LED_TBAJA,   OUTPUT);
  pinMode(PIN_LED_TMEDIA,  OUTPUT);
  pinMode(PIN_LED_TALTA,   OUTPUT);
  pinMode(PIN_RELAY_VENT,  OUTPUT);
  pinMode(PIN_BTN_TEMP,    INPUT_PULLUP);

  pinMode(PIN_RELAY_BOMBA, OUTPUT);
  pinMode(PIN_BTN_HUM,     INPUT_PULLUP);

  digitalWrite(PIN_RELAY_VENT,  LOW);
  digitalWrite(PIN_RELAY_BOMBA, LOW);
  apagarLEDs();

  Serial.println("=== Sistema general invernadero ===");
  Serial.println("BTN TEMP=D9  |  BTN HUM=D11");
}

void loop() {
  unsigned long ahora = millis();

  // Boton temperatura
  if (digitalRead(PIN_BTN_TEMP) == LOW) {
    if ((ahora - tUltimoDebTemp) > DEBOUNCE_MS) {
      tUltimoDebTemp = ahora;
      sistemaTempActivo = !sistemaTempActivo;
      tUltimoTemp = ahora - INTERVALO_MS;
      Serial.print("Temperatura: ");
      Serial.println(sistemaTempActivo ? "ON" : "OFF");
      if (!sistemaTempActivo) {
        apagarLEDs();
        digitalWrite(PIN_RELAY_VENT, LOW);
      }
    }
  }

  // Boton humedad
  if (digitalRead(PIN_BTN_HUM) == LOW) {
    if ((ahora - tUltimoDebHum) > DEBOUNCE_MS) {
      tUltimoDebHum = ahora;
      sistemaHumActivo = !sistemaHumActivo;
      tUltimaHum = ahora - INTERVALO_MS;
      Serial.print("Humedad: ");
      Serial.println(sistemaHumActivo ? "ON" : "OFF");
      if (!sistemaHumActivo) digitalWrite(PIN_RELAY_BOMBA, LOW);
    }
  }

  // Muestreo temperatura
  if (sistemaTempActivo && (ahora - tUltimoTemp) >= INTERVALO_MS) {
    tUltimoTemp = ahora;
    medirTemperatura();
  }

  // Muestreo humedad
  if (sistemaHumActivo && (ahora - tUltimaHum) >= INTERVALO_MS) {
    tUltimaHum = ahora;
    medirHumedad();
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

void medirHumedad() {
  int adcHum  = analogRead(PIN_HUM_SUELO);
  int adcAgua = analogRead(PIN_NIVEL_AGUA);

  int porcentajeHum = map(adcHum, HUM_SECO, HUM_HUMEDO, 0, 100);
  porcentajeHum = constrain(porcentajeHum, 0, 100);

  int nivelAgua = map(adcAgua, 0, 1023, 0, 100);
  nivelAgua = constrain(nivelAgua, 0, 100);

  Serial.print("Humedad relativa: ");
  Serial.print(porcentajeHum);
  Serial.print("% | Nivel de agua: ");
  Serial.print(nivelAgua);
  Serial.println("%");

  if (porcentajeHum < 25) {
    Serial.println("Estado: Humedad baja");
  } else if (porcentajeHum >= 75) {
    Serial.println("Estado: Humedad alta");
  } else {
    Serial.println("Estado: Humedad media");
  }

  if (porcentajeHum < 25) {
    if (nivelAgua > 10) {
      digitalWrite(PIN_RELAY_BOMBA, HIGH);
      Serial.println("RIEGO: Bomba activada");
    } else {
      digitalWrite(PIN_RELAY_BOMBA, LOW);
      Serial.println("ALERTA: Nivel de agua insuficiente. Bomba NO activada.");
    }
  } else {
    digitalWrite(PIN_RELAY_BOMBA, LOW);
  }
}

void apagarLEDs() {
  digitalWrite(PIN_LED_TBAJA,  LOW);
  digitalWrite(PIN_LED_TMEDIA, LOW);
  digitalWrite(PIN_LED_TALTA,  LOW);
}
