import json
import sqlite3

########################################################################
########################  VARIABILI ####################################
########################################################################
READ_OUT_TOPIC = (
    'out',
)
WRITE_TOPIC = 'domoticz/in'
DB_SQLITE = 'domoticz.db'
########################################################################


conn = sqlite3.connect(DB_SQLITE)
conn.row_factory = sqlite3.Row
c = conn.cursor()


def SensorID(stringa):
    if len(stringa.split()) > 0:
        host = stringa.split()[1].split('/')[0]
        pin = stringa.split()[1].split('/')[len(stringa.split()[1].split('/')) - 1]
        return host, pin
    else:
        return None, None


def QueryTopic(params, column):
    l = list()
    query = 'SELECT * FROM DeviceStatus WHERE ' + column + ' like \'' + params + '\''
    rows = c.execute(query).fetchall()
    for r in rows:
        host, pin = SensorID(r[column])
        if host:
            d = dict()
            d['host'] = host
            d['pin'] = pin
            d['id'] = r['ID']
            l.append(d)
    return l


def sendTopic(host, led):
    list_command = QueryTopic(params='%' + host + '%' + led + '%', column='StrParam1')
    for l in list_command:
        client.publish(WRITE_TOPIC, payload='{"idx": ' + str(l['id']) + ', "nvalue": 1}', qos=0, retain=False)
    list_command = QueryTopic(params='%' + host + '%' + led + '%', column='StrParam2')
    for l in list_command:
        client.publish(WRITE_TOPIC, payload='{"idx": ' + str(l['id']) + ', "nvalue": 0}', qos=0, retain=False)


import paho.mqtt.client as mqtt


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    # client.subscribe("$SYS/#")
    for t in READ_OUT_TOPIC:
        client.subscribe(t)


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.payload))
    for t in READ_OUT_TOPIC:
        if msg.topic == t:
            msg_revice = json.loads(msg.payload)
            if msg_revice['state'] == 'ready':
                sendTopic(host=msg_revice['host'], led=msg_revice['led'])


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("studio.htsystem.com", 20152, 60)

client.loop_forever()
