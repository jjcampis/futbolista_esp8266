// Basado en tu archivo original, ahora usando una variable global 'modo'
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncWebSocketClient *clienteConectado = nullptr;

String modo = "S"; // S = stop por defecto
unsigned long ultimaAccion = 0;
unsigned long intervalo = 100; // cada 100ms se ejecuta la acción

void avanzar() {
  Serial.println("Avanzando");
  // Código para avanzar
}
void retroceder() {
  Serial.println("Retrocediendo");
  // Código para retroceder
}
void izquierda() {
  Serial.println("Izquierda");
  // Código para girar a la izquierda
}
void derecha() {
  Serial.println("Derecha");
  // Código para girar a la derecha
}
void detener() {
  Serial.println("Detenido");
  // Código para detener
}

void ejecutarModo() {
  if (modo == "F") avanzar();
  else if (modo == "B") retroceder();
  else if (modo == "L") izquierda();
  else if (modo == "R") derecha();
  else if (modo == "F,L") { avanzar(); izquierda(); }
  else if (modo == "F,R") { avanzar(); derecha(); }
  else if (modo == "B,L") { retroceder(); izquierda(); }
  else if (modo == "B,R") { retroceder(); derecha(); }
  else if (modo == "CD") { avanzar(); derecha(); }
  else if (modo == "CI") { avanzar(); izquierda(); }
  else if (modo == "GD") { derecha(); }
  else if (modo == "GI") { izquierda(); }
  else detener();
}

void interpretarComando(String cmd) {
  modo = cmd;
  ultimaAccion = millis();
  Serial.println("Nuevo modo: " + modo);
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    if (clienteConectado != nullptr && clienteConectado->status() == WS_CONNECTED) {
      client->close();
      Serial.println("Otro cliente intentó conectarse y fue rechazado.");
    } else {
      clienteConectado = client;
      Serial.printf("Cliente conectado: #%u\n", client->id());
    }
  } else if (type == WS_EVT_DISCONNECT) {
    if (client == clienteConectado) {
      clienteConectado = nullptr;
      Serial.println("Cliente desconectado");
      modo = "S";
    }
  } else if (type == WS_EVT_DATA) {
    if (client == clienteConectado) {
      String msg;
      for (size_t i = 0; i < len; i++) msg += (char)data[i];
      interpretarComando(msg);
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.softAP("Autito", "12345678");
  Serial.println(WiFi.softAPIP());

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.begin();
}

void loop() {
  if (millis() - ultimaAccion >= intervalo) {
    ejecutarModo();
    ultimaAccion = millis();
  }
}