# Arduino-LDR-IR-Servo-Curtain-System
SmartCurtain360, LDR ışık sensörü ve IR kumanda ile çalışan çift 360° servo motorlu akıllı perde kontrol sistemidir. Işık seviyesine göre otomatik açılıp kapanır, manuel modda hassas adım kontrolü yapılabilir. 

SmartCurtain360 – Akıllı Perde Kontrol Sistemi

SmartCurtain360, LDR ışık sensörü, çift 360° servo motor ve IR kumanda kullanarak çalışan bir akıllı perde otomasyon sistemidir. Proje, ortam ışığına göre perdenin otomatik kontrolünü sağlar; ayrıca manuel mod ile kumanda üzerinden hassas adım hareketleri yapılabilir.

Bu sistem; ev otomasyonu, akıllı oda sistemleri ve prototip perde mekanizmaları için uygundur.

📌 Özellikler
🔹 Otomatik Mod (LDR Sensörü ile)

Ortam ışığını sürekli okur.

Belirli eşiğin altında perdeyi kapatır, üstünde açar.

5 ölçümlük ortalama ile daha stabil okuma gerçekleştirir.

Gürültü filtresi sayesinde küçük ışık değişimlerine tepki vermez.

🔹 Manuel Mod (IR Kumanda ile)

OK tuşu ile mod değiştirme.

UP → Perdeyi kapatma yönünde adım hareketi

DOWN → Perdeyi açma yönünde adım hareketi

Her adım servo motoru kısa süre çalıştırır ve açı değeri mantıksal olarak güncellenir.

🔹 360° Sürekli Döner Servo Kontrolü

Normal servo gibi açı verilmez.

PWM ile hız/yön kontrolü yapılır:

90 → durur

120 → ileri (açma)

60 → geri (kapama)

🔹 Çift Servo Motor Desteği

Aynı anda iki motor çalışır.

Perde mekanizmasını senkronize bir şekilde döndürür.

🔹 Akıcı Otomatik Hareket

Tam açılma/kapanma için süre tabanlı kontrol yapılır.

Motor süre boyunca sürekli dönüş modunda çalışır.

🛠️ Kullanılan Donanımlar
Parça	Adet
Arduino UNO	1
LDR Sensörü	1
360° Sürekli Servo Motor	2
IR Receiver (1838B vb.)	1
10k Direnç (LDR için)	1
Breadboard	1
Bağlantı kabloları	—
🔌 Devre Bağlantıları
LDR Sensörü

LDR’nin bir ucu → 5V

Diğer ucu → A0

A0 ile GND arasına 10k direnç

(LDR)----A0----(10k)----GND
   |                  
  5V

IR Receiver (IR Alıcı)

OUT → Pin 8

GND → GND

VCC → 5V

Servo Motorlar

Servo 1:

Sarı (sinyal) → Pin 9

Kırmızı → 5V

Siyah → GND

Servo 2:

Sarı → Pin 12

Kırmızı → 5V

Siyah → GND

⚠️ Önemli:
İki servo motor 5V hattından yüksek akım çekebilir.
Gerekirse harici 5V adaptör + ortak GND önerilir.

📜 Kodun Çalışma Mantığı
1️⃣ Başlangıç

Servo motorlar durdurularak başlar.

IR alıcı aktif edilir.

Sistem varsayılan olarak Otomatik Modda açılır.

2️⃣ IR kumanda okunur

OK tuşuna basılırsa Mod değiştirilir.

Manuel modda UP/DOWN ile servo adım hareketi yapılır.

3️⃣ Otomatik Mod

LDR değeri 5 defa okunup ortalaması alınır.

Değişim 10 birimden fazlaysa hareket yapılır.

Eşik değer:

350 altı → perde kapanır

350 üstü → perde açılır

4️⃣ Akıcı servo hareketi

Servo belirli bir süre boyunca ileri/geri döndürülür.

Süre, mevcut pozisyon ile hedef pozisyon arasındaki mesafeye göre hesaplanır.

📁 Proje Dosya Yapısı
SmartCurtain360/
│
├── SmartCurtain360.ino
├── README.md
└── lib/
    ├── IRremote
    └── Servo

▶️ Nasıl Kullanılır?

Arduino IDE → IRremote & Servo kütüphanelerini ekle.

Kodu Arduino UNO’ya yükle.

Sistemi çalıştır.

Mevcut mod seri monitörde yazacak:

“OTOMATİK”

“MANUEL”

Otomatik moddaysa → LDR ışığını değiştir, perde motorları hareket eder.

Manuel moddaysa → kumanda ile kontrol et.

🧪 Test Önerileri

LDR üzerine el feneri tutarak açılma test edin.

Odayı karartarak kapanma test edin.

Manuel modda UP/DOWN tuşlarına basarak motor adımı doğrulayın.

Seri monitörde:

LDR değeri

Mevcut açı

Hareket süresi
okunur.

⚠️ Dikkat Edilmesi Gerekenler

360° servo motorlar normal servo gibi açısal çalışmaz; PWM hız/yön ile kontrol edilir.

Harici güç kaynağı kullanmanız motor performansını artırır.

LDR’nin ortam ışığına göre konumu önemlidir.

Servo motorlar uzun süre yük altında çalışırsa ısınabilir.

📄 Lisans

Bu proje MIT lisansı ile kullanılabilir ve geliştirilebilir.

🙌 Katkı Sağlama

İsterseniz projeye:

yeni özellik ekleyebilir,

kod optimizasyonu yapabilir,

farklı sensörlerle genişletebilir
ve katkı sağlayabilirsiniz.
