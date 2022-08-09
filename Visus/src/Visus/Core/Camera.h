#pragma once

#include <Visus/Core/Macros.h>

#include <glm/glm.hpp>

namespace Motus3D {

	class VISUS_API Camera
	{
	public:
		Camera() {};

		virtual glm::mat4 GetViewProjectionMatrix() const = 0;
		virtual glm::mat4 GetProjectionMatrix() const = 0;
		virtual glm::mat4 GetViewMatrix() const = 0;		

	};

	class VISUS_API Camera2D : public Camera
	{
	public:
		Camera2D();

		glm::mat4 GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; };
		glm::mat4 GetProjectionMatrix() const { return m_ProjectionMatrix; };
		glm::mat4 GetViewMatrix() const { return m_ViewMatrix; };

		void SetProjection(float left, float right, float bottom, float top, float zNear = 0.0f, float zFar = 1.0f);
		void SetPosition(glm::vec3 position);
		void SetRotation(float rotation);

		glm::vec3 GetPosition() const { return m_Position; }
		float GetRotation() const { return m_Rotation; }

	private:
		glm::mat4 m_ViewProjectionMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;

		glm::vec3 m_Position;
		float m_Rotation;

	};

}