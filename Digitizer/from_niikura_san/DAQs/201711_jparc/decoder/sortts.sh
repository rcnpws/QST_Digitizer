#!/bin/bash

backup.sh

./mktree $1
./evtbuild/evtbuild $1
