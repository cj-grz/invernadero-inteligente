## 💻 Simulaciones

| Simulación | Herramienta | Archivo |
|---|---|---|
| Control de iluminación | Proteus 8 | `proteus_iluminacion.ino` |
| Monitoreo de temperatura | Tinkercad | `tinkercad_1_temperatura.ino` |
| Humedad y riego | Tinkercad | `tinkercad_2_humedad.ino` |
| Sistema general integrado | Tinkercad | `tinkercad_3_sistema_general.ino` |

## 🔌 Asignación de pines

### Subsistema de temperatura
| Pin | Componente |
|---|---|
| A0 | Sensor TMP36 |
| D5 | LED verde (temp. media) |
| D6 | LED azul (temp. baja) |
| D7 | LED rojo (temp. alta) |
| D8 | Relevador ventilador |
| D9 | Pulsador inicio/paro |

### Subsistema de humedad
| Pin | Componente |
|---|---|
| A1 | Sensor de humedad de suelo |
| A2 | Potenciómetro (nivel de agua) |
| D2 | Relevador bomba de riego |
| D11 | Pulsador inicio/paro |

### Iluminación (Proteus)
| Pin | Componente |
|---|---|
| A0 | Sensor LDR |
| D2 | LED azul |
| D3 | LED rojo |
| D4 | Pulsador inicio/paro |
| D5–D10 | LCD 16x2 (modo 4 bits) |

## 🔑 Lógica de control

```cpp
// Temperatura
if (T < 20°C)  → LED azul  + "Temperatura baja"
if (T >= 20°C) → LED verde + "Temperatura media"
if (T >= 30°C) → LED rojo  + "Temperatura alta"
if (T > 32°C)  → Ventilador ON

// Humedad
if (H < 25%)  → "Humedad baja"
if (H >= 25%) → "Humedad media"
if (H >= 75%) → "Humedad alta"
if (H < 25% && agua > 10%) → Bomba ON
if (agua <= 10%)            → Bomba OFF (protección)

// Iluminación
if (luz < 25%)  → "Ilum. baja"
if (luz >= 25%) → "Ilum. media"
if (luz >= 75%) → "Ilum. alta"
```
## 📐 Diseño mecánico

Invernadero de 60 × 60 × 60 cm (0.216 m³), desarrollado en
Autodesk Fusion 360. Incluye estructura de aluminio, paneles
transparentes, bandeja de cultivo, depósito de agua y caja
electrónica exterior.
