# rpi-rgb-matrix-sink-server
rpi-rgb-matrix-sink-server uses [hzeller/rpi-rgb-led-matrix](https://www.github.com/hzeller/rpi-rgb-led-matrix) to control
an array of rgb led matrices.

# Building
Simply cd into /src and run make.

## Dependencies

# JSON Config File Structure
The server is initialized using a JSON configuration file an
example configuration file is provided below.

~~~json
{
	"port" : 12567,
	"hardware_mapping" : "regular",
	"hres" : 64,
	"vres" : 64,
	"panel_hres" : 64,
	"panel_vres" : 64,
	"num_chains" : 1,
	"chain_length" : 1,
	"led_pwm_bits" : 8,
	"led_slowdown_gpio" : 3,
	"led_pwm_lsb_nanoseconds" : 100,
	"panel_maps" : [
		{
			"rot" : 0,
			"num" : 0,
			"source" : {
				"x1" : 96,
				"y1" : 65,
				"x2" : 159,
				"y2" : 128
			},
			"destination" : {
				"x1" : 0,
				"y1" : 0,
				"x2" : 63,
				"y2" : 63
			},
			"rgb_adjust" : {
				"red" : 0,
				"green" : 0,
				"blue" : 0
			}
		}
	]

}
~~~

## Matrix Configuration Options
|Name|Valid Values|Description|
|:---:|:---:|:---:|
|port|1024-6535| Set to an Ephemeral Port of your choice. The server will bind to it.|
|hardware_mapping| "regular", "adafruit-hat", "adafruit-hat-pwm", "compute-module"|Tells the [matrix library](https://www.github.com/hzeller/rpi-rgb-led-matrix) what pin mapping to use.|
|hres|1-256|Horizontal resolution of panel array|
|vres|1-256|Vertial resolution of panel array|
|panel_hres|16-64|Horizontal resolution of a single panel in your array|
|panel_vres|16-64|Vertical resolution of a single panel in your array|
|num_chains|1-6|Number of paralell chains [^num_chains]|
|chain_length|1-64|Number of dasiy chained panels in a single chain.|
|led_pwm_bits|1-11|Number of PWM bits for each pixel
|led_slowdown_gpio|
|led_pwm_lsb_nanoseconds|

## Panel Map Structures

# Network Protocol
## Handshake
This protocol uses a simple handshake mechanism to negotiate
which protocol version is used.

When connecting to a server, the client sends a handshake packet
with the requested protocol version set, as well as success cleared.
If the server can use the requested protocol version, the server
sends the client a handshake packet with success set to 1. The
connection is then assumed to be valid. Otherwise, the server sends
a handshake packet with success set to 0 and the protocol version
it supports. Upon receiving that packet, the client examines the
server's requested protocol version. If it can use that version,
it responds to the server with a handshake packet with success
set to 1. Otherwise, it responds to the server with success set to 0.


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

The value of priority will be used to decide which frame to display
when connection to multiple clients are ongoing.

To calculate how much data is in the packet, the server will add 1
to both sink_packet.h_res and sink_packet.v_res. Then it will multiply
those sums with sink_packet.bytes_per_pixel. If that number is greater
than the maximum packet size the data will be discarded, and the client
socket descriptor will be discarded.

This limits the largest frame that can be sent using this protocol to be
256 x 256 pixel. The server also defines that a pixel shall be no more
than 4 bytes.

### Handshake Header
|Member|Type|Description|
|:---:|:---:|:---:|
|protocol_vers|uint8_t|Protocol Version|
|success|uint8_t|Code indicating success|


# Demonstration

# Future Plans
## Netcode Rewrite

## Per Panel Adjustable Color

## Primitive Drawing RPC Framework



[^num_chains]: This value depends on what adatper board you are using. The Adafruit hat and bonnet both support only 1 chain.
while the electrodragon boards as well as the active-3 and passive-3 boards only support up to 3 chains. The compute module
can support up to 6 paralell chains.
