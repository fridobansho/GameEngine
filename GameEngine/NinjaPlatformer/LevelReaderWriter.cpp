#include "LevelReaderWriter.h"

#include <GameEngine/IOManager.h>
#include <GameEngine/ResourceManager.h>
#include <SDL.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <sstream>

// When you want to make a new version, add it here
const unsigned int TEXT_VERSION_0 = 100;
const unsigned int BINARY_VERSION_0 = 200;

// Make sure this is set to the current version
const unsigned int TEXT_VERSION = TEXT_VERSION_0;
const unsigned int BINARY_VERSION = BINARY_VERSION_0;

namespace {
	std::string s_currentLevelPath = "Levels/DumbLevel.txt";

	template <typename T>
	bool writePod(SDL_RWops* rw, const T& value) {
		return SDL_RWwrite(rw, &value, sizeof(T), 1) == 1;
	}

	bool writeBytes(SDL_RWops* rw, const void* data, size_t size) {
		if (size == 0) return true;
		return SDL_RWwrite(rw, data, 1, size) == size;
	}

	template <typename T>
	bool readPod(const std::vector<unsigned char>& buffer, size_t& offset, T& out) {
		if (offset + sizeof(T) > buffer.size()) {
			return false;
		}
		std::memcpy(&out, buffer.data() + offset, sizeof(T));
		offset += sizeof(T);
		return true;
	}

	bool readString(const std::vector<unsigned char>& buffer, size_t& offset, std::string& out) {
		uint32_t len = 0;
		if (!readPod(buffer, offset, len)) {
			return false;
		}
		if (len > buffer.size() - offset) {
			return false;
		}
		out.assign(reinterpret_cast<const char*>(buffer.data() + offset), len);
		offset += len;
		return true;
	}
}

const std::string& LevelReaderWriter::getCurrentLevelPath() {
	return s_currentLevelPath;
}

void LevelReaderWriter::setCurrentLevelPath(const std::string& path) {
	s_currentLevelPath = path;
}

bool LevelReaderWriter::saveAsText(const std::string& filePath, const Player& player, const std::vector<Box>& boxes, const std::vector<Light>& lights) {
	// Keep this updated with newest version
	return saveAsTextV0(filePath, player, boxes, lights);
}

bool LevelReaderWriter::saveAsTextV0(const std::string& filePath, const Player& player, const std::vector<Box>& boxes, const std::vector<Light>& lights) {
	// Open file and error check
	std::ofstream file(filePath);
	if (file.fail()) {
		perror(filePath.c_str());
		return false;
	}

	// Write version
	file << TEXT_VERSION << '\n';
	// Write player
	file << player.getPosition().x << ' ' << player.getPosition().y << ' '
	     << player.getDrawDims().x << ' ' << player.getDrawDims().y << ' '
	     << player.getCollisionDims().x << ' ' << player.getCollisionDims().y << ' '
	     << player.getColour().r << ' ' << player.getColour().g << ' '
	     << player.getColour().b << ' ' << player.getColour().a << '\n';

	// Write number of boxes
	file << boxes.size() << '\n';
	// Write all boxes
	for (auto& b : boxes) {
		file << b.getPosition().x << ' ' << b.getPosition().y << ' '
		     << b.getDimensions().x << ' ' << b.getDimensions().y << ' '
		     << b.getColour().r << ' ' << b.getColour().g << ' '
		     << b.getColour().b << ' ' << b.getColour().a << ' '
		     << b.getUvRect().x << ' ' << b.getUvRect().y << ' '
		     << b.getUvRect().z << ' ' << b.getUvRect().w << ' '
		     << b.getAngle() << ' ' << b.getTexture().filePath << ' '
		     << b.getIsDynamic() << ' ' << b.getFixedRotation() << '\n';
	}

	// Write number of lights
	file << lights.size() << '\n';
	// Write all lights
	for (auto& l : lights) {
		file << l.position.x << ' ' << l.position.y << ' '
		     << l.size << ' ' << l.colour.r << ' ' << l.colour.g << ' '
		     << l.colour.b << ' ' << l.colour.a << '\n';
	}

	// Write goal box indices if any exist (keeps format backwards-compatible with V0 files)
	std::vector<size_t> goalIndices;
	for (size_t i = 0; i < boxes.size(); ++i) {
		if (boxes[i].isGoal()) {
			goalIndices.push_back(i);
		}
	}
	if (!goalIndices.empty()) {
		file << "GOALS " << goalIndices.size() << '\n';
		for (size_t idx : goalIndices) {
			file << idx << '\n';
		}
	}

	return true;
}

bool LevelReaderWriter::saveAsBinary(const std::string& filePath, const Player& player, const std::vector<Box>& boxes, const std::vector<Light>& lights) {
	SDL_RWops* rw = SDL_RWFromFile(filePath.c_str(), "wb");
	if (!rw) {
		return false;
	}

	bool ok = true;
	const uint32_t version = BINARY_VERSION;
	ok = ok && writePod(rw, version);

	// Player
	const glm::vec2 playerPos = player.getPosition();
	const glm::vec2 playerDrawDims = player.getDrawDims();
	const glm::vec2 playerCollDims = player.getCollisionDims();
	const GameEngine::ColourRGBA8 playerColour = player.getColour();
	ok = ok && writePod(rw, playerPos.x) && writePod(rw, playerPos.y);
	ok = ok && writePod(rw, playerDrawDims.x) && writePod(rw, playerDrawDims.y);
	ok = ok && writePod(rw, playerCollDims.x) && writePod(rw, playerCollDims.y);
	ok = ok && writePod(rw, playerColour.r) && writePod(rw, playerColour.g)
	        && writePod(rw, playerColour.b) && writePod(rw, playerColour.a);

	// Boxes
	const uint32_t numBoxes = static_cast<uint32_t>(boxes.size());
	ok = ok && writePod(rw, numBoxes);
	for (const auto& b : boxes) {
		const glm::vec2 pos = b.getPosition();
		const glm::vec2 dims = b.getDimensions();
		const GameEngine::ColourRGBA8 colour = b.getColour();
		const glm::vec4 uvRect = b.getUvRect();
		const float angle = b.getAngle();
		const std::string& texPath = b.getTexture().filePath;
		const uint32_t texLen = static_cast<uint32_t>(texPath.size());
		const uint8_t isDynamic = b.getIsDynamic() ? 1 : 0;
		const uint8_t fixedRotation = b.getFixedRotation() ? 1 : 0;
		const uint8_t isGoal = b.isGoal() ? 1 : 0;

		ok = ok && writePod(rw, pos.x) && writePod(rw, pos.y);
		ok = ok && writePod(rw, dims.x) && writePod(rw, dims.y);
		ok = ok && writePod(rw, colour.r) && writePod(rw, colour.g)
		        && writePod(rw, colour.b) && writePod(rw, colour.a);
		ok = ok && writePod(rw, uvRect.x) && writePod(rw, uvRect.y)
		        && writePod(rw, uvRect.z) && writePod(rw, uvRect.w);
		ok = ok && writePod(rw, angle);
		ok = ok && writePod(rw, texLen) && writeBytes(rw, texPath.data(), texLen);
		ok = ok && writePod(rw, isDynamic) && writePod(rw, fixedRotation) && writePod(rw, isGoal);
		if (!ok) break;
	}

	// Lights
	const uint32_t numLights = static_cast<uint32_t>(lights.size());
	ok = ok && writePod(rw, numLights);
	for (const auto& l : lights) {
		ok = ok && writePod(rw, l.position.x) && writePod(rw, l.position.y);
		ok = ok && writePod(rw, l.size);
		ok = ok && writePod(rw, l.colour.r) && writePod(rw, l.colour.g)
		        && writePod(rw, l.colour.b) && writePod(rw, l.colour.a);
		if (!ok) break;
	}

	if (SDL_RWclose(rw) != 0) {
		ok = false;
	}
	return ok;
}

bool LevelReaderWriter::loadAsText(const std::string& filePath, b2World* world, Player& player, std::vector<Box>& boxes, std::vector<Light>& lights) {
	std::string buffer;
	if (!GameEngine::IOManager::ReadFileToBuffer(filePath, buffer)) {
		return false;
	}

	std::istringstream file(buffer, std::ios::in | std::ios::binary);

	// Get version
	unsigned int version = 0;
	if (!(file >> version)) {
		return false;
	}

	// Parse based on version
	switch (version) {
		case TEXT_VERSION_0:
			return loadAsTextV0(file, world, player, boxes, lights);
		default:
			puts("Unknown version number in level file. File may be corrupted...");
			return false;
	}
}

bool LevelReaderWriter::loadAsBinary(const std::string& filePath, b2World* world, Player& player, std::vector<Box>& boxes, std::vector<Light>& lights) {
	std::vector<unsigned char> buffer;
	if (!GameEngine::IOManager::ReadFileToBuffer(filePath, buffer)) {
		return false;
	}

	size_t offset = 0;
	uint32_t version = 0;
	if (!readPod(buffer, offset, version) || version != BINARY_VERSION_0) {
		puts("Unknown or invalid binary level version.");
		return false;
	}

	// Read player
	{
		glm::vec2 pos;
		glm::vec2 ddims;
		glm::vec2 cdims;
		GameEngine::ColourRGBA8 colour;
		if (!readPod(buffer, offset, pos.x) || !readPod(buffer, offset, pos.y) ||
		    !readPod(buffer, offset, ddims.x) || !readPod(buffer, offset, ddims.y) ||
		    !readPod(buffer, offset, cdims.x) || !readPod(buffer, offset, cdims.y) ||
		    !readPod(buffer, offset, colour.r) || !readPod(buffer, offset, colour.g) ||
		    !readPod(buffer, offset, colour.b) || !readPod(buffer, offset, colour.a)) {
			return false;
		}
		player.init(world, pos, ddims, cdims, colour);
	}

	// Read boxes
	{
		uint32_t numBoxes = 0;
		if (!readPod(buffer, offset, numBoxes)) {
			return false;
		}
		for (uint32_t i = 0; i < numBoxes; ++i) {
			glm::vec2 pos;
			glm::vec2 dims;
			GameEngine::ColourRGBA8 colour;
			glm::vec4 uvRect;
			float angle = 0.0f;
			std::string texturePath;
			uint8_t isDynamic = 0;
			uint8_t fixedRotation = 0;
			uint8_t isGoal = 0;

			if (!readPod(buffer, offset, pos.x) || !readPod(buffer, offset, pos.y) ||
			    !readPod(buffer, offset, dims.x) || !readPod(buffer, offset, dims.y) ||
			    !readPod(buffer, offset, colour.r) || !readPod(buffer, offset, colour.g) ||
			    !readPod(buffer, offset, colour.b) || !readPod(buffer, offset, colour.a) ||
			    !readPod(buffer, offset, uvRect.x) || !readPod(buffer, offset, uvRect.y) ||
			    !readPod(buffer, offset, uvRect.z) || !readPod(buffer, offset, uvRect.w) ||
			    !readPod(buffer, offset, angle) || !readString(buffer, offset, texturePath) ||
			    !readPod(buffer, offset, isDynamic) || !readPod(buffer, offset, fixedRotation) ||
			    !readPod(buffer, offset, isGoal)) {
				return false;
			}

			GameEngine::GLTexture texture = texturePath.empty()
				? GameEngine::GLTexture{}
				: GameEngine::ResourceManager::GetTexture(texturePath);

			boxes.emplace_back();
			boxes.back().init(world, pos, dims, texture, colour,
			                  fixedRotation != 0, isDynamic != 0, angle, uvRect, isGoal != 0);
		}
	}

	// Read lights
	{
		uint32_t numLights = 0;
		if (!readPod(buffer, offset, numLights)) {
			return false;
		}
		for (uint32_t i = 0; i < numLights; ++i) {
			Light light;
			if (!readPod(buffer, offset, light.position.x) || !readPod(buffer, offset, light.position.y) ||
			    !readPod(buffer, offset, light.size) ||
			    !readPod(buffer, offset, light.colour.r) || !readPod(buffer, offset, light.colour.g) ||
			    !readPod(buffer, offset, light.colour.b) || !readPod(buffer, offset, light.colour.a)) {
				return false;
			}
			lights.push_back(light);
		}
	}

	return true;
}

bool LevelReaderWriter::loadAsTextV0(std::istream& file, b2World* world, Player& player, std::vector<Box>& boxes, std::vector<Light>& lights) {
	{ // Read player
		glm::vec2 pos;
		glm::vec2 ddims;
		glm::vec2 cdims;
		GameEngine::ColourRGBA8 colour;
		if (!(file >> pos.x >> pos.y >> ddims.x >> ddims.y >> cdims.x >> cdims.y >> colour.r >> colour.g >> colour.b >> colour.a)) {
			return false;
		}
		player.init(world, pos, ddims, cdims, colour);
	}

	{ // Read boxes
		glm::vec2 pos;
		glm::vec2 dims;
		glm::vec4 uvRect;
		GameEngine::ColourRGBA8 colour;
		bool fixedRotation;
		bool isDynamic;
		float angle;
		GameEngine::GLTexture texture;
		std::string texturePath;
		size_t numBoxes;
		if (!(file >> numBoxes)) {
			return false;
		}
		for (size_t i = 0; i < numBoxes; i++) {
			if (!(file >> pos.x >> pos.y >> dims.x >> dims.y >> colour.r >> colour.g >> colour.b >> colour.a
			           >> uvRect.x >> uvRect.y >> uvRect.z >> uvRect.w
			           >> angle >> texturePath >> isDynamic >> fixedRotation)) {
				return false;
			}

			texture = GameEngine::ResourceManager::GetTexture(texturePath);

			boxes.emplace_back();
			boxes.back().init(world, pos, dims, texture, colour, fixedRotation, isDynamic, angle, uvRect);
		}
	}

	{ // Read lights
		glm::vec2 pos;
		float size;
		GameEngine::ColourRGBA8 colour;
		size_t numLights;
		if (!(file >> numLights)) {
			return false;
		}
		for (size_t i = 0; i < numLights; i++) {
			if (!(file >> pos.x >> pos.y >> size >> colour.r >> colour.g >> colour.b >> colour.a)) {
				return false;
			}
			lights.emplace_back();
			lights.back().colour = colour;
			lights.back().position = pos;
			lights.back().size = size;
		}
	}

	{ // Optional GOALS section
		std::string tag;
		if (file >> tag && tag == "GOALS") {
			size_t numGoals = 0;
			if (file >> numGoals) {
				for (size_t i = 0; i < numGoals; ++i) {
					size_t idx = 0;
					if (file >> idx && idx < boxes.size()) {
						boxes[idx].setIsGoal(true);
					}
				}
			}
		}
	}

	return true;
}
