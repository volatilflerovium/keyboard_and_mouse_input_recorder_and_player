# Keyboard and Mouse Input Recorder and Player

Keyboard and Mouse Input Recorder and Player (KeyboardAndMouseRecorderPlayer)
is a Linux desktop application for recording sequence of keyboard and mouse
input that can be saved to a file and be played it at any time to simulate
user input.

## Content
- [Overview](#overview)
- [Dependencies](#dependencies)
- [Features](#features)
- [AppImage](#appimage)
- [Examples](#examples)
- [Interface Method](#interface-method)
	- [uinput](#uinput)
	- [TinyUSB](#TinyUSB)
	- [vm](#vm)
- [Control Command](#control-command)
- [Things to be Considered](things-to-be-considered)
- [License](#license)

## Overview

KeyboardAndMouseRecorderPlayer allows you to record HID input from:
- keyboard: text input, shortcuts, keystroke, unicode input and
- mouse: left/right click, drag/drop/selection
and save it to a file and play them back as if someone were actually doing
the input. Also, KeyboardAndMouseRecorderPlayer is equipped with a control command that 
allows to take screenshots of a window or region in the screen and compare it 
with a master image, so it can be set to terminate the sequence of input commands
if the comparison fails.

In principle there are two scenarios where it can be useful:
- automatic GUI testing: when developing a GUI, one has to click buttons 
  and input text. We could use KeyboardAndMouseRecorderPlayer these inputs and play it
  put and play then back and compare the result with a master screenshot.
- Interacting with websites: despite lot of plug-in and script off the shelf
  to automatize interacting with a website, it is true that websites are increasingly
  blocking interaction with these automatic tools.

## Dependencies

In order to run KeyboardAndMouseRecorderPlayer, your system needs to meet
the following dependencies:

- import (is a member of the ImageMagick suite of tools)
- x11-utils

## Features

- Multiple recording modes: recording a sequence of HID commands can be a bit 
  challenging because some input might change the way windows are position/displaying. 
  For example a right click of the mouse usually display a context menu and the same menu will
  go away when it lose focus. In other cases it is straight forward. 
- Save input commands: the recorded input commands can be save to a file that can be loaded
  later.
- Recording of input commands: with global context (on the context of the main screen)
  or local context (on the context of a particular window).
- Loop: we can repeat sequence of commands.
- Image based control command: take a screenshot of an area in the screen
  and compare it to a master image. Set it to stop or continue the next input
  command if the control command fails of passes.
- Ability to use [TinyUSB](https://docs.TinyUSB.org/en/latest/index.html): as a proxy HID
  device so it can set the input commands on the OS.
- Time padding

## AppImage

[KeyboardAndMouseRecorderPlayer](https://github.com/volatilflerovium/keyboard_and_mouse_input_recorder_and_player/releases)
is available as an AppImage which means "one app = one file", which you can download and run on your
Linux system while you don't need a package manager and nothing gets changed
in your system.

AppImages are single-file applications that run on most Linux distributions.
Download it, make it executable, and run! No need to install.
No system libraries or system preferences are altered.
Most AppImages run on recent versions of Arch Linux, CentOS, Debian, Fedora,
openSUSE, Red Hat, Ubuntu, and other common desktop distributions.

## Examples

The following examples are here only to illustrate the functionality of KeyboardAndMouseRecorderPlayer

- GUI development
- download files
- text processing

## Interface Method

KeyboardAndMouseRecorderPlayer can interface with the OS via two methods
- via file descriptor to /dev/uinput
- [TinyUSB](https://docs.TinyUSB.org/en/latest/index.html)

**_Disclaimer_**: when using KeyboardAndMouseRecorderPlayer via /dev/uinput you should be aware of
the [File Descriptor Hijack vulnerability (CVE-2023-34059)](https://access.redhat.com/security/cve/cve-2023-34059).
Notice that (CVE-2023-34059) Does NOT say that open file descriptors to /dev/uinput will 
exposes your Linux OS, what it actually says is that if your system has already been 
compromised with a malicious actor, this could hijack the file descriptor to /dev/uinput 
to simulate user input.

### /dev/uinput

Using /dev/uinput is straight forward. 
- create a new group and add your username to this group
```
# groupadd the_new_group_name
# usermod -aG the_new_group_name username
```
Notice that group membership is re-read on login so You will have log out 
and back in for this to take effect.

- Set the permissions
```
# sudo chown root:the_new_group_name /dev/uinput;
chmod 720 /dev/uinput
```
Permission 720 should be enough, but in some cases you will need to set 777. 
Remember these permissions are not permanent, they will be revoke at reboot.
Alternatively you can use a device rule file.

### TinyUSB

[TinyUSB](https://docs.TinyUSB.org/en/latest/index.html) is an open-source cross-platform USB Host/Device stack for embedded system.
This project uses TinyUSB to emulate a phisical keyboard and mouse that can
be used as the keyboard/mouse for KeyboardAndMouseRecorderPlayer.

The directory [hid_keyboard_and_mouse](https://github.com/volatilflerovium/keyboard_and_mouse_input_recorder_and_player/tree/main/hid_keyboard_and_mouse)
has a project to build a image for RaspberryPi Pico W for this purpose. Then
we can configure KeyboardAndMouseRecorderPlayer as UDP client to send the input commands
to the installation of TinyUSB in the Pico W.

### vm

It would be worthy to consider running KeyboardAndMouseRecorderPlayer on
a Linux installation on a Virtual Machine, it this way, it will be on an isolated 
enviroment in particular if it has to run long sequences of input commands.
Beside in a VM, the risk of [CVE-2023-34059](https://access.redhat.com/security/cve/cve-2023-34059)
should be irrelevant.

## Control Command

The most important feature of KeyboardAndMouseRecorderPlayer is its functionality
to set a Control Command which takes a screenshot of a particular area of the
screen or of a window and compares it to one previously taken in order to take decision
to stop execution of next commands or continue. Without this, some commands
might miss their target. This decision can be set
as the image should be same or different. Imagine to setup a sequence of input
commands to login to a website. Sometime it could take few seconds to login.
We can set a Control Command to wait till the image of the login screen or
any particular area, changes before applying the next input command.
See the [user manual](https://github.com/volatilflerovium/keyboard_and_mouse_input_recorder_and_player/blob/main/user_manual.pdf)

## Things to be Considered

- KeyboardAndMouseRecorderPlayer will apply the input commands continuously as they are set. But 
KeyboardAndMouseRecorderPlayer is not aware about the context of the commands, in other words,
the commands will be input independently of which is the current active window
in the screen. For example an input command might be considered to be applied on a
particular window, but if the window is not active, that input command will be
captured by whichever window is active at that moment. In this sense keep in mind
that a particular input like a shortcut in a particular window might be set for a different
action in another window. 

## License
