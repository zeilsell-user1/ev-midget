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

# to keep things simple the following values are used to index the ight_map and state columns
SIDE, HDIP, HFUL, LIND, RIND, FFOG, SPOT = range(7)

# this is the pins that map to the lights in the order defined in the range above 
pin_map = [11, 12, 22, 13, 15, 16, 18]

# to keep things simple the following values are used to index the light_map rows
T_SIDE, T_HDIP, T_HFULL, T_LIND, T_RIND, T_HAZD, T_FFOG, T_SPOT, T_FLSH = range(9)

# for each event there is a possible combination of lights
ON  = 0 # ON is the light shold br switched on
OFF = 1 # OFF is the light should be switched off
ASC = 2 # ASC is the light should br switched on, but only switch off if the specific light status is off
TMR = 3 # TMR starts a repeat timer of 0.75s ON and 0.75s OFF

light_map = [
#   [sid, dip, ful, lid, rid, fog, spt] 
    [ON,  OFF, OFF, OFF, OFF, OFF, OFF], # T_SIDE 'topic/lights/side'
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
    
        for light_index in range(len(light_map[topic_index])):
            print ("light set to ", light_map[topic_index][light_index])
            if light_map[topic_index][light_index] == ON:
                if msg.payload == b'1': # turn on
                    print ("turn on light ", pin_map[light_index])
                    GPIO.output(pin_map[light_index], GPIO.LOW)
                else: # turn off
                    print ("turn off light ", pin_map[light_index])
                    GPIO.output(pin_map[light_index], GPIO.HIGH)
        

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BOARD)  # use the pin numbering over broadcom

for pin in pin_map:
    GPIO.setup(pin, GPIO.OUT)
    GPIO.output(pin, GPIO.HIGH)

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
    print ("programme interupted by big fat fingers")


except Exception as error:
    print ("some other interrpt - ", type(error).__name__, "–", error)


finally:
    GPIO.cleanup()

