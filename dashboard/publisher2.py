# subscriber2.py
import paho.mqtt.client as mqtt
import threading
import queue

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
        print('--> spot on')

#
# handle turning the front spot light off
#
def action_spot_off(lock):
    with lock:
        print('--> spot off')

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
    # Subscribe, which need to put into on_connect
    # If reconnect after losing the connection with the broker, it will continue to subscribe to the raspberry/topic topic
    client.subscribe("topic/lights/front/spot/on")
    client.subscribe("topic/lights/front/spot/off")
    client.subscribe("topic/lights/front/fog/on")
    client.subscribe("topic/lights/front/fog/off")

#
# callback function, it will be triggered when receiving messages
#
def on_message(client, userdata, message,tmp=None):
    print(" Received message " + str(message.payload)
        + " on topic '" + message.topic

def main():
    cmd_actions = {'foo': action_foo, 'bar': action_bar}
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
client.on_message = on_message

# Set the will message, when the Raspberry Pi is powered off, or the network is interrupted abnormally, it will send the will message to other clients
client.will_set('raspberry/status', b'{"status": "Off"}')

# Create connection, the three parameters are broker address, broker port number, and keep-alive time respectively
client.connect(broker, myport, 60)

main()


# Set the network loop blocking, it will not actively end the program before calling disconnect() or the program crash
#client.loop_forever()

