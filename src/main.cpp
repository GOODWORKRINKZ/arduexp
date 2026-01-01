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
const int BUZZER_PIN = 11;
const int BUZZER_BIT_PIN = 10;
const int C_PIN = 8;
const int D_PIN = 7;
const int E_PIN = 6;
const int F_PIN = 5;
const int G_PIN = 4;
const int A_PIN = 3;
const int B_PIN = 2;
const int BIT_PIN = 9;

// Массив доступных нот (частоты в Гц) - от низких до высоких
const int NOTES[] = {
    262, // C4
    294, // D4
    330, // E4
    349, // F4
    392, // G4
    440, // A4
    494, // B4
};

void setup()
{
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUZZER_BIT_PIN, OUTPUT);
  pinMode(C_PIN, INPUT);
  pinMode(D_PIN, INPUT);
  pinMode(E_PIN, INPUT);
  pinMode(G_PIN, INPUT);
  pinMode(A_PIN, INPUT);
  pinMode(B_PIN, INPUT);
  pinMode(BIT_PIN, INPUT);
}

void playButton(int buttonPin, int buzz_pin, int note)
{
  if (digitalRead(buttonPin) == HIGH)
  {
    tone(buzz_pin, note, 5);
  }
}

void loop()
{
  playButton(C_PIN, BUZZER_PIN, NOTES[0]);
  playButton(D_PIN, BUZZER_PIN, NOTES[1]);
  playButton(E_PIN, BUZZER_PIN, NOTES[2]);
  playButton(F_PIN, BUZZER_PIN, NOTES[3]);
  playButton(G_PIN, BUZZER_PIN, NOTES[4]);
  playButton(A_PIN, BUZZER_PIN, NOTES[5]);
  playButton(B_PIN, BUZZER_PIN, NOTES[6]);
  playButton(BIT_PIN, BUZZER_BIT_PIN, 666);
}
