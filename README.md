# Sistem Monitoring dan Kontrol Perangkat Berbasis ESP32 (IoT)
**Tugas Besar Sistem Mikroprosessor - TIF K 23A**

* **Nama:** Reza Apriandi
* **NIM:** 23552011216
* **Kelas:** TIF K 23A
* **Dosen:** Muhammad Ikhwan Fathulloh, S.Kom.
* **Kampus:** Universitas Teknologi Bandung (UTB)

---

## 🎥 Video Demo Sistem
Saksikan demonstrasi alat yang bekerja secara real-time (Hardware, MQTT, & FreeRTOS) di sini:

👉 **[TONTON VIDEO DEMO DI YOUTUBE](https://youtu.be/t_PRQEwPNks)**

*(Klik link di atas untuk melihat bagaimana alat dikontrol lewat internet)*

---

## 📄 Deskripsi Proyek
Proyek ini adalah implementasi sistem embedded berbasis **ESP32 (Wemos D1 R32)** yang menerapkan konsep **Multitasking** dan **Internet of Things (IoT)**. Sistem dirancang untuk mengontrol intensitas cahaya (PWM) secara lokal maupun jarak jauh tanpa blocking process.

### 🚀 Fitur Unggulan:
1.  **Multitasking (FreeRTOS):** Memisahkan proses kontrol hardware (Core 1) dan koneksi jaringan WiFi/MQTT (Core 0) agar sistem responsif.
2.  **Hardware Interrupt:** Input tombol menggunakan interupsi eksternal (Active Low) untuk efisiensi CPU (Tanpa Polling).
3.  **IoT Control (MQTT):** Komunikasi dua arah (Publish/Subscribe) dengan Broker **HiveMQ** untuk monitoring status dan kontrol lampu via Laptop/HP.
4.  **Non-Volatile Storage (NVS):** Menyimpan level kecerahan terakhir ke memori Flash (Preferences), aman saat mati listrik.
5.  **Robustness:** Fitur *Offline Mode* memastikan alat tetap bisa dipakai manual meski WiFi terputus.

---

## 🛠️ Spesifikasi Teknis

| Komponen | Keterangan |
| :--- | :--- |
| **Mikrokontroller** | ESP32 (Wemos D1 R32) |
| **Input** | Push Button (GPIO 14 - PullUp) |
| **Output** | Built-in LED (GPIO 2 - PWM Channel 0) |
| **Protokol** | MQTT (Port 1883) & TCP/IP |
| **Software** | Arduino IDE (C++) |

---

## 📂 Cara Menjalankan Project
1.  Download repository ini.
2.  Buka file `.ino` menggunakan Arduino IDE.
3.  Install library yang dibutuhkan: `PubSubClient` dan `WiFi`.
4.  Sesuaikan `SSID` dan `PASSWORD` WiFi pada kodingan.
5.  Upload ke board ESP32.
