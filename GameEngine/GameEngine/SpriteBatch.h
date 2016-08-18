#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <vector>
#include "Vertex.h"

namespace GameEngine
{
	enum class GlyphSortType
	{
		NONE,
		FRONT_TO_BACK,
		BACK_TO_FRONT,
		TEXTURE
	};

	class Glyph
	{
	public:
		Glyph() {}
		Glyph(const glm::vec4 & destRect, const glm::vec4 & uvRect, GLuint Texture, float Depth, const ColourRGBA8 & colour) :
			texture(Texture),
			depth(Depth)
		{
			topLeft.setColour(colour);
			topLeft.setPosition(destRect.x, destRect.y + destRect.w);
			topLeft.setUV(uvRect.x, uvRect.y + uvRect.w);

			bottomLeft.setColour(colour);
			bottomLeft.setPosition(destRect.x, destRect.y);
			bottomLeft.setUV(uvRect.x, uvRect.y);

			bottomRight.setColour(colour);
			bottomRight.setPosition(destRect.x + destRect.z, destRect.y);
			bottomRight.setUV(uvRect.x + uvRect.z, uvRect.y);

			topRight.setColour(colour);
			topRight.setPosition(destRect.x + destRect.z, destRect.y + destRect.w);
			topRight.setUV(uvRect.x + uvRect.z, uvRect.y + uvRect.w);
		}
		GLuint texture;
		float depth;

		Vertex topLeft;
		Vertex bottomLeft;
		Vertex topRight;
		Vertex bottomRight;
	};

	class RenderBatch
	{
	public:
		RenderBatch(GLuint offset, GLuint numVertices, GLuint texture) :_offset(offset), _numVertices(numVertices), _texture(texture) {}
		GLuint _offset;
		GLuint _numVertices;
		GLuint _texture;
	private:

	};

	class SpriteBatch
	{
	public:
		SpriteBatch();
		~SpriteBatch();

		void init();

		void begin(GlyphSortType sortType = GlyphSortType::TEXTURE);
		void end();

		void draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const ColourRGBA8& colour);

		void renderBatch();
	private:
		void createRenderBatches();
		void createVertexArray();
		void sortGlyphs();

		static bool compareFrontToBack(Glyph* a, Glyph* b);
		static bool compareBackToFront(Glyph* a, Glyph* b);
		static bool compareTexture(Glyph* a, Glyph* b);

		GLuint _vbo;
		GLuint _vao;

		GlyphSortType _sortType;

		std::vector<Glyph*> _glyphPointers;
		std::vector<Glyph> _glyphs;
		std::vector<RenderBatch> _renderBatches;
	};
}

