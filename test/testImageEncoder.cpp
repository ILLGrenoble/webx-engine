#include <image/WebXImage.h>
#include <image/WebXPNGImageConverter.h>
#include <image/WebXJPGImageConverter.h>
#include <image/WebXWebPImageConverter.h>

#include <png.h>
#include <stdlib.h>
#include <cstring>
#include <memory>
#include <X11/Xlib.h>

int width, height;
png_byte color_type;
png_byte bit_depth;
png_bytep *row_pointers;

struct TestResult {
    double cummulativeTimeUs;
    size_t fileSize;
};

void read_png_file(const char * filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("Failed to open file\n");
        abort();
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        printf("Failed to create PNG structure\n");
        abort();
    }
    png_infop info = png_create_info_struct(png);
    if (!info) {
        printf("Failed to create PNG info structure\n");
        abort();
    }

    if (setjmp(png_jmpbuf(png))) {
        printf("Failed to jmpbuf PNG\n");
        abort();
    }

    png_init_io(png, fp);

    png_read_info(png, info);

    width = png_get_image_width(png, info);
    height = png_get_image_height(png, info);
    color_type = png_get_color_type(png, info);
    bit_depth = png_get_bit_depth(png, info);

    // Read any color_type into 8bit depth, RGBA format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt

    if (bit_depth == 16)
        png_set_strip_16(png);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);

    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    // These color_type don't have an alpha channel then fill it with 0xff.
    if (color_type == PNG_COLOR_TYPE_RGB ||
         color_type == PNG_COLOR_TYPE_GRAY ||
         color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
         color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for(int y = 0; y < height; y++) {
        row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
    }

    png_read_image(png, row_pointers);

    fclose(fp);
}

TestResult test_convert(XImage & xImage, WebXImageConverter & converter, int nIter) {
    double cummulativeTimeUs = 0;
    size_t fileSize = 0;
    TestResult result;

    for (int i = 0; i < nIter; i++) {
        WebXImage * image = converter.convert(&xImage, 0.95);
        cummulativeTimeUs += image->getEncodingTimeUs();

        if (i == 0) {
            fileSize = image->getRawDataSize();
            std::string outputFilename = "test/output/screenshotOut";
            if (image->save(outputFilename)) {
                printf("Output image saved to %s\n", outputFilename.c_str());
            } else {
                printf("Failed to save image\n");
            }
        }
    }

    result.cummulativeTimeUs = cummulativeTimeUs;
    result.fileSize = fileSize;
    return result;
}

int main() {
    const char * filename = "test/resources/screenshot.png";
    read_png_file(filename);

    printf("Converting png to rawdata %d x %d...\n", width, height);
    unsigned char * imageData = (unsigned char *)malloc(width * height * 4);
    unsigned int bytes_per_line = width * 4;
    unsigned int offset = 0;
    for (int y = 0; y < height; y++) {
        png_bytep row = row_pointers[y];
        for (int x = 0; x < width; x++) {
            png_bytep pixel = &(row[x * 4]);
            imageData[offset + 0] = pixel[ + 2];
            imageData[offset + 1] = pixel[ + 1];
            imageData[offset + 2] = pixel[ + 0];
            imageData[offset + 3] = pixel[ + 3];
            offset += 4;
        }
    }
    printf("... done\n");


    XImage xImage;
    xImage.width = width;
    xImage.height = height;
    xImage.data = (char *)imageData;
    xImage.bytes_per_line = bytes_per_line;
    xImage.depth = 24;

    WebXJPGImageConverter jpgConverter;
    WebXPNGImageConverter pngConverter;
    WebXWebPImageConverter webPConverter;
    int nIter = 10;
    TestResult result = test_convert(xImage, jpgConverter, nIter);
    printf("JPG  test completed: %d iterations in %fms\n%fms / iteration for %luKB\n", nIter, result.cummulativeTimeUs / 1000, (result.cummulativeTimeUs / nIter) / 1000, result.fileSize / 1024);
    result = test_convert(xImage, pngConverter, nIter);
    printf("PNG test completed: %d iterations in %fms\n%fms / iteration for %luKB\n", nIter, result.cummulativeTimeUs / 1000, (result.cummulativeTimeUs / nIter) / 1000, result.fileSize / 1024);
    result = test_convert(xImage, webPConverter, nIter);
    printf("WebP test completed: %d iterations in %fms\n%fms / iteration for %luKB\n", nIter, result.cummulativeTimeUs / 1000, (result.cummulativeTimeUs / nIter) / 1000, result.fileSize / 1024);


}