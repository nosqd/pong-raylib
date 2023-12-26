//
// Created by nosqd on 27.12.2023.
//

#ifndef PHYSICS_H
#define PHYSICS_H
#include <cmath>

typedef struct {
    int x;
    int y;
    int r;
} CircleType;

typedef struct {
    int x;
    int y;
    int width;
    int height;
} RectType;

typedef struct {
    int x;
    int y;
} vec2i;

inline bool circleRectIntersection(CircleType circle, RectType rect)
{
    vec2i circleDistance;
    circleDistance.x = abs(circle.x - rect.x);
    circleDistance.y = abs(circle.y - rect.y);

    if (circleDistance.x > (rect.width/2 + circle.r)) { return false; }
    if (circleDistance.y > (rect.height/2 + circle.r)) { return false; }

    if (circleDistance.x <= (rect.width/2)) { return true; }
    if (circleDistance.y <= (rect.height/2)) { return true; }

    float cornerDistance_sq = (circleDistance.x - rect.width/2)^2 +
                         (circleDistance.y - rect.height/2)^2;

    return (cornerDistance_sq <= (circle.r^2));
}
#endif //PHYSICS_H
