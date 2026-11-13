#include "TestHarness.h"
#include <GL/glew.h>
#include <GameEngine/GLSLProgram.h>
#include <GameEngine/IOManager.h>
#include <GameEngine/SpriteBatch.h>

static std::string resolveShaderPath(const std::string& name) {
    std::string p1 = "Shaders/" + name;
    std::string p2 = "GameEngine/NinjaPlatformer/Shaders/" + name;
    FILE* f = fopen(p1.c_str(), "r");
    if (f) { fclose(f); return p1; }
    return p2;
}

TEST_CASE(Shader_TextureShadingCompileAndLink) {
    if (!TestHarness::hasGLContext()) return;

    GameEngine::GLSLProgram program;
    std::string vert = resolveShaderPath("textureShading.vert");
    std::string frag = resolveShaderPath("textureShading.frag");

    program.CompileShaders(vert, frag);
    program.AddAttribute("vertexPosition");
    program.AddAttribute("vertexColor");
    program.AddAttribute("vertexUV");
    program.LinkShaders();

    GLint pLoc = program.GetUniformLocation("P");
    CHECK(pLoc != -1);
    GLint sLoc = program.GetUniformLocation("mySampler");
    CHECK(sLoc != -1);

    program.Use();
    program.UnUse();
    program.Dispose();
}

TEST_CASE(Shader_LightShadingCompileAndLink) {
    if (!TestHarness::hasGLContext()) return;

    GameEngine::GLSLProgram program;
    std::string vert = resolveShaderPath("lightShading.vert");
    std::string frag = resolveShaderPath("lightShading.frag");

    program.CompileShaders(vert, frag);
    program.AddAttribute("vertexPosition");
    program.AddAttribute("vertexColor");
    program.AddAttribute("vertexUV");
    program.LinkShaders();

    GLint pLoc = program.GetUniformLocation("P");
    CHECK(pLoc != -1);

    program.Use();
    program.UnUse();
    program.Dispose();
}

TEST_CASE(Shader_ColorCompileAndLink) {
    if (!TestHarness::hasGLContext()) return;

    GameEngine::GLSLProgram program;
    std::string vert = resolveShaderPath("color.vert");
    std::string frag = resolveShaderPath("color.frag");

    program.CompileShaders(vert, frag);
    program.AddAttribute("vertexPosition");
    program.AddAttribute("vertexColor");
    program.AddAttribute("vertexUV");
    program.LinkShaders();

    GLint pLoc = program.GetUniformLocation("P");
    CHECK(pLoc != -1);

    program.Use();
    program.UnUse();
    program.Dispose();
}

TEST_CASE(Shader_GLES300Preprocess) {
    const std::string desktopVert =
        "#version 130\n"
        "in vec2 vertexPosition;\n"
        "void main() {\n"
        "    gl_Position = vec4(vertexPosition, 0.0, 1.0);\n"
        "}\n";

    std::string glesVert = GameEngine::GLSLProgram::PreprocessShaderSource(desktopVert, true);
    CHECK(glesVert.find("#version 300 es") != std::string::npos);
    CHECK(glesVert.find("precision mediump float;") != std::string::npos);
    CHECK(glesVert.find("#version 130") == std::string::npos);

    std::string keptDesktop = GameEngine::GLSLProgram::PreprocessShaderSource(desktopVert, false);
    CHECK(keptDesktop.find("#version 130") != std::string::npos);
    CHECK(keptDesktop.find("#version 300 es") == std::string::npos);

    // Also test preprocessing a real shipped shader file
    std::string fragSource;
    std::string fragPath = resolveShaderPath("textureShading.frag");
    if (GameEngine::IOManager::ReadFileToBuffer(fragPath, fragSource)) {
        std::string glesFrag = GameEngine::GLSLProgram::PreprocessShaderSource(fragSource, true);
        CHECK(glesFrag.find("#version 300 es") != std::string::npos);
        CHECK(glesFrag.find("precision mediump float;") != std::string::npos);
        CHECK(glesFrag.find("#version 130") == std::string::npos);

        std::string desktopFrag = GameEngine::GLSLProgram::PreprocessShaderSource(fragSource, false);
        CHECK(desktopFrag.find("#version 130") != std::string::npos);
    }
}

TEST_CASE(SpriteBatch_RepeatedInitAndDispose) {
    if (!TestHarness::hasGLContext()) return;

    while (glGetError() != GL_NO_ERROR) {}

    GameEngine::SpriteBatch batch;
    batch.init();
    batch.begin();
    batch.draw(glm::vec4(0.0f, 0.0f, 32.0f, 32.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
               0, 0.0f, GameEngine::ColourRGBA8(255, 255, 255, 255));
    batch.end();
    batch.renderBatch();

    // Repeated init() without explicit dispose() must clean up old GL buffers
    batch.init();
    batch.dispose();
    // Repeated dispose() must be safe
    batch.dispose();

    // Re-init after dispose() and render again
    batch.init();
    batch.begin();
    batch.draw(glm::vec4(10.0f, 10.0f, 16.0f, 16.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
               0, 0.0f, GameEngine::ColourRGBA8(0, 255, 0, 255));
    batch.end();
    batch.renderBatch();
    batch.dispose();

    CHECK_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));
}
