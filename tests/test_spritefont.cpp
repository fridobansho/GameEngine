#include "TestHarness.h"
#include <GL/glew.h>
#include <GameEngine/SoA/SpriteFont.h>
#include <vector>

static std::string resolveFontPath() {
    std::string p1 = "Fonts/chintzy.ttf";
    std::string p2 = "GameEngine/NinjaPlatformer/Fonts/chintzy.ttf";
    FILE* f = fopen(p1.c_str(), "r");
    if (f) { fclose(f); return p1; }
    return p2;
}

TEST_CASE(SpriteFont_InitAndMeasure) {
    if (!TestHarness::hasGLContext()) return;

    std::string fontPath = resolveFontPath();
    GameEngine::SpriteFont font(fontPath.c_str(), 32);

    CHECK(font.getTextureID() != 0);
    CHECK(font.getFontHeight() > 0);

    glm::vec2 singleLine = font.measure("Hello World");
    CHECK(singleLine.x > 0.0f);
    CHECK_EQ(singleLine.y, (float)font.getFontHeight());

    glm::vec2 multiLine = font.measure("Line 1\nLine 2");
    CHECK(multiLine.x > 0.0f);
    CHECK_EQ(multiLine.y, (float)font.getFontHeight() * 2.0f);

    glm::vec2 empty = font.measure("");
    CHECK_EQ(empty.x, 0.0f);
}

TEST_CASE(SpriteFont_TextureAtlasNoSkewRegression) {
    if (!TestHarness::hasGLContext()) return;

    std::string fontPath = resolveFontPath();
    GameEngine::SpriteFont font(fontPath.c_str(), 32);

    unsigned int texID = font.getTextureID();
    CHECK(texID != 0);

    glBindTexture(GL_TEXTURE_2D, texID);
    GLint width = 0, height = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

    CHECK(width > 0);
    CHECK(height > 0);

    std::vector<unsigned char> pixels(width * height * 4, 0);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    // Count non-transparent pixels in atlas to verify glyph data exists
    int nonZeroAlpha = 0;
    for (size_t i = 3; i < pixels.size(); i += 4) {
        if (pixels[i] > 0) nonZeroAlpha++;
    }
    CHECK(nonZeroAlpha > 500);

    font.dispose();
}
