# SlideScan
Filmolino SlideScan is a device that makes it easy to scan slides in magazines fully automatic using a modified slide projector and a digital camera (DSLR, MILC, DSLM etc.). It controls the slide projector and a digital camera using a cable or IR shutter release. It can be powered directly from the slide projector or over the mini USB connector.

It supports cameras with cable releases or infrared remote control (Nikon, Canon, Pentax, Sony, Olympus, Minolta). 

New cable releases and IR controls can easy be added to the software. 
 
Thanks to the [M5StampC3 from M5Stack](https://shop.m5stack.com/collections/m5-controllers/products/m5stamp-c3-mate-with-pin-headers) using a [ESP32-C3 RISC-V microcontroller from Espressif](https://www.espressif.com/en/products/socs/esp32-c3) it is also possible to control the SlideScan through a web interface over WiFi. The scanner has it's own access point to which you can connect.

Everything of the SlideScan is open source, so you can easy build your own!

If you don't want to build your own SlideScan and prefer to buy one please contact me.

## Guide
### Camera connection
You can connect your digital camera using a cable connection or infrared remote control. To control your camera using the cable release you need a shutter release cable for your camera with a 2.5 mm jack connector on the other end. You can find the right cable for your camera searching the web for *JJC Remote Control Shutter Release Cable*. They are not expensive you should get one for about 10 USD/EUR/CHF.


### DIN cable connection to pcb
Connect the male DIN 6 connector as follows to the pcb:

| Pin PCB | Pin DIN |Function |
| --- | --- |
| 1 | 4 | Reverse slide transport |
| 2 | 3 | Forward slide transport |
| 3 | 2 | Positive DC voltage |
| 4 | x | Not used |
| 5 | x | Not used |
| 6 | 1 | Negative DC voltage (only needed if you want to power SlideScan from the projector) |


### Slide projector connection
**Important: Use the Tape/remote control socket and not the lap dissolve socket.**
Connect the 6 pin DIN connector of the Filmolino SlideScan to the *remote control socket* of your slide projector. Usually SlideScan can be powered from the slide projector or you can power it over the Mini USB connector. 

To power it from the projector the following components *R2, C4, U1, C3, D2* need to be assembled on the pcb. If you power SlideScan over USB you don't need this components.

To not damage your SlideScan or slide projector check the pin map below first!
6 pin DIN connector
| Pin | Function |
| --- | --- |
| 1 | Reverse slide transport |
| 2 | Forward slide transport |
| 3 | Positive DC voltage (This voltage is usually about 30...35 VDC on most slide projectors) |
| 4 | Focus motor |
| 5 | Not used |
| 6 | Negative DC voltage |

**The maximum input voltage for SlideScan if you want to power it from the projector on pin 3 is 42 VDC!**


### USB power
If you don't want to power your SlideScan from the slide projector connect an USB charger to the mini USB connector of the SlideScan.


### Turn on
Now you are ready to turn SlideScan on. To do so turn on the slide projector and if needed the USB charger.
After turning on the green LED should light up and the red one slowly blinking (because there is no WiFi connection yet).

### WiFi Connection
SlideScan starts its own WiFi access point: 

Default WiFi credentials:
SSID : **scanner**
Password : **filmolino**
IP : **192.168.4.1**

To see the web user interface do the following:
1. Connect your PC/Tablet/... to the SLideScan network *scanner* using the password *filmolino*
2. In your webbrowser enter the address: *192.168.4.1*
3. You should now see the web interface of the scanner

### Configuration
To see the configuration click on *Config* in the side of the header. Now you should see the configuration menu.

#### Access Point
Here you can change the SSID and the password.

#### WiFi Network
Not implemnted yet.

#### Scanning
Here you can config the SlideScan.

##### Slide Change Time
This is the time the slide projector needs to change the slide.
Unit is milliseconds (1 second = 1000 milliseconds)

##### Slide Change Pulse Time
Duration of the pulse for the slide transport. During this time the slide transport contacts are closed. 
Unit is milliseconds (1 second = 1000 milliseconds)

##### Scan Time
This is the time the digital camera needs to capture (scan) the slide
Unit is milliseconds (1 second = 1000 milliseconds)

##### Slides to scan
Number of slides in the magazine.

##### Selected cable release
Can be OFF or the cable release you want to use.

##### Selected Infrared Release Code
Can be OFF or the brand name of your digital camera.

##### Buttons
**Save** Saves the changes
**Delete Config** Deletes the actual configuration, you need to click the *Restart System* button after that
**Restart System** Restarts the scanner

### Standalone operation 
The device is controlled with only two buttons. The functions are as follows:

| Button | Short press | Long press |
| --- | --- |
| Left | Forward slide transport | Backward slide transport |
| Right | Trigger camera (if auto an auto scan is active, stop it) | Start auto scan |

The actual state is indicated by a bi-color LED:
| State | LED green | LED red |
| --- | --- |
| Running and no WiFi connection | ON | BLINKING slowly |
| Running with WiFi connection | ON | OFF |
| Auto scan active | BLINKING | OFF |
| Error | OFF | ON |

#### Auto Scan
When starting an Auto Scan the scan is always starts with image 1 and then the number of images is scanned. If the image counter is higher it will be reset to 1 before start.

### WiFi operation
Connect to the WiFi and open the address *192.168.4.1* in your webbrowser as described above. The SlideScan is connected as long as you see the green connected sign in the top left corner.

This view shows the current slide counter and the slides in the magazine (0/50). 
You can start an auto scan or manually scan slides using the five buttons below. If you want to change the current configuration click *Config* in the top right corner.

The buttons are:
| Start Auto Scan | Forward | Backward | Capture (Scan) | Reset current slide |

To change to number of slides in the current magazine you can click on the current slide counter or go to the config menu.

## Remarks
- slidescan.local address not working yet. So you have to point your browser to the default IP **192.168.4.1**
- Station mode (to connect to a WiFi network) not working yet
- Tested with Nikon and Pentax cameras

## Hardware
You can find the KiCAD project with complete schematic and pcb design in the hardware folder. The double sided pcb can be easily soldered by hand. It uses just a few SMD components in not too small cases (0805). The components are easily available from the usual distributors.

## Software
### Project
The software is build with ESP-IDF using platform.io and Visuals Studio Code.

## Documentation
The documentation with images can be found in the docs folder. Please note that the images are showing version 1.0 of the hardware.