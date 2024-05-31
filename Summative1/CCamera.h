/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2024] Media Design School
File Name : Assignmen2
Description : GD1P04 - Assignment2
Author : Bach Thanh Duong
Mail : bach.duong@mds.ac.nz
**************************************************************************/
#ifndef CAMERA_H
#define CAMERA_H

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

class CCamera {
public:
    enum Mode {
        MANUAL,
        AUTO
    };
    CCamera(const glm::vec3& position, float aspectRatio, float fov = 45.0f);
    void Update(float deltaTime);
    void SetMode(Mode mode);
    Mode GetMode() const;
    void AdjustOrbit(float angleDelta, float radiusDelta);
    void SetPosition(const glm::vec3& position);
    void SetLookDirection(const glm::vec3& lookDir, const glm::vec3& upDir);
    glm::mat4 GetViewMat() const;
    glm::mat4 GetProjMat() const;
    glm::vec3 GetForwardVector() const;
    glm::vec3 GetRightVector() const;
    glm::vec3 GetPosition() const;

private:
    void UpdateMatrices();
    void UpdatePosition();
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_upDir;
    glm::mat4 m_viewMat;
    glm::mat4 m_projMat;
    float m_orbitRadius;
    float m_orbitAngle;
    float m_fov;
    float m_aspectRatio;
    Mode m_mode;
    float m_yOffset;
    glm::vec3 m_lookDir;
};

#endif // CAMERA_H
