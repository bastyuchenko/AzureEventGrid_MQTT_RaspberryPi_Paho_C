# Setup Environment

| [Create CA](#create-ca) | [Configure Event Grid](#configure-event-grid-namespace) | [Development tools](#configure-development-tools) |

Once your environment is configured you can configure your connection settings as environment variables that will be loaded by the [Mqtt client extensions](./mqttclients/README.md)

### Create CA

All samples require a CA to generate the client certificates to connect.

- Follow this link to install the `step cli`: [https://smallstep.com/docs/step-cli/installation/](https://smallstep.com/docs/step-cli/installation/)
- To create the root and intermediate CA certificates run:

```bash
step ca init \
    --deployment-type standalone \
    --name MqttAppSamplesCA \
    --dns localhost \
    --address 127.0.0.1:443 \
    --provisioner MqttAppSamplesCAProvisioner
```

Follow the cli instructions, when done make sure you remember the password used to protect the private keys, by default the generated certificates are stored in:

- `~/.step/certs/root_ca.crt`
- `~/.step/certs/intermediate_ca.crt`
- `~/.step/secrets/root_ca_key`
- `~/.step/secrets/intermediate_ca_key`

### My Setup
✔ Root certificate: /home/abastiuchenko/.step/certs/root_ca.crt  
✔ Root private key: /home/abastiuchenko/.step/secrets/root_ca_key  
✔ Root fingerprint: 03dfd990759ff3e1cec95da4c4dd747b307080983c0b228b1663721e3dbd873e  
✔ Intermediate certificate: /home/abastiuchenko/.step/certs/intermediate_ca.crt  
✔ Intermediate private key: /home/abastiuchenko/.step/secrets/intermediate_ca_key  
✔ Database folder: /home/abastiuchenko/.step/db  
✔ Default configuration: /home/abastiuchenko/.step/config/defaults.json  
✔ Certificate Authority configuration: /home/abastiuchenko/.step/config/ca.json  

## Configure Event Grid Namespace


### Configure environment variables

Create or update `az.env` file under MQTTApplicationSamples folder that includes an existing subscription, an existing resource group, and a new name of your choice for the Event Grid Namespace as follows:

```text
sub_id=<subscription-id>
rg=<resource-group-name>
name=<event-grid-namespace>
res_id="/subscriptions/${sub_id}/resourceGroups/${rg}/providers/Microsoft.EventGrid/namespaces/${name}"
```

To run the `az` cli:
- Install [AZ CLI](https://learn.microsoft.com/cli/azure/install-azure-cli)
- Authenticate using  `az login`.
- If the above does not work use `az login --use-device-code`

```bash
source az.env

az account set -s $sub_id
az resource create --id $res_id --is-full-object --properties '{
  "properties": {
    "isZoneRedundant": true,
    "topicsConfiguration": {
      "inputSchema": "CloudEventSchemaV1_0"
    },
    "topicSpacesConfiguration": {
      "state": "Enabled"
    }
  },
  "location": "westus2"
}'
```

Register the certificate to authenticate client certificates (usually the intermediate)

```bash
source az.env

capem=`cat ~/.step/certs/intermediate_ca.crt | tr -d "\n"`

az resource create \
  --id "$res_id/caCertificates/Intermediate01" \
  --properties "{\"encodedCertificate\" : \"$capem\"}"
```
> Each scenario includes the detailed instructions to configure the namespace resources needed for the scenario.

> [!NOTE]
> For portal configuration, use [this link](https://portal.azure.com/?microsoft_azure_marketplace_ItemHideKey=PubSubNamespace&microsoft_azure_eventgrid_assettypeoptions={"PubSubNamespace":{"options":""}}) and follow [these instructions](https://learn.microsoft.com/en-us/azure/event-grid/mqtt-publish-and-subscribe-portal).

### C

We are using standard C, and CMake to build. These are the required tools:
- [CMake](https://cmake.org/download/) Version 3.20 or higher to use CMake presets
- [Ninja build system](https://github.com/ninja-build/ninja/releases) Version 1.10 or higher
- GNU C++ compiler
- SSL
- [JSON-C](https://github.com/json-c/json-c) if running a sample that uses JSON - currently these are the Telemetry Samples
- UUID Library (if running a sample that uses correlation IDs - currently these are the Command Samples)
- [protobuf-c](https://github.com/protobuf-c/protobuf-c) If running a sample that uses protobuf - currently these are the Command Samples. Note that you'll need protobuf-c-compiler and libprotobuf-dev as well if you're generating code for new proto files.

An example of installing these tools (other than CMake) is shown below:

``` bash
# If running a sample that uses JSON
sudo apt-get install libjson-c-dev
# If running a sample that uses Correlation IDs
sudo apt-get install uuid-dev
# If running a sample that uses protobuf
sudo apt-get install libprotobuf-c-dev
```

## Setup Paho MQTT Embedded Client lobrary
sudo apt-get update
sudo apt-get install libpaho-mqtt-dev

### compli app
gcc -o your_program your_program.c -lpaho-mqtt3c

### use CMake file

cmake_minimum_required(VERSION 3.5)
project(YourProgramName VERSION 1.0.0 LANGUAGES C)

# Find the Paho MQTT C library
find_library(PAHO_MQTT_C paho-mqtt3c)

# Add your executable
add_executable(your_program your_program.c)

# Link your program with the Paho MQTT C library
target_link_libraries(your_program "${PAHO_MQTT_C}")


find_library(PAHO_MQTT_C NAMES paho-mqtt3c
             HINTS "/path/to/paho/installation")


See [c extensions](./mqttclients/c/README.md) for more details.