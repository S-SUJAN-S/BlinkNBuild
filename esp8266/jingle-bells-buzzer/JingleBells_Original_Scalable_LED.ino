/**
 * @file JingleBells_Original_Scalable_LED.ino
 * @author Sujan S.
 * @brief Professional buzzer music engine using original piano frequencies,
 *        global frequency scaling, official tempo, and LED–sound synchronization.
 *
 * Behavior:
 *  - LED turns ON exactly when the buzzer sounds
 *  - LED turns OFF only during articulation gaps or rests
 *
 * NOTE:
 *  - On ESP8266 / ESP32, LED_BUILTIN is ACTIVE-LOW
 *    LOW  = LED ON
 *    HIGH = LED OFF
 */

#include <Arduino.h>

// ===============================
// HARDWARE CONFIG
// ===============================
static const uint8_t BUZZER_PIN = D1;
static const uint8_t LED_PIN    = LED_BUILTIN;

// ===============================
// GLOBAL FREQUENCY SCALING
// ===============================
// 1.0  → exact piano pitch
// >1.0 → shifted upward (buzzer sweet spot)
static const float SCALE = 5.0f;

// ===============================
// TEMPO ENGINE
// ===============================
static const uint16_t BPM = 140;
static const uint32_t WHOLE_NOTE_MS = (60000UL * 4) / BPM;

// ===============================
// MUSICAL DURATIONS (FRACTIONS)
// ===============================
#define W  1.0f
#define H  0.5f
#define Q  0.25f
#define E  0.125f

// ===============================
// ARTICULATION
// ===============================
static const float GAP_RATIO = 0.15f;   // 15% silence between notes

// ===============================
// BASE PIANO FREQUENCIES (Hz)
// Order: G A B C D E F
// ===============================
static const float LOW_OCTAVE[]  = { 196.0, 220.0, 247.9, 261.6, 293.7, 329.6, 349.2 };
static const float MID_OCTAVE[]  = { 392.0, 440.0, 493.9, 523.3, 587.3, 659.3, 698.5 };
static const float HIGH_OCTAVE[] = { 784.0, 880.0, 987.8, 1046.5, 1174.7, 1318.5, 1396.9 };

// ===============================
// NOTE INDEX RESOLUTION
// ===============================
int noteIndex(char note) {
  switch (note) {
    case 'G': return 0;
    case 'A': return 1;
    case 'B': return 2;
    case 'C': return 3;
    case 'D': return 4;
    case 'E': return 5;
    case 'F': return 6;
    default:  return -1;   // Rest / invalid
  }
}

// ===============================
// PLAY ENGINE (LED-SYNCED)
// ===============================
void playNote(char note, char octave, float durationFrac) {

  const uint32_t durationMs = (uint32_t)(WHOLE_NOTE_MS * durationFrac);

  // REST
  if (note == 'R') {
    digitalWrite(LED_PIN, HIGH);   // LED OFF
    delay(durationMs);
    return;
  }

  const int idx = noteIndex(note);
  if (idx < 0) {
    digitalWrite(LED_PIN, HIGH);   // LED OFF
    delay(durationMs);
    return;
  }

  const float baseFreq =
      (octave == 'L') ? LOW_OCTAVE[idx]  :
      (octave == 'H') ? HIGH_OCTAVE[idx] :
                        MID_OCTAVE[idx];

  const uint16_t freq = (uint16_t)(baseFreq * SCALE);

  const uint32_t gapMs  = (uint32_t)(durationMs * GAP_RATIO);
  const uint32_t noteMs = durationMs - gapMs;

  // ===== NOTE ON =====
  tone(BUZZER_PIN, freq);
  digitalWrite(LED_PIN, LOW);    // LED ON (active-low)
  delay(noteMs);

  // ===== NOTE OFF (GAP) =====
  noTone(BUZZER_PIN);
  digitalWrite(LED_PIN, HIGH);   // LED OFF
  delay(gapMs);
}

// ===============================
// VERSE (LYRICALLY VERIFIED)
// ===============================
void playVerse() {

  playNote('G','L',E); playNote('E','M',E); playNote('D','M',E); playNote('C','M',E); playNote('G','L',H);
  playNote('G','L',E); playNote('G','L',E); playNote('G','L',E);
  playNote('E','M',E); playNote('D','M',E); playNote('C','M',E); playNote('A','L',H);

  playNote('A','L',E); playNote('F','M',E); playNote('E','M',E); playNote('D','M',E); playNote('B','L',H);
  playNote('G','M',E); playNote('G','M',E); playNote('F','M',E); playNote('D','M',E); playNote('E','M',H);

  playNote('G','M',E); playNote('E','M',E); playNote('D','M',E); playNote('C','M',E); playNote('G','L',H);
  playNote('G','M',E); playNote('E','M',E); playNote('D','M',E); playNote('C','M',E); playNote('A','L',H);

  playNote('A','L',E); playNote('A','L',E); playNote('F','M',E); playNote('E','M',E); playNote('D','M',H);
  playNote('G','M',E); playNote('G','M',E); playNote('G','M',E);
  playNote('A','M',E); playNote('G','M',E); playNote('F','M',E); playNote('D','M',E);

  playNote('C','M',W);
}

// ===============================
// CHORUS
// ===============================
void playChorus() {

  playNote('E','M',Q); playNote('E','M',Q); playNote('E','M',H);
  playNote('E','M',Q); playNote('E','M',Q); playNote('E','M',H);

  playNote('E','M',Q); playNote('G','M',Q); playNote('C','M',Q); playNote('D','M',Q);
  playNote('E','M',W);

  playNote('F','M',Q); playNote('F','M',Q); playNote('F','M',Q); playNote('F','M',Q);
  playNote('F','M',Q); playNote('E','M',Q); playNote('E','M',Q);
  playNote('E','M',E); playNote('E','M',E);

  playNote('E','M',Q); playNote('D','M',Q); playNote('D','M',Q); playNote('E','M',Q);
  playNote('D','M',H);
  playNote('G','H',H); // "HEY!"
}

// ===============================
// ARDUINO LIFECYCLE
// ===============================
void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);   // LED OFF (active-low)
}

void loop() {
  playVerse();
  playChorus();
  delay(3000);
}
