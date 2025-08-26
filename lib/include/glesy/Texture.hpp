#pragma once

#include <glm/vec4.hpp>

#include <vector>
#include <filesystem>

struct Texture {
    size_t width{};
    size_t height{};
    std::vector<glm::u8vec4> data;

    static Texture
    loadPng(std::filesystem::path filePath);
};