# 🌡️ Aquecedor Smart — ESP32 + OLED + Web Interface

O **Aquecedor Smart** é um sistema de monitoramento de temperatura diferencial baseado em ESP32. Ele realiza a leitura de dois sensores de temperatura **DS18B20** (um quente e um frio), exibe as informações em um display **OLED SSD1306** e disponibiliza um painel de controle interativo via **Web Server (HTML/CSS/JS)** com atualização em tempo real via API JSON.

---

## 🚀 Funcionalidades

- 📊 **Leitura Diferencial de Temperatura:** Medição contínua do sensor quente, sensor frio e cálculo do Delta T ($\Delta T$).
- 📱 **Display OLED Local:** Exibição clara e rápida das leituras atuais direto no dispositivo.
- 🌐 **Interface Web Responsiva:** Dashboard moderno (Modo Escuro) para acompanhar as temperaturas via navegador de qualquer dispositivo na mesma rede.
- 📶 **Gerenciador de Wi-Fi Inteligente (WiFiManager):** Não requer senha hardcoded no código. Ao ligar sem conexão, ele cria um ponto de acesso para configuração simplificada.
- 🔄 **Atualização Dinâmica:** O painel Web consulta a API `/dados` a cada 1 segundo sem recarregar a página.

---

## 🛠️ Hardware Utilizado

| Componente | Especificação / Observação |
| :--- | :--- |
| **Microcontrolador** | ESP32 Dev Module |
| **Display** | OLED 0.96" I2C (128x64px, Controlador SSD1306) |
| **Sensores** | 2x Sensores de Temperatura Impermeáveis DS18B20 |
| **Resistor** | Resistor de 4.7kΩ (Pull-up na linha de dados do DS18B20) |

---

## 📌 Esquemático de Conexões (Pinagem)

```text
[ ESP32 ]               [ Display OLED I2C ]
  GPIO 21 (SDA)  -------> SDA
  GPIO 22 (SCL)  -------> SCL
  3V3 / 5V       -------> VCC
  GND            -------> GND

[ ESP32 ]               [ Sensores DS18B20 ]
  GPIO 04        -------> Dados (DQ) *Requer Resistor 4.7k para VCC
  3V3            -------> VCC
  GND            -------> GND
```

---

## 📚 Bibliotecas Necessárias

Para compilar este código na Arduino IDE, você precisará instalar as seguintes bibliotecas através do **Gerenciador de Bibliotecas** (`Rascunho > Incluir Biblioteca > Gerenciar Bibliotecas`):

1. **Adafruit SSD1306** (por Adafruit)
2. **Adafruit GFX Library** (por Adafruit)
3. **OneWire** (por Jim Studt, Paul Stoffregen, etc.)
4. **DallasTemperature** (por Miles Burton)
5. **WiFiManager** (por tzapu)

---

## ⚙️ Configuração Prévia (Importante!)

Os sensores **DS18B20** possuem endereços físicos únicos de 64 bits (`DeviceAddress`). Antes de carregar o código final para a placa:

1. Rode um sketch de leitura de endereço (como o *DS18B20 Address Finder*) para identificar os endereços dos seus dois sensores.
2. Substitua os valores das variáveis `sensorQuente` e `sensorFrio` no código pelo endereço correspondente aos seus componentes:

```cpp
// ⚠️ Substitua pelos endereços Hexadecimais dos seus sensores
DeviceAddress sensorQuente = { 0x28, 0x69, 0xBE, 0x24, 0x00, 0x00, 0x00, 0xD8 };
DeviceAddress sensorFrio   = { 0x28, 0x1F, 0xBB, 0x24, 0x00, 0x00, 0x00, 0x20 };
```

---

## 📲 Como Usar

1. Faça o upload do código para o ESP32 usando a **Arduino IDE**.
2. Ao ligar a placa pela primeira vez, ela criará uma rede Wi-Fi com o nome **`Aquecedor_Config`**.
3. Conecte-se a essa rede no celular ou computador e configure o nome e a senha do seu Wi-Fi doméstico no portal que será aberto automaticamente.
4. Após conectar, o IP do ESP32 será exibido tanto na **Monitor Serial** quanto no **Display OLED**.
5. Abra o navegador e acesse o endereço IP informado (ex: `http://192.168.1.100`) para ver a interface em funcionamento.
