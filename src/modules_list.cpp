#include "modules_list.h"
#include "core/ModuleRegistry.h"

// ============================================================
//  modules_list.cpp
//  UNICO arquivo que voce edita para "plugar" um modulo novo
//  no sistema. Main.cpp nunca precisa mudar.
//
//  Passo a passo para adicionar uma funcionalidade nova:
//   1) Crie a pasta src/modules/nome_da_funcao/ com
//      nome_da_funcao.h e nome_da_funcao.cpp (copie um dos
//      modulos existentes como base).
//   2) Adicione o #include dela aqui embaixo.
//   3) Adicione uma linha em registerAllModules() aqui embaixo.
//   Pronto - ela aparece sozinha no menu.
// ============================================================

#include "modules/dns_server/dns_server.h"
#include "modules/ping_scanner/ping_scanner.h"
#include "modules/port_scanner/port_scanner.h"
#include "modules/ftp_client/ftp_client.h"
#include "modules/ota_update/ota_update.h"
#include "modules/ble_attack/ble_attack.h"
#include "modules/ble_sniffer/ble_sniffer.h"
#include "modules/bad_usb/bad_usb.h"
#include "modules/ble_mouse/ble_mouse.h"
#include "modules/ble_keyboard/ble_keyboard.h"
#include "modules/ble_spam/ble_spam.h"
#include "modules/ble_advertise/ble_advertise.h"
#include "modules/ble_scanner/ble_scanner.h"
#include "modules/captive_portal/captive_portal.h"
#include "modules/evil_portal/evil_portal.h"
#include "modules/beacon_spam/beacon_spam.h"
#include "modules/wifi_deauther/wifi_deauther.h"
#include "modules/wifi_detector/wifi_detector.h"
#include "modules/wifi_scanner/wifi_scanner.h"
#include "modules/ir_learn/ir_learn.h"
#include "modules/ir_receiver/ir_receiver.h"

void registerAllModules() {
    ModuleRegistry::registerModule(new DnsServerModule());
    ModuleRegistry::registerModule(new PingScannerModule());
    ModuleRegistry::registerModule(new PortScannerModule());
    ModuleRegistry::registerModule(new FtpClientModule());
    ModuleRegistry::registerModule(new OtaUpdateModule());
    ModuleRegistry::registerModule(new BleAttackModule());
    ModuleRegistry::registerModule(new BleSnifferModule());
    ModuleRegistry::registerModule(new BadUsbModule());
    ModuleRegistry::registerModule(new BleMouseModule());
    ModuleRegistry::registerModule(new BleKeyboardModule());
    ModuleRegistry::registerModule(new BleSpamModule());
    ModuleRegistry::registerModule(new BleAdvertiseModule());
    ModuleRegistry::registerModule(new BleScannerModule());
    ModuleRegistry::registerModule(new CaptivePortalModule());
    ModuleRegistry::registerModule(new EvilPortalModule());
    ModuleRegistry::registerModule(new BeaconSpamModule());
    ModuleRegistry::registerModule(new WifiDeautherModule());
    ModuleRegistry::registerModule(new WifiDetectorModule());
    ModuleRegistry::registerModule(new WifiScannerModule());
    ModuleRegistry::registerModule(new IrLearnModule());
    ModuleRegistry::registerModule(new IrReceiverModule());
}
