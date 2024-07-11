#include "FrustumCulling.hpp"

Plane::Plane() : normal(glm::vec3(0.0f, 0.0f, 0.0f)), distance(0.0f) {}
Plane::Plane(const glm::vec3& n, float d) : normal(n), distance(d) {}

// Compute the plane from 3 points in space
// a, b, c: the 3 points 
// The normal is computed as the cross product of the vectors ab and ac
// The distance is computed as the dot product of the normal and a
void Plane::fromPoints(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
    normal = glm::normalize(glm::cross(b - a, c - a));
    distance = -glm::dot(normal, a);
}

// Compute the distance from a point to the plane
// point: the point to compute the distance from
// The distance is computed as the dot product of the normal and the point plus the distance
float Plane::distanceToPoint(const glm::vec3& point) const {
    return glm::dot(normal, point) + distance;
}

// Update the frustum planes based on the view-projection matrix of the camera
// viewProjection: the view-projection matrix of the camera
// The frustum planes are computed as the 6 planes of the view-projection matrix (left, right, bottom, top, near, far) with a margin of 2.5
// The normal of the planes is inverted and the distance is negated to be in the same direction as the camera view
void Frustum::update(const glm::mat4& viewProjection) {
    const float margin = 2.5f;
    planes[0] = Plane(glm::vec3(viewProjection[0].w + viewProjection[0].x, 
                                viewProjection[1].w + viewProjection[1].x, 
                                viewProjection[2].w + viewProjection[2].x), 
                    viewProjection[3].w + viewProjection[3].x + margin); // Plan left

    planes[1] = Plane(glm::vec3(viewProjection[0].w - viewProjection[0].x, 
                                viewProjection[1].w - viewProjection[1].x, 
                                viewProjection[2].w - viewProjection[2].x), 
                    viewProjection[3].w - viewProjection[3].x + margin); // Plan right

    planes[2] = Plane(glm::vec3(viewProjection[0].w + viewProjection[0].y, 
                                viewProjection[1].w + viewProjection[1].y, 
                                viewProjection[2].w + viewProjection[2].y), 
                    viewProjection[3].w + viewProjection[3].y + margin); // Plan bottom

    planes[3] = Plane(glm::vec3(viewProjection[0].w - viewProjection[0].y, 
                                viewProjection[1].w - viewProjection[1].y, 
                                viewProjection[2].w - viewProjection[2].y), 
                    viewProjection[3].w - viewProjection[3].y + margin); // Plan top

    planes[4] = Plane(glm::vec3(viewProjection[0].w + viewProjection[0].z, 
                                viewProjection[1].w + viewProjection[1].z, 
                                viewProjection[2].w + viewProjection[2].z), 
                    viewProjection[3].w + viewProjection[3].z + margin); // Plan near

    planes[5] = Plane(glm::vec3(viewProjection[0].w - viewProjection[0].z, 
                                viewProjection[1].w - viewProjection[1].z, 
                                viewProjection[2].w - viewProjection[2].z), 
                    viewProjection[3].w - viewProjection[3].z + margin); // Plan far

    for (int i = 0; i < 6; ++i) {
        float length = glm::length(planes[i].normal);
        planes[i].normal = -planes[i].normal;
        planes[i].distance = -planes[i].distance;
    }
}

// Check if a bounding box intersects the frustum
// min, max: the minimum and maximum points of the bounding box
// The bounding box intersects the frustum if all the 8 vertices of the bounding box are in the frustum
// The vertices are computed based on the minimum and maximum points of the bounding box
// The tolerance is set to 0.5 to avoid floating point errors
bool Frustum::intersects(const glm::vec3& min, const glm::vec3& max) const {
    const float tolerance = 0.5f;

    for (int i = 0; i < 6; ++i) {
        const Plane& plane = planes[i];

        glm::vec3 pVertex = min;
        if (plane.normal.x > 0) pVertex.x = max.x;
        if (plane.normal.y > 0) pVertex.y = max.y;
        if (plane.normal.z > 0) pVertex.z = max.z;

        if (plane.distanceToPoint(pVertex) - tolerance > 0) {
            return false;
        }
    }
    return true;
}
