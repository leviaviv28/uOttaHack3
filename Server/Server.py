import certifi
import paho.mqtt.client as mqtt

packages = {}

def updateDatabase(msg):
    tracking = msg.route.split('/')[1]
    location = msg.payload
    packages[tracking] = location


def getDatabase(msg):
    if msg.payload in packages.keys():
        return msg.payload, packages[msg.payload]
    else:
        return None

# Callback on connection
def on_connect(client, userdata, flags, rc):
    print(rc)
    client.subscribe('fetch_tracking/listen')
    client.subscribe('update_tracking/#')


# Callback when message is received
def on_message(client, userdata, msg):
    print(f'Message received on topic: {msg.topic}. Message: {msg.payload}')
    if msg.topic == 'update_tracking':
        updateDatabase(msg)
    elif msg.topic  == 'fetch_tracking':
        tracking_num, package_state = getDatabase(msg.payload)
        client.publish(f'fetch_tracking/{tracking_num}', payload=package_state)


if __name__ == "__main__":
    client = mqtt.Client()

    client.on_connect = on_connect
    client.on_message = on_message

    client.tls_set(ca_certs=certifi.where())
    client.username_pw_set('solace-cloud-client', 'vvngpe3cn2ss72a7425hgf1l7f')

    client.connect('mr2hd0llj3vwjx.messaging.solace.cloud', port=8883)

    client.loop_forever()