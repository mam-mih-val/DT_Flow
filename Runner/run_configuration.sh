#!/bin/bash

name=$1

dirname=gt80$name
./batch.sh --signal adc --min 80 --max 999 file.list ../Output/dirname

dirname=gt80lt120$name
./batch.sh --signal adc --min 80 --max 120 file.list ../Output/dirname

dirname=gt120$name
./batch.sh --signal adc --min 120 --max 999 file.list ../Output/dirname