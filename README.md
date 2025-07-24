# rpi-rgb-matrix-sink-server
rpi-rgb-matrix-sink-server uses [hzeller/rpi-rgb-led-matrix](https://www.github.com/hzeller/rpi-rgb-led-matrix) to control
an array of rgb led matrices.

# Building
Currently building this project is only supported/test on dietpi.
to do so follow the instructions below.

~~~bash
$ sudo apt install libjsoncpp-dev
$ git clone --recursive https://www.github.com/ZachWWalden/rpi-rgb-matrix-sink-server
$ cd rpi-rgb-matrix-sink-server/src
$ make
~~~

# Installing


## Dependencies
[jsoncpp](https://www.github.com/open-source-parsers/jsoncpp)
[hzeller/rpi-rgb-led-matrix](https://www.github.com/hzeller/rpi-rgb-led-matrix)

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
	"led_pwm_bits" : 11,
	"led_slowdown_gpio" : 0,
    "led_pwm_lsb_nanoseconds" : 130,
	"led_pwm_dither_bits" : 0,
	"brightness" : 100,
	"scan_mode" : 0,
	"row_address_type" : 0,
	"multiplexing" : 0,
	"disable_hardware_pulsing" : false,
	"show_refresh_rate" : true,
	"inverse_colors" : false,
	"led_rgb_sequence" : "RGB",
	"panel_type" : NULL,
	"limit_refresh_rate_hz" : -1,
	"disable_busy_waiting" : true,
	"panel_maps" : [
		{
			"rot" : 0,
			"num" : 0,
			"source" : {
				"x1" : 0,
				"y1" : 0,
				"x2" : 63,
				"y2" : 63
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
|hardware_mapping| "regular", "adafruit-hat", "adafruit-hat-pwm", "compute-module"|Tells the [matrix library](https://www.github.com/hzeller/rpi-rgb-led-matrix) what pin mapping to use. Default is "regular".|
|hres|1-256|Horizontal resolution of panel array|
|vres|1-256|Vertial resolution of panel array|
|panel_hres|16-64|Horizontal resolution of a single panel in your array|
|panel_vres|8-64|Vertical resolution of a single panel in your array, must be even.|
|num_chains|1-6|Number of paralell chains [^num_chains]|
|chain_length|1-64|Number of dasiy chained panels in a single chain.|
|led_pwm_bits|1-11|Number of PWM bits for each pixel, more bits = more colors, fewer bits = faster refresh. Default is 11|
|led_slowdown_gpio|0-5|Slowing down the gpio pins is neccessary on newer Pi's. The higher the number the more the slowdown. Default is 0|
|led_pwm_lsb_nanoseconds|50-3000|Time hold for the least significant bit. default is 130|
|led_pwm_dither_bits|0-2|If zero, lsb time dithering is disabled. if 1 only the lsb will be dithered and so forth for 2. Default is 0|
|brightness|0-100|Brightness in %|
|scan_mode|0,1|0 -> Progressive, 1 -> Interlaced. Default is 0|
|row_address_type|0-5|Row address type values can be 0 (default), 1 (AB addressing), 2 (direct row select), 3 (ABC address), 4 (ABC Shift + DE direct), 5 (Test row select)|
|multiplexing|0,1,2|Multiplexing type. 0 -> direct, 1 -> stripe, 2 -> checker. Default is 0|
|disable_hardware_pulsing|true, false|If true, hardware PWM will not be used. Default is GPIO mapping dependent [^hw_pulse]|
|show_refresh_rate|true, false|If true, the refresh rate will be printed to stdout. Default is true.|
|inverse_colors|true, false|If true the colors will be inverted. Default is false|
|led_rgb_sequence|Any permutation of 'R', 'G', and 'B'|Used to shift color channels. Default is "RGB".|
|panel_type|NULL, "FM6126A", "FM6127"|Used to initialize special panels. Default is NULL|
|limit_refresh_rate_hz|0, any refresh rate your system can handle|Sets refresh rate limit. values <= 0 will disable the limit (default)|
|disable_busy_waiting|true, false|Busy waiting leads to better frame pacing in exchange for wasted CPU cycles. Default is true.|
|panel_maps|n/a|Array of PanelMap objects used for configuring your matrix array.|

More information on configuration can be found in the documentation and code of [hzeller/rpi-rgb-led-matrix](https://www.github.com/hzeller/rpi-rgb-led-matrix)

## Panel Map Structures
There should not be more than (num_chains * chain_length) panel maps in the array.

Panel Map
|Name|Valid Values|Description|
|:---:|:---:|:---:|
|rot|0,90,-270,180,-180,270,-90|Rotates the pixels by the given amount in degrees.|
|num|n/a|Unused number indicating the panel number|
|source|n/a|Rectangle indexing the source frame for copying into the matrix library framebuffer.|
|destination|n/a|Rectangle indicating the target location in the matrix library framebuffer.|
|rgb_adjust|n/a|Values to raise or lower the intensity of each color channel in a per panel manner.[^rgb_adj]|

Rectangle
|Name|Valid Values|Description|
|:---:|:---:|:---:|
|x1|n/a|X value of the top left point.|
|y1|n/a|Y value of the top left point.|
|x2|n/a|X value of the bottom right point.|
|y2|n/a|Y value of the bottom right point.|

RGB Adjust
|Name|Valid Values|Description|
|:---:|:---:|:---:|
|red|-128 -> 127|Red adjust value.|
|green|-128 -> 127|Green adjust value.|
|blue|-128 -> 127|Blue adjust value.|

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
[^hw_pulse]: This value is true when mapping is not set to "adafruit-hat".
[^rgb_adj]: It is best to keep these values under +-7. They are added using saturating arithmetic.
