#include <SPI.h>
#include <Wire.h>
#include "7semi_BME690.h"

#define BME690_CS_PIN PA4
#define LED_PIN PB2

SPISettings bmeSPI(1000000, MSBFIRST, SPI_MODE0);

// SPI MODE OBJECT
BME690_7semi bme(BME690_CS_PIN, BME690_7semi::MODE_SPI);

void setup()
{
  Serial1.begin(115200);   // PA9 TX, PA10 RX
  delay(500);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BME690_CS_PIN, OUTPUT);
  digitalWrite(BME690_CS_PIN, HIGH);

  Serial1.println("\n=== STM32F103 + BME690 (SPI MODE) ===");

  // ---------- SPI INIT ----------
  SPI.begin();   // PA5=SCK, PA6=MISO, PA7=MOSI
  delay(10);

  // ---------- CHIP ID TEST ----------
  SPI.beginTransaction(bmeSPI);
  digitalWrite(BME690_CS_PIN, LOW);

  SPI.transfer(0xD0 | 0x80);   // READ CHIP ID
  uint8_t chipID = SPI.transfer(0x00);

  digitalWrite(BME690_CS_PIN, HIGH);
  SPI.endTransaction();

  Serial1.print("BME690 CHIP ID: 0x");
  Serial1.println(chipID, HEX);

  if (chipID != 0x61) {
    Serial1.println("❌ NOT A VALID BME680/BME690 SPI DEVICE");
  }

  // ---------- SENSOR INIT ----------
  if (!bme.begin(Wire, SPI)) {   // ✅ CORRECT
    Serial1.println("❌ BME690 INIT FAILED");
  } else {
    Serial1.println("✅ BME690 INIT OK");
  }
}

void loop()
{
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));

  SPI.beginTransaction(bmeSPI);
  bool ok = bme.readSensorData();
  SPI.endTransaction();

  if (ok) {
    Serial1.print("Temp: ");
    Serial1.print(bme.getTemperature());
    Serial1.println(" °C");

    Serial1.print("Pressure: ");
    Serial1.print(bme.getPressure());
    Serial1.println(" hPa");

    Serial1.print("Humidity: ");
    Serial1.print(bme.getHumidity());
    Serial1.println(" %");

    Serial1.print("Gas: ");
    Serial1.print(bme.getGasResistance() / 1000.0);
    Serial1.println(" kOhm");

    Serial1.println("-----------------------");
  } else {
    Serial1.println("⚠ Sensor read failed");
  }

  delay(1000);
}
