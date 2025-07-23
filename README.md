# rpi-rgb-matrix-sink-server
rpi-rgb-matrix-sink-server uses [hzeller/rpi-rgb-led-matrix](https://www.github.com/hzeller/rpi-rgb-led-matrix) to control
an array of rgb led matrices.

# Building

# Network Protocol

## Header Structures
### Sink Packet Header
|Member|Type|Description|
|:---:|:---:|:---:|
|protocol_vers|uint8_t|Protocol Version|
|priority|uint8_t|Frame Priority|
|h_res|uint8_t|Horizontal resolution of frame|
|v_res|uint8_t|Vertical resolution of frame|
|bytes_per_pixel|uint8_t|Number of bytes per pixel|
|color_mode|uint8_t|Describes how to handle the pixel data|
|h_loc|uint8_t|Target horizontal location|
|v_loc|uint8_t|Target vertical location|
### Handshake Header
|Member|Type|Description|
|:---:|:---:|:---:|
|protocol_vers|uint8_t|Protocol Version|
|success|uint8_t|Code indicating success|

## Handshake

# JSON Config File Structure

## Matrix Configuration Options

## Panel Map Structures

# Demonstration

# Future Plans
## Netcode Rewrite

## Per Panel Adjustable Color

## Primitive Drawing RPC Framework
