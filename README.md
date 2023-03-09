# WebX Engine

X11 in the browser for the modern web.

> This project is a Work in Progress

## Development

### Building from source

Install the following dependencies:

```bash
apt install cmake dpkg-dev pkg-config build-essential libzmq3-dev libpng-dev libwebp-dev libjpeg-dev libxdamage-dev libxrender-dev libxext-dev libxfixes-dev libxcomposite-dev libxkbfile-dev libxtst-dev 
```

Compiling:

```bash
cmake .
make
```

This will compile the WebX server using a single core. To parallel build the project run the following, for example, to build using 4 cores:

```bash
make -j 4 webx-engine
```

Running WebX:

> WebX will connect to the XDisplay defined by the DISPLAY variable. Make sure this is set correctly beforing executing

```
./bin/webx-engine <options>
```

The following options are available:

|Option | Description|
|---|---|
|-i | Adds WebX interactive keyboard connector to assist in debugging the server|
|-k <layout> | Sets the keyboard layout in the X server (by default WebX will automatically use the existing X11 keyboard) |

WebX will expose three zeromq ports.

> For the moment these ports are not configurable. Please make sure you open these ports on your firewall


| Port | Name           | Description                                                                                         |
|------|----------------|-----------------------------------------------------------------------------------------------------|
| 5555 | Connector port | When a client requests a connection to WebX,  WebX will reply with the collector and publisher ports |
| 5556 | Collector port | The collector port listens for instructions from the client i.e. mouse and keyboard events          |
| 5557 | Publisher port | The publisher port send instructions to the client i.e. window updates, window images etc.          |

[WebX Relay](https://code.ill.fr/webx/webx-relay) is a relay client that *relays* instructions to WebX from web clients connected via websocket.
[WebX Router](https://code.ill.fr/webx/webx-router) is a router used to direct streams of data from multiple clients to a specific webx-engine
[WebX Session MAnager](https://code.ill.fr/webx/webx-session-lanager) is a session manager that handles login credentials and spawns new Xorg and window manager processes
[WebX Web](https://code.ill.fr/webx/webx-web) is a library that connects to the relay and renders a remote desktop environment inside the web browser.



## Running in standalone

*Standalone* allows webx-engine to connect to an existing Xorg session and the relay will avoid contacting a router/session manager. This is used for debug purposes.

Run the following script to launch Xorg and XFCE on DISPLAY :20

```bash
./run_wm.sh
```

You can then run webx-engine in standalone:

```bash
DISPLAY=:20 bin/webx-engine -s
```

### Running in a VISA instance

Clone the source onto a VISA instance and follow the instructions above to compile the webx-engine

Change the XFCE settings so that it uses compositing:

```bash
xfconf-query --channel=xfwm4 --property=/general/use_compositing --type=bool --toggle
```

When running in the remote desktop of VISA run the command:

```bash
bin/webx-engine -s
```

You then configure a relay to run in standalone too by giving it the IP address of the VISA instance.
