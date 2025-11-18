import paho.mqtt.client as mqtt

# MQTT_HOST = "localhost"
MQTT_HOST = "wilson.local"

client = mqtt.Client()
client.connect(MQTT_HOST, 1883, 60)
client.publish("test/usikker", "Hello annon uden TLS, fra ekstern klient!")