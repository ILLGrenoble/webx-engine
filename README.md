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

## Building, running and debugging in a devcontainer

This project includes a `.devcontainer` folder for developing and debugging the WebX Engine in a devcontainer (primarily focussing on using the cpptools extension in VSCode, but potentially accessible too under other IDEs).

Two development environments are available: standalone and multiuser.

### Standalone devcontainer environment

The <em>standalone</em> devcontainer environment is the simplest way to develop the WebX Engine. 

The devcontainer spins up the `ghcr.io/illgrenoble/webx-dev-env-ubuntu` environment which automatically launches Xorg and the Xfce4 desktop manager on DISPLAY=:20. 

The VSCode launch command <em>Debug WebX Standalone (:20)</em> will compile and run the WebX Engine in standalone mode and attach the X11 Server. You can debug the source directly by adding breakpoints to the source files.

### Multiuser devcontainer environment

If you want to debug the full WebX stack with both the WebX Router and WebX Session Manager running then you should run the <em>multiuser</em> devcontainer environment.

This environment installs extra packages on top of the `ghcr.io/illgrenoble/webx-dev-env-ubuntu` image:
 - Rust development tools are installed
 - The latest stable source of webx-session-manager and webx-router and downloaded and built
 - webx-session-manager and webx-router are launched automatically
 - The container is configured with several standard users (mario, luigi, peach, toad, yoshi and bowser - username and password identical)
 - webx-router is configured to launch the webx-engine built from the local sources

Running in this mode means you have to build the webx-engine manually (as described above).

The webx-engine is launched by the WebX Router when a login is requested. To debug the process you have to attach to it: Run the VSCode launch command <em>Debug Running WebX Process</em> and search for the `webx-engine` process in the list proposed by VSCode.

### Running the WebX Demo to test the WebX Engine

In a terminal on the host computer the WebX Engine the simplest way to test the WebX Engine is by running the [WebX Demo Deploy](https://github.com/ILLGrenoble/webx-demo-deploy) project. This runs the WebX Demo in a docker compose stack and can attach to the WebX Engine (or WebX Router in multi-user mode) running in the dev container.

#### Running in standalone mode

If you are just developing the WebX Engine on its own, within the webx-demo-deploy project run the command

```
./deploy.sh -sh host.docker.internal
```

This allows the demo to attach directly to a WebX Engine that you launch in the devcontainer.

In a browser open https://localhost

#### Running in multiuser mode

To fully test the WebX Stack run the demo as follows:

```
./deploy.sh
```

In a browser open https://localhost

You need to set the host of the WebX Server: running in a local devcontainer, set this to `host.docker.internal`.

Log in with any of the pre-defined users (mario, luigi, peach, toad, yoshi and bowser), the password is the same as the username.

This will send the request to the WebX Router: the WebX Session Manager will authenticate the user and run Xorg and Xfce4 for the user; WebX Router then launches the locally-built webx-engine. You can debug the webx-engine process as described above by attaching to the process in the VSCode launch command.