import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO
import threading

# define the MQTT broker to use

#broker = "dashboard.hhw470d.local"
broker = "192.168.2.2"
myport = 1883

# pin allocation 
# this is driving the 16 channel analogue multiplexer.

en = 11 # 0 for enable, 1 for disable
s0 = 12 # msb
s1 = 13
s2 = 15 
s3 = 16 # lsb
sg = 18 # the value of the switch

# set up the mulitplexer bit table

multiplexer = [
    [GPIO.LOW,GPIO.LOW,GPIO.LOW,GPIO.LOW],     # sidelights front and back
    [GPIO.LOW,GPIO.LOW,GPIO.LOW,GPIO.HIGH],    # headlights dipped 
    [GPIO.LOW,GPIO.LOW,GPIO.HIGH,GPIO.LOW],    # headlights full
    [GPIO.LOW,GPIO.LOW,GPIO.HIGH,GPIO.HIGH],   # left indicator front and back
    [GPIO.LOW,GPIO.HIGH,GPIO.LOW,GPIO.LOW],    # right indicator front and back
    [GPIO.LOW,GPIO.HIGH,GPIO.LOW,GPIO.HIGH],   # hazard indicators front and back
    [GPIO.LOW,GPIO.HIGH,GPIO.HIGH,GPIO.LOW],   # brake lights
    [GPIO.LOW,GPIO.HIGH,GPIO.HIGH,GPIO.HIGH],  # front fog light
    [GPIO.HIGH,GPIO.LOW,GPIO.LOW,GPIO.LOW],    # rear fog light
    [GPIO.HIGH,GPIO.LOW,GPIO.LOW,GPIO.HIGH],   # front spot light
    [GPIO.HIGH,GPIO.LOW,GPIO.HIGH,GPIO.LOW],   # reversing light
    [GPIO.HIGH,GPIO.LOW,GPIO.HIGH,GPIO.HIGH],  # flasher (instantaneous full and spot)
    [GPIO.HIGH,GPIO.HIGH,GPIO.LOW,GPIO.LOW],   # windscreen washers
    [GPIO.HIGH,GPIO.HIGH,GPIO.LOW,GPIO.HIGH],  # windscreen wipers
    [GPIO.HIGH,GPIO.HIGH,GPIO.HIGH,GPIO.LOW],  # blower
    [GPIO.HIGH,GPIO.HIGH,GPIO.HIGH,GPIO.HIGH], # spare
]

# state variable for the switches

state = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]

# the set of events that can be published

events = [
    'topic/lights/side',
    'topic/lights/head-dipped',
    'topic/lights/head-full',
    'topic/lights/flasher',
    'topic/light/left-indicator',
    'topic/lights/right-indicator',
    'topic/lights/hazards',
    'topic/lights/brakes',
    'topic/lights/front-fog',
    'topic/lights/rear-fog',
    'topic/lights/spot',
    'topic/lights/reversing',
    'topic/windscreen/washer',
    'topic/windscreen/wiper',
    'topic/heating/blower',
    'topic/spare'
]

#
# callback for a publishing a switch state change
#
def publish_event(channel, value):
        client.publish(events[channel], payload=value, qos=0, retain=False)
        print('--> ', events[channel], " ", value)

#
# callback for the connect to the MQTT broker
#
def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")

#
# The MQTT on_message callback function, it will be triggered when receiving messages
# GPIO.HIGH is light on
#
def on_message(client, userdata, msg):
    print(f"{msg.topic} {msg.payload}")

#
# the callback at the end of the timer that implements the switch check
#
def on_switch_check_expire():
    channel = 0
    while channel < 16:
        GPIO.output(en, GPIO.HIGH)
        GPIO.output(s0, multiplexer[channel][3])
        GPIO.output(s1, multiplexer[channel][2])
        GPIO.output(s2, multiplexer[channel][1])
        GPIO.output(s3, multiplexer[channel][0])
        GPIO.output(en, GPIO.LOW)
        value = GPIO.input(sg)

        if state[channel] != value:
            publish_event(channel, value)
            state[channel] = value

        channel += 1 
    timer = threading.Timer(0.5, on_switch_check_expire)
    timer.start() 

# connect to the broker

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Set the will message, when the Raspberry Pi is powered off, or 
# the network is interrupted abnormally, it will send the will message t
# o other clients
client.will_set('raspberry/front/status', b'{"status": "Off"}')

# Create connection, the three parameters are broker address, 
# broker port number, and keep-alive time respectively
client.connect(broker, myport, 60)

# set up the pins to drive the multiplexer

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BOARD)
GPIO.setup(en, GPIO.OUT)
GPIO.setup(s0, GPIO.OUT)
GPIO.setup(s1, GPIO.OUT)
GPIO.setup(s2, GPIO.OUT)
GPIO.setup(s3, GPIO.OUT)
GPIO.setup(sg, GPIO.IN,  pull_up_down=GPIO.PUD_DOWN)
GPIO.remove_event_detect(en)
GPIO.remove_event_detect(s0)
GPIO.remove_event_detect(s1)
GPIO.remove_event_detect(s2)
GPIO.remove_event_detect(s3)
GPIO.remove_event_detect(sg)


timer = threading.Timer(0.5, on_switch_check_expire)
timer.start() 

# Set the network loop blocking, it will not actively end the program before calling disconnect() or the program crash

try:
    client.loop_forever()

except KeyboardInterrupt:
    print ("programme interupted")

except Exception as error:
    print ("some other interrpt - ", type(error).__name__, "–", error)

finally:
    GPIO.cleanup()

