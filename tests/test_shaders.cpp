#include "TestHarness.h"
#include <GL/glew.h>
#include <GameEngine/GLSLProgram.h>

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
