# futbolista_esp8266

Proyecto para controlar un robot con dos motores utilizando un ESP8266 y un driver L293D. El microcontrolador expone un punto de acceso WiFi y un servidor WebSocket para recibir órdenes desde una interfaz web ligera.

## Características
- Punto de acceso WiFi autónomo (`AutitoJJ` / `12345678`).
- Control bidireccional de motores a través del integrado L293D.
- Interfaz web adaptable para enviar comandos desde un dispositivo móvil.
- Ajuste de velocidad para giros y curvas mediante sliders.

## Estructura del repositorio
```
├── arduino/esp8266.ino   # Firmware del ESP8266
├── Web/                  # Interfaz web estática
│   ├── index.html
│   ├── estilo.css
│   └── img/
└── README.md
```

## Requisitos
### Hardware
- ESP8266 (por ejemplo, NodeMCU o WeMOS D1 Mini).
- Driver de motores L293D.
- Dos motores DC y alimentación adecuada.

### Software
- [Arduino IDE](https://www.arduino.cc/en/software) o plataforma compatible.
- Librerías:
  - `ESP8266WiFi`
  - `ESPAsyncTCP`
  - `ESPAsyncWebServer`

## Instalación y uso
1. Clona este repositorio y abre `arduino/esp8266.ino` en el IDE.
2. Instala las librerías indicadas y carga el sketch en el ESP8266.
3. Conecta tu dispositivo al WiFi `AutitoJJ` con la clave `12345678`.
4. Abre `Web/index.html` en el navegador del dispositivo conectado.
5. Desde la interfaz podrás enviar comandos como `F`, `B`, `L`, `R` o `S` para controlar el movimiento. Los botones de la interfaz también permiten ajustes de giro (`GI`/`GD`) y de curva (`CI`/`CD`).

## Contribuciones
Las contribuciones y mejoras son bienvenidas. Si encuentras un problema o tienes una idea para mejorar el proyecto, abre un issue o envía un pull request.

