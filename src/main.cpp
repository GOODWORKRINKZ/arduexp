/*
 * Полифонический синтезатор на базе библиотеки Mozzi для Arduino UNO
 *
 * Подключение:
 * - Аудио выход: Пин 9 (через резистор 100 Ом и конденсатор к динамику)
 * - Кнопки нот: C=8, D=7, E=6, F=5, G=4, A=3, B=2 (семь нот)
 * - Кнопка выбора инструмента: пин 9 (BIT_PIN) - переключает между 4 инструментами
 *
 * ПОЛИФОНИЯ: можно нажимать несколько кнопок одновременно для игры аккордов!
 *
 * Инструменты (тембры):
 * 0 - Синусоида (чистый тон, флейта)
 * 1 - Пила (яркий, резкий звук, синтезатор)
 * 2 - Меандр (полый звук, кларнет)
 * 3 - Треугольник (мягкий звук, виолончель)
 *
 * Библиотека Mozzi создает высококачественный звук через PWM
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/sin2048_int8.h>       // Синусоида
#include <tables/saw2048_int8.h>       // Пила
#include <tables/square_no_alias_2048_int8.h> // Меандр
#include <tables/triangle2048_int8.h>  // Треугольник

#define CONTROL_RATE 128 // Частота опроса управления (Гц) - увеличена для лучшего отклика

// Количество нот (голосов)
#define NUM_VOICES 7

// Пины для кнопок (как они уже подключены)
const int NOTE_PINS[NUM_VOICES] = {8, 7, 6, 5, 4, 3, 2}; // C, D, E, F, G, A, B
const int BIT_PIN = 9; // Кнопка выбора инструмента (тембра)

// Массив нот (частоты в Гц)
const float NOTES[] = {
    261.63, // C4
    293.66, // D4
    329.63, // E4
    349.23, // F4
    392.00, // G4
    440.00, // A4
    493.88, // B4
};

// Создаем осцилляторы для каждой ноты (голоса) и каждого инструмента
// Синусоида
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> oscSin[NUM_VOICES];

// Пила
Oscil<SAW2048_NUM_CELLS, AUDIO_RATE> oscSaw[NUM_VOICES];

// Меандр
Oscil<SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> oscSquare[NUM_VOICES];

// Треугольник
Oscil<TRIANGLE2048_NUM_CELLS, AUDIO_RATE> oscTriangle[NUM_VOICES];

// Переменные состояния
int currentInstrument = 0;  // Текущий инструмент (0-3: синус, пила, меандр, треугольник)
bool noteActive[NUM_VOICES]; // Какие ноты сейчас играют
bool bitButtonPressed = false;
unsigned long lastBitPressTime = 0;
const unsigned long DEBOUNCE_DELAY = 200; // Антидребезг 200 мс

void setup() {
  // Инициализация кнопок нот
  for (int i = 0; i < NUM_VOICES; i++) {
    pinMode(NOTE_PINS[i], INPUT);
    noteActive[i] = false;
    
    // Инициализируем осцилляторы для каждого голоса
    oscSin[i].setTable(SIN2048_DATA);
    oscSin[i].setFreq(NOTES[i]);
    
    oscSaw[i].setTable(SAW2048_DATA);
    oscSaw[i].setFreq(NOTES[i]);
    
    oscSquare[i].setTable(SQUARE_NO_ALIAS_2048_DATA);
    oscSquare[i].setFreq(NOTES[i]);
    
    oscTriangle[i].setTable(TRIANGLE2048_DATA);
    oscTriangle[i].setFreq(NOTES[i]);
  }
  
  // Кнопка выбора инструмента
  pinMode(BIT_PIN, INPUT);
  
  // Запуск Mozzi
  startMozzi(CONTROL_RATE);
}

void updateControl() {
  // Проверяем кнопку переключения инструмента с антидребезгом
  if (digitalRead(BIT_PIN) == HIGH) {
    unsigned long currentTime = millis();
    if (!bitButtonPressed && (currentTime - lastBitPressTime > DEBOUNCE_DELAY)) {
      bitButtonPressed = true;
      lastBitPressTime = currentTime;
      currentInstrument = (currentInstrument + 1) % 4; // Переключаем между 4 инструментами
    }
  } else {
    bitButtonPressed = false;
  }
  
  // Проверяем кнопки нот - обновляем состояние каждой ноты
  for (int i = 0; i < NUM_VOICES; i++) {
    noteActive[i] = (digitalRead(NOTE_PINS[i]) == HIGH);
  }
}

AudioOutput_t updateAudio() {
  long sample = 0;
  int activeCount = 0;
  
  // Суммируем все активные голоса
  for (int i = 0; i < NUM_VOICES; i++) {
    if (noteActive[i]) {
      // Выбираем инструмент (осциллятор) в зависимости от currentInstrument
      switch (currentInstrument) {
        case 0: // Синусоида (чистый тон, флейта)
          sample += oscSin[i].next();
          break;
        case 1: // Пила (яркий, резкий звук, синтезатор)
          sample += oscSaw[i].next();
          break;
        case 2: // Меандр (полый звук, кларнет)
          sample += oscSquare[i].next();
          break;
        case 3: // Треугольник (мягкий звук, виолончель)
          sample += oscTriangle[i].next();
          break;
      }
      activeCount++;
    }
  }
  
  // Если играют несколько нот, нормализуем амплитуду чтобы избежать клиппинга
  if (activeCount > 1) {
    sample = sample / activeCount;
  }
  
  return MonoOutput::from8Bit(sample);
}

void loop() {
  audioHook(); // Обязательный вызов для Mozzi
}
