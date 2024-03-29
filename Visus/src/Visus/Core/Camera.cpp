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
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void Camera2D::SetPosition(const glm::vec3& position)
	{
		m_Position = position;
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), m_Rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		m_ViewProjectionMatrix = m_ProjectionMatrix * glm::inverse(m_ViewMatrix);
	}

	void Camera2D::SetRotation(const float& rotation)
	{
		m_Rotation = rotation;
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		m_ViewProjectionMatrix = m_ProjectionMatrix * glm::inverse(m_ViewMatrix);
	}

	// =======================
	// Camera3D Implementation	
	Camera3D::Camera3D()
	{
		m_ViewProjectionMatrix = glm::mat4(1.0f);
		m_ProjectionMatrix = glm::mat4(1.0f);
		m_ViewMatrix = glm::mat4(1.0f);
		m_Position = glm::vec3(0.0f);
		m_FrontVector = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
		m_Yaw = -90.0f;
		m_Pitch = 0.0f;
		m_Roll = 0.0f;
		m_MouseSensivity = 0.1f;
		m_MovementSpeed = 0.1f;

		CalculateVectors();
	}

	void Camera3D::SetProjection(const float& fov, const float& ratio, const float& zNear, const float& zFar)
	{
		m_ProjectionMatrix = glm::perspective(fov, ratio, zNear, zFar);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void Camera3D::SetPosition(const glm::vec3& position)
	{
		m_Position = position;
		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_FrontVector, m_UpVector);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void Camera3D::SetRotation(const float& yaw, const float& pitch, const float& roll /*= 0.0f*/)
	{
		m_Yaw = yaw;
		m_Pitch = pitch;

		CalculateVectors();

		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_FrontVector, m_UpVector);
		m_ViewProjectionMatrix = m_ProjectionMatrix * glm::inverse(m_ViewMatrix);
	}

	glm::mat4 Camera3D::GetRotationMatrix() const
	{
		return glm::lookAt(m_Position, m_Position + m_FrontVector, glm::vec3(0, 1, 0));
	}

	void Camera3D::LookAt(glm::vec3 at)
	{
		m_ViewMatrix = glm::lookAt(m_Position, at, m_UpVector);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void Camera3D::Rotate(const float& yawOffset, const float& pitchOffset, const float& rollOffset, bool lockPitch)
	{
		m_Yaw += yawOffset * m_MouseSensivity;

		// If pitch is locked (must be in -89.0f to 89.0f range) AND pitch is less than -89.0f OR greater than 89.0f
		if (lockPitch) 
		{
			if(m_Pitch + pitchOffset * m_MouseSensivity > 89.0f)
				m_Pitch = 89.0f;
			else if (m_Pitch + pitchOffset * m_MouseSensivity < -89.0f)
				m_Pitch = -89.0f;
			else
				m_Pitch += pitchOffset * m_MouseSensivity;
		}
		else {
			m_Pitch += pitchOffset * m_MouseSensivity;
		}

		m_Roll += rollOffset * m_MouseSensivity;

		CalculateVectors();

		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_FrontVector, glm::vec3(0, 1, 0));
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void Camera3D::Move(glm::vec3 direction)
	{
		m_Position += m_RightVector * direction.x * m_MovementSpeed;
		m_Position += m_UpVector * direction.y * m_MovementSpeed;
		m_Position += m_FrontVector * direction.z * m_MovementSpeed;

		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_FrontVector, m_UpVector);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void Camera3D::CalculateVectors()
	{
		// TODO: apply Roll rotation
		// NOTE: we explicitly call single-precision sin and cos functions,
		// to make sure that we use exactly them.
		glm::vec3 frontDirection;

		frontDirection.x = cos(2 * 3.14 * (m_Yaw / 360)) * cos(2 * 3.14 * (m_Pitch / 360));
		frontDirection.y = sin(2 * 3.14 * (m_Pitch / 360));
		frontDirection.z = sin(2 * 3.14 * (m_Yaw/ 360)) * cos(2 * 3.14 * (m_Pitch / 360));

		m_FrontVector = glm::normalize(frontDirection);
		m_RightVector = glm::normalize(glm::cross(m_FrontVector, glm::vec3(0.0f, 1.0f, 0.0f)));
		m_UpVector = glm::normalize(glm::cross(m_RightVector, m_FrontVector));
	}

}