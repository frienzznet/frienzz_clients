#!/bin/bash

set -e # stop on error

TARGETDIR="dist"
source secrets.sh
BRANCH=$(git rev-parse --abbrev-ref HEAD)
REVISION=$(git rev-list --count HEAD)



SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)

function build {
  echo '🏃 build the firmware image'

  .venv/bin/python3 $SCRIPT_DIR/deploy_support.py --config "$SCRIPT_DIR/deploy_support.yaml" --targetdir "$TARGETDIR"
# this command is copied out of the Arduino compile & upload log , where the --flash_xxx parameter need to be adjust
# based on the values shown in the logfile

# Settings for Nodemcu-32s
  # /home/steffen/.arduino15/packages/esp32/tools/esptool_py/5.1.0/esptool --chip esp32 merge-bin \
  # -o merged-firmware.bin \
  # --pad-to-size 4MB \
  # --flash-mode dio \
  # --flash-freq 80m \
  # --flash-size 4MB \
  # 0x1000 /home/steffen/Desktop/workcopies/frienzz_clients/arduino/Frienzz/Frienzz/build/Frienzz.ino.bootloader.bin \
  # 0x8000 /home/steffen/Desktop/workcopies/frienzz_clients/arduino/Frienzz/Frienzz/build/Frienzz.ino.partitions.bin \
  # 0xe000 /home/steffen/.arduino15/packages/esp32/hardware/esp32/3.3.5/tools/partitions/boot_app0.bin \
  # 0x10000 /home/steffen/Desktop/workcopies/frienzz_clients/arduino/Frienzz/Frienzz/build/Frienzz.ino.bin


echo "Frienzz firmware build done"
}

function build_test {
  echo '🏃 test to build the firmware image'

  .venv/bin/python3 $SCRIPT_DIR/deploy_support.py --config "$SCRIPT_DIR/deploy_support.yaml" --debug --targetdir "$TARGETDIR"


echo "Frienzz firmware build testdone"
}

 
function deploy {
  echo '🏃 copies everything to the webspace'
  cp index.html "$TARGETDIR"/index.html
  # Its important to use the trailing slash on the source directory to copy the contents of the directory rather than the directory itself.
  # I don't use the --delete option, because it would delete all files on the webspace that are not in the local directory, which is not what I want.
  #rsync -a --delete /"$TARGETDIR"/ "$WEBHOST":$WEBDIR/
  rsync -a  "$TARGETDIR"/ "$WEBHOST":$WEBDIR/
  echo "Webspace Copy done"
}


PS3='Please take your choice: '
options=("build" "build_test" "deploy" "Quit")
  echo "actual branch: $BRANCH"
  echo "actual revision: $REVISION"
  echo "actual hardware: $HARDWARE"
select opt in "${options[@]}"; do
  case $opt in
  "build")
    echo "you choose build "
    build
    ;;
  "build_test")
    echo "you choose build_test "
    build_test
    ;;
  "deploy")
    echo "you choose deploy "
    deploy
    ;;
  "Quit")
    break
    ;;
  *) echo invalid option ;;
  esac
done
