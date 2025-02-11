# keyboard_and_mouse_input_recorder_and_player

Linux desktop application for recording keyboard and mouse input, save
it and play it as you wish.

Consider the following scenery: automatic testing of a GUI. So one could
record which buttons to press and what keystroke to input and compare
the result with a screenshot of the expected result, which is done by the
same application.

# DOCUMENTATION WORK IN PROGRESS

# Content
- [Overview](#overview)
- [Features](#features)
- [Examples](#examples)
- [uinput](#uinput)
- [Tinyusb](#tinyusb)
- [VM](#vm)
- [License](#license)

# Overview

This application allows you to record HID input from keyboard and mouse
and save it to a file and play them back as if someone were actually doing
the input. It might be seems like a odd toy. However, in principle
there are two scenarios where it can be useful:
- Helping to test GUI:
  When developing a GUI, one has to click buttons and input text. We could use
  kmRecPlayerApp to record the input and play then back and compare it
  with a screenshot.
- Interacting with websites. Despite lot of plug-in and script off the shelve
  to automatize login or download files from websites, it is true that
  websites are increasingly blocking interaction with automatic tools.

# Features

- Multiple recording modes
  Recording a sequence of HID commands can be a bit challenging because
  some input might change the way windows are position/displaying. Foe example
  a mouse right click usually display a context menu and the same menu will
  go away when it lose focus. In other cases it is stright forward. 
- Save input commands
- Loop
- Image based control command: take a screenshot of an area in the screen
  and use it to compare 
- Ability to use /dev/uinput or tinyusb via serial port or udp socket.

# Examples

The following examples are here only to illustrate the functionality of
kmRecPlayerApp

- GUI development
- download files
- text processing


# uinput

The easy way to set the application is using /dev/uinput.
Disclaimer: when using kmRecPlayerApp via /dev/uinput you should keep
in mind the [File Descriptor Hijack vulnerability (CVE-2023-34059)](https://access.redhat.com/security/cve/cve-2023-34059).


However you could use a dedicated VM.


For it you will
need to set the appropriate permission on it. Notice that because of the nature of
/dev/uinput, the permissions are not permanent.

If you want to use /dev/uinput, you need the following steps:
- create a new group and add you to these group

```
# create my_uinput_group
```
then log out and log in so the new group take effect.

Set the permissions
```
# sudo chown root:my_uinput_group /dev/uinput;
chmod 720 /dev/uinput
```
In some cases you will need to set 777. Remember these permissions are
not permanent, they will be revoke at reboot.
Alternatively you can use a device rule file (link).


# Tinyusb

To remove the CVE-2023-34059 threat, you could install the companion project
in a Raspberry Pi Pico or Pico W (RP2040) which will act as a normal usb keyboard/mouse
[tinyusb](https://docs.tinyusb.org/en/latest/index.html). Then
we can configure kmRecPlayerApp to send the commands to the Pico
via UDP socket (only for Pico w) or via serial port (for Pico and Pico W).

Currently the companion project build an image for Pico W.

# VM

Notice that kmRecPlayerApp it would be better to use a VM to run HID commands
via kmRecPlayerApp because in this way the keyboard and/or mouse input will
be isolated to the VM without interfering with the host. For exaple
if you are using kmRecPlayerApp to download files from internet.

#
Some words on the [CVE-2023-34059](https://access.redhat.com/security/cve/cve-2023-34059).
If we use the analogy where a house is the OS and a telephone in the house is /dev/uinput.
CVE-2023-34059 is like allowing anybody in the house to use the telephone.


# License
