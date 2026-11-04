// ESP32 DMX-Scheinwerfersteuerung mit Weboberfläche
// DMX512-Ausgabe über UART2 + RS485-Transceiver (MAX485 / MAX3485 / SN75176)
// Standard-Pins: TX = GPIO17, DE/RE = GPIO4, Status-LED = GPIO2

#include <Arduino.h>
#include <initializer_list>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <Preferences.h>
#include <Update.h>
#include <ArduinoJson.h>
#include "driver/uart.h"
#include "esp_system.h"
#include "webui.h"

#define FW_VERSION "1.1.1"
#define MAX_FIX 24
#define MAX_CH 16
#define MAX_SCENES 12
#define DMX_PORT UART_NUM_2
#define CFG_MAGIC 0xD3A51002u
#define AP_IP IPAddress(192, 168, 1, 4)
#define SCN_MAGIC 0x5CE0E001u

// ---------------------------------------------------------------- Typen
enum Fn : uint8_t { F_NONE = 0, F_DIM, F_R, F_G, F_B, F_W, F_A, F_UV, F_STROBE, F_FIXED, F_DIMFINE, F_COUNT };

struct ChanDef { uint8_t fn; uint8_t val; };
struct FixDef {
  char name[24];
  uint16_t addr;      // 1..512
  uint8_t nch;        // 1..MAX_CH
  uint8_t flags;      // bit0 = Auto-Weiß
  ChanDef ch[MAX_CH];
};
struct Config {
  uint32_t magic;
  char ssid[33], pass[65];
  char apSsid[33], apPass[65];
  char host[32];
  uint8_t useStatic;
  uint8_t ip[4], gw[4], mask[4], dns[4];
  uint8_t txPin, dePin, ledPin;
  uint8_t rate;          // DMX-Bildrate (Hz)
  uint8_t artEnable;
  uint16_t artUniverse;
  uint8_t wifiMode;      // 0 = Direktverbindung (eigenes WLAN), 1 = Haus-WLAN (Direkt nur als Notfall)
  uint8_t setupDone;     // 0 = Einrichtungsassistent anzeigen
  uint8_t bootMode;      // 0 = aus, 1 = letzter Zustand, 2 = Szene
  int8_t bootScene;
  uint8_t nfix;
  FixDef fix[MAX_FIX];
};
struct FixState { uint8_t col[6]; uint8_t dim; uint8_t on; };
struct Scene {
  uint32_t magic;
  char name[24];
  uint8_t master, effect, speed, strobe;
  uint16_t fadeMs;
  uint32_t fxMask;
  uint8_t nfix;
  FixState fs[MAX_FIX];
};

// ---------------------------------------------------------------- Globale Daten
Config cfg;
Scene live;                 // aktueller Zustand (Struktur wie Szene)
bool blackout = false;
Preferences prefs;
WebServer server(80);
DNSServer dns;
WiFiUDP udp;
SemaphoreHandle_t lock;

uint8_t dmxOut[513];        // [0] = Startcode
int16_t override_[513];     // -1 = keine manuelle Übersteuerung
uint8_t artBuf[513];
volatile uint32_t artLast = 0;
uint32_t artPackets = 0;

float fCur[MAX_FIX][7], fFrom[MAX_FIX][7];
uint8_t fTgt[MAX_FIX][7];
uint32_t fStart[MAX_FIX];
uint16_t fDur[MAX_FIX];
int identifyFix = -1;
uint32_t identifyUntil = 0;
float fireState[MAX_FIX];

bool apActive = false;
uint32_t staLostSince = 0, staOkSince = 0;
bool stateDirty = false;
uint32_t stateDirtyAt = 0;
bool rebootPending = false;
uint32_t rebootAt = 0;

// ---------------------------------------------------------------- Hilfen
static void setIp(uint8_t *d, uint8_t a, uint8_t b, uint8_t c, uint8_t e) { d[0] = a; d[1] = b; d[2] = c; d[3] = e; }
static String ipStr(const uint8_t *d) { return String(d[0]) + "." + d[1] + "." + d[2] + "." + d[3]; }
static void parseIp(const char *s, uint8_t *d) { IPAddress a; if (s && a.fromString(s)) for (int i = 0; i < 4; i++) d[i] = a[i]; }
static void copyStr(char *dst, const char *src, size_t n) { strncpy(dst, src ? src : "", n - 1); dst[n - 1] = 0; }

static void setProfile(FixDef &f, const char *name, uint16_t addr, std::initializer_list<uint8_t> fns) {
  memset(&f, 0, sizeof(f));
  copyStr(f.name, name, sizeof(f.name));
  f.addr = addr;
  f.nch = 0;
  for (uint8_t fn : fns) { f.ch[f.nch].fn = fn; f.ch[f.nch].val = 0; f.nch++; }
}

void defaultConfig() {
  memset(&cfg, 0, sizeof(cfg));
  cfg.magic = CFG_MAGIC;
  cfg.wifiMode = 0; cfg.setupDone = 0;   // keine WLAN-Daten voreingestellt
  copyStr(cfg.apSsid, "DMX-Steuerung", sizeof(cfg.apSsid));
  copyStr(cfg.apPass, "dmx12345", sizeof(cfg.apPass));
  copyStr(cfg.host, "dmx", sizeof(cfg.host));
  setIp(cfg.ip, 192, 168, 1, 50); setIp(cfg.gw, 192, 168, 1, 1); setIp(cfg.mask, 255, 255, 255, 0); setIp(cfg.dns, 192, 168, 1, 1);
  cfg.txPin = 17; cfg.dePin = 4; cfg.ledPin = 2;
  cfg.rate = 40;
  cfg.artEnable = 0; cfg.artUniverse = 0;
  cfg.bootMode = 1; cfg.bootScene = 0;
  cfg.nfix = 4;
  // Beispiel: 4 RGB-PAR-Scheinwerfer im 7-Kanal-Modus (Dimmer, R, G, B, W, Strobe, Modus)
  for (int i = 0; i < 4; i++) {
    char n[24]; snprintf(n, sizeof(n), "PAR %d", i + 1);
    setProfile(cfg.fix[i], n, 1 + i * 7, {F_DIM, F_R, F_G, F_B, F_W, F_STROBE, F_FIXED});
  }
}

void defaultLive() {
  memset(&live, 0, sizeof(live));
  live.magic = SCN_MAGIC;
  live.master = 255; live.speed = 30; live.fadeMs = 300; live.fxMask = 0xFFFFFFFF;
  live.nfix = MAX_FIX;
  for (int i = 0; i < MAX_FIX; i++) {
    live.fs[i].col[0] = 255; live.fs[i].col[1] = 160; live.fs[i].col[2] = 60;  // Warmweiß
    live.fs[i].dim = 255; live.fs[i].on = 0;
  }
}

void sanitizeConfig() {
  if (cfg.nfix > MAX_FIX) cfg.nfix = MAX_FIX;
  if (cfg.rate < 10 || cfg.rate > 44) cfg.rate = 40;
  for (int i = 0; i < cfg.nfix; i++) {
    FixDef &f = cfg.fix[i];
    f.name[sizeof(f.name) - 1] = 0;
    if (f.nch < 1) f.nch = 1;
    if (f.nch > MAX_CH) f.nch = MAX_CH;
    if (f.addr < 1) f.addr = 1;
    if (f.addr > 512) f.addr = 512;
    for (int c = 0; c < f.nch; c++) if (f.ch[c].fn >= F_COUNT) f.ch[c].fn = F_NONE;
  }
}

void saveConfig() {
  prefs.begin("dmx", false);
  prefs.putBytes("cfg", &cfg, sizeof(cfg));
  prefs.end();
}

void loadConfig() {
  prefs.begin("dmx", true);
  size_t n = prefs.getBytesLength("cfg");
  bool ok = false;
  if (n == sizeof(cfg)) { prefs.getBytes("cfg", &cfg, sizeof(cfg)); ok = cfg.magic == CFG_MAGIC; }
  prefs.end();
  if (!ok) { defaultConfig(); saveConfig(); }
  sanitizeConfig();
}

bool loadSceneSlot(const char *key, Scene &s) {
  prefs.begin("dmx", true);
  bool ok = false;
  if (prefs.getBytesLength(key) == sizeof(Scene)) { prefs.getBytes(key, &s, sizeof(Scene)); ok = s.magic == SCN_MAGIC; }
  prefs.end();
  return ok;
}
void saveSceneSlot(const char *key, const Scene &s) {
  prefs.begin("dmx", false);
  prefs.putBytes(key, &s, sizeof(Scene));
  prefs.end();
}
void deleteSceneSlot(const char *key) {
  prefs.begin("dmx", false);
  prefs.remove(key);
  prefs.end();
}
static String sceneKey(int slot) { return String("s") + slot; }

void markDirty() { stateDirty = true; stateDirtyAt = millis(); }

// ---------------------------------------------------------------- DMX-Ausgabe
static void hsv2rgb(float h, float s, float v, float &r, float &g, float &b) {
  h = fmodf(h, 1.0f); if (h < 0) h += 1.0f;
  float i = floorf(h * 6), f = h * 6 - i, p = v * (1 - s), q = v * (1 - f * s), t = v * (1 - (1 - f) * s);
  switch (((int)i) % 6) {
    case 0: r = v; g = t; b = p; break; case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break; case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break; default: r = v; g = p; b = q; break;
  }
}
static uint32_t hash32(uint32_t x) { x ^= x >> 16; x *= 0x7feb352d; x ^= x >> 15; x *= 0x846ca68b; x ^= x >> 16; return x; }

void renderFrame(uint8_t *out) {
  uint32_t now = millis();
  float t = now / 1000.0f;
  memset(out, 0, 513);

  // Indizes der Geräte, auf die der Effekt wirkt
  int sel[MAX_FIX], nsel = 0, idxIn[MAX_FIX];
  for (int i = 0; i < cfg.nfix; i++) { idxIn[i] = -1; if (live.fxMask & (1u << i)) { idxIn[i] = nsel; sel[nsel++] = i; } }
  float sp = live.speed / 100.0f;                      // 0..1
  float rate = 0.05f + sp * 2.0f;                      // Zyklen pro Sekunde
  float stepRate = 0.5f + sp * 12.0f;                  // Schritte pro Sekunde
  float strobeHz = 1.0f + live.strobe * 0.19f;         // 1..20 Hz
  bool strobeGate = live.strobe == 0 || fmodf(t * strobeHz, 1.0f) < 0.3f;

  for (int i = 0; i < cfg.nfix; i++) {
    FixDef &fd = cfg.fix[i];
    FixState &fs = live.fs[i];
    // Ziel und Überblendung
    uint8_t tgt[7];
    for (int k = 0; k < 6; k++) tgt[k] = fs.col[k];
    tgt[6] = fs.on ? fs.dim : 0;
    if (memcmp(tgt, fTgt[i], 7) != 0) {
      for (int k = 0; k < 7; k++) fFrom[i][k] = fCur[i][k];
      memcpy(fTgt[i], tgt, 7);
      fStart[i] = now; fDur[i] = live.fadeMs;
    }
    float p = fDur[i] ? (float)(now - fStart[i]) / fDur[i] : 1.0f;
    if (p > 1) p = 1;
    for (int k = 0; k < 7; k++) fCur[i][k] = fFrom[i][k] + (tgt[k] - fFrom[i][k]) * p;

    float v[7]; for (int k = 0; k < 7; k++) v[k] = fCur[i][k];
    bool hasStrobeCh = false, hasDim = false, hasW = false;
    for (int c = 0; c < fd.nch; c++) { if (fd.ch[c].fn == F_STROBE) hasStrobeCh = true; if (fd.ch[c].fn == F_DIM) hasDim = true; if (fd.ch[c].fn == F_W) hasW = true; }

    // Effekte
    int ei = idxIn[i];
    if (live.effect && ei >= 0) {
      float r, g, b;
      switch (live.effect) {
        case 1: // Regenbogen
          hsv2rgb(t * rate + (float)ei / (nsel ? nsel : 1), 1, 1, r, g, b);
          v[0] = r * 255; v[1] = g * 255; v[2] = b * 255; v[3] = v[4] = v[5] = 0; break;
        case 2: // Atmen
          v[6] *= 0.5f - 0.5f * cosf(t * rate * 6.2832f); break;
        case 3: { // Lauflicht
          int pos = ((int)(t * stepRate)) % (nsel ? nsel : 1);
          if (pos != ei) v[6] = 0;
          break; }
        case 4: { // Farbwechsel (weich)
          static const uint8_t pal[7][3] = {{255,0,0},{255,128,0},{255,255,0},{0,255,0},{0,255,255},{0,0,255},{255,0,255}};
          float ph = t * rate * 2; int a = ((int)ph) % 7, bb = (a + 1) % 7; float fr = ph - floorf(ph);
          for (int k = 0; k < 3; k++) v[k] = pal[a][k] + (pal[bb][k] - pal[a][k]) * fr;
          v[3] = v[4] = v[5] = 0; break; }
        case 5: { // Feuer
          float target = 0.45f + (hash32(now / 60 + i * 977) % 1000) / 1000.0f * 0.55f;
          fireState[i] += (target - fireState[i]) * 0.35f;
          v[0] = 255; v[1] = 40 + fireState[i] * 90; v[2] = 0; v[3] = v[4] = v[5] = 0;
          v[6] *= fireState[i]; break; }
        case 6: { // Disco: zufällige Farben im Takt
          uint32_t beat = (uint32_t)(t * stepRate);
          hsv2rgb((hash32(beat * 131 + i * 7919) % 360) / 360.0f, 1, 1, r, g, b);
          v[0] = r * 255; v[1] = g * 255; v[2] = b * 255; v[3] = v[4] = v[5] = 0; break; }
        case 7: { // Blaulicht
          uint32_t beat = (uint32_t)(t * stepRate);
          bool phase = ((beat / 4) + ei) & 1;
          bool flash = (beat & 1) == 0;
          v[0] = phase ? 255 : 0; v[1] = 0; v[2] = phase ? 0 : 255; v[3] = v[4] = v[5] = 0;
          if (!flash) v[6] = 0;
          break; }
      }
    }
    // Identifizieren: weiß blinken
    if (identifyFix == i && now < identifyUntil) {
      bool on = (now / 150) & 1;
      for (int k = 0; k < 6; k++) v[k] = k < 4 ? 255 : 0;
      v[6] = on ? 255 : 0;
    }

    float level = (v[6] / 255.0f) * (live.master / 255.0f);
    if (blackout) level = 0;
    if (!hasStrobeCh && !strobeGate) level = 0;
    if (identifyFix == i && now < identifyUntil) level = v[6] / 255.0f;

    // Auto-Weiß: gemeinsamen Weißanteil aus RGB auf den W-Kanal legen
    if ((fd.flags & 1) && hasW) {
      float m = min(v[0], min(v[1], v[2]));
      v[0] -= m; v[1] -= m; v[2] -= m; v[3] = min(255.0f, v[3] + m);
    }
    float colScale = hasDim ? 1.0f : level;
    uint16_t lvl16 = (uint16_t)(level * 65535.0f + 0.5f);

    for (int c = 0; c < fd.nch; c++) {
      int a = fd.addr + c;
      if (a < 1 || a > 512) continue;
      uint8_t val = 0;
      switch (fd.ch[c].fn) {
        case F_DIM: val = lvl16 >> 8; break;
        case F_DIMFINE: val = lvl16 & 0xFF; break;
        case F_R: val = v[0] * colScale + 0.5f; break;
        case F_G: val = v[1] * colScale + 0.5f; break;
        case F_B: val = v[2] * colScale + 0.5f; break;
        case F_W: val = v[3] * colScale + 0.5f; break;
        case F_A: val = v[4] * colScale + 0.5f; break;
        case F_UV: val = v[5] * colScale + 0.5f; break;
        case F_STROBE: val = live.strobe ? (uint8_t)(10 + live.strobe * 245 / 100) : fd.ch[c].val; break;
        case F_FIXED: val = fd.ch[c].val; break;
        default: val = 0;
      }
      out[a] = val;
    }
  }

  // Art-Net hat Vorrang, solange Daten ankommen
  if (cfg.artEnable && artLast && now - artLast < 2500) memcpy(out + 1, artBuf + 1, 512);
  // Manuelle Kanal-Übersteuerung
  for (int a = 1; a <= 512; a++) if (override_[a] >= 0) out[a] = override_[a];
  out[0] = 0;
}

void dmxTask(void *) {
  uart_config_t uc = {};
  uc.baud_rate = 250000;
  uc.data_bits = UART_DATA_8_BITS;
  uc.parity = UART_PARITY_DISABLE;
  uc.stop_bits = UART_STOP_BITS_2;
  uc.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
  uc.source_clk = UART_SCLK_APB;
  uart_driver_install(DMX_PORT, 256, 1024, 0, NULL, 0);
  uart_param_config(DMX_PORT, &uc);
  uart_set_pin(DMX_PORT, cfg.txPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  uint8_t frame[513];
  TickType_t last = xTaskGetTickCount();
  for (;;) {
    xSemaphoreTake(lock, portMAX_DELAY);
    renderFrame(frame);
    memcpy(dmxOut, frame, 513);
    xSemaphoreGive(lock);

    uart_wait_tx_done(DMX_PORT, pdMS_TO_TICKS(100));
    // Break (>= 88 µs) und Mark-after-Break (>= 8 µs)
    uart_set_line_inverse(DMX_PORT, UART_SIGNAL_TXD_INV);
    delayMicroseconds(150);
    uart_set_line_inverse(DMX_PORT, UART_SIGNAL_INV_DISABLE);
    delayMicroseconds(16);
    uart_write_bytes(DMX_PORT, (const char *)frame, 513);
    vTaskDelayUntil(&last, pdMS_TO_TICKS(1000 / cfg.rate));
  }
}

// ---------------------------------------------------------------- Art-Net
void pollArtNet() {
  if (!cfg.artEnable) return;
  int len;
  uint8_t buf[530];
  while ((len = udp.parsePacket()) > 0) {
    int n = udp.read(buf, sizeof(buf));
    if (n < 18 || memcmp(buf, "Art-Net\0", 8) != 0) continue;
    uint16_t op = buf[8] | (buf[9] << 8);
    if (op != 0x5000) continue;
    uint16_t uni = buf[14] | ((buf[15] & 0x7F) << 8);
    if (uni != cfg.artUniverse) continue;
    int dl = (buf[16] << 8) | buf[17];
    if (dl > 512) dl = 512;
    if (dl > n - 18) dl = n - 18;
    xSemaphoreTake(lock, portMAX_DELAY);
    memset(artBuf + 1, 0, 512);
    memcpy(artBuf + 1, buf + 18, dl);
    artLast = millis(); if (!artLast) artLast = 1;
    artPackets++;
    xSemaphoreGive(lock);
  }
}

// ---------------------------------------------------------------- WLAN
void sendJson(JsonDocument &d, int code);
volatile uint8_t staReason = 0;     // letzter Trennungsgrund (ESP-IDF wifi_err_reason_t)
uint32_t lastRetry = 0;

const char *reasonText(uint8_t r) {
  switch (r) {
    case 0: return "";
    case 201: return "WLAN nicht gefunden (Name falsch, zu weit weg oder nur 5 GHz)";
    case 202: case 15: case 204: case 2: return "Anmeldung abgelehnt (Passwort falsch?)";
    case 203: return "Verbindung vom Router abgelehnt (z.B. MAC-Filter oder Router voll)";
    case 200: return "Kein Signal mehr (Beacon-Timeout)";
    case 205: return "Verbindung fehlgeschlagen";
    default: { static char b[24]; snprintf(b, sizeof(b), "Fehlercode %u", r); return b; }
  }
}

void startAP() {
  if (apActive) return;
  bool m = WiFi.mode(cfg.wifiMode == 0 ? WIFI_AP : WIFI_AP_STA);
  WiFi.setTxPower(WIFI_POWER_15dBm);   // etwas weniger Sendeleistung: stabiler bei schwacher USB-Versorgung
  bool ok = WiFi.softAP(cfg.apSsid, strlen(cfg.apPass) >= 8 ? cfg.apPass : nullptr, 1, 0, 4);
  delay(200);
  bool ipok = WiFi.softAPConfig(AP_IP, AP_IP, IPAddress(255, 255, 255, 0));
  Serial.printf("Direktverbindung: mode=%d softAP=%d ipconfig=%d  WLAN-Name '%s'  Passwort '%s'  IP %s  MAC %s\n",
                m, ok, ipok, cfg.apSsid, strlen(cfg.apPass) >= 8 ? cfg.apPass : "(offen)",
                WiFi.softAPIP().toString().c_str(), WiFi.softAPmacAddress().c_str());
  if (!ok) { delay(500); ok = WiFi.softAP(cfg.apSsid, strlen(cfg.apPass) >= 8 ? cfg.apPass : nullptr, 1, 0, 4); Serial.printf("softAP 2. Versuch: %d\n", ok); }
  dns.start(53, "*", WiFi.softAPIP());
  apActive = true;
  // Ständiges Neuverbinden lässt den Access Point unbrauchbar werden -> nur noch gezielt erneut versuchen
  if (cfg.wifiMode != 0) { WiFi.setAutoReconnect(false); WiFi.disconnect(false, false); }
  lastRetry = millis();
}
void stopAP() {
  if (!apActive) return;
  dns.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  apActive = false;
}

void staBegin() {
  if (!strlen(cfg.ssid)) return;
  if (cfg.useStatic) WiFi.config(IPAddress(cfg.ip), IPAddress(cfg.gw), IPAddress(cfg.mask), IPAddress(cfg.dns));
  WiFi.begin(cfg.ssid, cfg.pass);
}

void startWiFi() {
  WiFi.persistent(false);
  WiFi.setHostname(cfg.host);           // muss vor WiFi.mode() stehen
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.setMinSecurity(WIFI_AUTH_WEP);   // auch ältere Router (WPA/TKIP) zulassen
  WiFi.onEvent([](WiFiEvent_t e, WiFiEventInfo_t info) {
    staReason = info.wifi_sta_disconnected.reason;
    Serial.printf("WLAN getrennt: %u %s\n", staReason, reasonText(staReason));
  }, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.onEvent([](WiFiEvent_t e, WiFiEventInfo_t info) { staReason = 0; }, ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent([](WiFiEvent_t e, WiFiEventInfo_t info) { Serial.println("Geraet mit Direktverbindung verbunden"); }, ARDUINO_EVENT_WIFI_AP_STACONNECTED);
  if (cfg.wifiMode == 1 && strlen(cfg.ssid)) { WiFi.setAutoReconnect(true); staBegin(); }
  else startAP();   // Direktverbindung: ESP32 öffnet sein eigenes WLAN
  staLostSince = millis();
}

void handleWiFi() {
  uint32_t now = millis();
  if (cfg.wifiMode == 0 || !strlen(cfg.ssid)) { if (!apActive) startAP(); return; }
  if (WiFi.status() == WL_CONNECTED) {
    staLostSince = 0;
    if (!staOkSince) { staOkSince = now; Serial.printf("WLAN verbunden: %s  IP %s\n", cfg.ssid, WiFi.localIP().toString().c_str()); }
    // Notfall-AP wieder abschalten, wenn niemand daran hängt
    if (apActive && now - staOkSince > 60000 && WiFi.softAPgetStationNum() == 0) stopAP();
  } else {
    staOkSince = 0;
    if (!staLostSince) staLostSince = now;
    if (!apActive && now - staLostSince > 20000) {
      startAP();
      Serial.printf("WLAN nicht erreichbar -> Access Point '%s' (IP %s)\n", cfg.apSsid, WiFi.softAPIP().toString().c_str());
    }
    // Im AP-Betrieb nur erneut versuchen, wenn gerade niemand am Access Point hängt (alle 60 s, max. 15 s lang)
    if (apActive && strlen(cfg.ssid)) {
      static bool trying = false;
      if (trying && now - lastRetry > 15000) { trying = false; WiFi.disconnect(false, false); lastRetry = now; }
      else if (!trying && now - lastRetry > 60000 && WiFi.softAPgetStationNum() == 0) { trying = true; lastRetry = now; staBegin(); }
    }
  }
}

void handleScan() {
  int n = WiFi.scanComplete();
  if (n == WIFI_SCAN_FAILED || server.hasArg("start")) {
    if (WiFi.getMode() == WIFI_AP) WiFi.mode(WIFI_AP_STA);
    WiFi.scanNetworks(true, false, false, 120);
    server.send(200, "application/json", "{\"running\":true}");
    return;
  }
  if (n == WIFI_SCAN_RUNNING) { server.send(200, "application/json", "{\"running\":true}"); return; }
  JsonDocument d;
  JsonArray a = d["nets"].to<JsonArray>();
  for (int i = 0; i < n; i++) {
    JsonObject o = a.add<JsonObject>();
    o["ssid"] = WiFi.SSID(i); o["rssi"] = WiFi.RSSI(i); o["ch"] = WiFi.channel(i); o["enc"] = (int)WiFi.encryptionType(i);
  }
  WiFi.scanDelete();
  sendJson(d, 200);
}

// ---------------------------------------------------------------- Web-API
void sendJson(JsonDocument &d, int code) {
  String s; serializeJson(d, s);
  server.sendHeader("Cache-Control", "no-store");
  server.send(code, "application/json", s);
}
void sendOk() { server.send(200, "application/json", "{\"ok\":true}"); }
void sendErr(const char *m) { String s = String("{\"ok\":false,\"err\":\"") + m + "\"}"; server.send(400, "application/json", s); }

bool readBody(JsonDocument &d) {
  if (!server.hasArg("plain")) { sendErr("kein Inhalt"); return false; }
  if (deserializeJson(d, server.arg("plain"))) { sendErr("JSON ungueltig"); return false; }
  return true;
}

void handleState() {
  JsonDocument d;
  xSemaphoreTake(lock, portMAX_DELAY);
  d["fw"] = FW_VERSION;
  d["master"] = live.master; d["blackout"] = blackout; d["effect"] = live.effect; d["speed"] = live.speed;
  d["strobe"] = live.strobe; d["fade"] = live.fadeMs;
  JsonArray fx = d["fx"].to<JsonArray>();
  JsonArray fa = d["fixtures"].to<JsonArray>();
  for (int i = 0; i < cfg.nfix; i++) {
    if (live.fxMask & (1u << i)) fx.add(i);
    JsonObject o = fa.add<JsonObject>();
    o["n"] = cfg.fix[i].name; o["a"] = cfg.fix[i].addr; o["c"] = cfg.fix[i].nch;
    o["on"] = live.fs[i].on; o["dim"] = live.fs[i].dim;
    uint32_t caps = 0; for (int c = 0; c < cfg.fix[i].nch; c++) caps |= 1u << cfg.fix[i].ch[c].fn;
    o["caps"] = caps;
    JsonArray col = o["col"].to<JsonArray>(); for (int k = 0; k < 6; k++) col.add(live.fs[i].col[k]);
  }
  JsonObject art = d["art"].to<JsonObject>();
  art["en"] = cfg.artEnable; art["uni"] = cfg.artUniverse; art["active"] = cfg.artEnable && artLast && millis() - artLast < 2500; art["pk"] = artPackets;
  int ov = 0; for (int a = 1; a <= 512; a++) if (override_[a] >= 0) ov++;
  d["ovr"] = ov;
  xSemaphoreGive(lock);
  JsonObject w = d["wifi"].to<JsonObject>();
  w["sta"] = WiFi.status() == WL_CONNECTED; w["ssid"] = cfg.ssid; w["ip"] = WiFi.localIP().toString(); w["rssi"] = WiFi.RSSI();
  w["reason"] = reasonText(staReason); w["ap"] = apActive; w["apip"] = WiFi.softAPIP().toString(); w["host"] = cfg.host;
  w["mode"] = cfg.wifiMode; w["apSsid"] = cfg.apSsid; d["setup"] = !cfg.setupDone;
  d["up"] = millis() / 1000; d["heap"] = ESP.getFreeHeap();
  sendJson(d, 200);
}

void handleControl() {
  JsonDocument d; if (!readBody(d)) return;
  xSemaphoreTake(lock, portMAX_DELAY);
  uint32_t mask = 0;
  if (d["ids"].is<JsonArray>()) { for (int id : d["ids"].as<JsonArray>()) if (id >= 0 && id < cfg.nfix) mask |= 1u << id; }
  else mask = (cfg.nfix >= 32) ? 0xFFFFFFFF : ((1u << cfg.nfix) - 1);
  for (int i = 0; i < cfg.nfix; i++) {
    if (!(mask & (1u << i))) continue;
    FixState &fs = live.fs[i];
    if (d["col"].is<JsonArray>()) { int k = 0; for (JsonVariant x : d["col"].as<JsonArray>()) { if (k < 6 && x.is<int>() && x.as<int>() >= 0) fs.col[k] = constrain(x.as<int>(), 0, 255); k++; } }
    if (d["dim"].is<int>()) fs.dim = constrain(d["dim"].as<int>(), 0, 255);
    if (d["on"].is<bool>()) fs.on = d["on"].as<bool>();
    if (d["on"].is<const char *>() && !strcmp(d["on"].as<const char *>(), "toggle")) fs.on = !fs.on;
    if (d["on"].is<bool>() && d["on"].as<bool>() && fs.dim == 0) fs.dim = 255;
  }
  if (d["master"].is<int>()) live.master = constrain(d["master"].as<int>(), 0, 255);
  if (d["blackout"].is<bool>()) blackout = d["blackout"];
  if (d["speed"].is<int>()) live.speed = constrain(d["speed"].as<int>(), 1, 100);
  if (d["strobe"].is<int>()) live.strobe = constrain(d["strobe"].as<int>(), 0, 100);
  if (d["fade"].is<int>()) live.fadeMs = constrain(d["fade"].as<int>(), 0, 10000);
  if (d["effect"].is<int>()) { live.effect = constrain(d["effect"].as<int>(), 0, 7); live.fxMask = mask; }
  if (d["identify"].is<int>()) { identifyFix = d["identify"]; identifyUntil = millis() + 3000; }
  xSemaphoreGive(lock);
  markDirty();
  sendOk();
}

void fixToJson(JsonObject o, const FixDef &f) {
  o["n"] = f.name; o["a"] = f.addr; o["aw"] = (f.flags & 1) != 0;
  JsonArray ch = o["ch"].to<JsonArray>();
  for (int c = 0; c < f.nch; c++) { JsonArray x = ch.add<JsonArray>(); x.add(f.ch[c].fn); x.add(f.ch[c].val); }
}

void handleGetConfig() {
  JsonDocument d;
  JsonObject w = d["wifi"].to<JsonObject>();
  w["ssid"] = cfg.ssid; w["pass"] = cfg.pass; w["apSsid"] = cfg.apSsid; w["apPass"] = cfg.apPass; w["host"] = cfg.host;
  w["mode"] = cfg.wifiMode; w["static"] = cfg.useStatic; w["ip"] = ipStr(cfg.ip); w["gw"] = ipStr(cfg.gw); w["mask"] = ipStr(cfg.mask); w["dns"] = ipStr(cfg.dns);
  JsonObject x = d["dmx"].to<JsonObject>();
  x["tx"] = cfg.txPin; x["de"] = cfg.dePin; x["led"] = cfg.ledPin; x["rate"] = cfg.rate;
  x["art"] = cfg.artEnable; x["uni"] = cfg.artUniverse; x["boot"] = cfg.bootMode; x["bootScene"] = cfg.bootScene;
  JsonArray fa = d["fixtures"].to<JsonArray>();
  for (int i = 0; i < cfg.nfix; i++) fixToJson(fa.add<JsonObject>(), cfg.fix[i]);
  d["maxFix"] = MAX_FIX; d["maxCh"] = MAX_CH;
  sendJson(d, 200);
}

void handlePostConfig() {
  JsonDocument d; if (!readBody(d)) return;
  bool reboot = false;
  xSemaphoreTake(lock, portMAX_DELAY);
  if (d["wifi"].is<JsonObject>()) {
    JsonObject w = d["wifi"];
    if (w["ssid"].is<const char *>()) copyStr(cfg.ssid, w["ssid"].as<const char *>(), sizeof(cfg.ssid));
    if (w["pass"].is<const char *>()) copyStr(cfg.pass, w["pass"].as<const char *>(), sizeof(cfg.pass));
    if (w["apSsid"].is<const char *>()) copyStr(cfg.apSsid, w["apSsid"].as<const char *>(), sizeof(cfg.apSsid));
    if (w["apPass"].is<const char *>() && strlen(w["apPass"].as<const char *>()) >= 8) copyStr(cfg.apPass, w["apPass"].as<const char *>(), sizeof(cfg.apPass));
    if (w["host"].is<const char *>()) copyStr(cfg.host, w["host"].as<const char *>(), sizeof(cfg.host));
    if (!w["mode"].isNull()) cfg.wifiMode = w["mode"].as<int>() ? 1 : 0;
    cfg.setupDone = 1;
    if (!w["static"].isNull()) cfg.useStatic = w["static"].as<int>() ? 1 : 0;
    parseIp(w["ip"].as<const char *>(), cfg.ip); parseIp(w["gw"].as<const char *>(), cfg.gw); parseIp(w["mask"].as<const char *>(), cfg.mask); parseIp(w["dns"].as<const char *>(), cfg.dns);
    reboot = true;
  }
  if (d["dmx"].is<JsonObject>()) {
    JsonObject x = d["dmx"];
    uint8_t tx = x["tx"] | cfg.txPin, de = x["de"] | cfg.dePin, led = x["led"] | cfg.ledPin;
    if (tx != cfg.txPin || de != cfg.dePin || led != cfg.ledPin) reboot = true;
    cfg.txPin = tx; cfg.dePin = de; cfg.ledPin = led;
    cfg.rate = x["rate"] | cfg.rate;
    uint8_t art = x["art"].isNull() ? cfg.artEnable : (x["art"].as<int>() ? 1 : 0);
    if (art && !cfg.artEnable) udp.begin(6454);
    if (!art && cfg.artEnable) udp.stop();
    cfg.artEnable = art;
    cfg.artUniverse = x["uni"] | cfg.artUniverse;
    cfg.bootMode = x["boot"] | cfg.bootMode;
    cfg.bootScene = x["bootScene"] | cfg.bootScene;
  }
  if (d["fixtures"].is<JsonArray>()) {
    JsonArray fa = d["fixtures"];
    int n = 0;
    for (JsonObject o : fa) {
      if (n >= MAX_FIX) break;
      FixDef &f = cfg.fix[n];
      memset(&f, 0, sizeof(f));
      copyStr(f.name, o["n"] | "Gerät", sizeof(f.name));
      f.addr = o["a"] | 1;
      f.flags = (o["aw"] | false) ? 1 : 0;
      int c = 0;
      JsonArray chs = o["ch"].as<JsonArray>();
      for (JsonArray x : chs) { if (c >= MAX_CH) break; f.ch[c].fn = x[0] | 0; f.ch[c].val = x[1] | 0; c++; }
      f.nch = c;
      n++;
    }
    cfg.nfix = n;
    for (int i = 0; i < MAX_FIX; i++) memset(fTgt[i], 0, 7);
  }
  sanitizeConfig();
  saveConfig();
  xSemaphoreGive(lock);
  JsonDocument r; r["ok"] = true; r["reboot"] = reboot;
  sendJson(r, 200);
}

void handleScenes() {
  JsonDocument d;
  JsonArray a = d.to<JsonArray>();
  for (int s = 0; s < MAX_SCENES; s++) {
    Scene sc; JsonObject o = a.add<JsonObject>();
    o["slot"] = s;
    if (loadSceneSlot(sceneKey(s).c_str(), sc)) { o["used"] = true; o["name"] = sc.name; }
    else o["used"] = false;
  }
  sendJson(d, 200);
}

bool applyScene(int slot) {
  Scene sc;
  if (slot < 0 || slot >= MAX_SCENES || !loadSceneSlot(sceneKey(slot).c_str(), sc)) return false;
  xSemaphoreTake(lock, portMAX_DELAY);
  live = sc;
  live.nfix = MAX_FIX;
  blackout = false;
  xSemaphoreGive(lock);
  return true;
}

void handleScenePost() {
  JsonDocument d; if (!readBody(d)) return;
  const char *act = d["act"] | "";
  int slot = d["slot"] | -1;
  if (slot < 0 || slot >= MAX_SCENES) { sendErr("Slot ungueltig"); return; }
  if (!strcmp(act, "save")) {
    xSemaphoreTake(lock, portMAX_DELAY);
    Scene sc = live;
    xSemaphoreGive(lock);
    sc.magic = SCN_MAGIC;
    copyStr(sc.name, d["name"] | "Szene", sizeof(sc.name));
    saveSceneSlot(sceneKey(slot).c_str(), sc);
  } else if (!strcmp(act, "load")) {
    if (!applyScene(slot)) { sendErr("Szene leer"); return; }
    markDirty();
  } else if (!strcmp(act, "del")) {
    deleteSceneSlot(sceneKey(slot).c_str());
  } else { sendErr("Aktion unbekannt"); return; }
  sendOk();
}

void handleDmx() {
  static const char hex[] = "0123456789abcdef";
  String s; s.reserve(1030);
  s += "{\"d\":\"";
  xSemaphoreTake(lock, portMAX_DELAY);
  for (int a = 1; a <= 512; a++) { s += hex[dmxOut[a] >> 4]; s += hex[dmxOut[a] & 15]; }
  s += "\",\"o\":\"";
  for (int a = 1; a <= 512; a++) s += override_[a] >= 0 ? '1' : '0';
  xSemaphoreGive(lock);
  s += "\"}";
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", s);
}

void handleChan() {
  JsonDocument d; if (!readBody(d)) return;
  xSemaphoreTake(lock, portMAX_DELAY);
  if (d["clear"] | false) for (int a = 0; a <= 512; a++) override_[a] = -1;
  if (d["ch"].is<int>()) {
    int ch = d["ch"], v = d["v"] | -1;
    int cnt = d["n"] | 1;
    for (int k = 0; k < cnt; k++) if (ch + k >= 1 && ch + k <= 512) override_[ch + k] = v < 0 ? -1 : min(v, 255);
  }
  xSemaphoreGive(lock);
  sendOk();
}

void handleUpdateDone() {
  bool ok = !Update.hasError();
  server.send(ok ? 200 : 500, "text/plain", ok ? "OK - Neustart..." : "Update fehlgeschlagen");
  if (ok) { rebootPending = true; rebootAt = millis() + 800; }
}
void handleUpdateUpload() {
  HTTPUpload &up = server.upload();
  if (up.status == UPLOAD_FILE_START) Update.begin(UPDATE_SIZE_UNKNOWN);
  else if (up.status == UPLOAD_FILE_WRITE) Update.write(up.buf, up.currentSize);
  else if (up.status == UPLOAD_FILE_END) Update.end(true);
}

void setupServer() {
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Cache-Control", "no-cache");
    server.send_P(200, "text/html; charset=utf-8", INDEX_HTML);
  });
  server.on("/api/state", HTTP_GET, handleState);
  server.on("/api/control", HTTP_POST, handleControl);
  server.on("/api/config", HTTP_GET, handleGetConfig);
  server.on("/api/config", HTTP_POST, handlePostConfig);
  server.on("/api/scenes", HTTP_GET, handleScenes);
  server.on("/api/scenes", HTTP_POST, handleScenePost);
  server.on("/api/dmx", HTTP_GET, handleDmx);
  server.on("/api/chan", HTTP_POST, handleChan);
  server.on("/api/scan", HTTP_GET, handleScan);
  server.on("/api/reboot", HTTP_POST, []() { sendOk(); rebootPending = true; rebootAt = millis() + 500; });
  server.on("/api/factory", HTTP_POST, []() {
    prefs.begin("dmx", false); prefs.clear(); prefs.end();
    sendOk(); rebootPending = true; rebootAt = millis() + 500;
  });
  server.on("/update", HTTP_POST, handleUpdateDone, handleUpdateUpload);
  server.onNotFound([]() {
    // Captive-Portal: im Access-Point-Modus alles auf die Oberfläche umleiten
    if (apActive) { server.sendHeader("Location", String("http://") + WiFi.softAPIP().toString() + "/"); server.send(302, "text/plain", ""); }
    else server.send(404, "text/plain", "Nicht gefunden");
  });
  server.begin();
}

// ---------------------------------------------------------------- Status-LED
void handleLed() {
  if (cfg.ledPin == 255) return;
  uint32_t now = millis();
  bool on;
  if (WiFi.status() == WL_CONNECTED) on = true;                 // dauerhaft an: verbunden
  else if (apActive && cfg.wifiMode == 0) on = true;          // dauerhaft an: Direktverbindung bereit
  else if (apActive) on = (now / 1000) & 1;                     // langsam blinken: Access Point
  else on = (now / 150) & 1;                                    // schnell blinken: verbindet
  digitalWrite(cfg.ledPin, on);
}

// ---------------------------------------------------------------- Setup/Loop
void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\nESP32 DMX-Steuerung " FW_VERSION);
  Serial.printf("Chip %s Rev %d, %d Kerne, Flash %u MB, Reset-Grund %d (15 = Brownout/Stromversorgung zu schwach)\n",
                ESP.getChipModel(), ESP.getChipRevision(), ESP.getChipCores(), ESP.getFlashChipSize() / 1048576, (int)esp_reset_reason());
  lock = xSemaphoreCreateMutex();
  for (int a = 0; a <= 512; a++) override_[a] = -1;
  memset(artBuf, 0, sizeof(artBuf));
  memset(fCur, 0, sizeof(fCur)); memset(fFrom, 0, sizeof(fFrom)); memset(fTgt, 0, sizeof(fTgt));

  loadConfig();
  defaultLive();
  Scene sc;
  if (cfg.bootMode == 1 && loadSceneSlot("last", sc)) live = sc;
  else if (cfg.bootMode == 2 && cfg.bootScene >= 0 && loadSceneSlot(sceneKey(cfg.bootScene).c_str(), sc)) live = sc;
  live.nfix = MAX_FIX;

  if (cfg.dePin != 255) { pinMode(cfg.dePin, OUTPUT); digitalWrite(cfg.dePin, HIGH); }  // RS485 dauerhaft senden
  if (cfg.ledPin != 255) pinMode(cfg.ledPin, OUTPUT);

  xTaskCreatePinnedToCore(dmxTask, "dmx", 4096, NULL, 5, NULL, 0);

  startWiFi();
  if (MDNS.begin(cfg.host)) MDNS.addService("http", "tcp", 80);
  if (cfg.artEnable) udp.begin(6454);
  setupServer();
  Serial.printf("DMX: TX=GPIO%d DE/RE=GPIO%d, %d Hz, %d Geraete\n", cfg.txPin, cfg.dePin, cfg.rate, cfg.nfix);
}

void loop() {
  server.handleClient();
  if (apActive) dns.processNextRequest();
  pollArtNet();
  handleWiFi();
  handleLed();
  if (cfg.bootMode == 1 && stateDirty && millis() - stateDirtyAt > 10000) {
    stateDirty = false;
    xSemaphoreTake(lock, portMAX_DELAY);
    Scene sc = live;
    xSemaphoreGive(lock);
    copyStr(sc.name, "last", sizeof(sc.name));
    saveSceneSlot("last", sc);
  }
  static uint32_t lastLog = 0;
  if (millis() - lastLog > 10000) {
    lastLog = millis();
    Serial.printf("[%lus] Modus %s | Direkt-WLAN '%s' %s, %d Geraete verbunden | Haus-WLAN %s\n", millis() / 1000,
                  cfg.wifiMode == 0 ? "Direkt" : "WLAN", cfg.apSsid, apActive ? "AN" : "aus", WiFi.softAPgetStationNum(),
                  WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString().c_str() : (strlen(cfg.ssid) ? reasonText(staReason) : "nicht eingerichtet"));
  }
  if (rebootPending && millis() > rebootAt) ESP.restart();
  delay(2);
}
