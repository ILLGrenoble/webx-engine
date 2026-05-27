#ifndef WEBX_RDP_CLIENT_H
#define WEBX_RDP_CLIENT_H

#include <models/WebXRectangle.h>
#include <image/WebXPixelBuffer.h>
#include <freerdp/freerdp.h>
#include <freerdp/gdi/gdi.h>
#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <functional>

class WebXRdpClientSettings {
public:
    WebXRdpClientSettings(const std::string & hostname, const uint32_t port, const std::string & username, const std::string & password, const uint32_t desktopWidth, const uint32_t desktopHeight, const std::string & keyboardLayout) :
        hostname(hostname),
        port(port),
        username(username),
        password(password),
        desktopWidth(desktopWidth),
        desktopHeight(desktopHeight),
        keyboardLayout(keyboardLayout) {
    }
    const std::string hostname;
    const uint32_t port;
    const std::string username;
    const std::string password;
    const uint32_t desktopWidth;
    const uint32_t desktopHeight;
    const std::string keyboardLayout;
};

class WebXRdpClient;

struct WebXRdpClientContext {
    rdpClientContext base;
    WebXRdpClient * rdpClient;
};

class WebXRdpClient {
public:
    WebXRdpClient();
    ~WebXRdpClient();

    bool connect(const WebXRdpClientSettings & rdpClientSettings);

    void start();
    void terminate();

    void flushEvents();

    const WebXSize getScreenSize() const {
        return WebXSize(this->_desktopWidth, this->_desktopHeight);
    }

    void setFramebufferEventHandler(std::function<void(const std::vector<WebXRectangle> & invalidRectangles, const WebXPixelBuffer & framebuffer)> handler) {
        this->_framebufferEventHandler = handler;
    }

    void setResizeEventHandler(std::function<void(uint32_t desktopWidth, uint32_t desktopHeight)> handler) {
        this->_resizeEventHandler = handler;
    }

    WebXPixelBuffer getFramebuffer() const;

private:
    void mainLoop();

    BOOL preConnect(freerdp * instance);
    BOOL postConnect(freerdp * instance);
    void postDisconnect(freerdp * instance);
    BOOL beginPaint(rdpContext * context);
    BOOL endPaint(rdpContext * context);
    // BOOL frameMarker(rdpContext * context, const FRAME_MARKER_ORDER * marker);
    // BOOL surfaceFrameMarker(rdpContext * context, const SURFACE_FRAME_MARKER * marker);
    BOOL surfaceFrameAcknowledge(rdpContext * context, const uint32_t frameId);
    BOOL desktopResize(rdpContext * context);
    
    static BOOL PreConnect(freerdp * instance) {
        WebXRdpClientContext * clientContext = reinterpret_cast<WebXRdpClientContext *>(instance->context);
        return clientContext->rdpClient->preConnect(instance);
    }

    static BOOL PostConnect(freerdp * instance) {
        WebXRdpClientContext * clientContext = reinterpret_cast<WebXRdpClientContext *>(instance->context);
        return clientContext->rdpClient->postConnect(instance);
    }

    static void PostDisconnect(freerdp * instance) {
        WebXRdpClientContext * clientContext = reinterpret_cast<WebXRdpClientContext *>(instance->context);
        clientContext->rdpClient->postDisconnect(instance);
    }

    static BOOL BeginPaint(rdpContext * context) {
        WebXRdpClientContext * clientContext = reinterpret_cast<WebXRdpClientContext *>(context);
        return clientContext->rdpClient->beginPaint(context);
    }

    static BOOL EndPaint(rdpContext * context) {
        WebXRdpClientContext * clientContext = reinterpret_cast<WebXRdpClientContext *>(context);
        return clientContext->rdpClient->endPaint(context);
    }

    // static BOOL FrameMarker(rdpContext * context, const FRAME_MARKER_ORDER * marker) {
    //     WebXRdpClientContext * clientContext = reinterpret_cast<WebXRdpClientContext *>(context);
    //     return clientContext->rdpClient->FrameMarker(context, marker);
    // }

    // static BOOL SurfaceFrameMarker(rdpContext * context, const SURFACE_FRAME_MARKER * marker) {
    //     WebXRdpClientContext * clientContext = reinterpret_cast<WebXRdpClientContext *>(context);
    //     return clientContext->rdpClient->surfaceFrameMarker(context, marker);
    // }

    static BOOL SurfaceFrameAcknowledge(rdpContext * context, const uint32_t frameId) {
        WebXRdpClientContext * clientContext = reinterpret_cast<WebXRdpClientContext *>(context);
        return clientContext->rdpClient->surfaceFrameAcknowledge(context, frameId);
    }

    static BOOL DesktopResize(rdpContext * context) {
        WebXRdpClientContext * clientContext = reinterpret_cast<WebXRdpClientContext *>(context);
        return clientContext->rdpClient->desktopResize(context);
    }


    int waitForHandles(int timeoutMs);

private:
    std::thread * _thread;
    bool _running;
    std::mutex _eventMutex;
    std::mutex _frameUpdateMutex;
    bool _paintInProgress;
    
    freerdp * _instance;
    bool _desktopSizeIsDirty;
    uint32_t _desktopWidth;
    uint32_t _desktopHeight;

    std::vector<WebXRectangle> _invalidRectangles;

    std::function<void(const std::vector<WebXRectangle> & invalidRectangles, const WebXPixelBuffer & framebuffer)> _framebufferEventHandler;
    std::function<void(uint32_t desktopWidth, uint32_t desktopHeight)> _resizeEventHandler;

};

#endif /* WEBX_RDP_CLIENT_H */