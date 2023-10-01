# subscriber.py
import paho.mqtt.client as mqtt

def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    # Subscribe, which need to put into on_connect
    # If reconnect after losing the connection with the broker, it will continue to subscribe to the raspberry/topic topic
    client.subscribe("testTopic")

# The callback function, it will be triggered when receiving messages
def on_message(client, userdata, msg):
    print(f"{msg.topic} {msg.payload}")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Set the will message, when the Raspberry Pi is powered off, or the network is interrupted abnormally, it will send the will message to other clients
client.will_set('raspberry/status', b'{"status": "Off"}')

# Create connection, the three parameters are broker address, broker port number, and keep-alive time respectively
client.connect("dashboard", 1883, 60)

# Set the network loop blocking, it will not actively end the program before calling disconnect() or the program crash
client.loop_forever()

