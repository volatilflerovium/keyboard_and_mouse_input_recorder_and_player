/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * 
 * Base on https://github.com/hathach/tinyusb/tree/master/examples/device/hid_multiple_interface
 * Modification for pico w and UDP socket
 *    -- Dan Machado <dan-machado@yandex.com> 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "udp_server/picow_udp_server.h"

#include "bsp/board_api.h"
#include "tusb.h"


//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

// Interface index depends on the order in configuration descriptor
enum {
	ITF_KEYBOARD = 0,
	ITF_MOUSE = 1
};

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum  {
	BLINK_NOT_MOUNTED = 250,
	BLINK_MOUNTED = 1000,
	BLINK_SUSPENDED = 2500,
};

#define KEYBOARD_START 0xE8
#define KEYBOARD_END 0xE9
#define MOUSE_START 0xEB

static bool kbrd=false;

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

void led_blinking_task(void);

void hid_task(void);

void hid_job(void);

void hid_task_cb(const uint8_t* buffer, const uint16_t buffer_size);

//=====================================================================
//--------------------------------------------------------------------+
// MAIN
//--------------------------------------------------------------------+

int main(void)
{
	// Start network
	if(!init_network()){	
		return 1;
	}

	board_init();

	// init device stack on configured roothub port
	tusb_rhport_init_t dev_init = {
		.role = TUSB_ROLE_DEVICE,
		.speed = TUSB_SPEED_AUTO
	};

	tusb_init(BOARD_TUD_RHPORT, &dev_init);

	if (board_init_after_tusb) {
		board_init_after_tusb();
	}

	start_udp_server(&hid_task_cb);

	while(1){
		server_poll();//
		tud_task();
		led_blinking_task();
	}

	stop_network();
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

static uint8_t keycode[6] = {0};
static int key_count=0;
static uint8_t modifier=0;

static void reset_key_buffer()
{
	memset(keycode, 0, 6);
	key_count=0;
	modifier=0;
}

struct Mouse_Data
{
	uint8_t marker;
	uint8_t btns;
	int16_t absX;
	int16_t absY;
	int8_t scrollV;
	int8_t scrollH;
};

void hid_task_cb(const uint8_t* buffer, const uint16_t buffer_size)
{
	// Remote wakeup
	if(tud_suspended()){
		tud_remote_wakeup();
	}

	if(!tud_hid_n_ready(ITF_KEYBOARD) && !tud_hid_n_ready(ITF_MOUSE)){
		send_msg("Failed");
		return;
	}

	if(buffer_size==2){
		if(buffer[0]==KEYBOARD_START && buffer[1]==KEYBOARD_END){
			handshake();
		}
		return;
	}

	if(buffer[0]==KEYBOARD_START){
		for(int i=0; i<buffer_size; i++){
			if(buffer[i]==KEYBOARD_START){
				reset_key_buffer();
				continue;
			}
			if(buffer[i]==KEYBOARD_END){
				tud_hid_n_keyboard_report(ITF_KEYBOARD, 0, modifier, keycode);
				kbrd=true;
				continue;
			}
				
			if(buffer[i]>0xD9){
				if(buffer[i]<KEYBOARD_START){
					modifier=modifier | (1<<(int)(buffer[i]-0xE0));
				}
				continue;
			}
			keycode[key_count++]=buffer[i];
		}
	}
	else if(buffer[0]==MOUSE_START){
		if(buffer_size<8){
			uint8_t btn=buffer[1];
			const int8_t* buff=(const int8_t*) buffer;
			tud_hid_n_mouse_report(ITF_MOUSE, 0, btn, buff[2], buff[3], buff[4], buff[5]);
			kbrd=true;
		}
		else if(buffer_size==8){
			struct Mouse_Data mouse;
			memcpy((void*)&mouse, (const void*)buffer, 8);
			tud_hid_n_abs_mouse_report(ITF_MOUSE, 0, mouse.btns, mouse.absX, mouse.absY, mouse.scrollV, mouse.scrollH);

			kbrd=true;
			send_msg("position: (%d, %d)", mouse.absX, mouse.absY);
		}
	}
}

/*
 * Send empty key report if some keyboard or mouse report has previously sent.
 * Also, we notify the client we are ready to get the next input.
 * Sender should wait for notification before trying to send another input
 */ 
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
	(void) report;
	(void) len;
	if(kbrd){		
		kbrd=false;
		if(instance==ITF_KEYBOARD){
			tud_hid_n_keyboard_report(instance, 0, 0, NULL);
		}
		send_ok();			
	}
}

void tud_hid_report_failed_cb(uint8_t instance, hid_report_type_t report_type, uint8_t const* report, uint16_t xferred_bytes)
{
	(void) instance;
	(void) report_type;
	(void) report;
	(void) xferred_bytes;
	send_msg("fails");
}

// Default definition
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) itf;  (void) report_id;  (void) report_type;  (void) buffer;  (void) reqlen;
  return 0;
}

// Default definition
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  // TODO set LED based on CAPLOCK, NUMLOCK etc...
  (void) itf;  (void) report_id;  (void) report_type;  (void) buffer;  (void) bufsize;
}


//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+

void led_blinking_task(void)
{
  static uint32_t start_ms = 0;
  static bool led_state = false;

  // Blink every interval ms
  if ( board_millis() - start_ms < blink_interval_ms) return; // not enough time
  start_ms += blink_interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state; // toggle
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  blink_interval_ms = tud_mounted() ? BLINK_MOUNTED : BLINK_NOT_MOUNTED;
}

//--------------------------------------------------------------------+

