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
| **Resistor** | Resistor de $4.7k\Omega$ (Pull-up na linha de dados do DS18B20) |

---

## 📌 Esquemático de Conexões (Pinagem)
