# subscriber.py
import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO

# define the MQTT broker to use

broker = "192.168.2.2"
myport = 1883

# define the GPIO pins

side = 11
headdip = 12
leftind = 13
rightind = 15
fog = 16
spot = 18
headfull = 22
spare = 7

def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    # Subscribe, which need to put into on_connect
    # If reconnect after losing the connection with the broker, it will continue to subscribe to the raspberry/topic topic
    client.subscribe("topic/lights/front/spot/on")
    client.subscribe("topic/lights/front/spot/off")
    client.subscribe("topic/lights/front/fog/on")
    client.subscribe("topic/lights/front/fog/off")
    client.subscribe("topic/lights/front/headfull/on")
    client.subscribe("topic/lights/front/headfull/off")
    client.subscribe("topic/lights/front/headdip/on")
    client.subscribe("topic/lights/front/headdip/off")
    client.subscribe("topic/lights/front/leftind/on")
    client.subscribe("topic/lights/front/leftind/off")
    client.subscribe("topic/lights/front/rightind/on")
    client.subscribe("topic/lights/front/rightind/off")
    client.subscribe("topic/lights/front/side/on")
    client.subscribe("topic/lights/front/side/off")

#
# The callback function, it will be triggered when receiving messages
# GPIO.HIGH is light on
#

def on_message(client, userdata, msg):
    print(f"{msg.topic} {msg.payload}")
    if msg.topic == 'topic/lights/front/spot/on':
        GPIO.output(spot, GPIO.LOW)
        print('turn spot light on')
    elif msg.topic == 'topic/lights/front/spot/off':
        GPIO.output(spot, GPIO.HIGH)
        print('turn spot light off')
    elif msg.topic == 'topic/lights/front/fog/on':
        GPIO.output(fog, GPIO.LOW)
        print('turn front fog light on')
    elif msg.topic == 'topic/lights/front/fog/off':
        GPIO.output(fog, GPIO.HIGH)
        print('turn front fog lights off')
    elif msg.topic == 'topic/lights/front/headdip/on':
        GPIO.output(headdip, GPIO.LOW)
        print('turn head light dipped on')
    elif msg.topic == 'topic/lights/front/headdip/off':
        GPIO.output(headdip, GPIO.HIGH)
        print('turn head lights dipped off')
    elif msg.topic == 'topic/lights/front/headfull/on':
        GPIO.output(headfull, GPIO.LOW)
        print('turn head light full on')
    elif msg.topic == 'topic/lights/front/headfull/off':
        GPIO.output(headfull, GPIO.HIGH)
        print('turn head lights full off')
    elif msg.topic == 'topic/lights/front/side/on':
        GPIO.output(side, GPIO.LOW)
        print('turn front side light on')
    elif msg.topic == 'topic/lights/front/side/off':
        GPIO.output(side, GPIO.HIGH)
        print('turn front side lights off')
    elif msg.topic == 'topic/lights/front/leftind/on':
        GPIO.output(leftind, GPIO.LOW)
        print('turn front left indicator on')
    elif msg.topic == 'topic/lights/front/leftind/off':
        GPIO.output(leftind, GPIO.HIGH)
        print('turn front left indicator off')
    elif msg.topic == 'topic/lights/front/rightind/on':
        GPIO.output(rightind, GPIO.LOW)
        print('turn front right indicator on')
    elif msg.topic == 'topic/lights/front/rightind/off':
        GPIO.output(rightind, GPIO.HIGH)
        print('turn front right indicator off')
    else:
        print('unrecognised event')


GPIO.setwarnings(False)
GPIO.setmode(GPIO.BOARD)  # use the pin numbering over broadcom
GPIO.setup(side, GPIO.OUT)
GPIO.output(side, GPIO.HIGH)
GPIO.setup(headdip, GPIO.OUT)
GPIO.output(headdip, GPIO.HIGH)
GPIO.setup(leftind, GPIO.OUT)
GPIO.output(leftind, GPIO.HIGH)
GPIO.setup(rightind, GPIO.OUT)
GPIO.output(rightind, GPIO.HIGH)
GPIO.setup(fog, GPIO.OUT)
GPIO.output(fog, GPIO.HIGH)
GPIO.setup(spot, GPIO.OUT)
GPIO.output(spot, GPIO.HIGH)
GPIO.setup(headfull, GPIO.OUT)
GPIO.output(headfull, GPIO.HIGH)
GPIO.setup(spare, GPIO.OUT)
GPIO.output(spare, GPIO.HIGH)

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

