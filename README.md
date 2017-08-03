Fake-Smtp-Server
---------
Fake-smtp-server allows you to execute script when a device is trying to send an email.<br>
It can be used to interface a domotic devices such as a camera, with a domotic server (such as <a href="https://domoticz.com/">Domoticz</a>).

Supported platforms
---------
For now tests are done under Ubuntu 16.04.

Dependencies
---------
- Standard C++ librairies
- Boost

Compilation
---------
```Shell
sudo apt install cmake
sudo apt install libboost-filesystem-dev
git clone https://github.com/PicoI2/fake-smtp-server
cmake .
make
```

How to use it
---------
Modify or create your own script in the execution directory.<br>
The second line of each script file (*.sh) is read by fake-smtp-server which used it as a regular expression.<br>
You could use for example :
```Shell
.*
MAIL FROM: <sender@mail.com>
RCPT TO: <receiver@mail.com>
Subject: Motion detected by camera
```

Then add to your script file any command, such as sending info to your domotic server<br>
curl -s http://domoticz:8080/json.htm?type=command&param=switchlight&idx=39&switchcmd=On&username=MkE=&password=OVM=
