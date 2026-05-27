#include "WebXRdpClient.h"
#include <spdlog/spdlog.h>

WebXRdpClient::WebXRdpClient() :
    _thread(nullptr),
    _running(false),
    _paintInProgress(false),
    _instance(nullptr),
    _desktopSizeIsDirty(false),
    _desktopWidth(0),
    _desktopHeight(0),
    _framebufferEventHandler([](const std::vector<WebXRectangle> &, const WebXPixelBuffer &) {}),
    _resizeEventHandler([](uint32_t desktopWidth, uint32_t desktopHeight) {}) {
}

WebXRdpClient::~WebXRdpClient() {
    this->terminate();
}

bool WebXRdpClient::connect(const WebXRdpClientSettings & rdpClientSettings) {
    if (this->_instance) {
        return true;
    }

    this->_instance = freerdp_new();

    this->_instance->ContextSize = sizeof(WebXRdpClientContext);
    this->_instance->PreConnect = PreConnect;
    this->_instance->PostConnect = PostConnect;
    this->_instance->PostDisconnect = PostDisconnect;

    freerdp_context_new(this->_instance);

	auto * webxRdpContext = reinterpret_cast<WebXRdpClientContext*>(this->_instance->context);
	webxRdpContext->rdpClient = this;

    rdpSettings * settings = this->_instance->context->settings;

    freerdp_settings_set_string(settings, FreeRDP_ServerHostname, rdpClientSettings.hostname.c_str());
	freerdp_settings_set_uint32(settings, FreeRDP_ServerPort, rdpClientSettings.port);
    freerdp_settings_set_string(settings, FreeRDP_Username, rdpClientSettings.username.c_str());
    freerdp_settings_set_string(settings, FreeRDP_Password, rdpClientSettings.password.c_str());
	freerdp_settings_set_uint32(settings, FreeRDP_DesktopWidth, rdpClientSettings.desktopWidth);
	freerdp_settings_set_uint32(settings, FreeRDP_DesktopHeight, rdpClientSettings.desktopHeight);
	freerdp_settings_set_bool(settings, FreeRDP_NlaSecurity, TRUE);
	freerdp_settings_set_bool(settings, FreeRDP_TlsSecurity, TRUE);
	freerdp_settings_set_bool(settings, FreeRDP_RdpSecurity, FALSE);

    freerdp_settings_set_bool(settings, FreeRDP_SoftwareGdi, TRUE);
    freerdp_settings_set_uint32(settings, FreeRDP_ColorDepth, 24);
	freerdp_settings_set_bool(settings, FreeRDP_RemoteFxCodec, TRUE);
	freerdp_settings_set_bool(settings, FreeRDP_SupportGraphicsPipeline, TRUE);

    freerdp_settings_set_uint32(settings, FreeRDP_ConnectionType, CONNECTION_TYPE_LAN);

    /* Always request frame markers */
    freerdp_settings_set_bool(settings, FreeRDP_FrameMarkerCommandEnabled, TRUE);
    freerdp_settings_set_bool(settings, FreeRDP_SurfaceFrameMarkerEnabled, TRUE);

    freerdp_settings_set_bool(settings, FreeRDP_FastPathInput, TRUE);
    freerdp_settings_set_bool(settings, FreeRDP_FastPathOutput, TRUE);


    spdlog::info("Connecting to RDP server at {:s}:{:d}...", rdpClientSettings.hostname, rdpClientSettings.port);

    if (!freerdp_connect(this->_instance)) {
        spdlog::error("Failed to connect to RDP server");
        return false;
	}

    return true;
}

void WebXRdpClient::start() {
    if (!this->_running) {
        this->_running = true;
        if (this->_thread == NULL) {
            this->_thread = new std::thread(&WebXRdpClient::mainLoop, this);
        }
    }
}

void WebXRdpClient::terminate() {
    this->_running = false;
    if (this->_thread != nullptr) {
        if (this->_paintInProgress) {
            this->_frameUpdateMutex.unlock();
        }

        // Join thread and cleanup
        spdlog::info("Stopping rdp client...");
        this->_thread->join();
        spdlog::info("Stopped rdp client");
        delete this->_thread;
        this->_thread = nullptr;
    }
}

void WebXRdpClient::flushEvents() {
    // Wait for framecomplete mutex
    std::lock_guard<std::mutex> frameLock(this->_frameUpdateMutex);

    // Stop any other events during event flush
    std::lock_guard<std::mutex> eventLock(this->_eventMutex);

    // Handle resize events first
    if (this->_desktopSizeIsDirty) {
        this->_resizeEventHandler(this->_desktopWidth, this->_desktopHeight);
        this->_desktopSizeIsDirty = false;
    }

    // Update framebuffer
    rdpContext * context = this->_instance->context;
    rdpGdi * gdi = context->gdi;

    WebXPixelBuffer pixelBuffer = {(char *)gdi->primary_buffer, gdi->width, gdi->height, (int)gdi->stride, 24};

    this->_framebufferEventHandler(this->_invalidRectangles, pixelBuffer);

    this->_invalidRectangles.clear();

    // Get the current mouse position

}

WebXPixelBuffer WebXRdpClient::getFramebuffer() const {
    rdpContext * context = this->_instance->context;
    rdpGdi * gdi = context->gdi;

    WebXPixelBuffer pixelBuffer = {(char *)gdi->primary_buffer, gdi->width, gdi->height, (int)gdi->stride, 24};

    return pixelBuffer;
}


void WebXRdpClient::mainLoop() {
	while (this->_running) {
		int waitResult = this->waitForHandles(100);
		if (waitResult < 0) {
			break;
		}

        int connectionClosing = 0;
        do {
            if (!freerdp_check_event_handles(this->_instance->context)) {
                waitResult = -1;
                break;
            }

            connectionClosing = freerdp_shall_disconnect_context(this->_instance->context);

        } while (!connectionClosing && (waitResult = waitForHandles(0)) > 0);

		if (connectionClosing) {
            spdlog::error("rdp server requested disconnect");
		}
	}

    freerdp_disconnect(this->_instance);
    freerdp_context_free(this->_instance);
    freerdp_free(this->_instance);
    this->_instance = nullptr;
	spdlog::info("Disconnecting from rdp server");
}

BOOL WebXRdpClient::preConnect(freerdp * instance) {
	rdpSettings* settings = instance->context->settings;



	instance->context->update->BeginPaint = BeginPaint;
	instance->context->update->EndPaint = EndPaint;
	// instance->context->update->SurfaceFrameMarker = SurfaceFrameMarker;
	// instance->context->update->altsec->FrameMarker = FrameMarker;
	instance->context->update->DesktopResize = DesktopResize;
	instance->context->update->SurfaceFrameAcknowledge = SurfaceFrameAcknowledge;

	return TRUE;
}

BOOL WebXRdpClient::postConnect(freerdp * instance) {

    rdpContext * context = instance->context;
    rdpSettings * settings  = context->settings;

    if (!gdi_init(instance, PIXEL_FORMAT_BGR24)) {
        spdlog::error("gdi_init failed");
        return FALSE;
    }

    this->_desktopWidth = freerdp_settings_get_uint32(settings, FreeRDP_DesktopWidth);
    this->_desktopHeight = freerdp_settings_get_uint32(settings, FreeRDP_DesktopHeight);

	spdlog::info("... connected to RDP server, desktop size {:d}x{:d}", this->_desktopWidth, this->_desktopHeight);

    return TRUE;
}

void WebXRdpClient::postDisconnect(freerdp * instance) {
    gdi_free(instance);
    this->_running = false;
}

BOOL WebXRdpClient::beginPaint(rdpContext * context) {
    std::lock_guard<std::mutex> lock(this->_eventMutex);
    if (!this->_paintInProgress) {
        this->_frameUpdateMutex.lock();
        this->_paintInProgress = true;
        spdlog::info("Begin Paint mutex locked");
    }

    return TRUE;
}

BOOL WebXRdpClient::endPaint(rdpContext * context) {
    std::lock_guard<std::mutex> lock(this->_eventMutex);
	rdpGdi * gdi = context->gdi;

	HGDI_RGN inv = gdi->primary->hdc->hwnd->invalid;
	if (inv->null) {
		return TRUE;
	}

	INT32 x = gdi->primary->hdc->hwnd->invalid->x;
    INT32 y = gdi->primary->hdc->hwnd->invalid->y;
    UINT32 width = gdi->primary->hdc->hwnd->invalid->w;
    UINT32 height = gdi->primary->hdc->hwnd->invalid->h;

	gdi->primary->hdc->hwnd->invalid->null = TRUE;

    // Store dirty rectangle
    WebXRectangle invalidRectangle(x, y, width, height);
    this->_invalidRectangles.push_back(invalidRectangle);

    return TRUE;
}

// BOOL WebXRdpClient::frameMarker(rdpContext * context, const FRAME_MARKER_ORDER * marker) {
//    std::lock_guard<std::mutex> lock(this->_eventMutex);

//     spdlog::info("Frame Marker");
//     return TRUE;
// }

// BOOL WebXRdpClient::surfaceFrameMarker(rdpContext * context, const SURFACE_FRAME_MARKER * marker) {
//    std::lock_guard<std::mutex> lock(this->_eventMutex);

//     spdlog::info("Surface Frame Marker");

//     switch (marker->frameAction) {
//         case SURFACECMD_FRAMEACTION_BEGIN:
//             spdlog::info("Begin frame");
//             break;

//         case SURFACECMD_FRAMEACTION_END:
//             spdlog::info("End frame");
//             break;
//     }
//     return TRUE;
// }

BOOL WebXRdpClient::surfaceFrameAcknowledge(rdpContext * context, const uint32_t frameId) {
    std::lock_guard<std::mutex> lock(this->_eventMutex);
    spdlog::info("Surface Frame Acknowledge");
    if (this->_paintInProgress) {
        this->_frameUpdateMutex.unlock();
        this->_paintInProgress = false;
        spdlog::info("Unlocked");
    }

    return TRUE;
}


BOOL WebXRdpClient::desktopResize(rdpContext * context) {
    std::lock_guard<std::mutex> lock(this->_eventMutex);

    rdpSettings * settings = context->settings;

    const uint32_t width = freerdp_settings_get_uint32(settings, FreeRDP_DesktopWidth);
    const uint32_t height = freerdp_settings_get_uint32(settings, FreeRDP_DesktopHeight);

    spdlog::info("Desktop resize {:d}x{:d}", width, height);

    this->_desktopWidth = width;
    this->_desktopHeight = height;

    // Remove all invalid rectangles
    this->_invalidRectangles.clear();

    this->_desktopSizeIsDirty = true;

    return TRUE;
}

int WebXRdpClient::waitForHandles(int timeoutMs) {
   	rdpContext * context = this->_instance->context;

    HANDLE handles[MAXIMUM_WAIT_OBJECTS];
    DWORD nHandles = 0;

    nHandles = freerdp_get_event_handles(context, handles, ARRAYSIZE(handles));

	DWORD status = WaitForMultipleObjects(nHandles, handles, FALSE, timeoutMs);

	if (status == WAIT_TIMEOUT) {
		return 0;
	
	} else if (status == WAIT_FAILED) {
        spdlog::error("WaitForMultipleObjects failed with error {} ", GetLastError());
        return -1;
    }

	// Handles contain events
    return 1; 
}

