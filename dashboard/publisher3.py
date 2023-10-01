# publisher3.py
import paho.mqtt.client as mqtt
import threading
import queue

broker = "dashboard"
myport = 1883


#
# console that will request the event from the user
#
def console(q, lock):
    while 1:
        input()   # Afther pressing Enter you'll be in "input mode"
        with lock:
            cmd = input('> ')

        q.put(cmd)
        if cmd == 'quit':
            break

#
# handle turning the front spot light on
#
def action_spot_on(lock):
    with lock:
        client.publish('topic/lights/front/spot/on', payload='on', qos=0, retain=False)
        print('--> spot on')

#
# handle turning the front spot light off
#
def action_spot_off(lock):
    with lock:
        client.publish('topic/lights/front/spot/off', payload='off', qos=0, retain=False)
        print('--> spot off')

#
# handle turning the front fog light on
#
def action_front_fog_on(lock):
    with lock:
        client.publish('topic/lights/front/fog/on', payload='on', qos=0, retain=False)
        print('--> front fog on')

#
# handle turning the front fog light off
#
def action_front_fog_off(lock):
    with lock:
        client.publish('topic/lights/front/fog/off', payload='off', qos=0, retain=False)
        print('--> front fog off')

#
# handle turning the rear fog light on
#
def action_rear_fog_on(lock):
    with lock:
        client.publish('topic/lights/rear/fog/on', payload='on', qos=0, retain=False)
        print('--> rear fog on')

#
# handle turning the rear fog light off
#
def action_rear_fog_off(lock):
    with lock:
        client.publish('topic/lights/rear/fog/off', payload='off', qos=0, retain=False)
        print('--> rear fog off')

#
# handle an invalid request
#
def invalid_input(lock):
    with lock:
        print('--> Unknown command')

#
# callback for the connect to the MQTT broker
#
def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")

def main():
    cmd_actions = {'spot-on': action_spot_on, 'spot-off': action_spot_off, \
                   'front-fog-on': action_front_fog_on, 'front-fog-off': action_front_fog_off, \
                   'rear-fog-on': action_rear_fog_on, 'rear-fog-off': action_rear_fog_off}
    cmd_queue = queue.Queue()
    stdout_lock = threading.Lock()

    dj = threading.Thread(target=console, args=(cmd_queue, stdout_lock))
    dj.start()

    while 1:
        cmd = cmd_queue.get()
        if cmd == 'quit':
            break
        action = cmd_actions.get(cmd, invalid_input)
        action(stdout_lock)


client = mqtt.Client()
client.on_connect = on_connect
client.connect(broker, myport, 60)

main()


# Set the network loop blocking, it will not actively end the program before calling disconnect() or the program crash
#client.loop_forever()

