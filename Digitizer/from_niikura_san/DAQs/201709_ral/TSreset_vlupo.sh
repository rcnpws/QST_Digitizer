#!/bin/bash

date +"%Y/%m/%d %p %I:%M:%S" >> TSinit_vlupo.txt

# BA + %92 read access leads timestamp reset./ here, BA = 0x444444
cmdvme -wr 0x44444492 
