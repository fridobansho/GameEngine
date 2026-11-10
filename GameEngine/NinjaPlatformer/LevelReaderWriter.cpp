#include "LevelReaderWriter.h"

#include <GameEngine/ResourceManager.h>
#include <fstream>
#include <cstddef>

// When you want to make a new version, add it here
const unsigned int TEXT_VERSION_0 = 100;

// Make sure this is set to the current version
const unsigned int TEXT_VERSION = TEXT_VERSION_0;

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

	return true;
}

bool LevelReaderWriter::saveAsBinary(const std::string& filePath, const Player& player, const std::vector<Box>& boxes, const std::vector<Light>& lights) {
	puts("saveBinary not implemented.");
	return false;
}

bool LevelReaderWriter::loadAsText(const std::string& filePath, b2World* world, Player& player, std::vector<Box>& boxes, std::vector<Light>& lights) {
	// Open file and error check
	std::ifstream file(filePath);
	if (file.fail()) {
		perror(filePath.c_str());
		return false;
	}

	// Get version
	unsigned int version;
	file >> version;

	// Parse based on version
	switch (version) {
		case TEXT_VERSION_0:
			loadAsTextV0(file, world, player, boxes, lights);
			break;
		default:
			puts("Unknown version number in level file. File may be corrupted...");
			return false;
	}

	return true;
}

bool LevelReaderWriter::loadAsBinary(const std::string& filePath, b2World* world, Player& player, std::vector<Box>& boxes, std::vector<Light>& lights) {
	puts("loadBinary not implemented.");
	return false;
}

bool LevelReaderWriter::loadAsTextV0(std::ifstream& file, b2World* world, Player& player, std::vector<Box>& boxes, std::vector<Light>& lights) {
	{ // Read player
		glm::vec2 pos;
		glm::vec2 ddims;
		glm::vec2 cdims;
		GameEngine::ColourRGBA8 colour;
		file >> pos.x >> pos.y >> ddims.x >> ddims.y >> cdims.x >> cdims.y >> colour.r >> colour.g >> colour.b >> colour.a;
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
		file >> numBoxes;
		for (size_t i = 0; i < numBoxes; i++) {
			file >> pos.x >> pos.y >> dims.x >> dims.y >> colour.r >> colour.g >> colour.b >> colour.a
			     >> uvRect.x >> uvRect.y >> uvRect.z >> uvRect.w
			     >> angle >> texturePath >> isDynamic >> fixedRotation;

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
		file >> numLights;
		for (size_t i = 0; i < numLights; i++) {
			file >> pos.x >> pos.y >> size >> colour.r >> colour.g >> colour.b >> colour.a;
			lights.emplace_back();
			lights.back().colour = colour;
			lights.back().position = pos;
			lights.back().size = size;
		}
	}

	return true;
}
