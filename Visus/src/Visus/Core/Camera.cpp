#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Motus3D {

	// =======================
	// Camera2D Implementation
	Camera2D::Camera2D()
	{
		m_ViewProjectionMatrix = glm::mat4(1.0f);
		m_ProjectionMatrix = glm::mat4(1.0f);
		m_ViewMatrix = glm::mat4(1.0f);
		m_Rotation = 0.0f;
	}

	void Camera2D::SetProjection(float left, float right, float bottom, float top, float zNear /*= 0.0f*/, float zFar /*= 1.0f*/)
	{
		glm::mat4 projection = glm::ortho(left, right, bottom, top, zNear, zFar);
		m_ProjectionMatrix = projection;
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void Camera2D::SetPosition(glm::vec3 position)
	{
		m_Position = position;
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), m_Rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		m_ViewProjectionMatrix = m_ProjectionMatrix * glm::inverse(m_ViewMatrix);
	}

	void Camera2D::SetRotation(float rotation)
	{
		m_Rotation = rotation;
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		m_ViewProjectionMatrix = m_ProjectionMatrix * glm::inverse(m_ViewMatrix);
	}

}