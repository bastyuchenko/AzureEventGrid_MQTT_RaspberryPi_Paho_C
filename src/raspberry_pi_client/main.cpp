#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "MQTTClient.h"

// Configuration constants
#define TIMEOUT     10000L
#define QOS         1
#define KEEPALIVE   60

// Connection parameters
const char* MQTT_HOST_NAME = "mqtts://mqtteg.westeurope-1.ts.eventgrid.azure.net:8883";
const char* MQTT_USERNAME = "raspberry_pi_client";
const char* MQTT_CLIENT_ID = "raspberry_pi_client";



const char* MQTT_CERT_FILE = "/home/abastiuchenko/projects/AzureEventGrid_MQTT_RaspberryPi_Paho_C/src/raspberry_pi_client/certificates/raspberry_pi_client.pem"; 
const char* MQTT_KEY_FILE = "/home/abastiuchenko/projects/AzureEventGrid_MQTT_RaspberryPi_Paho_C/src/raspberry_pi_client/certificates/raspberry_pi_client.key";

volatile MQTTClient_deliveryToken deliveredtoken;

// Callback for when a message arrives
void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

// Callback for when a message arrives
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: %.*s\n", message->payloadlen, (char*)message->payload);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

// Connection lost callback
void connlost(void *context, char *cause)
{
    printf("Connection lost! Cause: %s\n", cause);
}

int main(int argc, char* argv[])
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;
    int rc;

    // Create MQTT client
    if ((rc = MQTTClient_create(&client, MQTT_HOST_NAME, MQTT_CLIENT_ID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to create client, return code %d\n", rc);
        return rc;
    }

    // Set callbacks
    if ((rc = MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to set callbacks, return code %d\n", rc);
        MQTTClient_destroy(&client);
        return rc;
    }

    // Configure connection options
    conn_opts.keepAliveInterval = KEEPALIVE;
    conn_opts.cleansession = 1;
    conn_opts.username = MQTT_USERNAME;
    conn_opts.MQTTVersion = MQTTVERSION_3_1_1;

    // Configure SSL/TLS options
    ssl_opts.verify = 1;
    ssl_opts.CApath = NULL;
    ssl_opts.keyStore = MQTT_CERT_FILE;
    ssl_opts.privateKey = MQTT_KEY_FILE;
    ssl_opts.privateKeyPassword = NULL;
    ssl_opts.enabledCipherSuites = NULL;
    ssl_opts.enableServerCertAuth = 1;
    
    conn_opts.ssl = &ssl_opts;

    // Connect to the broker
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        MQTTClient_destroy(&client);
        return rc;
    }

    printf("Connected to MQTT broker successfully\n");

    // Keep the connection alive for a while (replace with your actual application logic)
    sleep(10);

    // Disconnect and cleanup
    MQTTClient_disconnect(client, TIMEOUT);
    MQTTClient_destroy(&client);

    return rc;
}