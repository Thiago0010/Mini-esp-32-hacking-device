# Mini ESP32 Lab - Loader

Loader/menu touch para ESP32, no mesmo espírito do
[Mini-esp-32-hacking-device](https://github.com/Thiago0010/Mini-esp-32-hacking-device):
tela inicial com grade de ícones, toque para abrir um app, botão pra
voltar. A diferença é a arquitetura: aqui **cada funcionalidade é um
módulo plugável**, isolado em sua própria pasta, e o loader (`main.cpp`)
nunca precisa ser editado.

> ⚠️ **Uso**: este projeto vem **sem nenhuma funcionalidade ofensiva
> implementada** — todos os 21 módulos são cascas vazias (`TODO`).
> Vários dos recursos da sua lista (deauther, evil portal, BLE
> spam/BadUSB, beacon spam) só devem ser usados em **rede e
> dispositivos que você possui ou tem autorização explícita para
> testar**. Fora de laboratório controlado, isso pode ser crime
> (interceptação, interferência em telecomunicações, etc.) — vale a
> pena reforçar isso com os alunos antes de liberar o hardware.

## Estrutura

```
platformio.ini
src/
  main.cpp                  <- loader (NÃO precisa editar)
  modules_list.h/.cpp       <- ÚNICO arquivo que você edita p/ plugar módulo
  core/
    AppModule.h              <- interface que todo módulo implementa
    ModuleRegistry.h/.cpp    <- registro genérico dos módulos
    UI.h/.cpp                <- desenho do menu + botão voltar (genérico)
  modules/
    dns_server/dns_server.h .cpp
    ping_scanner/...
    port_scanner/...
    ftp_client/...
    ota_update/...
    ble_attack/...
    ble_sniffer/...
    bad_usb/...
    ble_mouse/...
    ble_keyboard/...
    ble_spam/...
    ble_advertise/...
    ble_scanner/...
    captive_portal/...
    evil_portal/...
    beacon_spam/...
    wifi_deauther/...
    wifi_detector/...
    wifi_scanner/...
    ir_learn/...
    ir_receiver/...
```

## Como implementar uma funcionalidade

Cada módulo é uma classe que implementa 4 métodos (`AppModule.h`):

- `begin(tft)` — chamado 1x quando o app abre. Desenhe a tela inicial,
  inicie WiFi/BLE/servidor/etc.
- `loop(tft)` — chamado continuamente enquanto o app está aberto.
  Coloque aqui a lógica "de fundo" (scan, sniffing, envio de pacotes...).
- `onTouch(tft, x, y)` — chamado quando o usuário toca a tela dentro do
  app (botões próprios da sua tela, listas, etc.).
- `end()` — chamado 1x ao voltar pro menu. Pare rádio/servidor, libere
  memória.

Exemplo (`src/modules/dns_server/dns_server.cpp`) já vem pronto como
esqueleto. Só editar dentro das 4 funções — não precisa mexer em mais
nada para essa função aparecer funcionando dentro do app.

## Como adicionar um módulo NOVO do zero

1. Copie uma pasta existente em `src/modules/` como base (ex:
   `dns_server` → `meu_modulo`), renomeie os arquivos e a classe.
2. Em `src/modules_list.cpp`, adicione:
   - `#include "modules/meu_modulo/meu_modulo.h"`
   - `ModuleRegistry::registerModule(new MeuModuloModule());`
3. Compile e ele já aparece sozinho no menu — **`main.cpp` continua
   intocado**.

## Hardware / build

- `platformio.ini` já está configurado para uma placa **CYD
  (ESP32-2432S028R)** com display ILI9341 320x240 + touch resistivo
  XPT2046 — é a base mais comum pra esse tipo de projeto e mantém a
  "mesma lógica de touch" do repositório de referência.
- Se você usa outra placa/tela, ajuste os pinos em `platformio.ini`
  (seção `build_flags`) e os pinos do touch em `src/main.cpp` (defines
  `XPT2046_*`).
- Se o toque estiver "desalinhado" na tela, calibre ajustando os
  valores de `map()` em `mapTouchX` / `mapTouchY` no `main.cpp`.
- `MAX_MODULES` (em `core/ModuleRegistry.h`) está em 32 — dá folga
  para os 21 módulos + expansão futura.

## Build

```bash
pio run                # compila
pio run -t upload      # grava no ESP32
pio device monitor      # log serial (115200)
```
