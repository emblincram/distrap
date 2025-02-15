#!/bin/bash

BIN_DIR="build/yocto/"
DEVICE="192.168.0.146"
FILES=("dacore" "dashell" "datest")

for FILE in "${FILES[@]}"; do
    FULL_PATH="${BIN_DIR}/${FILE}"

    if [ -f "${FULL_PATH}" ]; then
        echo "Kopiere ${FULL_PATH} nach ${DEVICE}:/usr/bin ..."
        scp -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null "${FULL_PATH}" root@"${DEVICE}":/usr/bin
    else
        echo "WARNUNG: ${FULL_PATH} existiert nicht, überspringe."
    fi
done

echo "done."
