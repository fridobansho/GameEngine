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
		BASCK_TO_FRONT,
		TEXTURE
	};

	struct Glyph
	{
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

		void draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const Colour& colour);

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

		std::vector<Glyph*> _glyphs;
		std::vector<RenderBatch> _renderBatches;
	};
}

