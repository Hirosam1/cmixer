#!/bin/bash
gcc -I../../src/ ../../src/cmixer.c src/main.c -g -L. -lportaudio -lasound -lm -lpthread -O3