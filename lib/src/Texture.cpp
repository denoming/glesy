#include "glesy/Texture.hpp"

#include <png.h>

#include <fstream>
#include <functional>
#include <iostream>

namespace fs = std::filesystem;

static constexpr glm::uint8 PngMaxPixelValue{0xFF};
static constexpr auto PngBitDepth8 = 8;
static constexpr auto PngBitDepth16 = 16;

static void
errorHandler(png_structp /*png*/, const png_const_charp msg)
{
    std::cerr << "Error: " << msg;
}

Texture
Texture::loadPng(std::filesystem::path filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (not file.is_open()) {
        throw std::runtime_error{"Unable to open file"};
    }

    png_byte header[8];
    file.read(reinterpret_cast<char*>(header), 8);
    if (png_sig_cmp(header, 0, 8)) {
        throw std::runtime_error{"Invalid PNG file"};
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, errorHandler, nullptr);
    if (not png) {
        throw std::runtime_error{"Unable to create PNG read struct"};
    }
    png_infop info = png_create_info_struct(png);
    if (not info) {
        png_destroy_read_struct(&png, nullptr, nullptr);
        throw std::runtime_error{"Unable to create PNG info struct"};
    }

    if (_setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, nullptr);
        throw std::runtime_error{"Unable to create PNG info struct"};
    }

    png_set_read_fn(png, &file, [](png_structp png, const png_bytep data, const png_size_t length) {
        auto* is = static_cast<std::istream*>(png_get_io_ptr(png));
        is->read(reinterpret_cast<char*>(data), static_cast<std::streamsize>(length));
        if (is->gcount() != length) {
            png_error(png, "Read error");
        }
    });

    png_set_sig_bytes(png, 8);
    png_read_info(png, info);

    const auto width = png_get_image_width(png, info);
    const auto height = png_get_image_height(png, info);
    const auto colorType = png_get_color_type(png, info);
    const auto bitDepth = png_get_bit_depth(png, info);

    if (bitDepth == PngBitDepth16) {
        png_set_strip_16(png);
    }
    if (colorType == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png);
    }
    if (colorType == PNG_COLOR_TYPE_GRAY and bitDepth < PngBitDepth8) {
        png_set_expand_gray_1_2_4_to_8(png);
    }
    if (png_get_valid(png, info, PNG_INFO_tRNS) != 0) {
        png_set_tRNS_to_alpha(png);
    }
    if (colorType == PNG_COLOR_TYPE_RGB or colorType == PNG_COLOR_TYPE_GRAY
        or colorType == PNG_COLOR_TYPE_PALETTE) {
        png_set_filler(png, PngMaxPixelValue, PNG_FILLER_AFTER);
    }
    if (colorType == PNG_COLOR_TYPE_GRAY or colorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(png);
    }

    png_read_update_info(png, info);

    // Allocate memory
    auto pointers = std::vector<png_bytep>(height, nullptr);
    for (uint32_t y = 0; y < height; y++) {
        pointers[y] = new png_byte[png_get_rowbytes(png, info)];
    }

    png_read_image(png, pointers.data());

    Texture texture;
    texture.width = width;
    texture.height = height;
    texture.data.reserve(height * width);
    for (size_t y = 0U; y < height; ++y) {
        const auto* raw = pointers[y];
        for (size_t x = 0U; x < width; ++x) {
            const auto* pixel = &raw[4U * x + 0U];
            texture.data.emplace_back(pixel[0U], pixel[1U], pixel[2U], pixel[3U]);
        }
    }

    // Cleanup
    for (uint32_t y = 0; y < height; y++) {
        free(pointers[y]);
    }
    png_destroy_read_struct(&png, &info, nullptr);
    file.close();

    return texture;
}