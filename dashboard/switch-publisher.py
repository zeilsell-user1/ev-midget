import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO

# define the MQTT broker to use

broker = "dashboard.hhw470d.local"
myport = 1883

# pin allocation 

headdip = 11
headfull = 12
leftind = 13
rightind = 15
side = 16
spot = 18
ffog = 22
rfog = 7
# wiper need more GPIO pins!
# wash need more GPIO pins!

# state variables for the switches

headdip_State = 0
headfull_state = 0
leftind_state = 0
rightind_state = 0
side_state = 0
spot_state = 0
ffog_state = 0
rfog_state = 0

#
# callback for a headlight dipped switch change
#
def my_callback_headdip(channel):
    global headdip_state
    if GPIO.input(channel) == GPIO.HIGH and headdip_state == 0:
        client.publish('topic/lights/front/headdip/on', payload='on', qos=0, retain=False)
        headdip_state = 1
        print('--> dipped headlights on')

    elif GPIO.input(channel) == GPIO.LOW and headdip_state == 1:
        client.publish('topic/lights/front/headdip/off', payload='off', qos=0, retain=False)
        headdip_state = 0
        print('--> dipped headlights off')
 
#
# callback for a headlight full switch change
#
def my_callback_headfull(channel):
    global headfull_state
    if GPIO.input(channel) == GPIO.HIGH and headfull_state == 0:
        client.publish('topic/lights/front/headfull/on', payload='on', qos=0, retain=False)
        headfull_state = 1
        print('--> full headlights on')

    elif GPIO.input(channel) == GPIO.LOW and headfull_state == 1:
        client.publish('topic/lights/front/headfull/off', payload='off', qos=0, retain=False)
        headfull_state = 0
        print('--> full headlights off')

#
# callback for a sideight switch change
#
def my_callback_side(channel):
    global side_state
    if GPIO.input(channel) == GPIO.HIGH and side_state == 0:
        client.publish('topic/lights/front/side/on', payload='on', qos=0, retain=False)
        client.publish('topic/lights/rear/side/on', payload='on', qos=0, retain=False)
        side_state = 1
        print('--> sidelights on')

    elif GPIO.input(channel) == GPIO.LOW and side_state == 1:
        client.publish('topic/lights/front/side/off', payload='off', qos=0, retain=False)
        client.publish('topic/lights/rear/side/off', payload='off', qos=0, retain=False)
        side_state = 0
        print('--> sidelights off')

#
# callback for a left indicator switch change
#
def my_callback_leftind(channel):
    global leftind_state
    if GPIO.input(channel) == GPIO.HIGH and leftind_state == 0:
        client.publish('topic/lights/front/leftind/on', payload='on', qos=0, retain=False)
        client.publish('topic/lights/rear/leftind/on', payload='on', qos=0, retain=False)
        leftind_state = 1
        print('--> left indicator on')

    elif GPIO.input(channel) == GPIO.LOW and leftind_state == 1:
        client.publish('topic/lights/front/leftind/off', payload='off', qos=0, retain=False)
        client.publish('topic/lights/rear/leftind/off', payload='off', qos=0, retain=False)
        leftind_state = 0
        print('--> left indicator off')

#
# callback for a right indicator switch change
#
def my_callback_rightind(channel):
    global rightind_state
    if GPIO.input(channel) == GPIO.HIGH and rightind_state == 0:
        client.publish('topic/lights/front/rightind/on', payload='on', qos=0, retain=False)
        client.publish('topic/lights/rear/rightind/on', payload='on', qos=0, retain=False)
        rightind_state = 1
        print('--> right indicator on')

    elif GPIO.input(channel) == GPIO.LOW and rightind_state == 1:
        client.publish('topic/lights/front/rightind/off', payload='off', qos=0, retain=False)
        client.publish('topic/lights/rear/rightind/off', payload='off', qos=0, retain=False)
        rightind_state = 0
        print('--> right indicator off')

#
# callback for a spotlight switch change
#
def my_callback_spot(channel):
    global spot_state
    if GPIO.input(channel) == GPIO.HIGH and spot_state == 0:
        client.publish('topic/lights/front/spot/on', payload='on', qos=0, retain=False)
        spot_state = 1
        print('--> spot on')

    elif GPIO.input(channel) == GPIO.LOW and spot_state == 1:
        client.publish('topic/lights/front/spot/off', payload='off', qos=0, retain=False)
        spot_state = 0
        print('--> spot off')
 
#
# callback for a front fog lights switch change
#
def my_callback_ffog(channel):
    global ffog_state
    if GPIO.input(channel) == GPIO.HIGH and ffog_state == 0:
        client.publish('topic/lights/front/fog/on', payload='on', qos=0, retain=False)
        ffog_state = 1
        print('--> front fog on')

    elif GPIO.input(channel) == GPIO.LOW and ffog_state == 1:
        client.publish('topic/lights/front/fog/off', payload='off', qos=0, retain=False)
        ffog_state = 0
        print('--> front fog off')

#
# callback for a rear fog light switch change
#
def my_callback_rfog(channel):
    global rfog_state
    if GPIO.input(channel) == GPIO.HIGH and rfog_state == 0:
        client.publish('topic/lights/rear/fog/on', payload='on', qos=0, retain=False)
        rfog_state = 1
        print('--> rearfog on')

    elif GPIO.input(channel) == GPIO.LOW and rfog_state == 1:
        client.publish('topic/lights/rear/fog/off', payload='off', qos=0, retain=False)
        rfog_state = 0
        print('--> rearfog off')

#
# callback for the connect to the MQTT broker
#
def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")

# connect to the broker

client = mqtt.Client()
client.on_connect = on_connect
client.connect(broker, myport, 60)

# set up the pins to listen to changes

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BOARD)
GPIO.setup(headdip, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.setup(headfull, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.setup(side, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.setup(leftind, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.setup(rightind, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.setup(spot, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.setup(ffog, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.setup(rfog, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.remove_event_detect(headdip)
GPIO.remove_event_detect(headfull)
GPIO.remove_event_detect(side)
GPIO.remove_event_detect(leftind)
GPIO.remove_event_detect(rightind)
GPIO.remove_event_detect(spot)
GPIO.remove_event_detect(ffog)
GPIO.remove_event_detect(rfog)
GPIO.add_event_detect(headdip, GPIO.BOTH, callback=my_callback_headdip)
GPIO.add_event_detect(headfull, GPIO.BOTH, callback=my_callback_headfull)
GPIO.add_event_detect(side, GPIO.BOTH, callback=my_callback_side)
GPIO.add_event_detect(leftind, GPIO.BOTH, callback=my_callback_leftind)
GPIO.add_event_detect(rightind, GPIO.BOTH, callback=my_callback_rightind)
GPIO.add_event_detect(spot, GPIO.BOTH, callback=my_callback_spot)
GPIO.add_event_detect(ffog, GPIO.BOTH, callback=my_callback_ffog)
GPIO.add_event_detect(rfog, GPIO.BOTH, callback=my_callback_rfog)

# Set the network loop blocking, it will not actively end the program before calling disconnect() or the program crash

try:
    client.loop_forever()

except KeyboardInterrupt:
    print ("programme interupted")

except:
    print ("some other interrpt")

finally:
    GPIO.cleanup()

