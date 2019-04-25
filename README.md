# WebX

X11 in the browser for the modern web.

> This project is a Work in Progress

### WebX Binary protocol

To be defined

### Development

#### Building from source

Install the following dependencies:

```bash
apt install cmake build-essential libzmq3-dev libpng-dev libxdamage-dev libxrender-dev libxext-dev libxfixes-dev
```

Compiling:

```bash
cmake
make .
```

Running WebX:

> WebX will connect to the XDisplay defined by the DISPLAY variable. Make sure this is set correctly beforing executing

```
./bin/webx
```
WebX will expose three zeromq ports

> For the moment these ports are not configurable. Please make sure you open these ports on your firewall


| Port | Name           | Description                                                                                         |
|------|----------------|-----------------------------------------------------------------------------------------------------|
| 5555 | Connector port | When a client requests a connection to WebX,  WebX will reply with the collector and publisher ports |
| 5556 | Collector port | The collector port listens for instructions from the client i.e. mouse and keyboard events          |
| 5557 | Publisher port | The publisher port send instructions to the client i.e. window updates, window images etc.          |

[WebX Relay](https://code.ill.fr/panosc/data-analysis-services/webx-relay) is a relay client that *relays* instructions to WebX from web clients connected via websocket.


[WebX Web](https://code.ill.fr/panosc/data-analysis-services/webx-web) is a library that connects to the relay and renders a remote desktop environment inside the web browser.


#### Tools

Here are some tools you can use when developing for WebX

**Xephyr**

Xephyr is a nested X server that runs as an X application.


Launch an Xephyr instance listening on display 20:

```
Xephyr -sw-cursor -no-host-grab -nolock -screen 1024x768 :20
```

#### Docs

**Instructions**

| ID | Name    | Description                                |
|----|---------|--------------------------------------------|
| 1  | Connect | Connect instruction                        |
| 2  | Windows | Get the X11 windows                        |
| 3  | Image   | Get an image for a given window identifier |
| 4  | Screen  | Get the screen dimensions                  |