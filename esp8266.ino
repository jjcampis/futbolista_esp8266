#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Pines L293D
//#define MOTOR_A_PWM 5 //D2  // GPIO4
//#define MOTOR_A_DIR 0 //D3  // GPIO0
//#define MOTOR_B_PWM 4 //D1  // GPIO5
//#define MOTOR_B_DIR 2 //D4  // GPIO2
#define MOTOR_A_PWM D1  // GPIO5
#define MOTOR_B_PWM D2  // GPIO4
#define MOTOR_A_DIR D3  // GPIO0
#define MOTOR_B_DIR D4  // GPIO2

const char* ssid = "AutitoJJ";
const char* password = "12345678";
String ultimoModo = "S";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

AsyncWebSocketClient* clienteConectado = nullptr;

// Variables de velocidad
int pwmCurva = 512;  // Valor inicial medio
int pwmGiro = 512;

void detener() {
  analogWrite(MOTOR_A_PWM, 0);
  analogWrite(MOTOR_B_PWM, 0);
}

void avanzar() {
  digitalWrite(MOTOR_A_DIR, HIGH);
  digitalWrite(MOTOR_B_DIR, LOW);
  analogWrite(MOTOR_A_PWM, 100);
  analogWrite(MOTOR_B_PWM, 100);
}

void retroceder() {
  digitalWrite(MOTOR_A_DIR, LOW);
  digitalWrite(MOTOR_B_DIR, HIGH);
  analogWrite(MOTOR_A_PWM, 100);
  analogWrite(MOTOR_B_PWM, 100);
}

/*void izquierda() {
  digitalWrite(MOTOR_A_DIR, HIGH);
  analogWrite(MOTOR_A_PWM, pwmCurva);
  analogWrite(MOTOR_B_PWM, 0);
}

void derecha() {
  digitalWrite(MOTOR_B_DIR, LOW);
  analogWrite(MOTOR_B_PWM, pwmCurva);
  analogWrite(MOTOR_A_PWM, 0);
}*/

void giroDerecha() {
  digitalWrite(MOTOR_A_DIR, HIGH); // adelante
  digitalWrite(MOTOR_B_DIR, HIGH); // atrás
  analogWrite(MOTOR_A_PWM, pwmGiro);
  analogWrite(MOTOR_B_PWM, pwmGiro);
}

void giroIzquierda() {
  digitalWrite(MOTOR_A_DIR, LOW);  // atrás
  digitalWrite(MOTOR_B_DIR, LOW);  // adelante
  analogWrite(MOTOR_A_PWM, pwmGiro);
  analogWrite(MOTOR_B_PWM, pwmGiro);
}

void curvaDerecha() {
  digitalWrite(MOTOR_A_DIR, HIGH); // Avanza
  digitalWrite(MOTOR_B_DIR, LOW);
  analogWrite(MOTOR_A_PWM, pwmCurva);
  analogWrite(MOTOR_B_PWM, 100);
}

void curvaIzquierda() {
  digitalWrite(MOTOR_A_DIR, HIGH);
  digitalWrite(MOTOR_B_DIR, LOW);
  analogWrite(MOTOR_A_PWM, 100);
  analogWrite(MOTOR_B_PWM, pwmCurva);
}
void curvaIzquierdaAtras() {
  digitalWrite(MOTOR_A_DIR, LOW);   // Motor A retrocede
  digitalWrite(MOTOR_B_DIR, HIGH);  // Motor B retrocede
  analogWrite(MOTOR_A_PWM, 100);        // lado rápido (izquierdo)
  analogWrite(MOTOR_B_PWM, pwmCurva);   // lado lento (derecho)
}

void curvaDerechaAtras() {
  digitalWrite(MOTOR_A_DIR, LOW);   // Motor A retrocede
  digitalWrite(MOTOR_B_DIR, HIGH);  // Motor B retrocede
  analogWrite(MOTOR_A_PWM, pwmCurva);   // lado lento (izquierdo)
  analogWrite(MOTOR_B_PWM, 100);        // lado rápido (derecho)
}

void interpretarComando(String cmd) {
  if (cmd == "F") avanzar();
  else if (cmd == "B") retroceder();
  else if (cmd == "L") giroIzquierda();
  else if (cmd == "R") giroDerecha();
  else if (cmd == "S") detener();
  else if (cmd == "F,L") { curvaIzquierda(); }
  else if (cmd == "F,R") { curvaDerecha(); }
  else if (cmd == "B,L") { curvaIzquierdaAtras(); }
  else if (cmd == "B,R") { curvaDerechaAtras(); }
  else if (cmd == "CD") { curvaDerecha(); ultimoModo = cmd; }
  else if (cmd == "CI") { curvaIzquierda(); ultimoModo = cmd; }
  else if (cmd == "GD") { giroDerecha(); ultimoModo = cmd; }
  else if (cmd == "GI") { giroIzquierda(); ultimoModo = cmd; }
  else if (cmd.startsWith("PWM_curva,")) {
    pwmCurva = constrain(cmd.substring(10).toInt(), 0, 100);
    Serial.print("PWM curva: "); Serial.println(pwmCurva);
    (ultimoModo  == "CD") ? curvaDerecha() : curvaIzquierda();
  }
  else if (cmd.startsWith("PWM_giro,")) {
    pwmGiro = constrain(cmd.substring(9).toInt(), 0, 100);
    Serial.print("PWM giro: "); Serial.println(pwmGiro);
    (ultimoModo  == "GD") ? giroDerecha() : giroIzquierda();
  }
  else {
    Serial.println("Comando no reconocido: " + cmd);
    detener();
  }
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    if (clienteConectado != nullptr) {
      if (clienteConectado->status() == WS_CONNECTED) {
        client->close();
        Serial.println("❌ Otro cliente intentó conectarse y fue rechazado.");
        return;
      } else {
        // El cliente anterior estaba desconectado, lo liberamos
        clienteConectado = nullptr;
        Serial.println("⚠️ Cliente anterior inválido liberado.");
      }
    }
    // Aceptamos nuevo cliente
    clienteConectado = client;
    Serial.printf("✅ Cliente conectado: #%u\n", client->id());
  }

  else if (type == WS_EVT_DISCONNECT) {
    if (client == clienteConectado) {
      clienteConectado = nullptr;
      Serial.println("🔌 Cliente desconectado");
      detener();
    }
  }

  else if (type == WS_EVT_DATA) {
    if (client == clienteConectado) {
      String msg;
      for (size_t i = 0; i < len; i++) msg += (char)data[i];
      msg.trim();
      Serial.println("📨 Comando recibido: " + msg);
      interpretarComando(msg);
    }
  }
}

void setup() {
  Serial.begin(115200);
  analogWriteRange(100);
  pinMode(MOTOR_A_PWM, OUTPUT);
  pinMode(MOTOR_A_DIR, OUTPUT);
  pinMode(MOTOR_B_PWM, OUTPUT);
  pinMode(MOTOR_B_DIR, OUTPUT);
  detener();

  WiFi.softAP(ssid, password);
  Serial.println("🚗 Autito en AP listo: " + WiFi.softAPIP().toString());

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  server.begin();
}

void loop() {
  // Nada necesario
}