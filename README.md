# ZebraBabyToy
Interactive stuffed animal baby toy

Hardware:
 - Firebeetle https://www.dfrobot.com/product-1590.html
 - Lis3dh accelerometer https://www.adafruit.com/product/2809
 - Microphone https://www.adafruit.com/product/1063
 - Speaker/Buzzer https://smile.amazon.com/gp/product/B072HRZXLG
 - Usb battery bank https://smile.amazon.com/Poweradd-Slim-Ultra-compact-Portable-External/dp/B00MWU1GGI/
 - Wireless charger https://smile.amazon.com/gp/product/B01GO07YNU
 - Leds ws2812b (I soldered them individually, but you can cut up/use these) https://smile.amazon.com/ALITOVE-WS2812B-Individually-Addressable-Waterproof/dp/B00ZHB9M6A
 - Wire harness (I cut open and used hdmi cables as they were very tiny and stranded
 - Covered hard electronics in foam https://smile.amazon.com/gp/product/B00TSVSA7C
 - Zipper https://smile.amazon.com/gp/product/B01N1U6P98

Requirements:

 - https://github.com/adafruit/Adafruit_LIS3DH 
 - https://github.com/adafruit/Adafruit_Sensor 
 - https://github.com/Makuna/NeoPixelBus
 

To setup the esp32 with arudino:

 - https://github.com/espressif/arduino-esp32

Useful documents/spec sheets in /docs folder

Features I've tested and will be adding:
 - Microphone fft->color
 - Accel -> speaker
 - Mp3 playback via https://www.dfrobot.com/product-1121.html
 - Games (hold X limb in Y position to play sounds/animation)

Pinout:
![Used Pins](https://i.imgur.com/Nz4bErz.png)

![Available Pins](https://i.imgur.com/lFgS4Zt.png)
