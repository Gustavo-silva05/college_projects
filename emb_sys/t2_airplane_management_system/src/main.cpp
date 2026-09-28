#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "FS.h"
#include "SPIFFS.h"
#include <PubSubClient.h>
#include <Wire.h>
// #include "GY521.h"
#include <ESP32Servo.h>

// ================= WiFi (AP/STA fallback, credenciais na NVS) =================
String ssid = "";
String password = "";
Preferences memoria;
AsyncWebServer server(80);
DNSServer dnsServer;
bool captivePortalActive = false;

// ================= MQTT =================
const char *mqtt_broker = "broker.emqx.io";
const int mqtt_port = 1883;
#define MQTT_ID "esp32_G1"         // <-- TROCAR: precisa ser UNICO no broker
#define TOPICO_PUB "Embarcados/G1" // <-- TROCAR: conforme o numero do seu grupo

WiFiClient espClient;
PubSubClient MQTT(espClient);

// ================= Pinos =================
const byte xAxisPin = 34;  // joystick eixo X (acelera/desacelera)
const byte yAxisPin = 35;  // joystick eixo Y (sobe/desce)
const byte buttonPin = 32; // joystick SW (inicia percurso)
const byte servoPin = 18;  // servo do profundor/asa

// ================= Sensores/atuadores =================
// GY521 sensor(0x68);
Servo asaServo;

// ================= Estado compartilhado entre as tasks =================
volatile float g_pitch = 0;
volatile float g_altitude = 4000.0;  // parte de 4000m
volatile float g_velocidade = 200.0; // parte de 200km/h
volatile float g_distancia = 0.0;
volatile bool g_started = false;

float servoAngle = 90.0; // 90 = nivelado

const float ANGLE_STEP = 0.5, ANGLE_MIN = 60.0, ANGLE_MAX = 120.0;
const float SPEED_STEP = 0.5, SPEED_MIN = 160.0, SPEED_MAX = 240.0;

SemaphoreHandle_t dataMutex;

// ============================================================
// Portal de configuracao
// ============================================================
void PaginaSalva(AsyncWebServerRequest *request)
{
  if (request->hasArg("ssid") && request->hasArg("password"))
  {
    String NovoSSID = request->arg("ssid");
    String NovaSenha = request->arg("password");
    memoria.begin("wifi", false);
    memoria.putString("ssid", NovoSSID);
    memoria.putString("password", NovaSenha);
    memoria.end();
    request->send(200, "text/html", "<h3>Configuracao salva! Reinicie o ESP32.</h3>");
    delay(1000);
    ESP.restart();
  }
  else
  {
    request->send(400, "text/plain", "Erro: parametros invalidos");
  }
}

void PaginaConfig(AsyncWebServerRequest *request)
{
  request->send(SPIFFS, "/config.html", "text/html");
}

// ============================================================
// WiFi manager
// ============================================================
bool isIp(String str)
{
  for (size_t i = 0; i < str.length(); i++)
  {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9'))
      return false;
  }
  return true;
}

void setupAP()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP32_GustavoSilva", "0123456789");

  vTaskDelay(pdMS_TO_TICKS(100)); // Estabiliza a interface AP

  IPAddress apIP = WiFi.softAPIP();
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(53, "*", apIP);
  captivePortalActive = true;

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("config.html");
  server.on("/save", HTTP_POST, PaginaSalva);

  server.onNotFound([apIP](AsyncWebServerRequest *r)
                    {
  if (!isIp(r->host())) {                       // probes de conectividade / domínios externos
    auto *res = r->beginResponse(302, "text/plain", "");
    res->addHeader("Location", "http://" + apIP.toString() + "/");
    res->addHeader("Cache-Control", "no-store");
    r->send(res);
    return;
  }
  if (r->method() == HTTP_GET && r->url().indexOf('.') < 0) {
  if (SPIFFS.exists("/config.html")) r->send(SPIFFS, "/config.html", "text/html");
  else r->send(500, "text/plain", "config.html ausente no SPIFFS");
  } });

  server.begin(); // <-- obrigatório
  Serial.print("AP iniciado. IP: ");
  Serial.println(apIP);
}

void setupSTA()
{
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.printf("Conectando em %s...\n", ssid.c_str());
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000)
  {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nConectado com sucesso!");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("\nFalha ao conectar, iniciando AP novamente...");
    setupAP();
  }
}

void conectaWifi()
{
  if (WiFi.status() == WL_CONNECTED || captivePortalActive)
    return;
  setupSTA();
}

// ============================================================
// MQTT
// ============================================================
void conectaBroker()
{
  while (!MQTT.connected())
  {
    if (MQTT.connect(MQTT_ID))
    {
      Serial.println("Conectado ao Broker!");
    }
    else
    {
      Serial.print("Falha na conexao. Status: ");
      Serial.println(MQTT.state());
      vTaskDelay(pdMS_TO_TICKS(2000));
    }
  }
}

void publicaDados()
{
  float pitch, altitude, velocidade, distancia;

  xSemaphoreTake(dataMutex, portMAX_DELAY);
  pitch = g_pitch;
  altitude = g_altitude;
  velocidade = g_velocidade;
  distancia = g_distancia;
  xSemaphoreGive(dataMutex);

  char payload[160];
  snprintf(payload, sizeof(payload),
           "{\"pitch\":%.1f,\"altitude\":%.0f,\"velocidade\":%.0f,\"distanciaPercorrida\":%.0f}",
           pitch, altitude, velocidade, distancia);

  MQTT.publish(TOPICO_PUB, payload);
  Serial.print("Publicado: ");
  Serial.println(payload);
}

// ============================================================
// Task de controle: joystick + IMU + servo (core 0)
// ============================================================
// void TaskControl(void *pvParameters) {
//   pinMode(buttonPin, INPUT_PULLUP);
//   analogSetPinAttenuation(xAxisPin, ADC_11db);
//   analogSetPinAttenuation(yAxisPin, ADC_11db);

//   Wire.begin();

//   if (sensor.begin() == false) {
//     Serial.println("Nao foi possivel conectar ao GY521");
//     while (1) {
//       vTaskDelay(pdMS_TO_TICKS(1000));
//     }
//   }

//   sensor.setAccelSensitivity(0);   // +/- 2g
//   sensor.setGyroSensitivity(0);    // +/- 250 deg/s
//   sensor.setThrottle(20);          // ~50Hz

//   Serial.println("Deixe a placa horizontal e parada");
//   vTaskDelay(pdMS_TO_TICKS(2000));
//   sensor.axe = 0;
//   sensor.aye = 0;
//   sensor.aze = 0;
//   sensor.gxe = 0;
//   sensor.gye = 0;
//   sensor.gze = 0;
//   Serial.println("Pronto!");

//   asaServo.attach(servoPin);
//   asaServo.write((int)servoAngle);

//   const float dt = 0.05; // 50Hz
//   TickType_t lastWake = xTaskGetTickCount();

//   for (;;) {
//     int xValue = analogRead(xAxisPin);
//     int yValue = analogRead(yAxisPin);
//     bool buttonPressed = (digitalRead(buttonPin) == LOW);

//     int deltaX = xValue - 2048;
//     int deltaY = yValue - 2048;
//     if (abs(deltaX) < 300) deltaX = 0;
//     if (abs(deltaY) < 300) deltaY = 0;

//     if (buttonPressed && !g_started) {
//       g_started = true;
//       Serial.println("Percurso iniciado!");
//     }

//     if (deltaY > 0)      servoAngle = min(servoAngle + ANGLE_STEP, ANGLE_MAX);
//     else if (deltaY < 0) servoAngle = max(servoAngle - ANGLE_STEP, ANGLE_MIN);
//     asaServo.write((int)servoAngle);

//     xSemaphoreTake(dataMutex, portMAX_DELAY);
//     float velocidadeLocal = g_velocidade;
//     if (deltaX > 0)      velocidadeLocal = min(velocidadeLocal + SPEED_STEP, SPEED_MAX);
//     else if (deltaX < 0) velocidadeLocal = max(velocidadeLocal - SPEED_STEP, SPEED_MIN);
//     g_velocidade = velocidadeLocal;
//     xSemaphoreGive(dataMutex);

//     sensor.read();
//     float pitch = -(sensor.getAngleY());

//     xSemaphoreTake(dataMutex, portMAX_DELAY);
//     g_pitch = pitch;
//     if (g_started) {
//       float v_ms = g_velocidade * 1000.0 / 3600.0;
//       g_altitude   += v_ms * sin(radians(pitch)) * dt;
//       g_distancia  += v_ms * dt;
//     }
//     xSemaphoreGive(dataMutex);

//     vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(50));
//   }
// }

// ============================================================
// Task de rede: wifi + mqtt (core 1)
// ============================================================
void TaskMQTT(void *pvParameters)
{
  MQTT.setServer(mqtt_broker, mqtt_port);

  for (;;)
  {
    if (WiFi.status() != WL_CONNECTED)
      conectaWifi();
    if (!MQTT.connected())
      conectaBroker();

    static unsigned long pooling = 0;
    if (millis() > pooling + 1000)
    {
      pooling = millis();
      publicaDados();
    }
    MQTT.loop();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// ============================================================
void setup()
{
  Serial.begin(115200);

  dataMutex = xSemaphoreCreateMutex();

  if (!SPIFFS.begin(true))
    Serial.println("Erro SPIFFS");
  File root = SPIFFS.open("/");
  for (File f = root.openNextFile(); f; f = root.openNextFile())
    Serial.printf("%s  %u bytes\n", f.path(), f.size());

  memoria.begin("wifi", true);
  ssid = memoria.getString("ssid", "");
  password = memoria.getString("password", "");
  memoria.end();

  if (ssid == "" || password == "")
  {
    setupAP();
  }
  else
  {
    setupSTA();
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    // xTaskCreatePinnedToCore(TaskControl, "TaskControl", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(TaskMQTT, "TaskMQTT", 4096, NULL, 1, NULL, 1);
  }
}

void loop()
{
  if (captivePortalActive)
  {
    dnsServer.processNextRequest();
  }
  vTaskDelay(pdMS_TO_TICKS(10));
}