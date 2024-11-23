#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"

// Include sensor library headers as needed
// #include "bme280.h"

#define ADDRESS     "tcp://yourbrokeraddress:1883"
#define CLIENTID    "ExampleClientPub"
#define TOPIC       "devices/rasp"
#define QOS         1
#define TIMEOUT     10000L

int main(int argc, char* argv[])
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;

    // Initialize sensor or other components as necessary
    // Bme280Sensor sensor;

    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    // Set up SSL/TLS options if necessary
    // conn_opts.ssl = &ssl_opts;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    char payload[100]; // Adjust payload size as necessary

    while (1) // Replace with a condition to exit as needed
    {
        // Read sensor data or generate payload as necessary
        // Bme280Data data = sensor.readBME280();
        // snprintf(payload, sizeof(payload), "Temp: %.2f°C Pressure: %.2f hPa Humidity: %.2f%%.", data.temperature, data.pressure, data.humidity);

        snprintf(payload, sizeof(payload), "Hello World!"); // Example payload

        pubmsg.payload = payload;
        pubmsg.payloadlen = strlen(payload);
        pubmsg.qos = QOS;
        pubmsg.retained = 0;
        MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
        printf("Waiting for up to %ld seconds for publication of %s\n"
                "on topic %s for client with ClientID: %s\n",
                (long int)TIMEOUT/1000, payload, TOPIC, CLIENTID);
        rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
        printf("Message with delivery token %d delivered\n", token);

        // Adjust the sleep time as necessary
        // sleep(1);
    }

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}