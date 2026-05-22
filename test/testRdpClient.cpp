#include <freerdp/freerdp.h>
#include <freerdp/client.h>
#include <freerdp/gdi/gdi.h>
#include <atomic>
#include <csignal>
#include <iostream>
#include <image/WebXJPGImageConverter.h>
#include <image/WebXImage.h>
#include <image/WebXPixelBuffer.h>

static std::atomic_bool shutdown(false);

static void signal_handler(int signum) {
    if (signum == SIGINT) {
        printf("\nCaught Ctrl-C, shutting down...\n");
        shutdown = true;
    }
}

struct WebXRdpContext {
    rdpClientContext base;

    std::atomic<bool> running;
	uint32_t work_done;
};

BOOL end_paint(rdpContext* context) {
	auto * webxRdpContext = reinterpret_cast<WebXRdpContext*>(context);

	rdpGdi * gdi = context->gdi;

	HGDI_RGN inv = gdi->primary->hdc->hwnd->invalid;
	if (inv->null) {
		return TRUE;         // nothing dirty
	}

	INT32 x = gdi->primary->hdc->hwnd->invalid->x;
    INT32 y = gdi->primary->hdc->hwnd->invalid->y;
    UINT32 w = gdi->primary->hdc->hwnd->invalid->w;
    UINT32 h = gdi->primary->hdc->hwnd->invalid->h;

	// printf("End paint at (%d, %d) %dx%d\n", x, y, w, h);

	gdi->primary->hdc->hwnd->invalid->null = TRUE;

	webxRdpContext->work_done++;

	return TRUE;
}

BOOL pre_connect(freerdp * instance) {
	printf("In pre_connect\n");

	rdpSettings* settings = instance->context->settings;

    if (!freerdp_settings_set_uint32(settings, FreeRDP_ColorDepth, 24)) {
        return FALSE;
	}

    // Enable GDI (software rendering) – required for primary_buffer access
    if (!freerdp_settings_set_bool(settings, FreeRDP_SoftwareGdi, TRUE)) {
        return FALSE;
	}

	instance->context->update->EndPaint = end_paint;

	return TRUE;
}

BOOL post_connect(freerdp * instance) {
	printf("In post_connect\n");
	
    if (!gdi_init(instance, PIXEL_FORMAT_BGR24)) {
        printf("gdi_init failed\n");
        return FALSE;
    }

    return TRUE;
}

void post_disconnect(freerdp*  instance) {
    gdi_free(instance);
	auto * webxRdpContext = reinterpret_cast<WebXRdpContext*>(instance->context);
    webxRdpContext->running = false;
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
    auto * webxRdpContext = reinterpret_cast<WebXRdpContext*>(context);
    webxRdpContext->running = true;
    webxRdpContext->work_done= 0;

	instance->PreConnect = pre_connect;
    instance->PostConnect = post_connect;
    instance->PostDisconnect = post_disconnect;

    return TRUE;
}

void output_image(freerdp * instance) {
	rdpContext * context = instance->context;
	rdpGdi * gdi = context->gdi;

	WebXPixelBuffer pixelBuffer = {(char *)gdi->primary_buffer, gdi->width, gdi->height, (int)gdi->stride, 24};

	WebXJPGImageConverter converter;
	WebXImage * image = converter.convert(&pixelBuffer, WebXQuality::MaxQuality());
	image->save("test/output/rdp");
}

int main(int argc, char** argv) {
    std::signal(SIGINT, signal_handler);

    freerdp * instance = freerdp_new();

    instance->ContextSize = sizeof(WebXRdpContext);
    instance->PreConnect = pre_connect;
    instance->PostConnect = post_connect;
    instance->PostDisconnect = post_disconnect;

    freerdp_context_new(instance);

	auto * webxRdpContext = reinterpret_cast<WebXRdpContext*>(instance->context);
	webxRdpContext->running = true;
    webxRdpContext->work_done= 0;

    rdpSettings * settings = instance->context->settings;

    freerdp_settings_set_string(settings, FreeRDP_ServerHostname, "localhost");
	freerdp_settings_set_uint32(settings, FreeRDP_ServerPort, 3389);
    freerdp_settings_set_string(settings, FreeRDP_Username, "mario");
    freerdp_settings_set_string(settings, FreeRDP_Password, "mario");
	freerdp_settings_set_bool(settings, FreeRDP_NlaSecurity, TRUE);
	freerdp_settings_set_bool(settings, FreeRDP_TlsSecurity, TRUE);
	freerdp_settings_set_bool(settings, FreeRDP_RdpSecurity, FALSE);
	freerdp_settings_set_uint32(settings, FreeRDP_DesktopWidth, 1920);
	freerdp_settings_set_uint32(settings, FreeRDP_DesktopHeight,1080);

	printf("Connecting...\n");

    if (!freerdp_connect(instance)) {
        return 1;
	}

	printf("... connected\n");

	while (webxRdpContext->running && !shutdown) {
		int previous_work_done = webxRdpContext->work_done;
		webxRdpContext->work_done = 0;

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

		if (webxRdpContext->work_done == 0 && previous_work_done > 0) {
			printf("Creating image\n");
			output_image(instance);
		}
	}


	printf("disconnect and clean up...\n");
    freerdp_disconnect(instance);
    freerdp_context_free(instance);
    freerdp_free(instance);
	printf("... done\n");

    return 0;
}
