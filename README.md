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
 - [WebX Router](https://github.com/ILLGrenoble/webx-router) The WebX Router manages multiple WebX sessions on single host, routing traffic between running WebX Engines and the WebX Relay. It authenticates session creation requests and spawns Xorg, window manager and WebX Engine processes.
 - [WebX Relay](https://github.com/ILLGrenoble/webx-relay) The WebX Relay provides a Java library that can be integrated into the backend of a web application, providing bridge functionality between WebX host machines and client browsers. TCP sockets (using the ZMQ protocol) connect the relay to host machines and websockets connect the client browsers to the relay. The relay transports data between a specific client and corresponding WebX Router/Engine.
 - [WebX Client](https://github.com/ILLGrenoble/webx-client) The WebX Client is a javascript package (available via NPM) that provides rendering capabilities for the remote desktop and transfers user input events to the WebX Engine via the relay.

To showcase the WebX technology, a demo is available. The demo also allows for simplified testing of the WebX remote desktop stack. The projects used for the demo are:
 - [WebX Demo Server](https://github.com/ILLGrenoble/webx-demo-server) The WebX Demo Server is a simple Java backend integrating the WebX Relay. It can manage a multiuser environment using the full WebX stack, or simply connect to a single user, <em>standalone</em> WebX Engine.
 - [WebX Demo Client](https://github.com/ILLGrenoble/webx-demo-client) The WebX Demo Client provides a simple web frontend packaged with the WebX Client library. The demo includes some useful debug features that help with the development and testing of WebX.
 - [WebX Demo Deploy](https://github.com/ILLGrenoble/webx-demo-deploy) The WebX Demo Deploy project allows for a one line deployment of the demo application. The server and client are run in a docker compose stack along with an Nginx reverse proxy. This provides a very simple way of connecting to a running WebX Engine for testing purposes.

 The following projects assist in the development of WebX:
 - [WebX Dev Environment](https://github.com/ILLGrenoble/webx-dev-env) This provides a number of Docker environments that contain the necessary libraries and applications to build and run a WebX Engine in a container. Xorg and Xfce4 are both launched when the container is started. Mounting the WebX Engine source inside the container allows it to be built there too.
 - [WebX Dev Workspace](https://github.com/ILLGrenoble/webx-dev-workspace) The WebX Dev Workspace regroups the WebX Engine and WebX Router as git submodules and provides a devcontainer environment with the necessary build and runtime tools to develop and debug all three projects in a single docker environment. Combined with the WebX Demo Deploy project it provides an ideal way of developing and testing the full WebX remote desktop stack.

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

If you want to debug the full WebX stack with the WebX Router running then you should run the <em>multiuser</em> devcontainer environment.

This environment installs extra packages on top of the `ghcr.io/illgrenoble/webx-dev-env-ubuntu` image:
 - Rust development tools are installed
 - The latest stable source of webx-router and downloaded and built
 - webx-router is launched automatically
 - The container is configured with several standard users (mario, luigi, peach, toad, yoshi and bowser - username and password identical)
 - webx-router is configured to launch the webx-engine built from the local sources

Running in this mode means you have to build the webx-engine manually (as described above).

The webx-engine is launched by the WebX Router when a login is requested. To debug the process you have to attach to it: Run the VSCode launch command <em>Debug Running WebX Process</em> and search for the `webx-engine` process in the list proposed by VSCode.

### Building, running and debugging using the WebX Dev Workspace

Building on the use of devcontainers, the [WebX Dev Workspace](https://github.com/ILLGrenoble/webx-dev-env) combines the development of The WebX Engine and WebX Router in a single workspace and the development and testing of these can be combined in a single devcontainer environment.

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

This will send the request to the WebX Router: the user is authenticated and then Xorg and Xfce4 processes are run for the user; WebX Router then launches the locally-built webx-engine. You can debug the webx-engine process as described above by attaching to the process in the VSCode launch command.

## Design

The WebX Engine is designed as an interface to enable Remote Desktop connections to an X11 display. The server starts by connecting as a client to a running X11 server. Clients can connect to the engine through ZeroMQ sockets.

As an X11 display client it can:
 - obtain display characteristics and window metadata, storing this information in an internal state structure
 - listen to X11 server events, update the state (window layout for example) and propagate visual data (window contents) to WebX clients
 - send keyboard and mouse actions

Clients connect to the WebX Engine on different ZeroMQ sockets.

A <em>controller</em> thread is used to manage th connection between the WebX Engine clients and the X11 display: at a specific frequency it will forward X11 event data to the clients (display and window parameters, image data, etc) and forward instructions from the clients. The frequency of this loop is determined by the requested quality of the Remote Desktop display: lower quality implies lower frequency.

The following sections provide more details on the architecture of the engine.

### Threading

The main thread of the WebX Engine runs a control loop in the WebXController. At a given frequency the control loop forwards mouse and keyboard instructions from clients to the X11 display, handles qny queued X11 events and publishes event data to the transport layer (connected ZMQ clients). The frequency at which the loop runs at is determined by the quality of the Remote Desktop (set by the client): lower frequency for lower quality.

Three other threads are used: one for each ZMQ socket. Events received from the sockets or sent to the sockets are managed asynchronously to the main controller loop allowing the WebXController to maintain a regular frequency.

### X11 Client

Each WebX Engine contains a single WebXManager which is used to propagate requests to the X11 display (via the WebXDisplay) and handle X11 events (through the WebXEventListener).

#### X11 Display and Windows

When the WebX Engine starts it will automatically attempt to connect the X11 display set by the DISPLAY environment variable. Secure X11 Displays require also the XAUTHORITY environment variable to be correctly set. The WebXDisplay is used to connect to the X11 display and maintain a equivalent state structure of the X11 windows. X11 returns a tree structure of windows on some of which are useful for WebX: some are not visible and only a few are <em>parent</em> windows that can be moved or resized.

Window image data is received through the WebXDisplay for a given X11 window Id. Window image data can either be for a full window or a sub-rectangle for partial updates.

#### Event listener

X11 events are registered with the WebXEventListener. These events are queued until released manually during the WebXController loop.

The events cover:
 - creation and deletion of X11 windows
 - changed to windows properties (position and size)
 - changes to window visibility
 - changes to window content (XDamage event)
 - changes to cursor type (XFixes event)

Events are handled by the WebXManager which will either notify directly the WebXController (to propagate event data to clients) or update the WebXDisplay. 

#### Keyboard layout

The WebXDisplay is used also to forward keyboard and mouse events to the X11 display. The requested <em>keysym</em> value is converted into a keyboard code. Due to the architecture of X11 not all keysyms can be converted for a given keyboard. For example characters with accents are generally not available on a US keyboard and so will not result in a successful keypress in the X11 server. 

### Image data

Raw image data for an X11 window can be received from the X11 server for either a full window or a sub-rectangle of the window.

The raw image needs to be converted and compressed for it to be usable over a network. After many test JPEG has been found to be the most optimal conversion format, especially through the use of libjpeg-turbo library to take advantage of accelerated encoding. Typically less than 10 milliseconds is required to encode a window image. PNG conversions were several hundred milliseconds.    

The generation of image data and transport is the most demanding part of WebX. Optimisations have been attempted whenever possible to reduce the amount of time needed to generate image data. Other optimisations may yet be possible.

The X11 XDamage event is used to determine which parts of a window has been updated. The <em>damaged</em> rectangles of a window or stored until the WebXController loop handles them and requests image data for each rectangle. WebX will automatically determine when two rectangles are touching or overlapping and generate a single encompassing rectangle to avoid multiple requests of identical image data. 

#### Transparency

Full managed windows generally contain transparency (for example the corners of the windows can be curved, or some applications - eg terminator - allow for semi-transparent contents). However, the JPEG format does not contain a transparency value. To account for transparent windows using JPEG images WebX sends the transparency as a separate grey-scale image. Transparency is detected from window image data and the case being, WebX will separate the alpha channel into a new image data and perform the JPEG conversion for both the color image and the alpha image.

The WebX Client uses these images for color map and alpha map texture mapping: both images are combined in WebGL to render and window on the screen with the required transparency.

#### Image quality

The quality of the JPEG image is determined by the quality of Remote Desktop specified by the client. The best image quality is 90% and the quality goes down to 30% at the worst.

### Transport layer

The transport layer allows for clients to connect to the WebX Engine to send instructions to the X11 display, request display information (window layout for example) and receive notifications of display updates.

#### Sockets

Connections are made using sockets. ZeroMQ is used to provide a layer of abstraction above the sockets and enable different communication patterns.

Three sockets are used to communicate with WebX Engine:
- Connector: Obtain connection parameters (configured sockets) and determine liveliness
- CommandCollector: Receive instructions from the client which can (if required) provoke response messages
- MessagePublisher: Asynchronous messages about the screen, windows, images, mouse and cursor

Running in <em>standalone</em> mode, these sockets are TCP sockets defaulting to port numbers 5555, 5556 and 5557 respectively. When used by the WebX Router (which is running on the same host machine), these sockets are unix file sockets specified by a file path (IPC sockets).

The Connector socket, running in response-request (`ZMQ_REP`) mode, is used to obtain the ports of CommandCollector and the MessagePublisher. It also is used to ensure that the WebX Engine is still running using ping-pong events.

The CommandCollector, running with the subscriber pattern (`ZMQ_SUB`), listens to instructions and requests published by the client. Instructions can be to update the mouse position, send keyboard command, modify remote desktop quality, etc. Request require a response and can be for example to get the windows layout, window image, etc. The commands are forwarded to WebXController and queued until the control loop runs.

The MessagePublisher, running with the publisher pattern (`ZMQ_PUB`) forwards messages to clients. Messages can either be responses to client requests or asynchronous events such as display updates. Messages are passed to MessagePublisher from the WebXController during it's control loop and sent asynchronously to avoid any latency withing the control loop.

#### Binary serialization

All instructions received and messages sent through the sockets have a binary format. The binary data contains a header including a session Id - used by WebX Router to distinguish messages from multiple engines, a message/instruction type, a unique message Id. Message headers sent from the server also include the length of data to be sent (instructions received from the client always have pre-determined lengths). After the header there is the specific instruction/message content data.  

Using the WebXBinaryBuffer, the content data can be composed of floating point or integer data of variable sizes. Values are easily written to and read from the buffer. Byte alignment is managed automatically.

Image (JPEG) data is copied directly into the binary buffer making for an efficient transfer of data. 

The Javascript WebX Client library decodes messages from the server into json objects or JPEG image data depending on the type specified in the header.

### Controller

The WebXController is the central part of the application, running on the main thread. It performs a control loop at fixed intervals (determined by the quality required of the Remote Desktop). 

At the best quality, the controller loop runs 30 times a second with the aim of updating clients at 30 frames per second (FPS) with an image quality of 90%.

At the worst quality, the controller runs twice a second with an image quality of 30%.

The control loop has different stages:
 - Sleep: sleep for specified time to obtain predefined frequency (related to Remote Desktop quality) 
 - Client Instructions: handle instructions from the client (mouse events and keyboard events)
 - Flush X11: flush X11 events with the WebXDisplay (this will cause all pending X11 events to be queued for use by the WebXEventListener)
 - Window Layout: If the window layout has changed, send up-to-date window information to the publisher
 - Window Images: send all window image updates to the publisher
 - Mouse: send mouse position and cursor type to the publisher 

messages sent to the publisher are sent to clients in a separate thread to assist the controller in maintaining a regular frequency.

#### Window Image Updates 

Window image changes are determined by the XDamage X11 extension. This allows us to better know the region of windows that has changed. A window may have several updates during the control loop sleep stage. The changed regions are stored by each window  (and as mentioned previously, regions that are touching or overlapping are regrouped into a single region).

During the <em>Window Images</em> stage, the controller retrieves all the <em>damaged</em> windows (image data required updating). For each window it will determine if the damaged area covers at least 90% of the window: if so it will update the full window, if not it will send individual sub-window updates

For full window updates it then:
 - Verify that the window is visible
 - Obtain a WebXImage object containing both color data and alpha (transparency) data
 - Verify that the color data and alpha (transparency) data have really changed - the XDamage X11 event is sometimes sent for a full window even if the data hasn't changed. To avoid sending image data unnecessarily a rapid test of pixel data is performed.
 - Create a window image message to sent to the client
 - Send the window image message to the transport layer publisher if the color data has changed (including alpha data only if it has changed)

For sub-window updates, the controller:
 - Create WebXImage for each sub-rectangle of the window that has changed
 - Concatenate the images into a single sub-window image message
 - Send the sub-window image message to the transport layer publisher to send to clients

The window image updates accounts for the most time spent withing the control loop and so several optimisations have been made. Potentially other optimisations can be made still.

