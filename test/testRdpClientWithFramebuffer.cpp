#include <freerdp/freerdp.h>
#include <freerdp/client.h>
#include <freerdp/gdi/gdi.h>
#include <atomic>
#include <csignal>
#include <iostream>
#include <image/WebXJPGImageConverter.h>
#include <image/WebXImage.h>
#include <image/WebXPixelBuffer.h>

struct DirtyRect {
    uint32_t x, y, width, height;
};

class Framebuffer {
public:
    void resize(uint32_t w, uint32_t h, uint32_t bytesPerPixel = 4) {
        std::lock_guard<std::mutex> lock(_mutex);
        _width  = w;
        _height = h;
        _bytesPerPixel = bytesPerPixel;
		_data.reset(new uint8_t[h * w * bytesPerPixel]());
        _dirtyRects.clear();
        std::cout << "[Framebuffer] resized to " << w << "x" << h << "\n";
    }

    void updateRegion(uint32_t dstX, uint32_t dstY, uint32_t rectW, uint32_t rectH, const uint8_t* src, uint32_t srcStride) {
        std::lock_guard<std::mutex> lock(_mutex);
        if (!_data) {
			std::cerr << "[Framebuffer] Pixel data is not available" << std::endl;
			return;
		}
		if (dstX + rectW > this->_width || dstY + rectH > this->_height) {
			std::cerr << "[Framebuffer] Update region is outside framebuffer boundary" << std::endl;
			return;
		}

        const uint32_t dstStride = _width * _bytesPerPixel;
        for (uint32_t row = 0; row < rectH; ++row) {
            const uint32_t fbRow = dstY + row;
            if (fbRow >= _height) {
				break;
			}

            uint8_t * dst = _data.get() + fbRow * dstStride + dstX * _bytesPerPixel;
            const uint8_t * s = src + row * srcStride;

            const uint32_t copyBytes = std::min(rectW * _bytesPerPixel, (_width - dstX) * _bytesPerPixel);
            std::memcpy(dst, s, copyBytes);
        }

        _dirtyRects.push_back({dstX, dstY, rectW, rectH});
    }

    std::vector<DirtyRect> takeDirtyRects() {
        std::lock_guard<std::mutex> lock(_mutex);
		std::vector<DirtyRect> result;
	    result.swap(_dirtyRects);

        return result;
    }

	void output_image(freerdp * instance) {

    	WebXPixelBuffer pixelBuffer = {(char *)this->_data.get(), this->_width, this->_height, this->_width * this->_bytesPerPixel, 24};

		WebXJPGImageConverter converter;
		WebXImage * image = converter.convert(&pixelBuffer, WebXQuality::MaxQuality());
		image->save("test/output/rdp");
	}

	uint32_t width() const {
		return _width;
	}
    
	uint32_t height() const {
		return _height;
	}
    
	uint32_t bpp() const {
		return _bytesPerPixel;
	}

private:
    std::mutex _mutex;
	std::unique_ptr<uint8_t[]> _data;
    std::vector<DirtyRect> _dirtyRects;
    uint32_t _width = 0;
    uint32_t _height = 0;
    uint32_t _bytesPerPixel = 4;
};

struct MyClientContext {
    rdpClientContext base;

    Framebuffer * framebuffer;
    std::atomic<bool> running;
	uint32_t work_done;
};

static MyClientContext* toMyContext(freerdp* instance) {
    return reinterpret_cast<MyClientContext*>(instance->context);
}

static std::atomic_bool shutdown(false);

static void signal_handler(int signum) {
    if (signum == SIGINT) {
        printf("\nCaught Ctrl-C, shutting down...\n");
        shutdown = true;
    }
}

static BOOL desktop_resize(rdpContext * context) {
    auto * myContext = reinterpret_cast<MyClientContext*>(context);

    rdpSettings * settings = context->settings;

	rdpGdi * gdi = context->gdi;
	const uint32_t bytesPerPixel = FreeRDPGetBytesPerPixel(gdi->dstFormat);

    const uint32_t width = freerdp_settings_get_uint32(settings, FreeRDP_DesktopWidth);
    const uint32_t height = freerdp_settings_get_uint32(settings, FreeRDP_DesktopHeight);
    myContext->framebuffer->resize(width, height, bytesPerPixel);

    // Re-initialise GDI with the new dimensions
    gdi_resize(context->gdi, width, height);
    return TRUE;
}

// BOOL begin_paint(rdpContext* context) {
// 	rdpGdi * gdi = context->gdi;

// 	return TRUE;
// }

BOOL end_paint(rdpContext* context) {
	auto* myContext = reinterpret_cast<MyClientContext*>(context);

	rdpGdi * gdi = context->gdi;
    if (!gdi || !gdi->primary_buffer) {
		return TRUE;
	}

	const uint32_t bytesPerPixel = FreeRDPGetBytesPerPixel(gdi->dstFormat);
	const uint32_t stride = gdi->stride;

	HGDI_RGN invalid = gdi->primary->hdc->hwnd->invalid;
	if (invalid->null) {
		return TRUE;
	}

	INT32 x = invalid->x;
    INT32 y = invalid->y;
    UINT32 width = invalid->w;
    UINT32 height = invalid->h;

	if (width > 0 && height > 0) {
        const uint8_t* src = gdi->primary_buffer + y * stride + x * bytesPerPixel;
        myContext->framebuffer->updateRegion(x, y, width, height, src, stride);
    }
	printf("End paint at (%d, %d) %dx%d\n", x, y, width, height);

	invalid->null = TRUE;

	myContext->work_done++;

	return TRUE;
}

// BOOL bitmap_update(rdpContext * context, const BITMAP_UPDATE * bitmap) {
//     for (UINT32 i = 0; i < bitmap->number; i++) {
//         const BITMAP_DATA* b = &bitmap->rectangles[i];

//         // b->destLeft, destTop, width, height
//         // b->data contains compressed/decoded pixel data
// 		printf("Update (%d, %d) %d x %d with %d bytes, compressed %d\n", b->destLeft, b->destTop, b->width, b->height, b->bitmapLength, b->compressed);
//     }
// 	return TRUE;
// }

BOOL pre_connect(freerdp * instance) {
	printf("In pre_connect\n");

	rdpSettings* settings = instance->context->settings;

	// Pixel format: 32-bit BGRX
    if (!freerdp_settings_set_uint32(settings, FreeRDP_ColorDepth, 32)) {
        return FALSE;
	}

    // Enable GDI (software rendering) – required for primary_buffer access
    if (!freerdp_settings_set_bool(settings, FreeRDP_SoftwareGdi, TRUE)) {
        return FALSE;
	}

	// instance->context->update->BitmapUpdate = bitmap_update;
	// instance->context->update->BeginPaint = begin_paint;
	instance->context->update->EndPaint = end_paint;
	instance->context->update->DesktopResize = desktop_resize;

	return TRUE;
}

BOOL post_connect(freerdp * instance) {
	printf("In post_connect\n");

	rdpContext * context = instance->context;
    rdpSettings * settings  = context->settings;
	
    if (!gdi_init(instance, PIXEL_FORMAT_BGRA32)) {
        printf("gdi_init failed\n");
        return FALSE;
    }

	const uint32_t width = freerdp_settings_get_uint32(settings, FreeRDP_DesktopWidth);
    const uint32_t height = freerdp_settings_get_uint32(settings, FreeRDP_DesktopHeight);
    toMyContext(instance)->framebuffer->resize(width, height);

    return TRUE;
}

void post_disconnect(freerdp*  instance) {
    gdi_free(instance);
    toMyContext(instance)->running = false;
    printf("[RDP] Disconnected\n");
}

static int wait_for_handles(freerdp * instance, int timeout_ms) {

	rdpContext * context = instance->context;

    HANDLE handles[MAXIMUM_WAIT_OBJECTS];
    DWORD nHandles = 0;

    nHandles = freerdp_get_event_handles(context, handles, ARRAYSIZE(handles));

	DWORD status = WaitForMultipleObjects(nHandles, handles, FALSE, timeout_ms);

	if (status == WAIT_TIMEOUT) {
		return 0;
	
	} else if (status == WAIT_FAILED) {
        std::cerr << "[ERROR] WaitForMultipleObjects failed: " << GetLastError() << "\n";
        return -1;
    }

	// Handles contain events
    return 1;
}

BOOL client_new(freerdp * instance, rdpContext * context) {
    auto * myContext = reinterpret_cast<MyClientContext*>(context);
    myContext->running = true;
    myContext->framebuffer = nullptr;

	instance->PreConnect = pre_connect;
    instance->PostConnect = post_connect;
    instance->PostDisconnect = post_disconnect;

    return TRUE;
}

void client_free(freerdp * /*instance*/, rdpContext * /*context*/) {

}

int main(int argc, char** argv) {
    // Install Ctrl-C handler
    std::signal(SIGINT, signal_handler);


    RDP_CLIENT_ENTRY_POINTS entry{};
    entry.Version         = RDP_CLIENT_INTERFACE_VERSION;
    entry.Size            = sizeof(RDP_CLIENT_ENTRY_POINTS);
    entry.ContextSize     = sizeof(MyClientContext);
    entry.ClientNew       = client_new;
    entry.ClientFree      = client_free;

    rdpContext* context = freerdp_client_context_new(&entry);
    if (!context) {
        std::cerr << "[ERROR] freerdp_client_context_new failed\n";
        return 1;
    }

    freerdp * instance = context->instance;
    auto * myContext = reinterpret_cast<MyClientContext*>(context);

    Framebuffer framebuffer;
    myContext->framebuffer = &framebuffer;

    rdpSettings * settings = context->settings;

    freerdp_settings_set_string(settings, FreeRDP_ServerHostname, "localhost");
	freerdp_settings_set_uint32(settings, FreeRDP_ServerPort, 3389);
    freerdp_settings_set_string(settings, FreeRDP_Username, "mario");
    freerdp_settings_set_string(settings, FreeRDP_Password, "mario");
	freerdp_settings_set_bool(settings, FreeRDP_NlaSecurity, TRUE);
	freerdp_settings_set_bool(settings, FreeRDP_TlsSecurity, TRUE);
	freerdp_settings_set_bool(settings, FreeRDP_RdpSecurity, FALSE);
	freerdp_settings_set_uint32(settings, FreeRDP_DesktopWidth, 1920);
	freerdp_settings_set_uint32(settings, FreeRDP_DesktopHeight,1080);
	// freerdp_settings_set_bool(settings, FreeRDP_BitmapCompressionDisabled, TRUE);
	// freerdp_settings_set_bool(settings, FreeRDP_NSCodec, FALSE);
	// freerdp_settings_set_bool(settings, FreeRDP_RemoteFxCodec, FALSE);
	// freerdp_settings_set_bool(settings, FreeRDP_SupportGraphicsPipeline, FALSE);
	// freerdp_settings_set_bool(settings, FreeRDP_BitmapCacheEnabled, FALSE);
    // freerdp_settings_set_bool(settings, FreeRDP_GfxAVC444,               FALSE);
    // freerdp_settings_set_bool(settings, FreeRDP_GfxAVC444v2,             FALSE);
    // freerdp_settings_set_bool(settings, FreeRDP_GfxH264,                 FALSE);
    // freerdp_settings_set_bool(settings, FreeRDP_RemoteFxCodec,           FALSE);

	printf("Connecting...\n");

    if (!freerdp_connect(instance)) {
		std::cerr << "[ERROR] freerdp_connect failed – error 0x" << std::hex << freerdp_get_last_error(context) << "\n";
        freerdp_client_context_free(context);
        return 1;
	}

	printf("... connected\n");

	while (myContext->running && !shutdown) {
		int previous_work_done = myContext->work_done;
		myContext->work_done = 0;

		int wait_result = wait_for_handles(instance, 100);
		if (wait_result < 0) {
			break;
		}

        int connection_closing = 0;
        do {
            if (!freerdp_check_event_handles(instance->context)) {
                wait_result = -1;
                break;
            }

            connection_closing = freerdp_shall_disconnect_context(instance->context);

        } while (!connection_closing && (wait_result = wait_for_handles(instance, 0)) > 0);

		if (connection_closing) {
			std::cout << "[RDP] Server requested disconnect\n";
		}

		if (myContext->work_done == 0 && previous_work_done > 0) {
			printf("Creating image\n");
			myContext->framebuffer->output_image(instance);
		}
	}


	printf("disconnect and clean up...\n");
    freerdp_disconnect(instance);
	// gdi_free(instance);
    freerdp_context_free(instance);
    freerdp_free(instance);
	printf("... done\n");

    return 0;
}
