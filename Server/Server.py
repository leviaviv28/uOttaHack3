import certifi
import paho.mqtt.client as mqtt

packages = {}
environment = {}

def updatePackage(msg):
    tracking = msg.topic.split('/')[1]
    print(tracking)
    location = msg.payload
    packages[tracking] = location

def updateEnvironment(msg):
    tracking = msg.topic.split('/')[1]
    location = msg.payload
    environment[tracking] = location

def getPackage(msg):
    if msg.payload in packages.keys():
        return msg.payload, packages[msg.payload]
    else:
        return None

# Callback on connection
def on_connect(client, userdata, flags, rc):
    print(rc)
    client.subscribe('fetch_tracking/listen')
    client.subscribe('update_tracking/#')
    client.subscribe('update_environment/#')

# Callback when message is received
def on_message(client, userdata, msg):
    print(packages)
    print(environment)
    print(f'Message received on topic: {msg.topic}. Message: {msg.payload}')
    if msg.topic.startswith('update_tracking'):
        updatePackage(msg)
    elif msg.topic.startswith('update_environment'):
        updateEnvironment(msg)
    elif msg.topic.startswith('fetch_tracking'):
        tracking_num, package_state = getPackage(msg.payload)
        client.publish(f'fetch_tracking/{tracking_num}', payload=package_state)

if __name__ == "__main__":
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.tls_set(ca_certs=certifi.where())
    client.username_pw_set('solace-cloud-client', 'vvngpe3cn2ss72a7425hgf1l7f')
    client.connect('mr2hd0llj3vwjx.messaging.solace.cloud', port=8883)
    client.loop_forever()
