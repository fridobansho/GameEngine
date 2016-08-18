#include "SpriteBatch.h"

#include <algorithm>

using namespace GameEngine;

SpriteBatch::SpriteBatch() : _vbo(0), _vao(0)
{
}

SpriteBatch::~SpriteBatch()
{
}

void GameEngine::SpriteBatch::init()
{
	createVertexArray();
}

void GameEngine::SpriteBatch::begin(GlyphSortType sortType)
{
	_sortType = sortType;
	_renderBatches.clear();
	_glyphs.clear();
}

void GameEngine::SpriteBatch::end()
{
	_glyphPointers.resize(_glyphs.size());
	for (int i = 0; i < _glyphs.size(); i++)
	{
		_glyphPointers[i] = &_glyphs[i];
	}
	sortGlyphs();
	createRenderBatches();
}

void GameEngine::SpriteBatch::draw(const glm::vec4 & destRect, const glm::vec4 & uvRect, GLuint texture, float depth, const ColourRGBA8 & colour)
{
	_glyphs.emplace_back(destRect, uvRect, texture, depth, colour);
}

void GameEngine::SpriteBatch::renderBatch()
{
	glBindVertexArray(_vao);
	for (int i = 0; i < _renderBatches.size(); i++)
	{
		glBindTexture(GL_TEXTURE_2D, _renderBatches[i]._texture);
		glDrawArrays(GL_TRIANGLES, _renderBatches[i]._offset, _renderBatches[i]._numVertices);
	}
	glBindVertexArray(0);
}

void GameEngine::SpriteBatch::createRenderBatches()
{
	if (_glyphPointers.empty()) return;

	std::vector<Vertex> vertices;
	vertices.resize(_glyphPointers.size() * 6);

	int offset = 0;
	int cv = 0;
	_renderBatches.emplace_back(offset, 6, _glyphPointers[0]->texture);
	vertices[cv++] = _glyphPointers[0]->topLeft;
	vertices[cv++] = _glyphPointers[0]->bottomLeft;
	vertices[cv++] = _glyphPointers[0]->bottomRight;
	vertices[cv++] = _glyphPointers[0]->bottomRight;
	vertices[cv++] = _glyphPointers[0]->topRight;
	vertices[cv++] = _glyphPointers[0]->topLeft;
	offset += 6;

	for (int cg = 1; cg < _glyphPointers.size(); cg++)
	{
		if (_glyphPointers[cg]->texture != _glyphPointers[cg - 1]->texture)
		{
			_renderBatches.emplace_back(offset, 6, _glyphPointers[cg]->texture);
		}
		else
		{
			_renderBatches.back()._numVertices += 6;
		}
		vertices[cv++] = _glyphPointers[cg]->topLeft;
		vertices[cv++] = _glyphPointers[cg]->bottomLeft;
		vertices[cv++] = _glyphPointers[cg]->bottomRight;
		vertices[cv++] = _glyphPointers[cg]->bottomRight;
		vertices[cv++] = _glyphPointers[cg]->topRight;
		vertices[cv++] = _glyphPointers[cg]->topLeft;
		offset += 6;
	}

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GameEngine::SpriteBatch::createVertexArray()
{
	if (_vao == 0)
	{
		glGenVertexArrays(1, &_vao);
	}

	glBindVertexArray(_vao);

	if (_vbo == 0)
	{
		glGenBuffers(1, &_vbo);
	}
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, colour));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	glBindVertexArray(0);
}

void GameEngine::SpriteBatch::sortGlyphs()
{
	switch (_sortType)
	{
	case GlyphSortType::BACK_TO_FRONT:
		std::stable_sort(_glyphPointers.begin(), _glyphPointers.end(), compareBackToFront);
		break;
	case GlyphSortType::FRONT_TO_BACK:
		std::stable_sort(_glyphPointers.begin(), _glyphPointers.end(), compareFrontToBack);
		break;
	case GlyphSortType::TEXTURE:
		std::stable_sort(_glyphPointers.begin(), _glyphPointers.end(), compareTexture);
		break;
	}
}

bool GameEngine::SpriteBatch::compareFrontToBack(Glyph * a, Glyph * b)
{
	return (a->depth < b->depth);
}

bool GameEngine::SpriteBatch::compareBackToFront(Glyph * a, Glyph * b)
{
	return (a->depth > b->depth);
}

bool GameEngine::SpriteBatch::compareTexture(Glyph * a, Glyph * b)
{
	return (a->texture < b->texture);
}
