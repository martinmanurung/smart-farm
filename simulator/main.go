package main

import (
	"encoding/json"
	"fmt"
	"log"
	"math/rand"
	"os"
	"os/signal"
	"sync"
	"syscall"
	"time"

	mqtt "github.com/eclipse/paho.mqtt.golang"
)

var (
	pumpStatus = "OFF"
	moisture   = 60 // start at 60%
	threshold  = 40
	mu         sync.Mutex
)

func main() {
	opts := mqtt.NewClientOptions()
	opts.AddBroker("tcp://localhost:1883")
	opts.SetClientID("esp32-simulator")

	opts.OnConnect = func(c mqtt.Client) {
		log.Println("[SIMULATOR] Connected to MQTT broker")
		
		// Subscribe to Pump Control
		c.Subscribe("smartfarm/pump/control", 0, func(client mqtt.Client, msg mqtt.Message) {
			var cmd struct {
				Command string `json:"command"`
			}
			if err := json.Unmarshal(msg.Payload(), &cmd); err == nil {
				mu.Lock()
				pumpStatus = cmd.Command
				mu.Unlock()
				log.Printf("[SIMULATOR] Pump control received: %s", cmd.Command)
				
				// Publish pump status back
				payload := fmt.Sprintf(`{"status":"%s"}`, cmd.Command)
				client.Publish("smartfarm/pump/status", 0, false, payload)
			}
		})
	}

	client := mqtt.NewClient(opts)
	for i := 0; i < 5; i++ {
		if token := client.Connect(); token.Wait() && token.Error() == nil {
			break
		}
		log.Println("[SIMULATOR] Waiting for MQTT broker...")
		time.Sleep(2 * time.Second)
	}

	if !client.IsConnected() {
		log.Fatal("[SIMULATOR] Failed to connect to MQTT.")
	}

	// Simulation loop
	go func() {
		for {
			mu.Lock()
			// Simulate soil drying over time
			if pumpStatus == "OFF" {
				if moisture > 10 {
					moisture -= rand.Intn(3) // Decrement by 0 to 2
				}
			} else {
				// Pump is ON, moisture goes up quickly
				if moisture < 100 {
					moisture += rand.Intn(10) + 5
				}
			}

			// Auto mode logic
			if moisture < threshold && pumpStatus == "OFF" {
				log.Println("[SIMULATOR] Moisture below threshold. Auto-starting pump.")
				pumpStatus = "ON"
				client.Publish("smartfarm/pump/status", 0, false, `{"status":"ON"}`)
			} else if moisture > 80 && pumpStatus == "ON" {
				log.Println("[SIMULATOR] Soil is wet enough. Auto-stopping pump.")
				pumpStatus = "OFF"
				client.Publish("smartfarm/pump/status", 0, false, `{"status":"OFF"}`)
			}
			
			currentMoisture := moisture
			mu.Unlock()

			payload := fmt.Sprintf(`{"moisture": %d}`, currentMoisture)
			client.Publish("smartfarm/soil", 0, false, payload)
			
			time.Sleep(2 * time.Second)
		}
	}()

	// Wait for interrupt signal to gracefully shutdown
	quit := make(chan os.Signal, 1)
	signal.Notify(quit, syscall.SIGINT, syscall.SIGTERM)
	<-quit
	log.Println("[SIMULATOR] Shutting down...")
	client.Disconnect(250)
}
