# subscriber.py
import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO
import threading

# define the MQTT broker to use

#broker = "dashboard.hhw470d.local"
broker = "192.168.2.2"
myport = 1883

# define the GPIO pins

brake    = 11
side     = 12
leftind  = 13
rightind = 15
reverse  = 16
fog      = 18
spare12  = 22
spare    = 7

# list of the topics supported - order must match the range below
topic_map = [
    'topic/lights/brake',
    'topic/lights/side',
    'topic/lights/left-indicator',
    'topic/lights/right-indicator',
    'topic/lights/hazards',
    'topic/lights/rear-fog',
    'topic/lights/reversing',
    'topic/lights/flasher'
]

# to keep things simple the following values are used to index the ight_map and state columns
BRAK, SIDE, LIND, RIND, REVS, RFOG = range(6)

# this is the pins that map to the lights in the order defined in the range above 
pin_map = [11, 12, 13, 15, 16, 18]

# to keep things simple the following values are used to index the light_map rows
T_BRAK, T_SIDE, T_LIND, T_RIND, T_HAZD, T_RFOG, T_REVS = range(7)

# for each event there is a possible combination of lights
ON  = 0 # ON is the light shold br switched on
OFF = 1 # OFF is the light should be switched off
ASC = 2 # ASC is the light should br switched on, but only switch off if the specific light status is off
TMR = 3 # TMR starts a repeat timer of 0.75s ON and 0.75s OFF

light_map = [
#   [brk, sid, lid, rid, rev, fog] 
    [ON,  OFF, OFF, OFF, OFF, OFF], # T_SIDE 'topic/lights/brake'
    [OFF, ON , OFF, OFF, OFF, OFF], # T_HDIP 'topic/lights/side'
    [OFF, OFF, TMR, OFF, OFF, OFF], # T_LIND 'topic/lights/left-indicator'
    [OFF, OFF, OFF, TMR, OFF, OFF], # T_RIND 'topic/lights/right-indicator'
    [OFF, OFF, TMR, TMR, OFF, OFF], # T_HAZD 'topic/lights/hazards'
    [OFF, OFF, OFF, OFF, OFF, ON ], # T_FFOG 'topic/lights/rear-fog'
    [OFF, OFF, OFF, OFF, ON , OFF], # T_SPOT 'topic/lights/reversing'
]

state = [OFF,OFF,OFF,OFF,OFF,OFF]

lind_timer = None
lind_duty  = OFF
rind_timer = None
rind_duty  = OFF

def on_lind_expire():
    global lind_duty, lind_timer
    if state[LIND] == ON:
        if lind_duty == ON:
            GPIO.output(pin_map[LIND], GPIO.LOW)
            lind_duty = OFF
        else:
            GPIO.output(pin_map[LIND], GPIO.HIGH)
            lind_duty = ON
        lind_timer = threading.Timer(0.75, on_lind_expire)
        lind_timer.start() 
    else:
        GPIO.output(pin_map[LIND], GPIO.HIGH)
        state[LIND] = OFF
        lind_timer = None
        lind_duty = OFF

def on_rind_expire():
    global rind_duty, rind_timer
    if state[RIND] == ON:
        if rind_duty == ON:
            GPIO.output(pin_map[RIND], GPIO.LOW)
            rind_duty = OFF
        else:
            GPIO.output(pin_map[RIND], GPIO.HIGH)
            rind_duty = ON
        rind_timer = threading.Timer(0.75, on_rind_expire)
        rind_timer.start() 
    else:
        GPIO.output(pin_map[RIND], GPIO.HIGH)
        state[RIND] = OFF
        rind_timer = None
        rind_duty = OFF

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
        for light_index in range(len(light_map[topic_index])):
            if light_map[topic_index][light_index] == ON:
                if msg.payload == b'1': # turn on
                    print ("turn on light ", pin_map[light_index])
                    GPIO.output(pin_map[light_index], GPIO.LOW)
                    state[light_index] = ON
                else: # turn off
                    print ("turn off light ", pin_map[light_index])
                    GPIO.output(pin_map[light_index], GPIO.HIGH)
                    state[light_index]= OFF

            if light_map[topic_index][light_index] == OFF: # shouldn't happen but JIC
                if state[light_index] != ON: # only turn off if not on by controler switch
                    GPIO.output(pin_map[light_index], GPIO.HIGH)

            if light_map[topic_index][light_index] == ASC:
                if msg.payload == b'1': # turn on
                    print ("turn on light ", pin_map[light_index])
                    GPIO.output(pin_map[light_index], GPIO.LOW)
                else: # turn off
                    if state[light_index] == OFF: # only turn off if not on by controler switch
                        print ("turn off light as not state on ", pin_map[light_index])
                        GPIO.output(pin_map[light_index], GPIO.HIGH)

            if light_map[topic_index][light_index] == TMR:
                if msg.payload == b'1': # turn on
                    print ("turn on light ", pin_map[light_index])
                    GPIO.output(pin_map[light_index], GPIO.LOW)
                    if light_index == LIND:
                        lind_timer = threading.Timer(0.75, on_lind_expire)
                        lind_timer.start() 
                        state[light_index] = ON
                        lind_duty = ON
                    elif light_index == RIND:
                        rind_timer = threading.Timer(0.75, on_rind_expire)
                        rind_timer.start() 
                        state[light_index] = ON
                        rind_duty = ON
                    else:
                        print("Error: TMR set butnot for an indicator")
                else: # turn off
                    print ("turn off light ", pin_map[light_index])
                    GPIO.output(pin_map[light_index], GPIO.HIGH)
                    if light_index == LIND:
                        state[light_index] = OFF
                    elif light_index == RIND:
                        state[light_index] = OFF
                    else:
                        print("Error: TMR set but not for an indicator")




        

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
client.will_set('raspberry/back/status', b'{"status": "Off"}')

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

