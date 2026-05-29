#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define RX_BIOMETRIA 2
#define TX_BIOMETRIA 3

struct Aluno {
  int id;
  String nome;
  String ra;
};

Aluno alunos[] = {
  {1, "Gustavo",   "RA:26001583"},
  {2, "Iago", "RA:26000755"},
  {3, "Rayanne", "RA:26000437"},
  {4, "Arthur", " RA:26000374"},
  {5, "Yasmin", "RA:26000288"}
};

const int totalAlunos = sizeof(alunos) / sizeof(alunos[0]);

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial biometriaSerial(RX_BIOMETRIA, TX_BIOMETRIA);
Adafruit_Fingerprint finger(&biometriaSerial);

void lcdMensagem(String linha1, String linha2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(linha1);
  lcd.setCursor(0, 1);
  lcd.print(linha2);
}

Aluno* buscarAluno(int id) {
  for (int i = 0; i < totalAlunos; i++) {
    if (alunos[i].id == id) return &alunos[i];
  }
  return nullptr;
}

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcdMensagem("Sistema pronto", "Aguardando...");

  biometriaSerial.begin(57600);
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Sensor encontrado!");
  } else {
    Serial.println("Sensor NAO encontrado.");
    lcdMensagem("ERRO:", "Sensor ausente");
    while (true);
  }
}

void loop() {
  int resultado = finger.getImage();
  if (resultado != FINGERPRINT_OK) return;

  resultado = finger.image2Tz();
  if (resultado != FINGERPRINT_OK) return;

  resultado = finger.fingerFastSearch();

  if (resultado == FINGERPRINT_OK) {
    Aluno* aluno = buscarAluno(finger.fingerID);

    if (aluno != nullptr) {
      Serial.println("Aluno: " + aluno->nome + " | RA: " + aluno->ra);
      lcdMensagem(aluno->nome, aluno->ra);
    } else {
      Serial.println("ID " + String(finger.fingerID) + " sem aluno cadastrado.");
      lcdMensagem("ID: " + String(finger.fingerID), "Sem cadastro");
    }
  } else {
    Serial.println("Digital nao reconhecida.");
    lcdMensagem("Nao reconhecido", "Tente novamente");
  }

  delay(2000);
  lcdMensagem("Aguardando", "dedo no sensor...");
}