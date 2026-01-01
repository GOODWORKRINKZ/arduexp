/*
 * Синтезатор на базе библиотеки Mozzi для Arduino UNO
 *
 * Подключение:
 * - Аудио выход: Пин 9 (через резистор 100 Ом и конденсатор к динамику)
 * - Кнопки нот: C=8, D=7, E=6, F=5, G=4, A=3, B=2 (семь нот)
 * - Кнопка выбора инструмента: пин 9 (BIT_PIN) - переключает между 4 инструментами
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

#define CONTROL_RATE 64 // Частота опроса управления (Гц)

// Пины для кнопок (как они уже подключены)
const int C_PIN = 8;
const int D_PIN = 7;
const int E_PIN = 6;
const int F_PIN = 5;
const int G_PIN = 4;
const int A_PIN = 3;
const int B_PIN = 2;
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

// Создаем осцилляторы для разных волновых форм
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> oscSin(SIN2048_DATA);
Oscil<SAW2048_NUM_CELLS, AUDIO_RATE> oscSaw(SAW2048_DATA);
Oscil<SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> oscSquare(SQUARE_NO_ALIAS_2048_DATA);
Oscil<TRIANGLE2048_NUM_CELLS, AUDIO_RATE> oscTriangle(TRIANGLE2048_DATA);

// Переменные состояния
int currentInstrument = 0;  // Текущий инструмент (0-3: синус, пила, меандр, треугольник)
int currentNote = -1;       // Текущая играющая нота (-1 = нет ноты)
bool bitButtonPressed = false;
unsigned long lastBitPressTime = 0;
const unsigned long DEBOUNCE_DELAY = 200; // Антидребезг 200 мс

void setup() {
  // Инициализация кнопок
  pinMode(C_PIN, INPUT);
  pinMode(D_PIN, INPUT);
  pinMode(E_PIN, INPUT);
  pinMode(F_PIN, INPUT);
  pinMode(G_PIN, INPUT);
  pinMode(A_PIN, INPUT);
  pinMode(B_PIN, INPUT);
  pinMode(BIT_PIN, INPUT);
  
  // Запуск Mozzi
  startMozzi(CONTROL_RATE);
  
  // Устанавливаем начальную громкость для всех осцилляторов
  oscSin.setFreq(440);
  oscSaw.setFreq(440);
  oscSquare.setFreq(440);
  oscTriangle.setFreq(440);
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
  
  // Проверяем кнопки нот
  currentNote = -1;
  
  if (digitalRead(C_PIN) == HIGH) currentNote = 0;
  else if (digitalRead(D_PIN) == HIGH) currentNote = 1;
  else if (digitalRead(E_PIN) == HIGH) currentNote = 2;
  else if (digitalRead(F_PIN) == HIGH) currentNote = 3;
  else if (digitalRead(G_PIN) == HIGH) currentNote = 4;
  else if (digitalRead(A_PIN) == HIGH) currentNote = 5;
  else if (digitalRead(B_PIN) == HIGH) currentNote = 6;
  
  // Устанавливаем частоту для всех осцилляторов
  if (currentNote >= 0) {
    float freq = NOTES[currentNote];
    oscSin.setFreq(freq);
    oscSaw.setFreq(freq);
    oscSquare.setFreq(freq);
    oscTriangle.setFreq(freq);
  }
}

AudioOutput_t updateAudio() {
  int sample = 0;
  
  // Если нажата кнопка ноты, воспроизводим звук выбранным инструментом
  if (currentNote >= 0) {
    // Выбираем инструмент (осциллятор) в зависимости от currentInstrument
    switch (currentInstrument) {
      case 0: // Синусоида (чистый тон, флейта)
        sample = oscSin.next();
        break;
      case 1: // Пила (яркий, резкий звук, синтезатор)
        sample = oscSaw.next();
        break;
      case 2: // Меандр (полый звук, кларнет)
        sample = oscSquare.next();
        break;
      case 3: // Треугольник (мягкий звук, виолончель)
        sample = oscTriangle.next();
        break;
    }
  }
  
  return MonoOutput::from8Bit(sample);
}

void loop() {
  audioHook(); // Обязательный вызов для Mozzi
}
