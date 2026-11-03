#!/bin/sh
# Lokal bauen: benötigt Python 3 (pip install platformio esptool)
set -e
cd "$(dirname "$0")"
python3 gen_webui.py
pio run -e esp32dev
B=.pio/build/esp32dev
mkdir -p out
esptool.py --chip esp32 merge_bin -o out/esp32-dmx-merged.bin --flash_mode dio --flash_freq 40m --flash_size 4MB \
  0x1000 $B/bootloader.bin 0x8000 $B/partitions.bin \
  0xe000 ~/.platformio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin 0x10000 $B/firmware.bin
cp $B/firmware.bin out/esp32-dmx-update.bin
echo "Fertig: out/esp32-dmx-merged.bin (an 0x0 flashen)"
