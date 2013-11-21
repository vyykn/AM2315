# AM2315 Encased I2C Temperature/Humidity Sensor

Simple asyncronous node.js library for reading the AM2315 I2C via Raspberry Pi


## Example

`npm install AM2315`


```
var sensor = require('AM2315');

// Get the current Temp and Humidity
sensor.read(function(err, data){
	if(err) console.error(err);
	else console.log("Temperature: %d F   Humidity: %d\%", data[0], data[1]);
});
```

Probably requires running node under `sudo` for I2C permissions, unless you've used [Wiring Pi's gpio utility](https://projects.drogon.net/raspberry-pi/wiringpi/the-gpio-utility/) or otherwise adjusted device permissions.

## API

### spi.read(cb(err, data){})

Collects the current Temerature and Humidity from the AM2315 via I2C on the Raspberry Pi and returns them as an array (data[temp, humidity]) with your callback.




## Compatibility Note

This library is subject to change.



## License

Copyright © 2013, Nathan Vander Wilt.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met: 

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies, 
either expressed or implied, of the FreeBSD Project.