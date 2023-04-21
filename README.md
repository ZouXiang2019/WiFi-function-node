# WiFi-functioned node

This project is based on ESP32 CSI Tool (https://stevenmhernandez.github.io/ESP32-CSI-Tool/)

The purpose of this project is to allow for the collection of Channel State Information (CSI) from the ESP32 Wi-Fi enabled microcontroller. 
The purpose of this project is to allow multiple RXs to collect CSI simultaneously and upload it to a server for real-time processing.

The following projects can be found in this repository:

* `./active_sta` - *Active CSI collection (Station)* - Connects to the Access Point (AP) (another ESP32), estimates the CSI and transmits CSI to SPI buffer.
* `./active_ap` - *Active CSI collection (AP)* - AP which can be connected to by devices (ESP32 Station).
* `./receiver` - *CSI relay* - Receives the CSI from Station and transmits to a router.

For tool installation steps, please refer to ESP32 CSI Tool (https://stevenmhernandez.github.io/ESP32-CSI-Tool/)

WiFN (see our paper) consists of two parts: Station and CSI relay. These two parts are connected by SPI bus and handshake line.
The connection method is as follows:
***********            ************
Station       ***       CSI relay 
***********            ************
      MISO**********MOSI
      MOSI**********MISO
      SCLK **********SCLK
           CS**********CS
        IO25**********IO25
***********             ************

Note that, both boards use the WiFi frequency band, so they need to be set in different channels (for example, channel1 and channel 5), and the antennas of these two boards must be one on the top and one on the bottom.

Note: we use ESP32 microcontroller not ESP32 development board for WiFN. You can use an ESP32 programmer to write the tool into ESP32 microcontroller.

The computer runs multiserver.py and collects CSI with .csv file. The collected CSI example is as "example.csv". The totle data has 71 columns, the last column is the timestamp. Columns 2 to 7 from the bottom of the table indicate the MAC address of the CSI relay. The valid data is [3:27 39:64]. Run tableC.m to calibrate the table of .csv file.
