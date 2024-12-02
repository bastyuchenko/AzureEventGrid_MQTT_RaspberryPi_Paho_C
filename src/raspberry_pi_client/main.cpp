#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <signal.h>

#include "MQTTClient.h"
#include "bme280.h"
// #include "logging.h"

// Configuration constants
constexpr char PUB_TOPIC[] = "devices/rasp";
constexpr int QOS_LEVEL = 1;
constexpr int TIMEOUT = 10000L;
constexpr int KEEP_ALIVE = 60;

// MQTT Connection Parameters
constexpr char MQTT_HOST_NAME[] = "mqtts://mqtteg.westeurope-1.ts.eventgrid.azure.net:8883";
constexpr char MQTT_USERNAME[] = "raspberry_pi_client";
constexpr char MQTT_CLIENT_ID[] = "raspberry_pi_client";
constexpr char MQTT_CERT_FILE[]  = "/home/abastiuchenko/projects/AzureEventGrid_MQTT_RaspberryPi_Paho_C/src/raspberry_pi_client/certificates/raspberry_pi_client.pem"; 
constexpr char MQTT_KEY_FILE[] = "/home/abastiuchenko/projects/AzureEventGrid_MQTT_RaspberryPi_Paho_C/src/raspberry_pi_client/certificates/raspberry_pi_client.key";

volatile MQTTClient_deliveryToken deliveredtoken;

// Global variables for clean shutdown
volatile bool keep_running = true;

void signal_handler(int signum) {
    keep_running = false;
}

// Connection options setup
void setup_connection_options(MQTTClient_connectOptions& conn_opts, 
                            MQTTClient_SSLOptions& ssl_opts) {
    conn_opts = MQTTClient_connectOptions_initializer;
     conn_opts.keepAliveInterval = KEEP_ALIVE;
    conn_opts.cleansession = 1;
    conn_opts.username = MQTT_USERNAME;
    conn_opts.MQTTVersion = MQTTVERSION_3_1_1;
    
    ssl_opts = MQTTClient_SSLOptions_initializer;
    ssl_opts.keyStore = MQTT_CERT_FILE;
    ssl_opts.privateKey = MQTT_KEY_FILE;
    ssl_opts.enableServerCertAuth = 1;
    ssl_opts.verify = 1;
    ssl_opts.sslVersion = MQTT_SSL_VERSION_TLS_1_2;
    
    conn_opts.ssl = &ssl_opts;
}

// Callback for connection lost
void connection_lost(void* context, char* cause) {
    std::cerr << "Connection lost! Cause: " << (cause ? cause : "unknown") << std::endl;
}

// Callback for message delivery
int message_delivered(void* context, MQTTClient_deliveryToken token) {
    std::cout << "Message delivered (token: " << token << ")" << std::endl;
    return 1;
}

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
int main(int argc, char* argv[]) {
    MQTTClient client;
    MQTTClient_connectOptions conn_opts;
    MQTTClient_SSLOptions ssl_opts;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;
    
    // Set up signal handler for graceful shutdown
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Create MQTT client
    rc = MQTTClient_create(&client, 
                          MQTT_HOST_NAME,
                          MQTT_CLIENT_ID,
                          MQTTCLIENT_PERSISTENCE_NONE,
                          NULL);
    
    if (rc != MQTTCLIENT_SUCCESS) {
        std::cerr << "Failed to create client, return code: " << rc << std::endl;
        return rc;
    }
    
    // Set callbacks
    rc = MQTTClient_setCallbacks(client, NULL, connection_lost, msgarrvd, delivered);
    if (rc != MQTTCLIENT_SUCCESS) {
        std::cerr << "Failed to set callbacks, return code: " << rc << std::endl;
        MQTTClient_destroy(&client);
        return rc;
    }
    
    // Set up connection options with SSL
    setup_connection_options(conn_opts, ssl_opts);
    
    // Connect to the broker
    rc = MQTTClient_connect(client, &conn_opts);
    if (rc != MQTTCLIENT_SUCCESS) {
        std::cerr << "Failed to connect, return code: " << rc << std::endl;
        MQTTClient_destroy(&client);
        return rc;
    }
    
    std::cout << "Connected to MQTT broker successfully" << std::endl;
    
    // Main loop
    while (keep_running) {
        std::string payload = "vvv_paho_c_vvv"; // Placeholder payload
        std::cout << payload << std::endl;
        
        pubmsg.payload = (void*)payload.c_str();
        pubmsg.payloadlen = static_cast<int>(payload.length());
        pubmsg.qos = QOS_LEVEL;
        pubmsg.retained = 0;
        
        rc = MQTTClient_publishMessage(client, PUB_TOPIC, &pubmsg, &token);
        if (rc != MQTTCLIENT_SUCCESS) {
            std::cerr << "Failed to publish message, return code: " << rc << std::endl;
            break;
        }
        
        // Wait for message to be delivered
        rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
        if (rc != MQTTCLIENT_SUCCESS) {
            std::cerr << "Message delivery failed, return code: " << rc << std::endl;
            break;
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    
    // Cleanup
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    
    return rc;
}