#ifndef FRUSTUM_CULLING_H
#define FRUSTUM_CULLING_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Plane {
    glm::vec3 normal;
    float distance;

    Plane();
    Plane(const glm::vec3& n, float d);
    void fromPoints(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
    float distanceToPoint(const glm::vec3& point) const;
};

struct Frustum {
    Plane planes[6];

    void update(const glm::mat4& viewProjection);
    bool intersects(const glm::vec3& min, const glm::vec3& max) const;
};

#endif // FRUSTUM_CULLING_H
