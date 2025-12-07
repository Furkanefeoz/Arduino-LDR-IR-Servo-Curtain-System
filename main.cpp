#include <Arduino.h>
#include <IRremote.h>
#include <Servo.h>

// ========== PIN TANIMLAMALARI ==========
const int ldrPin = A0;      
const int servoPin1 = 9;     
const int servoPin2 = 12;    
const int RECV_PIN = 8;      // IR Alıcı Pini

// ========== IR KUMANDA KODLARI (HEX) ==========
const long IR_OK_CODE   = 0xE31CFF00;
const long IR_UP_CODE   = 0xE718FF00;
const long IR_DOWN_CODE = 0xAD52FF00;

// ========== 360 DERECE SERVO KONTROLÜ ==========
const int SERVO_DUR = 90;     
const int YON_ILERI = 120;    // Fiziksel Yön A (Açılma)
const int YON_GERI = 60;      // Fiziksel Yön B (Kapanma)
const int ADIM_BUYUKLUGU = 5; // MANUEL Adım Büyüklüğü
const int ADIM_SURESI_MS = 200; // MANUEL Adım Süresi
// TAM HAREKET SÜRESİ: 140 birim mesafe (120 - (-20)) için denenen süre.
const int TAM_ACIK_SURE_MS = 4800; // Burayı motoruna göre mutlaka ayarla!

// ========== LDR VE SINIRLAR (GÜNCELLENDİ) ==========
const int ORTA_ESIK = 350;    // EŞİK DEĞERİ
const int MAX_ACI = 120;      // Tam Açık Mantıksal Konum
const int MIN_ACI = -20;      // YENİ TAM KAPALI MANTIKSAL KONUM

// GÜRÜLTÜ FİLTRESİ
const int DEGISIM_ESIGI = 10; 

// ========== DURUM DEĞİŞKENLERİ ==========
int mevcutServoAci = 0;     
int sonLdrDeger = 0;        
bool otomatikModAktif = true; 
bool motorCalisiyor = false; 

Servo perdeServo1;          
Servo perdeServo2;          

// Motorları çalıştıran ve açıyı güncelleyen tek adım fonksiyonu (Sadece MANUEL için kullanılır)
void servoAdimAt(int yonKomutu, int adimBuyuklugu) {
    // 1. Motorları bağla
    if (!perdeServo1.attached()) {
        perdeServo1.attach(servoPin1);
    }
    if (!perdeServo2.attached()) {
        perdeServo2.attach(servoPin2);
    }
    
    // Motorları çalıştır
    perdeServo1.write(yonKomutu);
    perdeServo2.write(yonKomutu);
    
    // 2. Belirlenen süre kadar bekle (5 derecelik adım süresi)
    delay(ADIM_SURESI_MS); 

    // 3. Durdur ve bağlantıyı kes
    perdeServo1.write(SERVO_DUR); 
    perdeServo2.write(SERVO_DUR);
    delay(100); 
    
    perdeServo1.detach();
    perdeServo2.detach();
    
    // 4. MANTIKSAL AÇI GÜNCELLEMESİ
    if (yonKomutu == YON_ILERI) {
        mevcutServoAci += adimBuyuklugu;
    } else if (yonKomutu == YON_GERI) {
        mevcutServoAci -= adimBuyluklugu;
    }
    // Sınır kontrolü: Yeni MIN_ACI'yı kullanır.
    if (mevcutServoAci > MAX_ACI) mevcutServoAci = MAX_ACI;
    if (mevcutServoAci < MIN_ACI) mevcutServoAci = MIN_ACI;
    
    Serial.print("MANUEL Adım Bitti. Yeni Aci: ");
    Serial.println(mevcutServoAci);
}

// Akıcı hareket başlatma fonksiyonu (Sadece OTOMATİK için kullanılır)
void baslatAkiciHareket(int yonKomutu, int hedefAci) {
    if (motorCalisiyor) return; 

    // YENİ MANTIKSAL ARALIK (140 birim): (MAX_ACI - MIN_ACI)
    const float MANTIKSAL_ARALIK = (float)(MAX_ACI - MIN_ACI);

    // Motorun çalışacağı süreyi hesapla (Mevcut pozisyona göre)
    int mesafe = abs(hedefAci - mevcutServoAci);
    
    // Oransal süre hesaplaması yeni aralığa göre yapıldı.
    long sure = (long)(((float)mesafe / MANTIKSAL_ARALIK) * TAM_ACIK_SURE_MS);
    
    if (sure == 0) return; 
    
    // Motorları bağla
    perdeServo1.attach(servoPin1);
    perdeServo2.attach(servoPin2);
    
    // Sürekli çalışma komutu
    perdeServo1.write(yonKomutu);
    perdeServo2.write(yonKomutu);
    
    Serial.print("--- OTOMATİK HAREKET BAŞLADI: Hedef Aci ");
    Serial.print(hedefAci);
    Serial.print(". Süre: ");
    Serial.print(sure);
    Serial.println("ms ---");

    delay(sure); // Belirlenen süre boyunca motorlar sürekli döner
    
    // Hareket bitince durdur
    perdeServo1.write(SERVO_DUR); 
    perdeServo2.write(SERVO_DUR);
    delay(100); 
    
    perdeServo1.detach();
    perdeServo2.detach();
    
    mevcutServoAci = hedefAci; // Konumu anında hedefe güncelle
    Serial.println("OTOMATİK HAREKET TAMAMLANDI ve durduruldu.");
}


void setup() {
  Serial.begin(9600);           
  // Servoların başlangıç ayarları ve durdurulması
  perdeServo1.attach(servoPin1); 
  perdeServo2.attach(servoPin2); 
  perdeServo1.write(SERVO_DUR); 
  perdeServo2.write(SERVO_DUR); 
  delay(1000); 
  perdeServo1.detach();
  perdeServo2.detach();
  
  // IR alıcısını başlat
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("Sistem Baslatildi. Başlangıç Modu: OTOMATİK.");
}

void loop() {
  
  // --- BÖLÜM 1: IR KUMANDA KONTROLÜ ---
  if (IrReceiver.decode()) {
    
    long komutKodu = IrReceiver.decodedIRData.decodedRawData;
    
    // 1a. OK Tuşu: Mod Değiştirme
    if (komutKodu == IR_OK_CODE) {
        otomatikModAktif = !otomatikModAktif;
        Serial.print("--- KONTROL MODU DEĞİŞTİ: ");
        Serial.println(otomatikModAktif ? "OTOMATIK (LDR)" : "MANUEL (Kumanda)");
    } 
    
    // 1b. Manuel Kontrol (Sadece Manuel Modda)
    else if (!otomatikModAktif) {
        if (komutKodu == IR_DOWN_CODE && mevcutServoAci < MAX_ACI) {
            servoAdimAt(YON_ILERI, ADIM_BUYUKLUGU);
        }
        else if (komutKodu == IR_UP_CODE && mevcutServoAci > MIN_ACI) {
            servoAdimAt(YON_GERI, ADIM_BUYUKLUGU);
        }
    }
    
    IrReceiver.resume();
  }
  
  // --- BÖLÜM 2: LDR OTOMATİK KONTROL (Sadece Otomatik Modda) ---
  if (otomatikModAktif) {
      
      // LDR Okuması ve Ortalama Alma
      int toplamDeger = 0;
      for (int i = 0; i < 5; i++) {
        toplamDeger += analogRead(ldrPin);
        delay(2);
      }
      int ldrDeger = toplamDeger / 5; 

      // Işık değerini her okumada ekrana yazdır.
      Serial.print("LDR Degeri: ");
      Serial.print(ldrDeger);
      Serial.print(" | Mevcut Aci: ");
      Serial.println(mevcutServoAci);
      
      // Gürültü Filtresi
      if (abs(ldrDeger - sonLdrDeger) > DEGISIM_ESIGI) {
          sonLdrDeger = ldrDeger; 

          // YÜKSEK IŞIK (LDR <= 350) -> KAPANMA (Hedef MIN_ACI = -20)
          if (ldrDeger <= ORTA_ESIK && mevcutServoAci > MIN_ACI) {
              baslatAkiciHareket(YON_GERI, MIN_ACI); 
          } 
          
          // DÜŞÜK IŞIK (LDR > 350) -> AÇILMA (Hedef MAX_ACI = 120)
          else if (ldrDeger > ORTA_ESIK && mevcutServoAci < MAX_ACI) {
              baslatAkiciHareket(YON_ILERI, MAX_ACI);
          }
      }
  }

  // IR alıcısının tekrar okumaya hazırlandığından emin olmak için
  delay(100); 
}
