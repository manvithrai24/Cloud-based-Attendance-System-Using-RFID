#include <MFRC522.h>
#include <SPI.h>

#define RST_PIN   5     // Reset pin
#define SS_PIN    4    // Slave Select pin

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

void setup() {
  Serial.begin(9600);
  SPI.begin();       // Initialize SPI communication
  mfrc522.PCD_Init();  // Initialize MFRC522

  Serial.println("Scan an RFID tag to write a name...");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Read the current name from the tag
  byte buffer[18];
  byte size = sizeof(buffer);
  MFRC522::StatusCode status = mfrc522.MIFARE_Read(1, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.println("Failed to read data from the tag. Make sure it's formatted for writing.");
    return;
  }

  // Print the current name
  Serial.print("Current name: ");
  for (byte i = 0; i < size; i++) {
    Serial.write(buffer[i]);
  }
  Serial.println();

  // Prompt for a new name
  Serial.println("Enter a new name to write to the tag:");
  while (!Serial.available()) {
    // Wait for user input
  }
  delay(100);

  // Read the new name from Serial input
  String newName = Serial.readStringUntil('\n');
  newName.trim();

  // Validate the new name length
  if (newName.length() > 16) {
    Serial.println("Name is too long. Maximum length is 16 characters.");
    return;
  }

  // Prepare the new name buffer
  byte newBuffer[18] = {0};
  newName.getBytes(newBuffer, newName.length());

  // Write the new name to the tag
  status = mfrc522.MIFARE_Write(1, newBuffer, newName.length());
  if (status != MFRC522::STATUS_OK) {
    Serial.println("Failed to write data to the tag.");
    return;
  }

  Serial.println("Name successfully written to the tag.");

  // Halt PICC (tag) to stop further operations
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD (reader)
  mfrc522.PCD_StopCrypto1();
}
