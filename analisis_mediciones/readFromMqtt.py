import csv
from pprint import pprint as pp
import paho.mqtt.client as mqtt
import datetime


def on_connect(client, userdata, flags, rc):

    print("Connected with result code "+str(rc))
    client.subscribe("Esp8266!D4ta/#")              #subscripción a todos los temas de la forma 'Esp8266!D4ta/*'

def on_message(client, userdata, message):

    msg = str(message.payload.decode("utf-8"))      #decodificación del mensaje enviado 
    topic = message.topic                           #obtención del tema de llegada del mensaje

    topic = topic.split('/')                        #obtención de la información importante que se encuentra
    topic = topic[1:]                               #expresada en el tema del mensajede llegada

    #obtención de la fecha de llegada de los datos 
    now = datetime.datetime.now()
    currtime = now.strftime("%Y-%m-%d %H:%M:%S")
    
    #añadidura de el mensage a la lista 'topic', junto con la fecha de llegada
    #esto es de la forma: ['pressure', '10370001', '743.20', '2021-04-01 12:34:55']
    topic.append(msg)
    topic.append(currtime)

    #añadido del paquete de datos temporal a una lista temporal 
    #que se llena hasta 4 veces con paquetes de info similares
    #ya que se reciben 4 variables en cada carga de mensajes 'presion', 'humedad', 'temperatura', 'lux'
    tmpMsgCargo.append(topic)

    if len(tmpMsgCargo) == 4: 
        pp(tmpMsgCargo)
        
        msgCargo.append(tmpMsgCargo.copy())    #agragado del paquete de datos temporal a 
        tmpMsgCargo.clear()                    #el compendio de paquetes de datos temporal


    if len(msgCargo) == 2:
        
        print("displaying message cargo:  ")
        pp(msgCargo)
        print() 

        with open('sensor_data.csv', 'w') as csvfile: 
            filewriter = csv.writer(csvfile, delimiter=',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
            
            for row in msgCargo:
                pp(row)
                filewriter.writerows(row)
    
            msgCargo.clear()

msgCargo = []           #lista que contiene un compendio de datos que llegan por mqtt
tmpMsgCargo = []        #lista para guardar cada paquete pequeño de datos que va llegando

#definición del broker a usar 
mqttBroker = "192.168.20.25"        #ip del broker en red local
port = 1884                         #puerto a conectar

client = mqtt.Client()

client.on_message = on_message
client.on_connect = on_connect

client.username_pw_set(username="bbbtest1", password="bbbtest1_nuncasupecomoseescribe")
client.connect(mqttBroker, port) #conexión al broker

client.loop_forever()
