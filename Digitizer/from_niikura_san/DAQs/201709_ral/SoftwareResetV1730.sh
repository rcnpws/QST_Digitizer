#!/bin/bash

# by executing this script, V1730s with BA 1111 and 2222 is remotly software reset.
# ef24 is sw reset address
# ef34 is load config

cmdvme -lw 0x1111ef24 0x01
cmdvme -lw 0x2222ef24 0x01

sleep 1

cmdvme -lw 0x1111ef34 0x01
cmdvme -lw 0x2222ef34 0x01
