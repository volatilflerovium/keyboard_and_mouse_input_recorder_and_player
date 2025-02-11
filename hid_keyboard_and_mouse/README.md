# Companion Project for keyboard_and_mouse_input_recorder_and_player

Build an image for RaspberryPi Pico W, making it into a proxy keyboard and mouse
for keyboard_and_mouse_input_recorder_and_player.

# How to build

```
# cd hid_keyboard_and_mouse
# mkdir build
# cd build
# cmake .. -DFAMILY=rp2040 -DPICO_SDK_PATH=/path/to/pico-sdk
[
-DWIFI_SSID="YOUR_SSID" -DWIFI_PASSWORD="YOUR_PASSWD"\ 
-DUDP_SERVER_BUFFER_SIZE=512 -DUDP_SERVER_PORT=4444\
-DLWIP_MODE=background
]
```

- LWIP_MODE accepts: background or poll. Default: background.

After flashing your Pico W, it should connect to the local network and it will be
given an ip address. You can check the ip asigned to the UDP server with the command:
(first with Pico w unplugged)
```
#nmap -sn 192.168.1.*
```
Then repeat the command with Pico w plugged.