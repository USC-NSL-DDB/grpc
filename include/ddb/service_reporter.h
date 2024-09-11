#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <MQTTClient.h>
#include <ddb/simple_ini.h>

// #define ADDRESS     "tcp://10.10.1.2:10101"
#define CLIENTID    "service_reporter"
#define INI_FILEPATH "/tmp/ddb/service_discovery/config.ini"
// #define T_SERVICE_DISCOVERY "service_discovery/report"
#define QOS         1 // at least once
#define TIMEOUT     10000L

typedef struct {
    uint32_t ip;    // ip address
    char* tag;      // tag name
    pid_t pid;      // process ID
} ServiceInfo;

typedef struct {
    MQTTClient client; // client for pub
    char* address;     // broker address
    char* topic;       // topic for pub
} DDBServiceReporter;

static inline int read_ini_data(DDBServiceReporter* reporter) {
    CSimpleIniA ini;
	ini.SetUnicode();

	int rc = ini.LoadFile(INI_FILEPATH);
	if (rc < 0) { 
        printf("Failed to load serviec discovery configuration file: config.ini\n");
        return rc;
    };

    const char* transport;
	transport = ini.GetValue("broker", "transport");
    if (transport == NULL) {
        printf("Failed to get broker transport\n");
        return -1;
    }

    const char* host;
	host = ini.GetValue("broker", "host");
    if (host == NULL) {
        printf("Failed to get broker host\n");
        return -1;
    }

    const char* port;
	port = ini.GetValue("broker", "port");
    if (port == NULL) {
        printf("Failed to get broker host\n");
        return -1;
    }

    const char* topic;
    topic = ini.GetValue("broker", "topic");
    if (topic == NULL) {
        printf("Failed to get broker topic\n");
        return -1;
    }

    reporter->topic = strdup(topic);
    if (reporter->topic == NULL) {
        printf("Failed to allocate memory for topic\n");
        return -1;
    }

    char address[1024]; // Hardcoded buffer size
    snprintf(address, sizeof(address), "%s://%s:%s", transport, host, port);
    reporter->address = strdup(address);
    if (reporter->address == NULL) {
        printf("Failed to allocate memory for broker address\n");
        return -1;
    }
}

static inline int service_reporter_init(DDBServiceReporter* reporter) {
    int rc = read_ini_data(reporter);
    if (rc != 0) return rc;

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_create(&reporter->client, reporter->address, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    if ((rc = MQTTClient_connect(reporter->client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        return rc;
    }
    return 0;
}

static inline int service_reporter_deinit(DDBServiceReporter* reporter) {
    MQTTClient_disconnect(reporter->client, 10000);
    MQTTClient_destroy(&reporter->client);
    return 0;
}

static inline int report_service(
    DDBServiceReporter* reporter, 
    const ServiceInfo* service_info
) {
    int rc;

    MQTTClient_message pubmsg = MQTTClient_message_initializer;

    char payload[256];

    // Format the ServiceInfo struct fields into the buffer
    snprintf(payload, sizeof(payload), "%u:%s:%d",
             service_info->ip, service_info->tag, service_info->pid);

    pubmsg.payload = (void*) payload;
    pubmsg.payloadlen = (int) strlen(payload);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(reporter->client, T_SERVICE_DISCOVERY, &pubmsg, &token);
    // printf("Waiting for up to %d seconds for publication of %s\n"
    //        "on topic %s for client with ClientID: %s\n",
    //        (int)(TIMEOUT/1000), PAYLOAD, TOPIC, CLIENTID);
    rc = MQTTClient_waitForCompletion(reporter->client, token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", token);
    return rc;
}

#ifdef __cplusplus
}
#endif
