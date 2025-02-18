#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Light
{
public:
    Light(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, double a, double b, double c);
    Light(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, double a, double b, double c, glm::vec3 dir, double ang);
    glm::vec3 position, ambient, diffuse, specular;
    double a, b, c; //a*d² + b*d + c
    glm::vec3 direction;
    double angle;

    glm::vec3 get_vector_L(glm::vec3 s);
    float get_atenuation(glm::vec3 s);
    bool isPointInCone(glm::vec3 point);
};

