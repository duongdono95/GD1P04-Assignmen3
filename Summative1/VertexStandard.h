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
#pragma once

#include <glm.hpp>

struct VertexStandard {
    glm::vec3 position;
    glm::vec2 texcoord;
    glm::vec3 normal;

    VertexStandard() {
        position = glm::vec3(0.0f);
        texcoord = glm::vec2(0.0f);
        normal = glm::vec3(0.0f);
    };

    VertexStandard(glm::vec3 pos, glm::vec2 texc, glm::vec3 norm) {
        position = pos;
        texcoord = texc;
        normal = norm;
    }
};