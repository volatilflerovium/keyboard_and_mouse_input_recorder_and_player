# Keyboard and Mouse Input Recorder and Player

Keyboard and Mouse Input Recorder and Player (kmRecPlayerApp) is a Linux desktop application 
for recording sequence of keyboard and mouse input (HID), save it in a file and play it as you wish.

One potential application could be automatic GUI testing. So one could
record a sequence of keyboard/mouse input on a GUI that is been testing.


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

kmRecPlayerApp allows you to record HID input:
- keyboard input: text input, shortcuts, keystroke, unicode input
- mouse input: left/right click, drag/drop/selection
and save it to a file and play them back as if someone were actually doing
the input. Also, kmRecPlayerApp is equipped with a control command that 
allows to take screenshots of a window or region in the screen and compare it 
with a master image, so it can be set to terminate the sequence of input commands
if the comparism fails.

It might be seems like a odd toy. However, in principle
there are two scenarios where it can be useful:
- automatic GUI testing: when developing a GUI, one has to click buttons 
  and input text. We could use kmRecPlayerApp these inputs and play it
  put and play then back and compare the result with a master screenshot.
- Interacting with websites: despite lot of plug-in and script off the shelf
  to automatize interacting with a website, it is true that websites are increasingly
  blocking interaction with these automatic tools.
  
# Features

- Multiple recording modes
  Recording a sequence of HID commands can be a bit challenging because
  some input might change the way windows are position/displaying. Foe example
  a mouse right click usually display a context menu and the same menu will
  go away when it lose focus. In other cases it is stright forward. 
- Save input commands.
- Record input commands in the main screen context (globally) or in a particular window
  context (locally).
- Loop: we can repeat sequence of commands.
- Image based control command: take a screenshot of an area in the screen
  and compare it to a master image. Set it to stop or continue the next input
  command if the control command fails of passes.
- Ability to use [tinyusb](https://docs.tinyusb.org/en/latest/index.html): as a proxy HID
  device so it can set the input commands on the OS.

# Examples

The following examples are here only to illustrate the functionality of kmRecPlayerApp

- GUI development
- download files
- text processing

# uinput

Disclaimer: when using kmRecPlayerApp via /dev/uinput you should keep
in mind the [File Descriptor Hijack vulnerability (CVE-2023-34059)](https://access.redhat.com/security/cve/cve-2023-34059).

The easy way to set the application is using /dev/uinput.
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

# [CVE-2023-34059](https://access.redhat.com/security/cve/cve-2023-34059).
Do NOT say that exposing file descriptors to /dev/uinput will exposes your Linux OS,
it actually says that if your system has already been compromised (a malicious actor might have
access to the system) it could hijack the file descriptor to /dev/uinput to simulate user input.

# License
