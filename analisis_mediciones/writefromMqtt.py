# -*- coding: utf-8 -*-

import paho.mqtt.client as mqtt
import time 

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))


def on_message(client, userdata, message):
    msg = str(message.payload.decode("utf-8"))
    print("recieved message: ", msg)
    

#definición del broker a usar 
mqttBroker = "192.168.20.25"        #ip del broker en localhost
port = 1884            #puerto a conectar

client = mqtt.Client()

client.on_message = on_message
client.on_connect = on_connect

client.username_pw_set(username="", password="")
client.connect(mqttBroker, port) #conexión al broker

client.loop_start()
client.publish("**/pressure/**","1000")
time.sleep(6) # wait
client.loop_stop()
