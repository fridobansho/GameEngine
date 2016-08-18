#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace GameEngine
{
	class Camera2D
	{
	public:
		Camera2D();
		~Camera2D();

		void Init(int, int);
		void Update();

		bool isBoxInView(const glm::vec2& position, const glm::vec2& dimensions);

		glm::vec2 convertScreenToWorld(glm::vec2 screenCoords);

		void SetPosition(const glm::vec2& newPosition) { _position = newPosition; _needsMatrixUpdate = true;	}
		void SetScale(float newScale) { _scale = newScale; _needsMatrixUpdate = true; }

		glm::vec2 & GetPosition() { return _position; }
		float GetScale() { return _scale; }
		glm::mat4 getCameraMatrix() { return _cameraMatrix; }

	private:
		int _screenWidth;
		int _screenHeight;
		bool _needsMatrixUpdate;
		float _scale;
		glm::vec2 _position;
		glm::mat4 _cameraMatrix;
		glm::mat4 _orthoMatrix;
	};

}