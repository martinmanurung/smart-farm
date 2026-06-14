package main

import (
	"encoding/json"
	"log"
	"net/http"
	"os"
	"time"

	mqtt "github.com/eclipse/paho.mqtt.golang"
	"gorm.io/driver/mysql"
	"gorm.io/gorm"
)

// Models
type SensorLog struct {
	ID        int64     `json:"id" gorm:"primaryKey"`
	Moisture  int       `json:"moisture"`
	CreatedAt time.Time `json:"created_at"`
}

type PumpLog struct {
	ID        int64     `json:"id" gorm:"primaryKey"`
	Status    string    `json:"status"`
	Source    string    `json:"source"`
	CreatedAt time.Time `json:"created_at"`
}

var (
	db         *gorm.DB
	mqttClient mqtt.Client
	pumpStatus = "OFF"
)

func initDB() {
	dsn := "root:root@tcp(127.0.0.1:3306)/smartfarming?charset=utf8mb4&parseTime=True&loc=Local"
	var err error

	// Retry connection for docker readiness
	for i := 0; i < 10; i++ {
		db, err = gorm.Open(mysql.Open(dsn), &gorm.Config{})
		if err == nil {
			break
		}
		log.Println("Waiting for database...")
		time.Sleep(2 * time.Second)
	}

	if err != nil {
		log.Printf("Warning: Failed to connect to database: %v. Running without DB.", err)
		return
	}

	// Auto migrate is handled by schema.sql, but let's make sure
	db.AutoMigrate(&SensorLog{}, &PumpLog{})
	log.Println("Database connected!")
}

func initMQTT() {
	opts := mqtt.NewClientOptions()
	opts.AddBroker("tcp://localhost:1883") // using local mosquitto container
	opts.SetClientID("smartfarm-backend")

	opts.OnConnect = func(c mqtt.Client) {
		log.Println("Connected to MQTT broker")

		// Subscribe to Soil Moisture
		c.Subscribe("smartfarm/soil", 0, func(client mqtt.Client, msg mqtt.Message) {
			var data struct {
				Moisture int `json:"moisture"`
			}
			if err := json.Unmarshal(msg.Payload(), &data); err == nil {
				log.Printf("Received moisture: %d\n", data.Moisture)
				if db != nil {
					db.Create(&SensorLog{
						Moisture:  data.Moisture,
						CreatedAt: time.Now(),
					})
				}
			}
		})

		// Subscribe to Pump Status
		c.Subscribe("smartfarm/pump/status", 0, func(client mqtt.Client, msg mqtt.Message) {
			var data struct {
				Status string `json:"status"`
			}
			if err := json.Unmarshal(msg.Payload(), &data); err == nil {
				pumpStatus = data.Status
				log.Printf("Pump status updated: %s\n", data.Status)
				if db != nil {
					db.Create(&PumpLog{
						Status:    data.Status,
						Source:    "AUTO/MANUAL", // We'll just log it
						CreatedAt: time.Now(),
					})
				}
			}
		})
	}

	mqttClient = mqtt.NewClient(opts)
	// Retry connection for mosquitto
	for i := 0; i < 5; i++ {
		if token := mqttClient.Connect(); token.Wait() && token.Error() == nil {
			return
		}
		log.Println("Waiting for MQTT broker...")
		time.Sleep(2 * time.Second)
	}
	log.Println("Failed to connect to MQTT broker, it might be down.")
}

// CORS Middleware
func enableCORS(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Access-Control-Allow-Origin", "*")
		w.Header().Set("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
		w.Header().Set("Access-Control-Allow-Headers", "Content-Type")

		if r.Method == "OPTIONS" {
			w.WriteHeader(http.StatusOK)
			return
		}

		next.ServeHTTP(w, r)
	})
}

// Handlers
func getLatestSensor(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "application/json")
	if db == nil {
		json.NewEncoder(w).Encode(map[string]interface{}{"moisture": 0})
		return
	}
	var logData SensorLog
	db.Order("created_at desc").First(&logData)
	json.NewEncoder(w).Encode(logData)
}

func getSensorHistory(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "application/json")
	if db == nil {
		json.NewEncoder(w).Encode([]SensorLog{})
		return
	}
	var history []SensorLog
	db.Order("created_at desc").Limit(50).Find(&history)
	json.NewEncoder(w).Encode(history)
}

func getPumpStatus(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(map[string]interface{}{"status": pumpStatus})
}

func pumpOn(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Error(w, "Method Not Allowed", http.StatusMethodNotAllowed)
		return
	}
	payload := `{"command": "ON"}`
	if mqttClient.IsConnected() {
		mqttClient.Publish("smartfarm/pump/control", 0, false, payload)
	}
	if db != nil {
		db.Create(&PumpLog{Status: "ON", Source: "MANUAL", CreatedAt: time.Now()})
	}
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(map[string]interface{}{"message": "Pump turn on command sent"})
}

func pumpOff(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Error(w, "Method Not Allowed", http.StatusMethodNotAllowed)
		return
	}
	payload := `{"command": "OFF"}`
	if mqttClient.IsConnected() {
		mqttClient.Publish("smartfarm/pump/control", 0, false, payload)
	}
	if db != nil {
		db.Create(&PumpLog{Status: "OFF", Source: "MANUAL", CreatedAt: time.Now()})
	}
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(map[string]interface{}{"message": "Pump turn off command sent"})
}

func main() {
	initDB()
	initMQTT()

	mux := http.NewServeMux()

	mux.HandleFunc("/api/sensor/latest", getLatestSensor)
	mux.HandleFunc("/api/sensor/history", getSensorHistory)
	mux.HandleFunc("/api/pump/status", getPumpStatus)
	mux.HandleFunc("/api/pump/on", pumpOn)
	mux.HandleFunc("/api/pump/off", pumpOff)

	port := os.Getenv("PORT")
	if port == "" {
		port = "8080"
	}

	log.Printf("Server starting on :%s", port)
	handler := enableCORS(mux)
	if err := http.ListenAndServe(":"+port, handler); err != nil {
		log.Fatalf("Server failed to start: %v", err)
	}
}
