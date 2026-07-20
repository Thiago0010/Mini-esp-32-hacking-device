#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>

// ============================================================
//  AppModule
//  Interface base para TODOS os módulos/apps do sistema.
//
//  Cada funcionalidade (DNS Server, Ping Scanner, BLE Spam, etc.)
//  deve ter uma classe que herda de AppModule, implementada em
//  seu próprio par de arquivos dentro de src/modules/<nome>/.
//
//  Você NUNCA precisa editar main.cpp para adicionar um módulo
//  novo — só o arquivo do próprio módulo + uma linha em
//  src/modules_list.cpp (ver esse arquivo).
// ============================================================
class AppModule {
public:
    virtual ~AppModule() {}

    // Nome exibido no ícone do menu (curto, cabe pouco espaço)
    virtual const char* getName() = 0;

    // Cor de destaque do ícone no menu (formato RGB565 do TFT_eSPI)
    virtual uint16_t getColor() { return TFT_DARKGREY; }

    // Chamado UMA VEZ quando o usuário toca no ícone e abre o app.
    // Use para desenhar a tela inicial e iniciar o que for preciso.
    virtual void begin(TFT_eSPI &tft) = 0;

    // Chamado repetidamente (a cada iteração do loop principal)
    // enquanto o app estiver aberto.
    virtual void loop(TFT_eSPI &tft) = 0;

    // Chamado quando o usuário toca na tela DENTRO do app.
    // x, y já vêm convertidos para coordenadas de tela (0..largura/altura).
    virtual void onTouch(TFT_eSPI &tft, int16_t x, int16_t y) {}

    // Chamado UMA VEZ quando o usuário aperta "voltar" e sai do app.
    // Use para parar servidores, desligar rádio, liberar memória, etc.
    virtual void end() {}
};
