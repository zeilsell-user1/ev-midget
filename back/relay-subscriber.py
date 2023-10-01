# subscriber.py
import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO

# define the MQTT broker to use

broker = "192.168.2.2"
myport = 1883

# define the GPIO pins

side = 11
brake = 12
leftind = 13
rightind = 15
fog = 16
reverse = 18
spare1 = 22
spare2 = 7

def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    # Subscribe, which need to put into on_connect
    # If reconnect after losing the connection with the broker, it will continue to subscribe to the raspberry/topic topic
    client.subscribe("topic/lights/rear/brake/on")
    client.subscribe("topic/lights/rear/brake/off")
    client.subscribe("topic/lights/rear/fog/on")
    client.subscribe("topic/lights/rear/fog/off")
    client.subscribe("topic/lights/rear/reverse/on")
    client.subscribe("topic/lights/rear/reverse/off")
    client.subscribe("topic/lights/rear/side/on")
    client.subscribe("topic/lights/rear/side/off")
    client.subscribe("topic/lights/rear/leftind/on")
    client.subscribe("topic/lights/rear/leftind/off")
    client.subscribe("topic/lights/rear/rightind/on")
    client.subscribe("topic/lights/rear/rightind/off")

#
# The callback function, it will be triggered when receiving messages
# GPIO.HIGH is light on
#

def on_message(client, userdata, msg):
    print(f"{msg.topic} {msg.payload}")
    if msg.topic == 'topic/lights/rear/reverse/on':
        GPIO.output(reverse, GPIO.LOW)
        print('turn reverse light on')
    elif msg.topic == 'topic/lights/rear/reverse/off':
        GPIO.output(reverse, GPIO.HIGH)
        print('turn reverse light off')
    elif msg.topic == 'topic/lights/rear/fog/on':
        GPIO.output(fog, GPIO.LOW)
        print('turn rear fog light on')
    elif msg.topic == 'topic/lights/rear/fog/off':
        GPIO.output(fog, GPIO.HIGH)
        print('turn rear fog lights off')
    elif msg.topic == 'topic/lights/rear/brake/on':
        GPIO.output(brake, GPIO.LOW)
        print('turn brake light dipped on')
    elif msg.topic == 'topic/lights/rear/brake/off':
        GPIO.output(brake, GPIO.HIGH)
        print('turn brake lights dipped off')
    elif msg.topic == 'topic/lights/rear/side/on':
        GPIO.output(side, GPIO.LOW)
        print('turn rear side light on')
    elif msg.topic == 'topic/lights/rear/side/off':
        GPIO.output(side, GPIO.HIGH)
        print('turn rear side lights off')
    elif msg.topic == 'topic/lights/rear/leftind/on':
        GPIO.output(leftind, GPIO.LOW)
        print('turn rear left indicator on')
    elif msg.topic == 'topic/lights/rear/leftind/off':
        GPIO.output(leftind, GPIO.HIGH)
        print('turn rear left indicator off')
    elif msg.topic == 'topic/lights/rear/rightind/on':
        GPIO.output(rightind, GPIO.LOW)
        print('turn rear right indicator on')
    elif msg.topic == 'topic/lights/rear/rightind/off':
        GPIO.output(rightind, GPIO.HIGH)
        print('turn rear right indicator off')
    else:
        print('unrecognised event')


GPIO.setwarnings(False)
GPIO.setmode(GPIO.BOARD)  # use the pin numbering over broadcom
GPIO.setup(side, GPIO.OUT)
GPIO.output(side, GPIO.HIGH)
GPIO.setup(brake, GPIO.OUT)
GPIO.output(brake, GPIO.HIGH)
GPIO.setup(leftind, GPIO.OUT)
GPIO.output(leftind, GPIO.HIGH)
GPIO.setup(rightind, GPIO.OUT)
GPIO.output(rightind, GPIO.HIGH)
GPIO.setup(fog, GPIO.OUT)
GPIO.output(fog, GPIO.HIGH)
GPIO.setup(reverse, GPIO.OUT)
GPIO.output(reverse, GPIO.HIGH)
GPIO.setup(spare1, GPIO.OUT)
GPIO.output(spare1, GPIO.HIGH)
GPIO.setup(spare2, GPIO.OUT)
GPIO.output(spare2, GPIO.HIGH)

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Set the will message, when the Raspberry Pi is powered off, or the network is interrupted abnormally, it will send the will message to other clients
client.will_set('raspberry/status', b'{"status": "Off"}')

# Create connection, the three parameters are broker address, broker port number, and keep-alive time respectively
client.connect(broker, myport, 60)

# Set the network loop blocking, it will not actively end the program before calling disconnect() or the program crash
client.loop_forever()

try:
    client.loop_forever()

except KeyboardInterrupt:
    print ("programme interupted")

except:
    print ("some other interrpt")

finally:
    GPIO.cleanup()

