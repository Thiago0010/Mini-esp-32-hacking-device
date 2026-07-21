# [NEV]² - NEV-OS CyberLab v3.0

> Projeto educacional de cybersecurity para ESP32 + TFT 320x240 + Touch XPT2046.
> Desenvolvido para uso em laboratório e ensino de segurança ofensiva a alunos.

⚠️ **ADVERTÊNCIA LEGAL**: Use este projeto **apenas** em redes e equipamentos
próprios ou com autorização explícita. Pentesting sem autorização é crime
(Art. 154-A do Código Penal brasileiro; Computer Fraud and Abuse Act nos EUA;
equivalentes em outros países). O autor não se responsabiliza por uso indevido.

---

## 📋 Mapa de Menus

```
MENU PRINCIPAL
├── Wi-Fi Tools
│   ├── Wi-Fi Scanner        - Lista redes próximas (SSID, Ch, RSSI, enc)
│   ├── Deauth Attack        - Deautentica clientes de uma rede alvo
│   ├── Captive Portal       - Sobe AP aberto com página de login
│   ├── Evil Portal          - AP com templates de phishing (Starbucks, etc)
│   ├── Beacon Spam          - Inundar o ar com SSIDs fake
│   ├── Wi-Fi Detector       - Modo monitor passivo (probe requests)
│   ├── Ping Scanner         - Varre /24 e mostra hosts ativos
│   ├── Port Scanner         - TCP connect scan em portas comuns
│   ├── FTP Client           - USER/PASS/LIST básico
│   └── OTA Update           - Atualiza firmware via Wi-Fi (Arduino IDE)
│
├── BLE Tools
│   ├── BLE Scanner          - Lista dispositivos BLE próximos
│   ├── BLE Spam             - Popups iOS/Android (Apple/Continuity, Samsung)
│   ├── BLE Advertise        - Custom ADV com nome editável
│   ├── BLE Sniffer          - Captura passiva de advertisements
│   ├── BLE Attack           - DoS por flood de ADV packets
│   ├── BLE Mouse [exp.]     - HID mouse (requer lib NimBLE-Mouse-Keyboard)
│   └── BLE Keyboard [exp.]  - HID keyboard (mesma limitação)
│
├── Misc & Demo
│   ├── IR Receive           - Captura sinais de controles remotos
│   ├── IR Replay            - Reenvia o último sinal salvo (TX LED IR)
│   ├── SD Browser           - Lista arquivos no cartão SD
│   ├── Cyber Pong           - Jogo Pong com o touch
│   └── BadUSB [demo]        - Stub didático (ESP32-WROOM não tem USB)
│
└── Settings
    ├── Wi-Fi Config         - (em breve)
    ├── About                - Info do firmware
    └── Reboot               - Reinicia o ESP32
```

---

## 🎮 Controles

| Toque | Ação |
|---|---|
| **Curto** (< 500ms) | Avança item / próxima página / reset |
| **Longo** (≥ 500ms) | Entra no item / executa ação |
| **Longo no BACK** (canto sup. dir.) | Volta ao menu anterior |

---

## 🔧 Instalação

### 1. Hardware Necessário
- **ESP32** (WROOM, S2, S3, C3 - testado em WROOM)
- **TFT 320x240** com chip ILI9341 ou ST7789 (SPI)
- **Touch XPT2046** (compartilha SPI)
- **Cartão SD** (formato FAT32)
- **Receptor IR** TSOP1738 (GPIO 27)
- **LED IR + transistor 2N2222 + resistor 100Ω** (GPIO 25) - para IR Replay
- Cabo USB

### 2. Bibliotecas (Arduino IDE → Library Manager)
- `TFT_eSPI` (Bodmer)
- `IRremoteESP8266` (crankyoldgit) → v2.8.6+
- `NimBLE-Arduino` (h2zero) → v1.4.0+
- `ESP32Ping` (marian-craciunescu)

Já inclusas no ESP32 Arduino Core: `WiFi`, `WebServer`, `DNSServer`, `SD`, `ArduinoOTA`

### 3. Configurar TFT_eSPI
Edite `~/Arduino/libraries/TFT_eSPI/User_Setup.h` para o seu display:
```cpp
#define ILI9341_DRIVER
#define TFT_CS   5
#define TFT_DC   2
#define TFT_MOSI 23
#define TFT_SCLK 18
```

### 4. Compilar e Gravar
1. Abra `mainhck.ino` no Arduino IDE
2. Selecione **Tools → Board → ESP32 Dev Module**
3. Selecione a porta serial correta
4. **Sketch → Upload**

A Arduino IDE compila **todos os arquivos da pasta** juntos (`.ino` + `.h`).

---

## 🔄 Atualização OTA (rede)

Após a primeira gravação via USB:
1. Conecte o ESP32 a uma rede Wi-Fi (via Settings → Wi-Fi Config — em breve)
2. No Arduino IDE: **Tools → Port → Network Ports → mainhck**
3. Upload normal — sem cabo USB

---

## 📁 Estrutura do Código

```
mainhck/
├── mainhck.ino         ← setup() + loop() principal (~200 linhas)
├── config.h            ← Pinos, cores, constantes
├── types.h             ← Estruturas e enums
├── assets.h            ← Bitmaps PROGMEM
├── touch_driver.h      ← Touch XPT2046 (NÃO ALTERADO)
├── ui_helpers.h        ← drawHeader, drawStatusBar, etc
├── menu_system.h       ← MenuItem tree + navegação
├── module_ir.h         ← IR RX + IR TX novo
├── module_sd.h         ← SD card browser
├── module_game.h       ← Cyber Pong
├── module_wifi_*.h     ← 10 módulos Wi-Fi
├── module_ble_*.h      ← 7 módulos BLE
├── module_badusb.h     ← Stub didático
└── README.md           ← Este arquivo
```

---

## 🐛 Bugs Corrigidos (vs v2.0)

1. ✅ `_networks[16]` agora declarado em `types.h`
2. ✅ AP de captive não sobe mais automaticamente no setup
3. ✅ `dnsServer.processNextRequest()` agora é chamado no loop
4. ✅ `isAttacking` agora muda via botão na tela de deauth
5. ✅ `STATE_WIFI_DEAUTH_PORTAL` substituído por states específicos
6. ✅ `enterWifiConstruction` removido (labels errados)
7. ✅ `renderCurrentView` reescrito como dispatchers por estado
8. ✅ Sistema de menu baseado em árvore de MenuItem (escala 20+ itens)
9. ✅ `ITEMS_PER_PAGE` dinâmico (não mais hardcoded 2)
10. ✅ `WiFi.mode()` e `NimBLEDevice::deinit()` chamados ao sair de cada feature

---

## 🎓 Para Educadores

O projeto é dividido de forma que cada `module_*.h` é **didático**:
- Tem o que faz de verdade
- Tem comentários `TODO` mostrando o que pode evoluir
- Tem stubs honestos quando o hardware não permite (BadUSB, BLE Mouse)

Sugestões de aula:
- **Aula 1**: Wi-Fi Scanner + visualizar como o canal afeta RSSI
- **Aula 2**: Captive Portal + entender DNS hijacking
- **Aula 3**: Deauth + frames 802.11
- **Aula 4**: IR Receber + Replay = clonar controles remotos
- **Aula 5**: BLE Scanner + entender advertising

---

## 📜 Licença

Projeto pessoal do Thiago. Use livremente para fins educacionais.
Créditos do design "LOPAKA" ao autor original do layout de telas.
