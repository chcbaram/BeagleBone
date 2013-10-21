#!/bin/bash
mjpg_streamer -i "input_file.so -f /mnt/ramdisk/depth" -o "output_http.so -p 8081 -w /srv/"
