# toy_cdc_cmd

USB Infrared Toy V2 commands that run on a general-purpose PC

## Getting Started

This is program for Windows(MinGW)/Ubuntu/OSX.

### Prerequisites

install libserialport for build(Ubuntu/OSX). 

```
sudo apt-get install libserialport-dev
     or
brew install libserialport
```

## How to use

### Receive infrared remote control

When the reception of the infrared remote control succeeds, it is output to the standard output in Text HEX format.

```
saitou155@Endeavor-AT991E:~/toy_cdc_cmd$ ./toy_cdc_cmd -r
Initialize complate.
SUCCESS:Receive complate. length=600
00 A4 00 4F 00 16 00 13 00 15 00 3A 00 16 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 3A 00 16 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 3A 00 16 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 14 00 13 00 14 00 13 00 14 00 3C 00 14 00 13 00 14 00 3C 00 14 00 3C 00 14 00 3C 00 14 00 3C 00 14 00 13 00 14 00 13 00 14 00 3C 00 14 00 13 00 14 00 3C 00 14 00 3C 00 14 00 3C 00 14 00 3C 00 14 00 13 00 14 00 3C 00 14 0D A3 00 A4 00 50 00 15 00 14 00 13 00 3C 00 14 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 3C 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 3C 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 3C 00 14 00 13 00 14 00 3C 00 14 00 3C 00 14 00 3C 00 14 00 3C 00 14 00 13 00 14 00 13 00 14 00 3C 00 14 00 13 00 14 00 3C 00 14 00 3C 00 14 00 3C 00 14 00 3C 00 14 00 13 00 14 00 3C 00 14 0D A4 00 A5 00 4E 00 17 00 12 00 16 00 3B 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 3C 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 3C 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 3C 00 14 00 13 00 14 00 3C 00 14 00 3C 00 14 00 3D 00 13 00 3D 00 13 00 15 00 13 00 15 00 13 00 3D 00 13 00 15 00 13 00 3D 00 13 00 3D 00 13 00 3F 00 12 00 3F 00 12 00 17 00 10 00 43 00 0E FF FF 
```

To save the standard output Text HEX output to a file, do as follows.

```
./toy_cdc_cmd -r | tee panasonic_tv_power.txt
```

### Transmit infrared remote control

For infrared remote control transmission, put Text HEX as an argument.

```
saitou155@Endeavor-AT991E:~/toy_cdc_cmd$ ./toy_cdc_cmd -t '00 A4 00 4F 00 16 00 13 00 15 00 3A 00 16 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 3A 00 16 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 3A 00 16 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 15 00 13 00 14 00 13 00 14 00 13 00 14 00 3C 00 14 00 13 00 14 00 3C 00 14 00 3C 00 14 00 3C 00 14 00 3C 00 14 00 13 00 14 00 13 00 14 00 3C 00 14 00 13 00 14 00 3C 00 14 00 3C 00 14 00 3C 00 14 00 3C 00 14 00 13 00 14 00 3C 00 14 0D A3 00 A4 00 50 00 15 00 14 00 13 00 3C 00 14 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 3C 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 3C 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 3C 00 14 00 13 00 14 00 3C 00 14 00 3C 00 14 00 3C 00 14 00 3C 00 14 00 13 00 14 00 13 00 14 00 3C 00 14 00 13 00 14 00 3C 00 14 00 3C 00 14 00 3C 00 14 00 3C 00 14 00 13 00 14 00 3C 00 14 0D A4 00 A5 00 4E 00 17 00 12 00 16 00 3B 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 3C 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 3C 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 13 00 14 00 3C 00 14 00 13 00 14 00 3C 00 14 00 3C 00 14 00 3D 00 13 00 3D 00 13 00 15 00 13 00 15 00 13 00 3D 00 13 00 15 00 13 00 3D 00 13 00 3D 00 13 00 3F 00 12 00 3F 00 12 00 17 00 10 00 43 00 0E FF FF'
600 bytes
Initialize complate.
SUCCESS:Tansfer 600 bytes
```

To output the Text HEX file with this command, use the shell function as follows.

```
./toy_cdc_cmd -t "$(cat panasonic_tv_power.txt)"
      or
./toy_cdc_cmd -t "`cat panasonic_tv_power.txt`"
```

### Linux permission setting

Add a rule to udev In LINUX as follows:
```
# /etc/udev/rules.d/99-irusb.rules
#	irdroid
SUBSYSTEMS=="usb", ATTRS{idVendor}=="04d8", ATTRS{idProduct}=="fd08", ENV{ID_MM_DEVICE_IGNORE}="1", GROUP="users", MODE="0666"
```

Update is as follows:
```
sudo udevadm control --reload-rules
```

## License

This project is Free software.


