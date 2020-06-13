# 4chord MIDI Bootloader

The bootloader allows to update the 4chord MIDI firmware through USB. It uses a rather simple, home-brew protocol to set up the firmware transfer, transfer and flash the firmware itself, as well as writing EEPROM data. It consists of a host-side and a device-side part, with the host side initiating the transfer, and the device-side flashing chunks of the new firmware file as it receives it.

## Protocol

The protocol used to communicate between the host-side Python script and the device-side firmware uses USB control transfers and looks like this:

| Command           | Type | Hex    |  Description |
| ---               | ---- |  ---   | ---- | 
| HELLO             | Recv | `0x01` | Initiates communication and retrieves the bootloader version string of size 'buffer'|
| FWUPDATE_INIT     | Send | `0x10` | Initiates firmware transfer, giving the number of expected 128 byte chunks|
| FWUPDATE_MEMPAGE  | Send | `0x11` | Transfer a single firmware chunk |
| FWUPDATE_VERIFY   | Recv | `0x12` | Read back the last transferred firmware chunk for verification |
| FWUPDATE_FINALIZE | Send | `0x13` | Finalize firmware transfer |
| EEPROM_READ       | Recv | `0x20` | Read EEPROM value from a given address |
| EEPROMT_WRITE     | Send | `0x30` | Write EEPROM value to a given address |
| BYE               | Send | `0xf0` | Terminate the communication |
| RESET             | Send | `0xfa` | Reset the device |

With the Python script being the initiating side, the Recv/Send type refers to its point of view.

### Device Side

On the device side, the bootloader firmware is located in the ATmega's dedicated bootloader memory section, and the communication is implemented via V-USB.

#### State machine

There's a simple state machine on the device side making sure that everything has its order.

| State    | Description
|  ---     | --- |
| IDLE     | No communication initiated, requires `HELLO` command |
| HELLO    | Communication initiated, ready to either start firmware update via `FWUPDATE_INIT` command or EEPROM operations |
| FWUPDATE | Firmware update ongoing, only `FWUPDATE_*` related commands are handled |
| RESET    | Device reset initiated

#### Debug build

In case you want to modify the device-side bootloader firmware and have to debug something, it can be built with a `DEBUG` flag that adds extra debug output via UART. However, due to size limitations, enabling UART debug will disable graphical output on the LCD. So it's either LCD or UART, and without the `DEBUG` flag, no UART functions are available. Also, flashing with debug output enabled is really slow.

If you switch between `DEBUG` and normal built, it's best to `make clean` between the builds.


### Host Side

To run the host-side bootflash script, Python 3, PyUSB, and a udev rule to allow access to the device is needed. Using `sudo` or being root will also do for the latter, but nothing wrong with a udev rule to make life easier.

#### udev rule

There are two options to set up the permissions: set the device ownership to the current user, or set the device's read and write permission for everyone. The more flexible one is the latter, and would like this:

```
SUBSYSTEM=="usb", ATTRS{idVendor}=="1209", ATTRS{idProduct}=="deaf", MODE="0666"
```

You can either manually copy this to a udev file in `/etc/udev/rules.d/`, or use the installation script in the repository's [`sys/udev/` directory](../sys/udev/) to do it automatically.
