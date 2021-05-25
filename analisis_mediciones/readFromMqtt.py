import csv
from pprint import pprint as pp
import paho.mqtt.client as mqtt
import datetime


def on_connect(client, userdata, flags, rc):

    print("Connected with result code "+str(rc))
    client.subscribe("esp/#")              #subscripción a todos los temas de la forma "esp/*"

def on_message(client, userdata, message): # documentacion mqtt

    msg = str(message.payload.decode("utf-8"))       
    topic = message.topic                           

    topic = topic.split('/')                        
    topic = topic[1:]                               

    now = datetime.datetime.now()
    current_time = now.strftime("%Y-%m-%d %H:%M:%S") # guarda la fecha como timestap
    
    topic.append(msg)
    topic.append(current_time)


    tmpMsg.append(topic) # temas de las mediciones

    if len(tmpMsg) == 5: 
        pp(tmpMsg)
        
        msgN.append(tmpMsg.copy())    
        tmpMsg.clear()                    


    if len(msgN) == 2:
        
        print("displaying message:  ")
        pp(msgN)
        print() 

        with open('data.csv', 'a') as csvfile: 
            filewriter = csv.writer(csvfile, delimiter=',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
            
            for row in msgN:
                pp(row)
                filewriter.writerows(row)
    
            msgN.clear()

msgN = []           
tmpMsg = []        


mqttBroker = "192.168.20.25"        
port = 1886                       

client = mqtt.Client()

client.on_message = on_message
client.on_connect = on_connect

client.username_pw_set(username="aaatest1", password="aaatest1_nuncasupecomoseescribe")
client.connect(mqttBroker, port)

client.loop_forever()
