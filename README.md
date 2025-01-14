# WebX Engine

## Description

The WebX Engine provides the core to the WebX Remote Desktop stack. It connects to an X11 server and listens to server events such as window updates or mouse movements and publishes the event data using its own binary protocol.

Clients connect to the WebX Engine using ZeroMQ IPC or TCP sockets. Clients are able to request information about the current display (such as the screen size, windows layout, window image) and send instructions to the X11 server (such as mouse position or keyboard actions). The Client also subscribes to display updates (including window movements, resize and image updates).

The WebX Engine aims to produce a high quality, low latency representation of the X11 Display however it includes the possibility to reduce quality and update frequency if bandwidth to the end user is low.

Designed primarily to be used in a multi-user environment, spawing of a WebX Engine is usually managed by the [WebX Router](https://github.com/ILLGrenoble/webx-router). However, for development purposes the WebX Engine can be launched in a <em>standalone</em> from a terminal and connect to an X11 Display defined using the DISPLAY environment variable.

### Included in this project

This project includes:
 - The WebX Engine C++ source code and tests
 - CMake build script
 - VSCode Launch commands
 - devcontainer development environments
 - Dockerfiles to build the WebX Engine and package it in a Debian Package
 - Github actions to buid Debian Packages and add them to releases
 - A tool to generate keyboard symbol mapping source code from [Apache Guacamole keyboard mappings](https://github.com/apache/guacamole-server/tree/main/src/protocols/rdp/keymaps)
 - A window manager launch script (`run_wm.sh`) to quickly start an Xorg process and run xfce4.

## WebX Overview

WebX is a Remote Desktop technology allowing an X11 desktop to be rendered in a user's browser. It's aim is to allow a secure connection between a user's browser and a remote linux machine such that the user's desktop can be displayed and interacted with, ideally producing the effect that the remote machine is behaving as a local PC.

WebX's principal differentiation to other Remote Desktop technologies is that it manages individual windows within the display rather than treating the desktop as a single image. A couple of advantages with a window-based protocol is that window movement events are efficiently passed to clients (rather than graphically updating regions of the desktop) and similarly it avoids <em>tearing</em> render effects during the movement. WebX aims to optimise the flow of data from the window region capture, the transfer of data and client rendering.

> The full source code is openly available and the technology stack can be (relatively) easily demoed but it should be currently considered a work in progress.

The WebX remote desktop stack is composed of a number of different projects:
 - [WebX Engine](https://github.com/ILLGrenoble/webx-engine) The WebX Engine is the core of WebX providing a server that connects to an X11 display obtaining window parameters and images. It listens to X11 events and forwards event data to connected clients. Remote clients similarly interact with the desktop and the actions they send to the WebX Engine are forwarded to X11.
 - [WebX Router](https://github.com/ILLGrenoble/webx-router) The WebX Router manages multiple WebX sessions on single host, routing traffic between running WebX Engines and the WebX Relay. 
 - [WebX Session Manager](https://github.com/ILLGrenoble/webx-session-manager) The WebX Session manager is used by the WebX Router to authenticate and initiate new WebX sessions. X11 displays and desktop managers are spawned when new clients successfully authenticate.
 - [WebX Relay](https://github.com/ILLGrenoble/webx-relay) The WebX Relay provides a Java library that can be integrated into the backend of a web application, providing bridge functionality between WebX host machines and client browsers. TCP sockets (using the ZMQ protocol) connect the relay to host machines and websockets connect the client browsers to the relay. The relay transports data between a specific client and corresponding WebX Router/Engine.
 - [WebX Client](https://github.com/ILLGrenoble/webx-client) The WebX Client is a javascript package (available via NPM) that provides rendering capabilities for the remote desktop and transfers user input events to the WebX Engine via the relay.

To showcase the WebX technology, a demo is available. The demo also allows for simplified testing of the WebX remote desktop stack. The projects used for the demo are:
 - [WebX Demo Server](https://github.com/ILLGrenoble/webx-demo-server) The WebX Demo Server is a simple Java backend integrating the WebX Relay. It can manage a multiuser environment using the full WebX stack, or simply connect to a single user, <em>standalone</em> WebX Engine.
 - [WebX Demo Client](https://github.com/ILLGrenoble/webx-demo-client) The WebX Demo Client provides a simple web frontend packaged with the WebX Client library. The demo includes some useful debug features that help with the development and testing of WebX.
 - [WebX Demo Deploy](https://github.com/ILLGrenoble/webx-demo-deploy) The WebX Demo Deploy project allows for a one line deployment of the demo application. The server and client are run in a docker compose stack along with an Nginx reverse proxy. This provides a very simple way of connecting to a running WebX Engine for testing purposes.

 The following projects assist in the development of WebX:
 - [WebX Dev Environment](https://github.com/ILLGrenoble/webx-dev-env) This provides a number of Docker environments that contain the necessary libraries and applications to build and run a WebX Engine in a container. Xorg and Xfce4 are both launched when the container is started. Mounting the WebX Engine source inside the container allows it to be built there too.
 - [WebX Dev Workspace](https://github.com/ILLGrenoble/webx-dev-workspace) The WebX Dev Workspace regroups the WebX Engine, WebX Router and WebX Session Manager as git submodules and provides a devcontainer environment with the necessary build and runtime tools to develop and debug all three projects in a single docker environment. Combined with the WebX Demo Deploy project it provides an ideal way of developing and testing the full WebX remote desktop stack.

## Development

The WebX Engine is designed to be built and run in a Linux environment and it connects to a running X11 display. Development can be made directly on a linux machine providing the relevant libraries are installed or (as advised) to develop within a devcontainer.

### Building and running from source on a linux machine

The following assumes a Debian or Ubuntu development environment.

Install the following dependencies:

```bash
apt install file cmake dpkg-dev pkg-config build-essential libzmq3-dev libpng-dev libwebp-dev libjpeg-dev libxdamage-dev libxrender-dev libxext-dev libxfixes-dev libxcomposite-dev libxkbfile-dev libxtst-dev
```

Compiling:

```bash
cmake .
cmake --build . -j 4 --target webx-engine
```

This will compile the WebX Engine using 4 cores and generate the `webx-engine` executable in the `bin` directory.

If you wish to build the tests as well do the following:

```bash
cmake --build . -j 4
```

#### Running WebX in <em>standalone</em> mode from a terminal

The WebX Engine is designed to be run in a multi-user environment where the [WebX Router](https://github.com/ILLGrenoble/webx-router) manages the launching of Xorg and the WebX Engine. However to simplify the development the engine can be run in <em>standalone</em> mode where it will attach to the X11 server defined by the DISPLAY environment variable.

To run WebX Engine in standalone mode run the following command:

```
./bin/webx-engine -s
```

The following options for running the WebX Engine are available:

|Option | Description|
|---|---|
|-i | Adds WebX interactive keyboard connector to assist in debugging the server|
|-k <layout> | Sets the keyboard layout in the X server (by default WebX will automatically use the existing X11 keyboard) |
|-s | Starts WebX in standalone mode |

Running in standalone mode, the WebX Engine will expose three zeromq ports:

| Port | Name           | Description                                                                                         |
|------|----------------|-----------------------------------------------------------------------------------------------------|
| 5555 | Connector port | Used by a client to obtain WebX Engine collector and publisher ports and to verify connection status |
| 5556 | Collector port | The collector port listens to all instructions and requests from the client |
| 5557 | Publisher port | The publisher port sends messages and responses to the client e.g. displayed window metadata, window updates, window images etc. |

> When not running in standalone mode (ie, WebX Engine is launched by the WebX Router) unix sockets (ZeroMQ IPC sockets) are used to communicate between the rotuer and the engine. Similarly, internal messaging in the WebX Engine is handled using ZeroMQ Inproc sockets.

The following environment variables can be set (although most are only useful for the WebX Router integration):

| Environment Variable | Description | Default value |
|------------|----------|----------|
| WEBX_ENGINE_LOG | Logging level | info |
| WEBX_ENGINE_CONNECTOR_PORT | Connector port | 5555 |
| WEBX_ENGINE_CONNECTOR_PORT | Collector port | 5556 |
| WEBX_ENGINE_CONNECTOR_PORT | Publisher port | 5557 |
| WEBX_ENGINE_IPC_MESSAGE_PROXY_PATH | IPC message path | /tmp/webx-router-message-proxy.ipc |
| WEBX_ENGINE_IPC_INSTRUCTION_PROXY_PATH | IPC instruction path | /tmp/webx-router-instruction-proxy.ipc |
| WEBX_ENGINE_IPC_SESSION_CONNECTOR_PATH | IPC session connector path | /tmp/webx-engine-session-connector.ipc |
| WEBX_ENGINE_INPROC_EVENT_BUS_ADDRESS | Internal process event bus path | inproc://webx-engine/event-bus |
| WEBX_ENGINE_SESSION_ID | A unique session Id (managed by the WebX Router) | `<empty>` |

##### Starting Xorg and Xfce4 on a virtual device driver

If you are developing on a linux environment without a physical display driver you can still develop the WebX Engine and run Xorg and a display manager on a virtual device. For simplicity we recommend using Xrdp and Xfce4:

```
apt install xrdp xfce4
```

In a separate terminal you can run the startup script `run_wm.sh` which will launch these processes by default on DISPLAY=:20

```bash
./run_wm.sh
```

To modify the display or the dimensions of the X11 Display you can run the script such as:

```bash
./run_wm.sh -d :10 -w 1920 -h 1080
```

This example runs the window manager on DISPLAY=:10 with height and width equal to 1920 and 1080 respectively.

### Building, running and debugging in a devcontainer

This project includes a `.devcontainer` folder for developing and debugging the WebX Engine in a devcontainer (primarily focussing on using the cpptools extension in VSCode, but potentially accessible too under other IDEs).

Two development environments are available: standalone and multiuser.

#### Standalone devcontainer environment

The <em>standalone</em> devcontainer environment is the simplest way to develop the WebX Engine. 

The devcontainer spins up the `ghcr.io/illgrenoble/webx-dev-env-ubuntu` environment which automatically launches Xorg and the Xfce4 desktop manager on DISPLAY=:20. 

The VSCode Launch Command <em>Debug WebX Standalone (:20)</em> will compile and run the WebX Engine in standalone mode and attach the X11 Server. You can debug the source directly by adding breakpoints to the source files.

#### Multiuser devcontainer environment

If you want to debug the full WebX stack with both the WebX Router and WebX Session Manager running then you should run the <em>multiuser</em> devcontainer environment.

This environment installs extra packages on top of the `ghcr.io/illgrenoble/webx-dev-env-ubuntu` image:
 - Rust development tools are installed
 - The latest stable source of webx-session-manager and webx-router and downloaded and built
 - webx-session-manager and webx-router are launched automatically
 - The container is configured with several standard users (mario, luigi, peach, toad, yoshi and bowser - username and password identical)
 - webx-router is configured to launch the webx-engine built from the local sources

Running in this mode means you have to build the webx-engine manually (as described above).

The webx-engine is launched by the WebX Router when a login is requested. To debug the process you have to attach to it: Run the VSCode launch command <em>Debug Running WebX Process</em> and search for the `webx-engine` process in the list proposed by VSCode.

### Building, running and debugging using the WebX Dev Workspace

Building on the use of devcontainers, the [WebX Dev Workspace](https://github.com/ILLGrenoble/webx-dev-env) combines the development of The WebX Engine, WebX Router and WebX Session Manager in a single workspace and the development and testing of all of these can be combined in a single devcontainer environment.

This is the recommended way of building, running and debuggine the WebX stack as it provides the most flexible approach to development. Please refer to this project's README for more information.

### Running the WebX Demo to test the WebX Engine

In a terminal on the host computer, the simplest way to test the WebX Engine is by running the [WebX Demo Deploy](https://github.com/ILLGrenoble/webx-demo-deploy) project. This runs the WebX Demo in a docker compose stack and can attach to the WebX Engine (or WebX Router in multi-user mode) running in the dev container.

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
