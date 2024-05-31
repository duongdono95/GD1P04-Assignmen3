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
#ifndef INPUT_H
#define INPUT_H

#include <glfw3.h>
#include <map>
class Input {
public:
    static void Initialize(GLFWwindow* window);
    static void Update();
    static bool IsKeyPressed(int key);
    static bool IsKeyToggled(int key);
    static bool IsShiftPressed();

private:
    static GLFWwindow* m_window;
    static std::map<int, bool> m_keyStates;
};

GLFWwindow* Input::m_window = nullptr;
std::map<int, bool> Input::m_keyStates;
void Input::Initialize(GLFWwindow* window) {
    m_window = window;
}

void Input::Update() {
    glfwPollEvents();
    for (auto& keyState : m_keyStates) {
        bool currentlyPressed = (glfwGetKey(m_window, keyState.first) == GLFW_PRESS);
        if (!keyState.second && currentlyPressed) {
            m_keyStates[keyState.first] = true; // Key is toggled on
        }
        else if (keyState.second && !currentlyPressed) {
            m_keyStates[keyState.first] = false; // Reset when key is released
        }
    }
}

bool Input::IsKeyToggled(int key) {
    if (m_keyStates.find(key) == m_keyStates.end()) {
        m_keyStates[key] = false; // Initialize if not already in map
    }
    bool toggled = m_keyStates[key];
    if (toggled) {
        m_keyStates[key] = false; // Reset toggle state
    }
    return toggled;
}

bool Input::IsKeyPressed(int key) {
    return glfwGetKey(m_window, key) == GLFW_PRESS;
}

bool Input::IsShiftPressed() {
    return IsKeyPressed(GLFW_KEY_LEFT_SHIFT) || IsKeyPressed(GLFW_KEY_RIGHT_SHIFT);
}

#endif 