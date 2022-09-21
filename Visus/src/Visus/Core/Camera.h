#pragma once

#include <Visus/Core/Macros.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Motus3D {

	// Interface for Camera2D and Camera3D
	class VISUS_API Camera
	{
	public:

		virtual glm::mat4 GetViewProjectionMatrix() const = 0;
		virtual glm::mat4 GetProjectionMatrix() const = 0;
		virtual glm::mat4 GetViewMatrix() const = 0;

	};

	// 2D camera, which uses orthographic matrix as projection matrix.
	// Should be used in 2D games or to render GUI.
	class VISUS_API Camera2D : public Camera
	{
	public:
		Camera2D();

		// Matrix getters
		glm::mat4 GetViewProjectionMatrix() const override { return m_ViewProjectionMatrix; };
		glm::mat4 GetProjectionMatrix() const override { return m_ProjectionMatrix; };
		glm::mat4 GetViewMatrix() const override { return m_ViewMatrix; };

		// Getters for common camera data
		void SetProjection(float left, float right, float bottom, float top, float zNear = 0.0f, float zFar = 1.0f);
		void SetPosition(const glm::vec3& position);
		void SetRotation(const float& rotation);

		// Getters for common camera data
		glm::vec3 GetPosition() const { return m_Position; }
		float GetRotation() const { return m_Rotation; }

	private:
		glm::mat4 m_ViewProjectionMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;

		glm::vec3 m_Position;
		float m_Rotation;
	};


	// 3D camera, which uses perspective matrix as projection matrix.
	// Should be used in any kind of 3D games, because it makes engine be able
	// to display depth correctly.
	class VISUS_API Camera3D : public Camera 
	{
	public:
		Camera3D();

		// Matrix getters
		glm::mat4 GetViewProjectionMatrix() const override { return m_ViewProjectionMatrix; };
		glm::mat4 GetProjectionMatrix() const override { return m_ProjectionMatrix; };
		glm::mat4 GetViewMatrix() const override { return m_ViewMatrix; };

		// Setters for common camera data
		void SetProjection(const float& fov, const float& ratio, const float& zNear, const float& zFar);
		void SetPosition(const glm::vec3& position);
		void SetRotation(const float& yaw, const float& pitch, const float& roll = 0.0f);
		void SetSensivity(const float& sensivity) { m_MouseSensivity = sensivity; }
		void SetMovementSpeed(const float& speed) { m_MovementSpeed = speed; }

		// Getters for common camera data
		glm::vec3 GetPosition() const { return m_Position; }
		glm::mat4 GetRotationMatrix() const;
		float GetYaw() const { return m_Yaw; };
		float GetPitch() const { return m_Pitch; };
		float GetRoll() const { return m_Roll; };
		float GetSensivity() const { return m_MouseSensivity; }
		float GetMovementSpeed() const { return m_MovementSpeed; }

		// Special methods to move / rotate 3D camera
		void LookAt(glm::vec3 at);
		void Rotate(const float& yawOffset, const float& pitchOffset, const float& rollOffset, bool lockPitch);
		void Move(glm::vec3 direction);

	private:
		void CalculateVectors();

	private:
		// Matrices
		glm::mat4 m_ViewProjectionMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;


		// Common Camera data
		glm::vec3 m_Position;
		float m_Yaw;
		float m_Pitch;
		float m_Roll;

		float m_MouseSensivity;
		float m_MovementSpeed;

		// Other math data, used to calculate view matrix
		// Represents direction (unit vector) at which camera looks.
		glm::vec3 m_FrontVector;
		glm::vec3 m_UpVector;
		glm::vec3 m_RightVector;
	};

}