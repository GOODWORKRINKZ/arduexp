/*
 * Полифонический проект с двумя бузерами для Arduino UNO
 * 
 * Подключение:
 * - Бузер 1: Пин 9 (с резистором 100 Ом)
 * - Бузер 2: Пин 10 (с резистором 100 Ом)
 * 
 * Каждый бузер может играть свою ноту, создавая полифонический эффект
 */

#include <Arduino.h>

// Пины для бузеров
const int BUZZER1_PIN = 9;
const int BUZZER2_PIN = 10;

// Определение нот (частоты в Гц)
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define REST     0

// Структура для хранения ноты
struct Note {
  int frequency;
  int duration;
};

// Мелодия 1: "Jingle Bells" (упрощённая версия)
Note melody1[] = {
  {NOTE_E5, 250}, {NOTE_E5, 250}, {NOTE_E5, 500},
  {NOTE_E5, 250}, {NOTE_E5, 250}, {NOTE_E5, 500},
  {NOTE_E5, 250}, {NOTE_G5, 250}, {NOTE_C5, 250}, {NOTE_D5, 250},
  {NOTE_E5, 1000},
  {NOTE_F5, 250}, {NOTE_F5, 250}, {NOTE_F5, 250}, {NOTE_F5, 250},
  {NOTE_F5, 250}, {NOTE_E5, 250}, {NOTE_E5, 250}, {NOTE_E5, 125}, {NOTE_E5, 125},
  {NOTE_E5, 250}, {NOTE_D5, 250}, {NOTE_D5, 250}, {NOTE_E5, 250},
  {NOTE_D5, 500}, {NOTE_G5, 500},
};

// Мелодия 2: Аккомпанемент (басовая линия)
Note melody2[] = {
  {NOTE_C4, 250}, {NOTE_G4, 250}, {NOTE_C4, 500},
  {NOTE_C4, 250}, {NOTE_G4, 250}, {NOTE_C4, 500},
  {NOTE_C4, 250}, {NOTE_E4, 250}, {NOTE_G4, 250}, {NOTE_A4, 250},
  {NOTE_C4, 1000},
  {NOTE_D4, 250}, {NOTE_A4, 250}, {NOTE_D4, 250}, {NOTE_A4, 250},
  {NOTE_D4, 250}, {NOTE_C4, 250}, {NOTE_G4, 250}, {NOTE_C4, 125}, {NOTE_G4, 125},
  {NOTE_G4, 250}, {NOTE_B4, 250}, {NOTE_G4, 250}, {NOTE_C5, 250},
  {NOTE_G4, 500}, {NOTE_E4, 500},
};

// Мелодия 3: "Happy Birthday" - мелодия
Note melody3[] = {
  {NOTE_C4, 375}, {NOTE_C4, 125}, {NOTE_D4, 500}, {NOTE_C4, 500}, {NOTE_F4, 500}, {NOTE_E4, 1000},
  {NOTE_C4, 375}, {NOTE_C4, 125}, {NOTE_D4, 500}, {NOTE_C4, 500}, {NOTE_G4, 500}, {NOTE_F4, 1000},
  {NOTE_C4, 375}, {NOTE_C4, 125}, {NOTE_C5, 500}, {NOTE_A4, 500}, {NOTE_F4, 500}, {NOTE_E4, 500}, {NOTE_D4, 1000},
  {NOTE_AS4, 375}, {NOTE_AS4, 125}, {NOTE_A4, 500}, {NOTE_F4, 500}, {NOTE_G4, 500}, {NOTE_F4, 1000},
};

// Мелодия 4: "Happy Birthday" - аккомпанемент
Note melody4[] = {
  {NOTE_C3, 375}, {NOTE_G3, 125}, {NOTE_F3, 500}, {NOTE_E3, 500}, {NOTE_D3, 500}, {NOTE_C3, 1000},
  {NOTE_C3, 375}, {NOTE_G3, 125}, {NOTE_F3, 500}, {NOTE_E3, 500}, {NOTE_E3, 500}, {NOTE_D3, 1000},
  {NOTE_F3, 375}, {NOTE_C4, 125}, {NOTE_A3, 500}, {NOTE_F3, 500}, {NOTE_D3, 500}, {NOTE_C3, 500}, {NOTE_G3, 1000},
  {NOTE_D3, 375}, {NOTE_F3, 125}, {NOTE_F3, 500}, {NOTE_D3, 500}, {NOTE_E3, 500}, {NOTE_F3, 1000},
};

// Мелодия 5: "Ode to Joy" (Бетховен) - мелодия
Note melody5[] = {
  {NOTE_E5, 250}, {NOTE_E5, 250}, {NOTE_F5, 250}, {NOTE_G5, 250},
  {NOTE_G5, 250}, {NOTE_F5, 250}, {NOTE_E5, 250}, {NOTE_D5, 250},
  {NOTE_C5, 250}, {NOTE_C5, 250}, {NOTE_D5, 250}, {NOTE_E5, 250},
  {NOTE_E5, 375}, {NOTE_D5, 125}, {NOTE_D5, 500},
  {NOTE_E5, 250}, {NOTE_E5, 250}, {NOTE_F5, 250}, {NOTE_G5, 250},
  {NOTE_G5, 250}, {NOTE_F5, 250}, {NOTE_E5, 250}, {NOTE_D5, 250},
  {NOTE_C5, 250}, {NOTE_C5, 250}, {NOTE_D5, 250}, {NOTE_E5, 250},
  {NOTE_D5, 375}, {NOTE_C5, 125}, {NOTE_C5, 500},
};

// Мелодия 6: "Ode to Joy" - аккомпанемент
Note melody6[] = {
  {NOTE_C4, 250}, {NOTE_G3, 250}, {NOTE_A3, 250}, {NOTE_C4, 250},
  {NOTE_C4, 250}, {NOTE_A3, 250}, {NOTE_G3, 250}, {NOTE_F3, 250},
  {NOTE_E3, 250}, {NOTE_E3, 250}, {NOTE_F3, 250}, {NOTE_G3, 250},
  {NOTE_G3, 375}, {NOTE_F3, 125}, {NOTE_F3, 500},
  {NOTE_C4, 250}, {NOTE_G3, 250}, {NOTE_A3, 250}, {NOTE_C4, 250},
  {NOTE_C4, 250}, {NOTE_A3, 250}, {NOTE_G3, 250}, {NOTE_F3, 250},
  {NOTE_E3, 250}, {NOTE_E3, 250}, {NOTE_F3, 250}, {NOTE_G3, 250},
  {NOTE_F3, 375}, {NOTE_E3, 125}, {NOTE_E3, 500},
};

int melody1Length = sizeof(melody1) / sizeof(melody1[0]);
int melody2Length = sizeof(melody2) / sizeof(melody2[0]);
int melody3Length = sizeof(melody3) / sizeof(melody3[0]);
int melody4Length = sizeof(melody4) / sizeof(melody4[0]);
int melody5Length = sizeof(melody5) / sizeof(melody5[0]);
int melody6Length = sizeof(melody6) / sizeof(melody6[0]);

// Переменные для отслеживания воспроизведения
unsigned long buzzer1Time = 0;
unsigned long buzzer2Time = 0;
int currentNote1 = 0;
int currentNote2 = 0;
bool isPlaying = false;

// Указатели на текущие мелодии
Note* currentMelody1 = melody1;
Note* currentMelody2 = melody2;
int currentLength1 = melody1Length;
int currentLength2 = melody2Length;

void setup() {
  pinMode(BUZZER1_PIN, OUTPUT);
  pinMode(BUZZER2_PIN, OUTPUT);
  
  Serial.begin(9600);
  Serial.println(F("=== Полифонический проект с двумя бузерами ==="));
  Serial.println(F("Команды:"));
  Serial.println(F("  1 - Jingle Bells"));
  Serial.println(F("  2 - Happy Birthday"));
  Serial.println(F("  3 - Ode to Joy"));
  Serial.println(F("  s - Стоп"));
  Serial.println();
  
  // Начинаем с первой мелодии
  startMelody(1);
}

void startMelody(int melodyNumber) {
  noTone(BUZZER1_PIN);
  noTone(BUZZER2_PIN);
  
  currentNote1 = 0;
  currentNote2 = 0;
  buzzer1Time = millis();
  buzzer2Time = millis();
  isPlaying = true;
  
  switch(melodyNumber) {
    case 1:
      currentMelody1 = melody1;
      currentMelody2 = melody2;
      currentLength1 = melody1Length;
      currentLength2 = melody2Length;
      Serial.println(F("Играем: Jingle Bells"));
      break;
    case 2:
      currentMelody1 = melody3;
      currentMelody2 = melody4;
      currentLength1 = melody3Length;
      currentLength2 = melody4Length;
      Serial.println(F("Играем: Happy Birthday"));
      break;
    case 3:
      currentMelody1 = melody5;
      currentMelody2 = melody6;
      currentLength1 = melody5Length;
      currentLength2 = melody6Length;
      Serial.println(F("Играем: Ode to Joy"));
      break;
    default:
      isPlaying = false;
      Serial.println(F("Неизвестная мелодия"));
      return;
  }
  
  // Начинаем играть первые ноты сразу
  if (currentMelody1[0].frequency != REST) {
    tone(BUZZER1_PIN, currentMelody1[0].frequency);
  }
  if (currentMelody2[0].frequency != REST) {
    tone(BUZZER2_PIN, currentMelody2[0].frequency);
  }
}

void stopMelody() {
  isPlaying = false;
  noTone(BUZZER1_PIN);
  noTone(BUZZER2_PIN);
  Serial.println(F("Музыка остановлена"));
}

void loop() {
  // Проверяем команды из Serial
  if (Serial.available() > 0) {
    char command = Serial.read();
    
    if (command == '1') {
      startMelody(1);
    } else if (command == '2') {
      startMelody(2);
    } else if (command == '3') {
      startMelody(3);
    } else if (command == 's' || command == 'S') {
      stopMelody();
    }
  }
  
  if (!isPlaying) {
    return;
  }
  
  unsigned long currentTime = millis();
  
  // Управление первым бузером
  if (currentNote1 < currentLength1) {
    if (currentTime - buzzer1Time >= currentMelody1[currentNote1].duration) {
      // Переходим к следующей ноте
      currentNote1++;
      buzzer1Time = currentTime;
      
      if (currentNote1 < currentLength1) {
        // Играем следующую ноту
        if (currentMelody1[currentNote1].frequency == REST) {
          noTone(BUZZER1_PIN);
        } else {
          tone(BUZZER1_PIN, currentMelody1[currentNote1].frequency);
        }
      } else {
        noTone(BUZZER1_PIN);
      }
    }
  }
  
  // Управление вторым бузером
  if (currentNote2 < currentLength2) {
    if (currentTime - buzzer2Time >= currentMelody2[currentNote2].duration) {
      // Переходим к следующей ноте
      currentNote2++;
      buzzer2Time = currentTime;
      
      if (currentNote2 < currentLength2) {
        // Играем следующую ноту
        if (currentMelody2[currentNote2].frequency == REST) {
          noTone(BUZZER2_PIN);
        } else {
          tone(BUZZER2_PIN, currentMelody2[currentNote2].frequency);
        }
      } else {
        noTone(BUZZER2_PIN);
      }
    }
  }
  
  // Если обе мелодии закончились, начинаем сначала
  if (currentNote1 >= currentLength1 && currentNote2 >= currentLength2) {
    delay(2000); // Пауза перед повтором
    currentNote1 = 0;
    currentNote2 = 0;
    buzzer1Time = millis();
    buzzer2Time = millis();
    
    // Начинаем играть первые ноты
    if (currentMelody1[0].frequency != REST) {
      tone(BUZZER1_PIN, currentMelody1[0].frequency);
    }
    if (currentMelody2[0].frequency != REST) {
      tone(BUZZER2_PIN, currentMelody2[0].frequency);
    }
    
    Serial.println(F("Повтор мелодии..."));
  }
}
