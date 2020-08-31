#!/bin/bash

export DISPLAY=:1
export XDG_RUNTIME_DIR=/run/user/1000

/home/jblades/Documents/cpp/notify/notify --icon "${1}" --now-text "${2}" --later-text "${3}" -d "${4}" "${5}" "${6}"
