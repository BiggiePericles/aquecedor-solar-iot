#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <WiFi.h>
#include <WebServer.h>
#include <WiFiManager.h>

// =========================================================
//                    CONFIGURAÇÕES
// =========================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define PINO_DS18B20 4
#define ENDERECO_OLED 0x3C

// Atualização dos sensores
const unsigned long INTERVALO_SENSOR = 1000;

// =========================================================
//              ENDEREÇOS DOS DS18B20
// =========================================================

// 🔥 SENSOR QUENTE
DeviceAddress sensorQuente = {
  0x28,
  0x69,
  0xBE,
  0x24,
  0x00,
  0x00,
  0x00,
  0xD8
};

// ❄️ SENSOR FRIO
DeviceAddress sensorFrio = {
  0x28,
  0x1F,
  0xBB,
  0x24,
  0x00,
  0x00,
  0x00,
  0x20
};

// =========================================================
//                         OLED
// =========================================================

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// =========================================================
//                    DS18B20 / ONE WIRE
// =========================================================

OneWire oneWire(PINO_DS18B20);

DallasTemperature sensores(&oneWire);

// =========================================================
//                    SERVIDOR WEB
// =========================================================

WebServer server(80);

// =========================================================
//                         WI-FI
// =========================================================

String redeAtual = "";

// =========================================================
//                    TEMPERATURAS
// =========================================================

float tempQuente = DEVICE_DISCONNECTED_C;
float tempFrio = DEVICE_DISCONNECTED_C;
float deltaT = 0.0;

// =========================================================
//                    CONTROLE DE TEMPO
// =========================================================

unsigned long ultimoUpdate = 0;

// =========================================================
//                    PROTÓTIPOS
// =========================================================

void lerSensores();
void atualizarOLED();
void handleRoot();
void handleDados();
String formatarTempo();

// =========================================================
//                         SETUP
// =========================================================

void setup() {

  Serial.begin(115200);

  delay(500);

  Serial.println();
  Serial.println("================================");
  Serial.println("       AQUECEDOR SMART");
  Serial.println("================================");

  // =======================================================
  //                    INICIA SENSORES
  // =======================================================

  sensores.begin();

  Serial.print("Sensores encontrados: ");
  Serial.println(sensores.getDeviceCount());

  // =======================================================
  //                    INICIA OLED
  // =======================================================

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        ENDERECO_OLED
      )) {

    Serial.println("ERRO: OLED nao encontrado!");

    for (;;) {
      delay(100);
    }
  }

  display.clearDisplay();
  display.setTextColor(WHITE);

  // =======================================================
  //                    TELA INICIAL
  // =======================================================

  display.setTextSize(1);
  display.setCursor(22, 5);
  display.println("Aquecedor Smart");

  display.setCursor(30, 25);
  display.println("A iniciar...");

  display.display();

  delay(1000);

  // =======================================================
  //                    WIFI MANAGER
  // =======================================================

 // =======================================================
  //                    WIFI MANAGER (INTELIGENTE)
  // =======================================================


WiFi.mode(WIFI_STA);
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Conectando Wi-Fi...");
  display.display();

  WiFiManager wifiManager;

  // IMPORTANTE: Define um tempo limite (ex: 3 minutos / 180 segundos)
  // Assim, se ninguém configurar a rede, ele não fica bloqueado para sempre
  wifiManager.setConfigPortalTimeout(180);

  // Tenta ligar à rede guardada. Se não encontrar, cria o portal "Aquecedor_Config"
  if (!wifiManager.autoConnect("Aquecedor_Config")) {
    
    Serial.println("Falha ao conectar ou tempo esgotado. A reiniciar...");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Falha no Wi-Fi!");
    display.setCursor(0, 20);
    display.println("A reiniciar...");
    display.display();
    
    delay(3000);
    ESP.restart(); // Reinicia a placa para tentar novamente
  }

  // Se o código chegou a esta linha, é porque conseguiu ligar-se a alguma rede!
  redeAtual = WiFi.SSID();

  Serial.println();
  Serial.println("Wi-Fi conectado!");
  Serial.print("Rede: ");
  Serial.println(redeAtual);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // =======================================================
  //                  WIFI CONECTADO
  // =======================================================

  redeAtual = WiFi.SSID();

  Serial.println();
  Serial.println("Wi-Fi conectado!");

  Serial.print("Rede: ");
  Serial.println(redeAtual);

  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // =======================================================
  //                    ROTAS WEB
  // =======================================================

  server.on("/", handleRoot);
  server.on("/dados", handleDados);

  server.begin();

  Serial.println();
  Serial.println("Servidor Web iniciado!");

  // =======================================================
  //                    PRIMEIRA LEITURA
  // =======================================================

  lerSensores();
  atualizarOLED();
}

// =========================================================
//                          LOOP
// =========================================================

void loop() {

  // Mantém o servidor Web funcionando
  server.handleClient();

  // =======================================================
  //              ATUALIZAÇÃO DOS SENSORES
  // =======================================================

  unsigned long agora = millis();

  if (agora - ultimoUpdate >= INTERVALO_SENSOR) {

    ultimoUpdate = agora;

    lerSensores();
    atualizarOLED();

    // =====================================================
    //                  MONITOR SERIAL
    // =====================================================

    Serial.print("Quente: ");

    if (tempQuente != DEVICE_DISCONNECTED_C) {
      Serial.print(tempQuente, 1);
      Serial.print(" C");
    } else {
      Serial.print("ERRO");
    }

    Serial.print(" | Frio: ");

    if (tempFrio != DEVICE_DISCONNECTED_C) {
      Serial.print(tempFrio, 1);
      Serial.print(" C");
    } else {
      Serial.print("ERRO");
    }

    Serial.print(" | Delta: ");

    if (
      tempQuente != DEVICE_DISCONNECTED_C &&
      tempFrio != DEVICE_DISCONNECTED_C
    ) {

      Serial.print(deltaT, 1);
      Serial.print(" C");

    } else {
      Serial.print("ERRO");
    }

    Serial.println();
  }
}

// =========================================================
//                    LER SENSORES
// =========================================================

void lerSensores() {

  sensores.requestTemperatures();

  // Sensor Quente
  tempQuente = sensores.getTempC(sensorQuente);

  // Sensor Frio
  tempFrio = sensores.getTempC(sensorFrio);

  // Delta T
  if (
    tempQuente != DEVICE_DISCONNECTED_C &&
    tempFrio != DEVICE_DISCONNECTED_C
  ) {

    deltaT = tempQuente - tempFrio;

  } else {

    deltaT = 0.0;
  }
}

// =========================================================
//                    ATUALIZAR OLED
// =========================================================

void atualizarOLED() {

  display.clearDisplay();

  // =======================================================
  //                       TÍTULO
  // =======================================================

  display.setTextSize(1);
  display.setCursor(22, 0);
  display.print("Aquecedor Smart");

  display.drawLine(
    0,
    10,
    127,
    10,
    WHITE
  );

  // =======================================================
  //                    SENSOR QUENTE
  // =======================================================

  display.setTextSize(1);
  display.setCursor(8, 18);
  display.print("QUENTE");

  display.setTextSize(2);
  display.setCursor(0, 30);

  if (tempQuente != DEVICE_DISCONNECTED_C) {

    display.print(tempQuente, 1);
    display.print("C");

  } else {

    display.print("ERRO");
  }

  // =======================================================
  //                     SENSOR FRIO
  // =======================================================

  display.setTextSize(1);
  display.setCursor(82, 18);
  display.print("FRIO");

  display.setTextSize(2);
  display.setCursor(68, 30);

  if (tempFrio != DEVICE_DISCONNECTED_C) {

    display.print(tempFrio, 1);
    display.print("C");

  } else {

    display.print("ERRO");
  }

  // Atualiza OLED
  display.display();
}

// =========================================================
//                    PÁGINA WEB
// =========================================================

void handleRoot() {

  String html = R"rawliteral(

<!DOCTYPE html>

<html>

<head>

<meta charset="UTF-8">

<meta name="viewport"
content="width=device-width, initial-scale=1">

<title>Aquecedor Smart</title>

<style>

* {
  box-sizing: border-box;
}

body {

  margin: 0;

  padding: 20px;

  font-family: Arial, sans-serif;

  background: #101010;

  color: white;

}

.container {

  max-width: 500px;

  margin: auto;

}

h1 {

  text-align: center;

  color: #00bfff;

  margin-bottom: 25px;

}

.card {

  background: #1d1d1d;

  border-radius: 15px;

  padding: 22px;

  margin-bottom: 15px;

  box-shadow:
    0 4px 10px
    rgba(0,0,0,0.35);

  text-align: center;

}

.titulo {

  font-size: 18px;

  color: #aaa;

  margin-bottom: 10px;

}

.temperatura {

  font-size: 42px;

  font-weight: bold;

  margin: 10px 0;

}

.quente {

  color: #ff6347;
}

.frio {

  color: #1e90ff;
}

.delta {

  font-size: 48px;

  font-weight: bold;

  color: #00ff99;

}

.unidade {

  font-size: 16px;

  color: #aaa;

}

.info {

  display: flex;

  align-items: center;

  justify-content: center;

  gap: 8px;

  color: #aaa;

  margin-top: 12px;

  font-size: 14px;

}

.status {

  text-align: center;

  padding: 12px;

  border-radius: 10px;

  background: #151515;

  color: #00ff99;

  margin-bottom: 15px;

}

.erro {

  color: #ff4444;

}

.rodape {

  text-align: center;

  color: #666;

  font-size: 12px;

  margin-top: 20px;

}

svg {

  flex-shrink: 0;

}

</style>

</head>

<body>

<div class="container">

<h1>Aquecedor Smart</h1>

<!-- ================================================= -->
<!-- SENSOR QUENTE -->
<!-- ================================================= -->

<div class="card">

<div class="titulo">

🔥 Sensor Quente

</div>

<div
id="quente"
class="temperatura quente">

--

</div>

<div class="unidade">

°C

</div>

</div>

<!-- ================================================= -->
<!-- SENSOR FRIO -->
<!-- ================================================= -->

<div class="card">

<div class="titulo">

❄️ Sensor Frio

</div>

<div
id="frio"
class="temperatura frio">

--

</div>

<div class="unidade">

°C

</div>

</div>

<!-- ================================================= -->
<!-- DELTA T -->
<!-- ================================================= -->

<div class="card">

<div class="titulo">

Diferença de temperatura

</div>

<div
id="delta"
class="delta">

--

</div>

<div class="unidade">

°C

</div>

</div>

<!-- ================================================= -->
<!-- STATUS -->
<!-- ================================================= -->

<div
id="status"
class="status">

● Sistema funcionando

</div>

<!-- ================================================= -->
<!-- INFORMAÇÕES -->
<!-- ================================================= -->

<div class="card">

<!-- WIFI -->

<div class="info">

<svg
width="20"
height="20"
viewBox="0 0 24 24"
fill="none"
stroke="#00bfff"
stroke-width="2.5"
stroke-linecap="round"
stroke-linejoin="round">

<path
d="M5 12.55a11 11 0 0 1 14.08 0">
</path>

<path
d="M1.42 9a16 16 0 0 1 21.16 0">
</path>

<path
d="M8.53 16.11a6 6 0 0 1 6.95 0">
</path>

<line
x1="12"
y1="20"
x2="12.01"
y2="20">
</line>

</svg>

<span id="rede">
Wi-Fi
</span>

</div>

<!-- IP -->

<div class="info">

IP:

<span id="ip">
--
</span>

</div>

<!-- UPTIME -->

<div class="info">

Tempo ligado:

<span id="uptime">
--
</span>

</div>

</div>

<div class="rodape">

Atualização automática a cada 1 segundo

</div>

</div>

<script>

function atualizarDados() {

  fetch('/dados')

  .then(response => response.json())

  .then(data => {

    // =================================================
    // SENSOR QUENTE
    // =================================================

    const quente =
      document.getElementById('quente');

    if (data.quente_ok) {

      quente.innerHTML =
        data.quente.toFixed(1);

      if (data.quente >= 25) {

        quente.style.color =
          "#ff6347";

      } else {

        quente.style.color =
          "#1e90ff";
      }

    } else {

      quente.innerHTML =
        "ERRO";

      quente.style.color =
        "#ff4444";
    }

    // =================================================
    // SENSOR FRIO
    // =================================================

    const frio =
      document.getElementById('frio');

    if (data.frio_ok) {

      frio.innerHTML =
        data.frio.toFixed(1);

      if (data.frio >= 25) {

        frio.style.color =
          "#ff6347";

      } else {

        frio.style.color =
          "#1e90ff";
      }

    } else {

      frio.innerHTML =
        "ERRO";

      frio.style.color =
        "#ff4444";
    }

    // =================================================
    // DELTA T
    // =================================================

    const delta =
      document.getElementById('delta');

    if (
      data.quente_ok &&
      data.frio_ok
    ) {

      delta.innerHTML =
        data.delta.toFixed(1);

    } else {

      delta.innerHTML =
        "--";
    }

    // =================================================
    // STATUS
    // =================================================

    const status =
      document.getElementById('status');

    if (
      data.quente_ok &&
      data.frio_ok
    ) {

      status.innerHTML =
        "● Sistema funcionando";

      status.className =
        "status";

    } else {

      status.innerHTML =
        "⚠ Verificar sensores";

      status.className =
        "status erro";
    }

    // =================================================
    // WIFI
    // =================================================

    document.getElementById('rede')
      .innerHTML =
      data.rede;

    // =================================================
    // IP
    // =================================================

    document.getElementById('ip')
      .innerHTML =
      data.ip;

    // =================================================
    // UPTIME
    // =================================================

    document.getElementById('uptime')
      .innerHTML =
      data.uptime;

  })

  .catch(error => {

    const status =
      document.getElementById('status');

    status.innerHTML =
      "⚠ Sem comunicação";

    status.className =
      "status erro";

  });

}

// Primeira atualização
atualizarDados();

// Atualiza a cada 1 segundo
setInterval(
  atualizarDados,
  1000
);

</script>

</body>

</html>

)rawliteral";

  server.send(
    200,
    "text/html",
    html
  );
}

// =========================================================
//                    API DE DADOS
// =========================================================

void handleDados() {

  bool quenteOK =
    tempQuente !=
    DEVICE_DISCONNECTED_C;

  bool frioOK =
    tempFrio !=
    DEVICE_DISCONNECTED_C;

  String json = "{";

  // =======================================================
  //                    SENSOR QUENTE
  // =======================================================

  json += "\"quente_ok\":";
  json += quenteOK ? "true" : "false";
  json += ",";

  if (quenteOK) {

    json += "\"quente\":";
    json += String(tempQuente, 1);

  } else {

    json += "\"quente\":null";
  }

  json += ",";

  // =======================================================
  //                     SENSOR FRIO
  // =======================================================

  json += "\"frio_ok\":";
  json += frioOK ? "true" : "false";
  json += ",";

  if (frioOK) {

    json += "\"frio\":";
    json += String(tempFrio, 1);

  } else {

    json += "\"frio\":null";
  }

  json += ",";

  // =======================================================
  //                       DELTA T
  // =======================================================

  if (
    quenteOK &&
    frioOK
  ) {

    json += "\"delta\":";
    json += String(deltaT, 1);

  } else {

    json += "\"delta\":null";
  }

  json += ",";

  // =======================================================
  //                         REDE
  // =======================================================

  json += "\"rede\":\"";
  json += redeAtual;
  json += "\",";

  // =======================================================
  //                          IP
  // =======================================================

  json += "\"ip\":\"";
  json += WiFi.localIP().toString();
  json += "\",";

  // =======================================================
  //                       UPTIME
  // =======================================================

  json += "\"uptime\":\"";
  json += formatarTempo();
  json += "\"";

  // =======================================================
  //                         FINAL
  // =======================================================

  json += "}";

  server.send(
    200,
    "application/json",
    json
  );
}

// =========================================================
//                    FORMATAR TEMPO
// =========================================================

String formatarTempo() {

  unsigned long segundos =
    millis() / 1000;

  unsigned long horas =
    segundos / 3600;

  segundos %= 3600;

  unsigned long minutos =
    segundos / 60;

  segundos %= 60;

  char buffer[20];

  snprintf(
    buffer,
    sizeof(buffer),
    "%02lu:%02lu:%02lu",
    horas,
    minutos,
    segundos
  );

  return String(buffer);
}
