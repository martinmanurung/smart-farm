# Smart Farming IoT System 🌱

Sistem Smart Farming berbasis Internet of Things (IoT) untuk memonitor kelembaban tanah dan melakukan penyiraman tanaman secara otomatis maupun manual melalui dashboard web.

---

# Daftar Isi

- [Latar Belakang](#latar-belakang)
- [Tujuan](#tujuan)
- [Fitur](#fitur)
- [Arsitektur Sistem](#arsitektur-sistem)
- [Komponen Hardware](#komponen-hardware)
- [Komponen Software](#komponen-software)
- [Flow Sistem](#flow-sistem)
- [Topik MQTT](#topik-mqtt)
- [Struktur Project](#struktur-project)
- [Database Design](#database-design)
- [REST API](#rest-api)
- [Dashboard](#dashboard)
- [State Diagram](#state-diagram)
- [Sequence Diagram](#sequence-diagram)
- [Future Improvements](#future-improvements)
- [Teknologi yang Digunakan](#teknologi-yang-digunakan)

---

# Latar Belakang

Penyiraman tanaman secara manual memiliki beberapa kelemahan, antara lain:

- Membutuhkan intervensi manusia secara terus-menerus.
- Sulit mengetahui kondisi kelembaban tanah secara real-time.
- Risiko tanaman mengalami kekurangan atau kelebihan air.
- Tidak terdapat riwayat data kelembaban tanah.

Dengan memanfaatkan teknologi Internet of Things (IoT), sistem ini memungkinkan pemantauan kondisi tanah dan pengendalian pompa air secara otomatis maupun melalui dashboard berbasis web.

---

# Tujuan

- Mengukur kelembaban tanah secara real-time.
- Menyalakan pompa air secara otomatis ketika tanah terlalu kering.
- Mengendalikan pompa air secara manual melalui web dashboard.
- Menyimpan histori data sensor ke database.
- Menampilkan status pompa dan grafik kelembaban tanah.

---

# Fitur

## Monitoring

- Monitoring kelembaban tanah secara real-time.
- Menampilkan status pompa.
- Menampilkan status koneksi ESP32.
- Menampilkan nilai threshold kelembaban.

## Control

- Menyalakan pompa secara manual.
- Mematikan pompa secara manual.
- Mode AUTO.
- Mode MANUAL.

## Analytics

- Histori data sensor.
- Histori penyiraman.
- Grafik kelembaban tanah.

---

# Arsitektur Sistem

```text
                     +------------------+
                     | Soil Moisture    |
                     | Sensor           |
                     +---------+--------+
                               |
                               v
                          +---------+
                          | ESP32   |
                          +---------+
                          |         |
              Publish MQTT|         |Subscribe MQTT
                          |         |
                          v         ^
                  +----------------------+
                  | MQTT Broker           |
                  | (Mosquitto)           |
                  +----------------------+
                               |
                               v
                     +------------------+
                     | Backend API       |
                     | Spring Boot / Go  |
                     +------------------+
                        |             |
                        |             |
                        v             v
                +-------------+  +-------------+
                | MySQL       |  | WebSocket   |
                +-------------+  +-------------+
                                      |
                                      v
                               +--------------+
                               | React UI     |
                               +--------------+
```

---

# Komponen Hardware

| Komponen | Fungsi |
|----------|--------|
| ESP32 | Mikrokontroler utama |
| Soil Moisture Sensor | Mengukur kelembaban tanah |
| Relay Module 1 Channel | Mengontrol pompa air |
| Mini Water Pump Aquarium | Menyiram tanaman |
| Selang Air | Menyalurkan air |
| Breadboard / PCB | Perakitan rangkaian |
| Power Supply | Sumber daya sistem |
| Kabel Jumper | Koneksi antar komponen |

---

# Komponen Software

## Embedded Layer

- Arduino IDE
- ESP32 Board Package
- WiFi Library
- PubSubClient (MQTT)

## Communication Layer

- Mosquitto MQTT Broker

## Backend Layer

- Spring Boot / Golang
- Eclipse Paho MQTT
- JPA / GORM
- WebSocket

## Database

- MySQL

## Frontend

- React
- TypeScript
- TailwindCSS
- Chart.js

---

# Flow Sistem

## Monitoring Sensor

```text
Soil Moisture Sensor
        ↓
ESP32
        ↓
MQTT Publish
        ↓
MQTT Broker
        ↓
Backend Subscriber
        ↓
MySQL
        ↓
Dashboard
```

## Kontrol Pompa

```text
Dashboard
      ↓
REST API
      ↓
Backend
      ↓
MQTT Publish
      ↓
MQTT Broker
      ↓
ESP32
      ↓
Relay
      ↓
Water Pump
```

---

# Topik MQTT

## Sensor Kelembaban

### Topic

```text
smartfarm/soil
```

### Payload

```json
{
  "moisture": 745
}
```

---

## Status Pompa

### Topic

```text
smartfarm/pump/status
```

### Payload

```json
{
  "status": "ON"
}
```

---

## Kontrol Pompa

### Topic

```text
smartfarm/pump/control
```

### Menyalakan Pompa

```json
{
  "command": "ON"
}
```

### Mematikan Pompa

```json
{
  "command": "OFF"
}
```

---

# Struktur Project

```text
smart-farming/

├── firmware/
│
│   └── esp32/
│       ├── main.ino
│       ├── wifi_config.h
│       ├── mqtt_config.h
│       └── relay_controller.h
│
├── backend/
│   ├── controller/
│   ├── service/
│   ├── repository/
│   ├── entity/
│   ├── mqtt/
│   ├── websocket/
│   ├── config/
│   └── main.go
│
├── frontend/
│   ├── src/
│   │   ├── components/
│   │   ├── pages/
│   │   ├── hooks/
│   │   ├── services/
│   │   ├── charts/
│   │   └── App.tsx
│
├── database/
│   └── schema.sql
│
└── README.md
```

---

# Database Design

## Table `sensor_logs`

| Field | Type |
|--------|------|
| id | bigint |
| moisture | int |
| created_at | datetime |

---

## Table `pump_logs`

| Field | Type |
|--------|------|
| id | bigint |
| status | varchar(10) |
| source | varchar(20) |
| created_at | datetime |

### Source

- AUTO
- MANUAL

---

# REST API

## Menyalakan Pompa

### Request

```http
POST /api/pump/on
```

### Response

```json
{
  "message": "Pump turned on"
}
```

---

## Mematikan Pompa

### Request

```http
POST /api/pump/off
```

### Response

```json
{
  "message": "Pump turned off"
}
```

---

## Mendapatkan Status Pompa

### Request

```http
GET /api/pump/status
```

### Response

```json
{
  "status": "ON"
}
```

---

## Mendapatkan Sensor Terbaru

### Request

```http
GET /api/sensor/latest
```

### Response

```json
{
  "moisture": 645
}
```

---

## Mendapatkan Histori Sensor

### Request

```http
GET /api/sensor/history
```

### Response

```json
[
  {
    "moisture": 720,
    "created_at": "2026-06-10T08:00:00"
  },
  {
    "moisture": 680,
    "created_at": "2026-06-10T08:10:00"
  }
]
```

---

# Dashboard

Dashboard menyediakan:

## Monitoring

- Nilai kelembaban tanah.
- Persentase kelembaban.
- Status pompa.
- Status koneksi ESP32.
- Threshold kelembaban.

## Control

- Tombol ON pompa.
- Tombol OFF pompa.
- Mode AUTO.
- Mode MANUAL.

## Analytics

- Grafik kelembaban tanah.
- Histori sensor.
- Histori penyiraman.

---

# State Diagram

```text
                +-----------+
                | Pump OFF  |
                +-----------+
                      |
       Moisture < Threshold
                      |
                      v
                +-----------+
                | Pump ON   |
                +-----------+
                      |
          Moisture >= Threshold
                      |
                      v
                +-----------+
                | Pump OFF  |
                +-----------+
```

---

# Sequence Diagram

## Penyiraman Manual

```text
User
 |
 | Click ON
 v
Frontend
 |
 | POST /api/pump/on
 v
Backend
 |
 | Publish MQTT
 v
Mosquitto
 |
 | smartfarm/pump/control
 v
ESP32
 |
 | Relay ON
 v
Pump
```

---

## Monitoring Sensor

```text
Soil Sensor
    |
    v
ESP32
    |
    | Publish MQTT
    v
Mosquitto
    |
    v
Backend
    |
    v
MySQL
    |
    v
Frontend Dashboard
```

---

# Future Improvements

## Notification

- Telegram Bot Notification
- WhatsApp Notification
- Email Notification

## Additional Sensors

- DHT22 (Temperature & Humidity)
- Light Sensor
- Rain Sensor
- pH Sensor

## Real-time Communication

- WebSocket
- Server Sent Events (SSE)

## Cloud Deployment

### MQTT Broker

- EMQX
- HiveMQ

### Backend

- Spring Boot
- Golang

### Frontend

- React + Nginx

### Database

- MySQL
- PostgreSQL

---

# Teknologi yang Digunakan

| Layer | Teknologi |
|---------|----------|
| Embedded | ESP32 |
| Sensor | Soil Moisture Sensor |
| Protocol | MQTT |
| Broker | Mosquitto |
| Backend | Spring Boot / Golang |
| Database | MySQL |
| Frontend | React |
| Styling | TailwindCSS |
| Charts | Chart.js |

---

# Author

Smart Farming IoT Project

> Sistem monitoring dan pengendalian penyiraman tanaman berbasis Internet of Things menggunakan ESP32, MQTT, REST API, dan Web Dashboard.
