# ESP32 DMX-Scheinwerfersteuerung

Firmware für ein normales **ESP32-DevKit (ESP32-WROOM-32, 4 MB Flash)**. Der ESP32 verbindet sich mit dem WLAN **gastro** (Passwort `gastroGH29`) und stellt eine Weboberfläche bereit. Die Lichtdaten gehen als **DMX512** über einen RS485-Baustein an die Scheinwerfer.

## 1. Verkabelung

### Benötigte Teile
- ESP32-DevKit (z. B. „ESP32 DevKitC V4“ oder „ESP32-WROOM-32 NodeMCU“)
- RS485-Modul: am besten **MAX3485** (läuft mit 3,3 V). Ein **MAX485**-Modul (5 V) geht auch, siehe Hinweis unten.
- XLR-Buchse (female), 3-polig oder 5-polig
- optional: 120-Ω-Widerstand als Abschluss am letzten Scheinwerfer

### ESP32 → RS485-Modul

| ESP32 | RS485-Modul | Funktion |
|---|---|---|
| **GPIO17** (TX2) | **DI** | DMX-Daten |
| **GPIO4** | **DE** und **RE** (beide zusammen brücken) | Sender dauerhaft an |
| **3V3** | **VCC** | Versorgung (MAX3485) |
| **GND** | **GND** | Masse |
| GPIO2 | (Onboard-LED) | Status-LED |

`RO` vom Modul bleibt frei (es wird nur gesendet).

**MAX485 mit 5 V:** VCC an **VIN/5V** des DevKits. DI und DE/RE vertragen die 3,3-V-Pegel des ESP32 problemlos. Wichtig: **RO nicht** an den ESP32 anschließen, denn dort kämen 5 V heraus.

### RS485-Modul → XLR (DMX-Standard)

| XLR-Pin | Signal | RS485-Modul |
|---|---|---|
| 1 | Masse/Schirm | GND |
| 2 | Data − | **B** |
| 3 | Data + | **A** |

Bei 5-poligem XLR sind Pin 4 und 5 frei. Am **letzten** Scheinwerfer der Kette sollte ein 120-Ω-Abschlusswiderstand zwischen Pin 2 und 3 sitzen. Viele RS485-Module haben schon einen 120-Ω-Widerstand (R7) eingebaut, das ist am Anfang der Kette auch in Ordnung.

Die Pins lassen sich später in der Oberfläche unter **System → DMX** ändern.

## 2. Flashen

Datei: **`esp32-dmx-merged.bin`**. Sie enthält Bootloader, Partitionstabelle und Programm und wird an **Adresse 0x0** geschrieben.

### Variante A: im Browser (Chrome/Edge, ohne Installation)
1. ESP32 per USB anschließen.
2. https://espressif.github.io/esptool-js/ öffnen, Baudrate 921600, **Connect** und den Port wählen.
3. Flash Address **0x0**, die Datei `esp32-dmx-merged.bin` wählen und auf **Program** klicken.
4. Wenn die Verbindung nicht klappt, beim Verbinden die **BOOT**-Taste gedrückt halten.

### Variante B: esptool (Kommandozeile)
```
pip install esptool
esptool.py --chip esp32 --baud 921600 write_flash 0x0 esp32-dmx-merged.bin
```
Wenn vorher schon etwas anderes drauf war, vorher einmal `esptool.py erase_flash` ausführen.

Spätere Updates gehen ohne Kabel über **System → Firmware-Update** mit der Datei `esp32-dmx-update.bin`.

## 3. Verbinden

- Handy oder Laptop ins WLAN **gastro** und dann **http://dmx.local** öffnen. Auf Android geht `.local` oft nicht. Die IP siehst du im Router oder im seriellen Monitor (115200 Baud).
- Findet der ESP32 das WLAN 20 s lang nicht, öffnet er den **Notfall-Access-Point „DMX-Steuerung“** (Passwort `dmx12345`). Nach dem Verbinden öffnet sich die Oberfläche automatisch, sonst http://192.168.4.1 aufrufen.
- Status-LED: leuchtet dauerhaft = mit WLAN verbunden, blinkt langsam = Access-Point, blinkt schnell = verbindet gerade.

## 4. Oberfläche

- **Steuerung:** Scheinwerfer auswählen (lange drücken = Gerät blinkt zur Identifikation), AN/AUS, Farbrad, Farbfelder, Regler für R/G/B/W/Amber/UV, Dimmer, Master, Überblendzeit, Effekte (Regenbogen, Atmen, Lauflicht, Farbwechsel, Feuer, Disco, Blaulicht) mit Tempo, Stroboskop und Blackout.
- **Szenen:** 12 Speicherplätze. Tippen ruft eine Szene ab, lange drücken speichert oder löscht sie.
- **Geräte (DMX-Patch):** bis zu 24 Scheinwerfer mit Name, Startadresse, Kanalzahl und Kanalbelegung. Fertige Profile gibt es für RGB, RGBW, PAR 7-Kanal, RGBWA+UV und Dimmer. Zusätzlich: Warnung bei Adressüberschneidung, automatische Adressvergabe und Auto-Weiß.
- **Kanäle:** einzelne DMX-Kanäle direkt setzen (Kanaltest) und ein Live-Monitor aller 512 Kanäle.
- **System:** WLAN (auch mit fester IP), Pins, Bildrate, Art-Net-Empfang mit Universum (damit lässt sich auch ein Lichtpult oder eine App wie QLC+ über WLAN nutzen), Startverhalten (aus, letzter Zustand oder bestimmte Szene), Backup und Wiederherstellung, Firmware-Update und Werkseinstellungen.

### Scheinwerfer einstellen
Am Scheinwerfer muss die **DMX-Startadresse** (z. B. `A001` / `d001`) und der **Kanal-Modus** (z. B. `7CH`) zur Oberfläche passen. Voreingestellt sind 4 × PAR mit 7 Kanälen (Dimmer, R, G, B, W, Strobe, Modus) auf den Adressen 1, 8, 15 und 22. Die Kanalbelegung steht in der Anleitung des Scheinwerfers. Den Kanal „Modus“ meistens auf Festwert 0 lassen, dann reagiert der Scheinwerfer auf DMX.

## 5. Selbst bauen
`pip install platformio esptool` und dann `./build.sh`. Das Ergebnis liegt in `out/`.
