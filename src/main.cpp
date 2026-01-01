/*
 * Интерактивный музыкальный проект с фоторезисторами для Arduino UNO
 * 
 * Подключение:
 * - Бузер: Пин 9 (с резистором 100 Ом)
 * - Фоторезистор 1: A0 (управление нотой/частотой)
 * - Фоторезистор 2: A1 (управление длительностью интервала)
 * 
 * Управляйте звуком, изменяя освещённость фоторезисторов
 */

#include <Arduino.h>

// Пины для бузера и фоторезисторов
const int BUZZER_PIN = 9;
const int PHOTORESISTOR1_PIN = A0;  // Управление нотой
const int PHOTORESISTOR2_PIN = A1;  // Управление длительностью

// Массив доступных нот (частоты в Гц) - от низких до высоких
const int NOTES[] = {
  131,  // C3
  147,  // D3
  165,  // E3
  175,  // F3
  196,  // G3
  220,  // A3
  247,  // B3
  262,  // C4
  294,  // D4
  330,  // E4
  349,  // F4
  392,  // G4
  440,  // A4
  494,  // B4
  523,  // C5
  587,  // D5
  659,  // E5
  698,  // F5
  784,  // G5
  880,  // A5
  988,  // B5
  1047  // C6
};

const int NUM_NOTES = sizeof(NOTES) / sizeof(NOTES[0]);

// Переменные для работы с фоторезисторами
int photo1Value = 0;  // Значение с первого фоторезистора (нота)
int photo2Value = 0;  // Значение с второго фоторезистора (длительность)

// Переменные для управления звуком
int currentFrequency = 0;
int currentDuration = 100;  // Минимальная длительность в мс
unsigned long lastNoteTime = 0;

// Параметры для сглаживания показаний
const int SMOOTH_SAMPLES = 5;
int photo1History[SMOOTH_SAMPLES] = {0};
int photo2History[SMOOTH_SAMPLES] = {0};
int photo1HistoryIndex = 0;
int photo2HistoryIndex = 0;

// Интервал вывода информации в Serial Monitor (мс)
const int SERIAL_PRINT_INTERVAL = 500;

// Функция для сглаживания показаний фоторезистора
int smoothReading(int newValue, int history[], int numSamples, int& historyIdx) {
  history[historyIdx] = newValue;
  historyIdx = (historyIdx + 1) % numSamples;
  
  long sum = 0;
  for (int i = 0; i < numSamples; i++) {
    sum += history[i];
  }
  
  return sum / numSamples;
}

// Преобразование значения фоторезистора в индекс ноты
int mapToNote(int photoValue) {
  // Диапазон значений АЦП: 0-1023
  // Преобразуем в индекс от 0 до NUM_NOTES-1
  return map(photoValue, 0, 1023, 0, NUM_NOTES - 1);
}

// Преобразование значения фоторезистора в длительность (50-1000 мс)
int mapToDuration(int photoValue) {
  return map(photoValue, 0, 1023, 50, 1000);
}

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  
  Serial.begin(9600);
  Serial.println(F("=== Интерактивный музыкальный проект с фоторезисторами ==="));
  Serial.println(F(""));
  Serial.println(F("Управление:"));
  Serial.println(F("  Фоторезистор 1 (A0) - изменяет ноту (частоту звука)"));
  Serial.println(F("  Фоторезистор 2 (A1) - изменяет длительность интервала"));
  Serial.println(F(""));
  Serial.println(F("Закрывайте и открывайте фоторезисторы для создания музыки!"));
  Serial.println();
  
  // Инициализация истории значений
  for (int i = 0; i < SMOOTH_SAMPLES; i++) {
    photo1History[i] = analogRead(PHOTORESISTOR1_PIN);
    photo2History[i] = analogRead(PHOTORESISTOR2_PIN);
  }
  
  lastNoteTime = millis();
}

void loop() {
  unsigned long currentTime = millis();
  
  // Читаем значения с фоторезисторов
  int rawPhoto1 = analogRead(PHOTORESISTOR1_PIN);
  int rawPhoto2 = analogRead(PHOTORESISTOR2_PIN);
  
  // Сглаживаем показания
  photo1Value = smoothReading(rawPhoto1, photo1History, SMOOTH_SAMPLES, photo1HistoryIndex);
  photo2Value = smoothReading(rawPhoto2, photo2History, SMOOTH_SAMPLES, photo2HistoryIndex);
  
  // Преобразуем показания в ноту и длительность
  int noteIndex = mapToNote(photo1Value);
  int duration = mapToDuration(photo2Value);
  
  // Проверяем, пора ли играть новую ноту
  if (currentTime - lastNoteTime >= currentDuration) {
    // Останавливаем предыдущий звук
    noTone(BUZZER_PIN);
    
    // Получаем новую частоту
    currentFrequency = NOTES[noteIndex];
    currentDuration = duration;
    
    // Играем новую ноту
    tone(BUZZER_PIN, currentFrequency);
    
    // Запоминаем время
    lastNoteTime = currentTime;
    
    // Выводим информацию в Serial Monitor
    static unsigned long lastPrintTime = 0;
    if (currentTime - lastPrintTime >= SERIAL_PRINT_INTERVAL) {
      Serial.print(F("Фоторезистор1: "));
      Serial.print(photo1Value);
      Serial.print(F(" | Нота: "));
      Serial.print(currentFrequency);
      Serial.print(F(" Гц | Фоторезистор2: "));
      Serial.print(photo2Value);
      Serial.print(F(" | Длительность: "));
      Serial.print(currentDuration);
      Serial.println(F(" мс"));
      lastPrintTime = currentTime;
    }
  }
  
  // Небольшая задержка для стабильности чтения АЦП
  delay(10);
}
