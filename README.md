# Plug for VCV

### Control sex toys using CV signals in VCV

This module requires [Intiface Central](https://intiface.com/central/) to be installed, with the Intiface server running on the default port (ws://localhost:12345).

Each device discovered by Intiface is controllable over one channel of a polyphonic cable. In other words, if 3 devices are discovered, they can each be independently controlled on channels 1, 2, 3 of a polyphonic cable, respectively. If multiple devices are connected and a monophonic cable is inserted, each device will be controlled using the same signal. The setting of each connected device is updated approximately 10 times per second.

Only Windows is supported at this time.

This module makes use of the following open source software:

C++ bindings for Buttplug.io:
https://github.com/dumbowumbo/buttplugCpp
License:
https://github.com/buttplugio/buttplug/blob/master/LICENSE

Buttplug library:
https://github.com/buttplugio/buttplug
License:
https://github.com/buttplugio/buttplug/blob/master/LICENSE

IXWebSocket library:
https://github.com/machinezone/IXWebSocket
License:
https://github.com/machinezone/IXWebSocket/blob/master/LICENSE.txt

nlohmann json Library:
https://github.com/nlohmann/json
License:
https://github.com/nlohmann/json/blob/develop/LICENSES/MIT.txt



