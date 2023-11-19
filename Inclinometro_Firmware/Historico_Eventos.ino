#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include <WiFi.h>

const char* ssid = "LIVE TIM_0820_2G";
const char* pwd = "a3ehn6rep6";

#define INFLUXDB_URL "https://us-east-1-1.aws.cloud2.influxdata.com"
#define INFLUXDB_TOKEN "X0zO3ujnfr20JXsm3HnCaatkWWW0xtf_VG9XRuj33as8Kb-zG1CGRPpfczL0xyNwmXZ2UaS2m65PITWf1ePskA=="
#define INFLUXDB_ORG "9f389c7f82a02efa"
#define INFLUXDB_BUCKET "IncliMax"
#define TZ_INFO "BRST+3BRDT+2,M10.3.0,M2.3.0"
//#define TZ_INFO "UTC-3"
#define DEVICE "ESP32"
Point angulos("Angulos");

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);


void Init_Events() {
  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  //timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
  //timeSync(TZ_INFO, "ntp.cais.rnp.br", "pool.ntp.br");
  timeSync(TZ_INFO, "a.st1.ntp.br", "time.nis.gov");

  angulos.addTag("device", DEVICE);

  // Check server connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

void Add_Event(String event) {

    // if(Wifi.status == WL_CONNECTED){
    //   switch(event){
    //     case EVT_BLOQUEIO:

    //       break;
    //   }
    // }

  angulos.clearFields();
  angulos.addField("AnguloLateral", AnguloLateral, 2);
  angulos.addField("AnguloFrontal", AnguloFrontal, 2);

  Serial.print("Writing: ");
  Serial.println(client.pointToLineProtocol(angulos));

  if (!client.writePoint(angulos)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}
