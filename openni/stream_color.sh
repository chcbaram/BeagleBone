#!/bin/bash
mjpg_streamer -i "input_file.so -f /mnt/ramdisk/color" -o "output_http.so -p 8080 -w /mnt/ramdisk/web1/"
