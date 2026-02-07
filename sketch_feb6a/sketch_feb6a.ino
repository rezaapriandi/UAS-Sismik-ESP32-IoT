#include <WiFi.h>
#include <PubSubClient.h>
#include <Preferences.h>

// --- BAGIAN INI WAJIB DIGANTI ---
const char* ssid ="KENZIO";        // Ganti dengan nama WiFi (Huruf besar/kecil berpengaruh)
const char* password = "27112021"; // Ganti Password WiFi
const char* mqtt_server = "broker.hivemq.com"; 

// Topik MQTT
const char* topic_status = "reza/proyek/status";
const char* topic_control = "reza/proyek/control";

// --- PIN MAPPING ---
#define LED_PIN 2       // <--- Ganti jadi 2 (Ini alamat lampu biru bawaan)
#define BUTTON_PIN 14   // Tetap 14 (Tombol)

// Variabel Global
volatile int brightness = 0;
volatile bool updateFlag = false;
volatile unsigned long lastDebounceTime = 0;

// Objek Library
WiFiClient espClient;
PubSubClient client(espClient);
Preferences preferences;

// --- 1. MANAJEMEN INTERUPSI (TOMBOL) ---
void IRAM_ATTR onButtonPress() {
  unsigned long currentTime = millis();
  // Debounce (Mencegah tombol terpencet 2x cepat)
  if ((currentTime - lastDebounceTime) > 200) {
    brightness += 51; // Naik 20%
    if (brightness > 255) brightness = 0; // Reset ke 0
    updateFlag = true;
    lastDebounceTime = currentTime;
  }
}

// --- 2. MANAJEMEN MEMORI (SIMPAN DATA) ---
void saveLastState() {
  preferences.begin("proyek-reza", false);
  preferences.putInt("level", brightness);
  preferences.end();
}

void loadLastState() {
  preferences.begin("proyek-reza", true);
  brightness = preferences.getInt("level", 0);
  preferences.end();
}

// --- FUNGSI MQTT ---
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Perintah MQTT Masuk: ");
  Serial.println(message);
  brightness = message.toInt();
  updateFlag = true;
}

// --- 3. MANAJEMEN TUGAS (FREERTOS) ---

// Task Hardware: Urus Lampu & Tombol (Prioritas Tinggi)
void TaskHardware(void *pvParameters) {
  while(1) {
    if (updateFlag) {
      ledcWrite(0, brightness); // Ubah terang lampu
      Serial.printf("Lampu Update ke Level: %d\n", brightness);
      
      // Simpan memori (Opsional, matikan jika bikin crash)
      // saveLastState(); 
      
      updateFlag = false;
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

// Task Network: Urus WiFi & MQTT (Prioritas Rendah)
void TaskNetwork(void *pvParameters) {
  while(1) {
    // Hanya kerja kalau WiFi connect
    if (WiFi.status() == WL_CONNECTED) {
      if (!client.connected()) {
        // Coba connect MQTT (Non-blocking / Sekali coba)
        String clientId = "Reza-ESP32-" + String(random(0xffff), HEX);
        if (client.connect(clientId.c_str())) {
          Serial.println("MQTT Terhubung!");
          client.subscribe(topic_control);
        }
      } else {
        client.loop(); // Wajib jalan
        
        // Kirim laporann status tiap 3 detik
        static unsigned long lastMsg = 0;
        if (millis() - lastMsg > 3000) {
          char msg[10];
          itoa(brightness, msg, 10);
          client.publish(topic_status, msg);
          lastMsg = millis();
        }
      }
    }
    vTaskDelay(500 / portTICK_PERIOD_MS); // Istirahat panjang biar gak panas
  }
}

void setup() {
  Serial.begin(115200);

  // Setup PWM LED
  ledcSetup(0, 5000, 8);
  ledcAttachPin(LED_PIN, 0);

  // Setup Tombol
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), onButtonPress, FALLING);

  // Cek nyala awal (Test LED saat start)
  ledcWrite(0, 255); delay(500); // Nyala terang
  ledcWrite(0, 0);   delay(500); // Mati
  Serial.println("Sistem Siap!");

  // Koneksi WiFi dengan TIMEOUT (Agar tidak hang selamanya)
  Serial.print("Menghubungkan WiFi");
  WiFi.begin(ssid, password);
  
  int tryLimit = 0;
  while (WiFi.status() != WL_CONNECTED && tryLimit < 20) { // Max tunggu 10 detik
    delay(500);
    Serial.print(".");
    tryLimit++;
  }

  if(WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Sukses!");
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
  } else {
    Serial.println("\nWiFi Gagal/Salah Password. Masuk Mode Offline.");
  }

  // Jalankan Multi-tasking
  xTaskCreatePinnedToCore(TaskNetwork, "NetTask", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(TaskHardware, "HwTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL); // Kosongkan loop utama
}