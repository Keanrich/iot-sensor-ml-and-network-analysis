# 🌐 IoT Monitoring System using MQTT

## Deskripsi
Proyek ini merupakan sistem Internet of Things (IoT) yang memanfaatkan beberapa sensor, yaitu LDR, PIR, dan DHT, serta aktuator berupa servo dan lampu LED. Sistem secara default akan membaca data dari sensor, kemudian mengontrol lampu secara otomatis berdasarkan tingkat intensitas cahaya yang terdeteksi oleh sensor LDR.

Data sensor yang diperoleh akan ditampilkan pada Node-RED Dashboard dan disimpan ke dalam database Firebase. Data tersebut selanjutnya diolah dan dianalisis menggunakan Python (Jupyter Notebook), baik untuk machine learning maupun analisis statistika deskriptif.

Pengendalian aktuator dibagi menjadi dua mekanisme. Pertama, servo dapat dinyalakan atau dimatikan melalui button control yang tersedia pada Node-RED Dashboard. Kedua, lampu LED dapat dikendalikan secara otomatis maupun manual, tergantung pada kondisi tombol fisik pada rangkaian. Pada mode manual, lampu dapat dinyalakan atau dimatikan melalui Telegram Bot.

Selain itu, Telegram Bot juga berfungsi sebagai sistem notifikasi dengan memberikan peringatan apabila intensitas cahaya berada di bawah ambang batas tertentu. Seluruh komunikasi data dalam sistem ini menggunakan protokol MQTT, yang kemudian dilakukan analisis trafik jaringan menggunakan Wireshark untuk memahami pola komunikasi dan keamanan data.


## Teknologi yang Digunakan
- ESP8266 (PlatformIO)
- MQTT (Mosquitto)
- Node-RED
- Python (Pandas, Matplotlib)
- Wireshark

## Arsitektur Sistem
![Architecture](images/architecture.png)

## Analisis Data
- Forecasting data sensor
- Analisis statistik deskriptif

## Analisis Jaringan
Analisis trafik MQTT dilakukan menggunakan Wireshark untuk memahami pola komunikasi data.

![Wireshark](images/wireshark.png)

## Cara Menjalankan
1. Upload kode ke ESP menggunakan PlatformIO
2. Jalankan broker MQTT
3. Import flow Node-RED
4. Jalankan notebook Python
