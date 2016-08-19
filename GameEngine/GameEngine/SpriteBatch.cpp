#include "SpriteBatch.h"

#include <algorithm>

using namespace GameEngine;

SpriteBatch::SpriteBatch() : m_vbo(0), m_vao(0)
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
	m_sortType = sortType;
	m_renderBatches.clear();
	m_glyphs.clear();
}

void GameEngine::SpriteBatch::end()
{
	m_glyphPointers.resize(m_glyphs.size());
	for (int i = 0; i < m_glyphs.size(); i++)
	{
		m_glyphPointers[i] = &m_glyphs[i];
	}
	sortGlyphs();
	createRenderBatches();
}

void GameEngine::SpriteBatch::draw(const glm::vec4 & destRect, const glm::vec4 & uvRect, GLuint texture, float depth, const ColourRGBA8 & colour)
{
	m_glyphs.emplace_back(destRect, uvRect, texture, depth, colour);
}

void GameEngine::SpriteBatch::renderBatch()
{
	glBindVertexArray(m_vao);
	for (int i = 0; i < m_renderBatches.size(); i++)
	{
		glBindTexture(GL_TEXTURE_2D, m_renderBatches[i].m_texture);
		glDrawArrays(GL_TRIANGLES, m_renderBatches[i].m_offset, m_renderBatches[i].m_numVertices);
	}
	glBindVertexArray(0);
}

void GameEngine::SpriteBatch::createRenderBatches()
{
	if (m_glyphPointers.empty()) return;

	std::vector<Vertex> vertices;
	vertices.resize(m_glyphPointers.size() * 6);

	int offset = 0;
	int cv = 0;
	m_renderBatches.emplace_back(offset, 6, m_glyphPointers[0]->texture);
	vertices[cv++] = m_glyphPointers[0]->topLeft;
	vertices[cv++] = m_glyphPointers[0]->bottomLeft;
	vertices[cv++] = m_glyphPointers[0]->bottomRight;
	vertices[cv++] = m_glyphPointers[0]->bottomRight;
	vertices[cv++] = m_glyphPointers[0]->topRight;
	vertices[cv++] = m_glyphPointers[0]->topLeft;
	offset += 6;

	for (int cg = 1; cg < m_glyphPointers.size(); cg++)
	{
		if (m_glyphPointers[cg]->texture != m_glyphPointers[cg - 1]->texture)
		{
			m_renderBatches.emplace_back(offset, 6, m_glyphPointers[cg]->texture);
		}
		else
		{
			m_renderBatches.back().m_numVertices += 6;
		}
		vertices[cv++] = m_glyphPointers[cg]->topLeft;
		vertices[cv++] = m_glyphPointers[cg]->bottomLeft;
		vertices[cv++] = m_glyphPointers[cg]->bottomRight;
		vertices[cv++] = m_glyphPointers[cg]->bottomRight;
		vertices[cv++] = m_glyphPointers[cg]->topRight;
		vertices[cv++] = m_glyphPointers[cg]->topLeft;
		offset += 6;
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GameEngine::SpriteBatch::createVertexArray()
{
	if (m_vao == 0)
	{
		glGenVertexArrays(1, &m_vao);
	}

	glBindVertexArray(m_vao);

	if (m_vbo == 0)
	{
		glGenBuffers(1, &m_vbo);
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

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
	switch (m_sortType)
	{
	case GlyphSortType::BACK_TO_FRONT:
		std::stable_sort(m_glyphPointers.begin(), m_glyphPointers.end(), compareBackToFront);
		break;
	case GlyphSortType::FRONT_TO_BACK:
		std::stable_sort(m_glyphPointers.begin(), m_glyphPointers.end(), compareFrontToBack);
		break;
	case GlyphSortType::TEXTURE:
		std::stable_sort(m_glyphPointers.begin(), m_glyphPointers.end(), compareTexture);
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
