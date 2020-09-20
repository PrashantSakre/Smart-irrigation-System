/**
* Author: Prashant Sakre
* Date:   March - May 2020
* 
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h> // https://github.com/knolleary/pubsubclient/releases/tag/v2.3
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson/releases/tag/v5.0.7

//-------- Customise these values -----------
const char* ssid = "********";        //wifi name
const char* password = "********";    //wifi password

#define ORG "*******"                 //Bellow info is from ibm cloud platform
#define DEVICE_TYPE "ESP8266_01"
#define DEVICE_ID "50-02-91-da-2e-6d"
#define TOKEN "****************"
//-------- Customise the above values --------

char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

const char publishTopic1[] = "iot-2/evt/status1/fmt/json";
const char publishTopic2[] = "iot-2/evt/status2/fmt/json";
const char publishTopic3[] = "iot-2/evt/status3/fmt/json";
const char publishTopic4[] = "iot-2/evt/status4/fmt/json";
const char responseTopic[] = "iotdm-1/response";
const char manageTopic[] = "iotdevice-1/mgmt/manage";
const char updateTopic[] = "iotdm-1/device/update";
const char rebootTopic[] = "iotdm-1/mgmt/initiate/device/reboot";

void callback(char* topic, byte* payload, unsigned int payloadLength);

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

int publishInterval = 30000; // 30 seconds
long lastPublishMillis;


String buffer1, buffer2;
unsigned char buff[10];
int data1, data2;String data3;int data4, data5;


void setup() {
 Serial.begin(115200);
 Serial.println();

 wifiConnect();
 mqttConnect();
 initManagedDevice();
}

void loop() {
 if (millis() - lastPublishMillis > publishInterval) {
   publishData();
   lastPublishMillis = millis();
 }

 if (!client.loop()) {
   mqttConnect();
   initManagedDevice();
 }
}

void wifiConnect() {
   Serial.print("Connecting to "); Serial.print(ssid);
   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
   }
   Serial.print("WiFi connected, IP address: "); Serial.println(WiFi.localIP());
   // To print info about Wifi Status use bellow code
   // WiFi.printDiag(Serial);
}

void mqttConnect() {
 if (!!!client.connected()) {
   Serial.print("Reconnecting MQTT client to "); Serial.println(server);
   while (!!!client.connect(clientId, authMethod, token)) {
     Serial.print(".");
     delay(500);
   }
   Serial.println("Bluemix connected");
 }
}

void initManagedDevice() {
   if (client.subscribe("iotdm-1/response")) {
     Serial.println("subscribe to responses OK");
   } else {
     Serial.println("subscribe to responses FAILED");
   }

   if (client.subscribe(rebootTopic)) {
     Serial.println("subscribe to reboot OK");
   } else {
     Serial.println("subscribe to reboot FAILED");
   }

   if (client.subscribe("iotdm-1/device/update")) {
     Serial.println("subscribe to update OK");
   } else {
     Serial.println("subscribe to update FAILED");
   }

   StaticJsonBuffer<300> jsonBuffer;
   JsonObject& root = jsonBuffer.createObject();
   JsonObject& d = root.createNestedObject("d");
   JsonObject& metadata = d.createNestedObject("metadata");
   metadata["publishInterval"] = publishInterval;
   JsonObject& supports = d.createNestedObject("supports");
   supports["deviceActions"] = true;


 char buff[300];
 root.printTo(buff, sizeof(buff));
 Serial.println("publishing device metadata:"); Serial.println(buff);
   if (client.publish(manageTopic, buff)) {
     Serial.println("device Publish ok");
   } else {
     Serial.print("device Publish failed:");
   }
}


void publishData() {
        if (Serial.available() > 0) {
            //delay(100);
            
            while (Serial.available() > 0) {
              buffer1 = Serial.readString();

              if (buffer1[0] == '*') {
                if (buffer1[10] == '#'){
                  Serial.println(buffer1);

                  data1 = ((buffer1[1] - 0x30) * 10 + (buffer1[2] - 0x30));
                  data2 = ((buffer1[3] - 0x30) * 10 + (buffer1[4] - 0x30));
                    if (buffer1[5] == 45){                                                        //For values from -10 to -99
                      data3 = (buffer1[5]); data3.concat(buffer1[6]); data3.concat(buffer1[7]);
                      //break;
                    }
                    else if (buffer1[6] == 45 /*&& buffer1[7] == 0*/){                            //For values between
                      data3 = ((buffer1[6])); data3.concat(buffer1[7]);                           //-1 to 9
                      //break;
                    }
                    else {
                      data3 = ((buffer1[6] - 0x30) * 10 + (buffer1[7] - 0x30));
                    }
                  data4 = ((buffer1[8] - 0x30) * 10 + (buffer1[9] - 0x30));
                }
              }
            }
          }

        //Serial.print("data1:");
        // Serial.println(data1);

       String Motorstate;
        switch (data4){
          case 11:
            Motorstate = "On";
            break;
          case 12:
            Motorstate = "Off";
            break;
          default:
            Motorstate = "No data";
            break;
        }

        int temperature = data1;
        int humidity = data2;
        String moisture = data3;
        
        Serial.println(data1);
        Serial.println(data2);
        Serial.println(data3);
        Serial.println(Motorstate);

        //Sending data 1
        String payload1 = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
               payload1 += ",\"temperature\":";
               payload1 += temperature;
               payload1 += "}}";
        /*  String payload1 = "{
         *    \"d\" : \"Name\" : \""DEVICE_ID\"",
         *    \"temperature\" : \"temperature\"
         *    }"
        */

        Serial.print("Sending payload:");
        Serial.println(payload1);

        if (client.publish(publishTopic1, (char*) payload1.c_str())) {
            Serial.println("Publish ok");
        } else {
            Serial.println("Publish failed");
        }

        //Sending data 2
        String payload2 = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
               payload2 += ",\"humidity\":";
               payload2 += humidity;
               payload2 += "}}";

        Serial.print("Sending payload:");
        Serial.println(payload2);

         if (client.publish(publishTopic2, (char*) payload2.c_str())) {
            Serial.println("Publish ok");
        } else {
            Serial.println("Publish failed");
        }

        //Sending data 3
        String payload3 = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
               payload3 += ",\"moisture\":";
               payload3 += moisture;
               payload3 += "}}";

        Serial.print("Sending payload:");
        Serial.println(payload3);

         if (client.publish(publishTopic3, (char*) payload3.c_str())) {
            Serial.println("Publish ok");
        } else {
            Serial.println("Publish failed");
        }

        //Sending data 4
        //String payload4 = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
        //       payload4 += ",\"Motorstate\":";
        //       payload4 += Motorstate;
        //       payload4 += "}}";
        StaticJsonBuffer<300> jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        JsonObject& d = root.createNestedObject("d");
        d["Name"] = DEVICE_ID;
        d["Motorstate"] = Motorstate;
        
        //String payload4 = "{\"d\":{\"Name\":\"" + DEVICE_ID + "\",\"Motorstate\":\"" + Motorstate + "\"}}";
        char payload4[300];
        root.printTo(payload4, sizeof(payload4));
 
        Serial.print("Sending payload:");
        Serial.println(payload4);

         if (client.publish(publishTopic4, payload4)) {
            Serial.println("Publish ok");
        } else {
            Serial.println("Publish failed");
        }

}

void callback(char* topic, byte* payload, unsigned int payloadLength) {
 Serial.print("callback invoked for topic: "); Serial.println(topic);

    if (strcmp (responseTopic, topic) == 0) {
      return; // just print of response for now
    }

    if (strcmp (rebootTopic, topic) == 0) {
      Serial.println("Rebooting...");
      ESP.restart();
    }

    if (strcmp (updateTopic, topic) == 0) {
      handleUpdate(payload);
    }
}

void handleUpdate(byte* payload) {
   StaticJsonBuffer<300> jsonBuffer;
   JsonObject& root = jsonBuffer.parseObject((char*)payload);

   if (!root.success()) {
     Serial.println("handleUpdate: payload parse FAILED");
     return;
   }
 Serial.println("handleUpdate payload:"); root.prettyPrintTo(Serial); Serial.println();

   JsonObject& d = root["d"];
   JsonArray& fields = d["fields"];
   for (JsonArray::iterator it = fields.begin(); it != fields.end(); ++it) {
     JsonObject& field = *it;
     const char* fieldName = field["field"];
     if (strcmp (fieldName, "metadata") == 0) {
       JsonObject& fieldValue = field["value"];
       if (fieldValue.containsKey("publishInterval")) {
         publishInterval = fieldValue["publishInterval"];
         Serial.print("publishInterval:"); Serial.println(publishInterval);
       }
     }
   }
}
