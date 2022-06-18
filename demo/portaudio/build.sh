#!/bin/bash
gcc -I../../src/ ../../src/cmixer.c src/main.c -g -L. -lportaudio -O3