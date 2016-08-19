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

		void SetPosition(const glm::vec2& newPosition) { m_position = newPosition; m_needsMatrixUpdate = true;	}
		void SetScale(float newScale) { m_scale = newScale; m_needsMatrixUpdate = true; }

		glm::vec2 & GetPosition() { return m_position; }
		float GetScale() { return m_scale; }
		glm::mat4 getCameraMatrix() { return m_cameraMatrix; }

	private:
		int m_screenWidth = 500;
		int m_screenHeight = 500;
		bool m_needsMatrixUpdate = true;
		float m_scale = 1.0f;
		glm::vec2 m_position = glm::vec2(0.0f, 0.0f);
		glm::mat4 m_cameraMatrix = glm::mat4(1.0f);
		glm::mat4 m_orthoMatrix = glm::mat4(1.0f);
	};

}