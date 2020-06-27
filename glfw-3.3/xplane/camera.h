#pragma once

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

#define _USE_MATH_DEFINES
#include "material.h"
#include "transform.h"

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
	UP,
	DOWN
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 10.3f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 60.0f;
const float ASPECTR = 1.0f;

class Camera: public Component
{
public:
    const static uint64_t componentID = 4;
	
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Euler Angles
    float Yaw;
    float Pitch;
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    float aspectRatio;// = w/h
    // Constructor with vectors
    Camera(Object* _owner, float aspect, float _zoom = ZOOM);
    Camera(Object* _owner, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW,
        float pitch = PITCH, float aspect = ASPECTR);
    

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() const;

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);


private:

    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();


};