# High Power LED Lamp (HomeKit enabled, dimmable)
I designed this LED lighting system as a replacement for old halogen R7S uplights.      
These lamps often use high power (400W) bulbs outputing (according to the manufacturer) around 8800 lumen. Meanwhile the only off the shelf LED uplights I could find output 2000 lumen while using around 25W. Furthermore, altough replacement cob LEDs for R7S sockets are available they are typically "20W". I tested several of them and they started at around 15W, after around one hour they only used 10W, they are not bright enough, and they started flickering from time to time.

Also, I dislike how modern LED uplights have to be replaced entirely if a fault with the LEDs or dirver circuit occurs.
Using a modular design, we can reduce waste and wast less power.

### Features
- [x] High power Cree XHP50.3 LEDs (efficient, high CRI 90+) 
- [x] Modular design (2, 4, or 6 LEDs) 
- [x] Dimmable
- [x] Up to 108W, approx 12000 lumen
- [x] Homekit enabled (uses esp8266)
- [x] Manual On/Off switch in addition to Homekit (I use a 433MHz remote)
- [x] Uses 12V to 48V
- [x] Temperature sensor on each LED daughter board
- [x] Each LED on a separate daughter board, thus each led can be raplaced easily
- [x] Open source (Arduino compatible)



### References
The most useful resource for this project was SDG Electronics long-lasting video series on designing an LED ring-light for his microscope using the slightly older Cree XHP50.2 LEDs, as well as his modular LED driver board series.
