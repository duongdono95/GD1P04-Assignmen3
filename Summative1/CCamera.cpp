#include "CCamera.h"

CCamera::CCamera(const glm::vec3& position, float aspectRatio, float fov)
    : m_position(position), m_target(0.0f), m_upDir(0.0f, 1.0f, 0.0f),
    m_fov(fov), m_aspectRatio(aspectRatio), m_orbitRadius(70.0f), m_orbitAngle(0.0f),
    m_mode(AUTO) {
    UpdateMatrices();
}

void CCamera::Update(float deltaTime) {
    if (m_mode == AUTO) {
        m_orbitAngle += deltaTime * 0.1f;  // Adjust speed
    }
    UpdatePosition();
    UpdateMatrices();
}

void CCamera::SetMode(Mode mode) {
    m_mode = mode;
}

CCamera::Mode CCamera::GetMode() const {
    return m_mode;
}

void CCamera::AdjustOrbit(float angleDelta, float radiusDelta) {
    m_orbitAngle += angleDelta;
    m_orbitRadius = glm::max(10.0f, m_orbitRadius + radiusDelta);
}

void CCamera::UpdatePosition() {
    m_position.x = m_orbitRadius * cos(m_orbitAngle);
    m_position.z = m_orbitRadius * sin(m_orbitAngle);
    m_position.y = 30.0f;
}

void CCamera::UpdateMatrices() {
    m_viewMat = glm::lookAt(m_position, m_target, m_upDir);
    m_projMat = glm::perspective(glm::radians(m_fov), m_aspectRatio, 0.001f, 1000.0f);
}

void CCamera::SetPosition(const glm::vec3& position) {
    m_position = position;
    UpdateMatrices();
}

void CCamera::SetLookDirection(const glm::vec3& lookDir, const glm::vec3& upDir) {
    m_lookDir = lookDir;
    m_upDir = upDir;
    UpdateMatrices();
}

glm::mat4 CCamera::GetViewMat() const {
    return m_viewMat;
}

glm::mat4 CCamera::GetProjMat() const {
    return m_projMat;
}

glm::vec3 CCamera::GetForwardVector() const {
    return glm::normalize(glm::vec3(m_viewMat[0][2], m_viewMat[1][2], m_viewMat[2][2]));  // Assuming view matrix is correctly updated
}

glm::vec3 CCamera::GetRightVector() const {
    return glm::normalize(glm::vec3(m_viewMat[0][0], m_viewMat[1][0], m_viewMat[2][0]));  // Assuming view matrix is correctly updated
}

glm::vec3 CCamera::GetPosition() const {
    return m_position;
}
