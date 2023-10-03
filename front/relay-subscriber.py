# subscriber.py
import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO

# define the MQTT broker to use

#broker = "dashboard.hhw470d.local"
broker = "192.168.2.2"
myport = 1883

# define the GPIO pins

side     = 11
headdip  = 12
leftind  = 13
rightind = 15
fog      = 16
spot     = 18
headfull = 22
spare    = 7

# list of the topics supported - order must match the range below
topic_map = [
    'topic/lights/side',
    'topic/lights/head-dipped',
    'topic/lights/head-full',
    'topic/lights/left-indicator',
    'topic/lights/right-indicator',
    'topic/lights/hazards',
    'topic/lights/front-fog',
    'topic/lights/spot',
    'topic/lights/flasher'
]

#to keep things simple the following values are used to index the light_map rows
T_SIDE, T_HDIP, T_HFULL, T_LIND, T_RIND, T_HAZD, T_FFOG, T_SPOT, T_FLSH = range(9)

# to keep things simple the following values are used to index the ight_map and state columns
SIDE, HDIP, HFUL, LIND, RIND, FFOG, SPOT = range(7)

# for each event there is a possible combination of lights
ON  = 0 # ON is the light shold br switched on
OFF = 1 # OFF is the light should be switched off
ASC = 2 # ASC is the light should br switched on, but only switch off if the specific light status is off
TMR = 3 # TMR starts a repeat timer of 0.75s ON and 0.75s OFF

light_map = [
#   [sid, dip, ful, lid, rid, fog, spt] 
    [ON,  ON , OFF, OFF, OFF, OFF, OFF], # T_SIDE 'topic/lights/side'
    [ASC, ON , OFF, OFF, OFF, OFF, OFF], # T_HDIP 'topic/lights/head-dipped'
    [ASC, OFF, ON , OFF, OFF, OFF, OFF], # T_HFUL 'topic/lights/head-full'
    [OFF, OFF, OFF, TMR, OFF, OFF, OFF], # T_LIND 'topic/lights/left-indicator'
    [OFF, OFF, OFF, OFF, TMR, OFF, OFF], # T_RIND 'topic/lights/right-indicator'
    [OFF, OFF, OFF, TMR, TMR, OFF, OFF], # T_HAZD 'topic/lights/hazards'
    [ASC, ASC, OFF, OFF, OFF, ON , OFF], # T_FFOG 'topic/lights/front-fog'
    [OFF, OFF, ASC, OFF, OFF, OFF, ON ], # T_SPOT 'topic/lights/spot'
    [OFF, OFF, ON , OFF, OFF, OFF, ON ]  # T_FLSH 'topic/lights/flasher'
]

state = [0,0,0,0,0,0,0]

def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    # If reconnect after losing the connection with the broker, it will continue to 
    # subscribe to the raspberry/topic topic
    for topic in topic_map:
        client.subscribe(topic)

#
# The callback function, it will be triggered when receiving messages
# GPIO.HIGH is light on
#

def on_message(client, userdata, msg):
    print(f"{msg.topic} {msg.payload}")

    try:
        topic_index = topic_map.index(msg.topic)
    except ValueError:
        print("topic was not found in subscribable list")
    except:
        print("some other erro rhappened with topic_map.index")
    else:
        print(light_map[topic_index])

def on_message1(client, userdata, msg):
    print(f"{msg.topic} {msg.payload}")
    if msg.topic == 'topic/lights/side' and msg.payload == b'0':
        GPIO.output(side, GPIO.HIGH)
        print('turn side light on')
    elif msg.topic == 'topic/lights/side' and msg.payload == b'1':
        GPIO.output(side, GPIO.LOW)
        print('turn side light on')
    elif msg.topic == 'topic/lights/head-dipped' and msg.payload == b'0':
        GPIO.output(headdip, GPIO.HIGH)
        headdip_state = 0
        print('turn head dipped light on')
    elif msg.topic == 'topic/lights/head-dipped' and msg.payload == b'1':
        GPIO.output(headdip, GPIO.LOW)
        headdip_state = 1
        print('turn head dipped light on')
    elif msg.topic == 'topic/lights/head-full' and msg.payload == b'0':
        GPIO.output(headfull, GPIO.HIGH)
        headfull_state = 0
        print('turn head full light on')
    elif msg.topic == 'topic/lights/head-full' and msg.payload == b'1':
        GPIO.output(headfull, GPIO.LOW)
        headfull_state = 1
        print('turn head full light on')
    elif msg.topic == 'topic/lights/left-indicator' and msg.payload == b'0':
        GPIO.output(leftind, GPIO.HIGH)
        leftind_state = 0
        print('turn left indicator light off')
    elif msg.topic == 'topic/lights/left-indicator' and msg.payload == b'1':
        GPIO.output(leftind, GPIO.LOW)
        leftind_state = 1
        print('turn left indicator light on')
    elif msg.topic == 'topic/lights/right-indicator' and msg.payload == b'0':
        GPIO.output(rightind, GPIO.HIGH)
        rightind_state =0
        print('turn right indicator light off')
    elif msg.topic == 'topic/lights/right-indicator' and msg.payload == b'1':
        GPIO.output(rightind, GPIO.LOW)
        rightind_state = 1
        print('turn right indicator light on')
    elif msg.topic == 'topic/lights/hazards' and msg.payload == b'0':
        GPIO.output(leftind, GPIO.HIGH)
        GPIO.output(rightind, GPIO.HIGH)
        print('turn hazards light off')
    elif msg.topic == 'topic/lights/hazards' and msg.payload == b'1':
        GPIO.output(leftind, GPIO.LOW)
        GPIO.output(rightind, GPIO.LOW)
        print('turn hazards light on')
    elif msg.topic == 'topic/lights/front-fog' and msg.payload == b'0':
        GPIO.output(fog, GPIO.HIGH)
        print('turn fog light off')
    elif msg.topic == 'topic/lights/front-fog' and msg.payload == b'1':
        GPIO.output(fog, GPIO.LOW)
        print('turn fog light on')
    elif msg.topic == 'topic/lights/spot' and msg.payload == b'0':
        GPIO.output(spot, GPIO.HIGH)
        print('turn spot light off')
    elif msg.topic == 'topic/lights/spot' and msg.payload == b'1':
        GPIO.output(spot, GPIO.LOW)
        print('turn spot light on')    
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

# Set the will message, when the Raspberry Pi is powered off, or 
# the network is interrupted abnormally, it will send the will message t
# o other clients
client.will_set('raspberry/front/status', b'{"status": "Off"}')

# Create connection, the three parameters are broker address, 
# broker port number, and keep-alive time respectively
client.connect(broker, myport, 60)

# Set the network loop blocking, it will not actively end the program 
# before calling disconnect() or the program crash
try:
    client.loop_forever()

except KeyboardInterrupt:
    print ("programme interupted")

except:
    print ("some other interrpt")

finally:
    GPIO.cleanup()

