/*
 ============================================================
 INVERNADERO INTELIGENTE
 Subsistema 2: Monitoreo de Humedad y Control de Riego
 Simulacion: Tinkercad
 ============================================================
 PINES:
   A1  -> Sensor de humedad de suelo
   A2  -> Potenciometro (simula sensor ultrasonico nivel agua)
   D2  -> Relevador bomba de riego
   D11 -> Pulsador inicio/paro (INPUT_PULLUP)

 CALIBRACION SENSOR SUELO:
   HUM_SECO   = 880  -> 0%
   HUM_HUMEDO = 20   -> 100%
   (Ajustar segun lectura RAW en Tinkercad)

 LOGICA DE RIEGO:
   Humedad < 25% Y nivel agua > 10% -> Activar bomba
   Nivel agua <= 10%                -> Bomba NO activada (proteccion)

 FRECUENCIA DE MUESTREO: 1 medicion por minuto (60000 ms)
 ============================================================
*/

const int PIN_HUM_SUELO   = A1;
const int PIN_NIVEL_AGUA  = A2;
const int PIN_RELAY_BOMBA = 2;
const int PIN_BTN_HUM     = 11;

const int HUM_SECO   = 880;
const int HUM_HUMEDO = 20;

bool sistemaActivo = false;

unsigned long tUltimoDeb = 0;
unsigned long tUltimaHum = 0;

const unsigned long DEBOUNCE_MS  = 200;
const unsigned long INTERVALO_MS = 60000UL;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_RELAY_BOMBA, OUTPUT);
  pinMode(PIN_BTN_HUM,     INPUT_PULLUP);
  digitalWrite(PIN_RELAY_BOMBA, LOW);
  Serial.println("Sistema de humedad listo.");
  Serial.println("Presiona el boton para iniciar.");
}

void loop() {
  unsigned long ahora = millis();

  if (digitalRead(PIN_BTN_HUM) == LOW) {
    if ((ahora - tUltimoDeb) > DEBOUNCE_MS) {
      tUltimoDeb = ahora;
      sistemaActivo = !sistemaActivo;
      tUltimaHum = ahora - INTERVALO_MS;
      Serial.print("Sistema: ");
      Serial.println(sistemaActivo ? "ON" : "OFF");
      if (!sistemaActivo) digitalWrite(PIN_RELAY_BOMBA, LOW);
    }
  }

  if (sistemaActivo && (ahora - tUltimaHum) >= INTERVALO_MS) {
    tUltimaHum = ahora;
    medirHumedad();
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
