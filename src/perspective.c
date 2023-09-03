#include "../include/shapes.h"
#include "../include/perspective.h"

Point Vec2Point(Camera* camera, Vector* point) {
    float x = (point->x*camera->d/point->z)*SCREEN_WIDTH/camera->Vw;
    float y = (point->y*camera->d/point->z)*SCREEN_HEIGHT/camera->Vh;

    return (Point) {x + SCREEN_WIDTH/2, -y + SCREEN_HEIGHT/2, 1/point->z};
}


float calcIntensity(Vector normal, Vector point, Light* lights, int len_l) {
    float intensity = 0;
    for (int i = 0; i < len_l; i++) {
        int type = lights[i].type;
        if (type == 0){
            //printf("ha\n");
            intensity += lights[i].intensity;    
        } else if (type == 1) {
            Vector diff = sub(point, lights[i].direction);
            float d = lights[i].intensity*(dot(diff, normal));
            float eh = d/(getLength(normal)*getLength(diff));
            if (eh < 0) eh*= -1;
            intensity += eh;
        }
    }
    return intensity;
}


void renderObj(SDL_Renderer* renderer, Camera* camera, Object obj) {
    Point projs[obj.len_v];
    for (int i = 0; i < obj.len_v; i++) {
        projs[i] = Vec2Point(camera, &obj.points[i]);
    }

    for (int i = 0; i < obj.len_tr; i++) {
        DrawTriangle(renderer, 
            &projs[obj.triangles[i].v1], 
            &projs[obj.triangles[i].v2], 
            &projs[obj.triangles[i].v3], obj.triangles[i].color);
    }
}

Vector add(Vector v1, Vector v2) {
    return (Vector) {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}



Vector sub(Vector v1, Vector v2) {
    return (Vector) {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

float dot(Vector v1, Vector v2) {
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

float getLength(Vector v) {
    return sqrt(dot(v, v));
}

Vector cross(Vector v1, Vector v2) {
    return (Vector) {v1.y*v2.z - v2.y*v1.z, v2.x*v1.z - v1.x*v2.z, v1.x*v2.y - v2.x*v1.y};
}

Vector scale(Vector v, float s) {
    return (Vector) {v.x*s, v.y*s, v.z*s};
}

Vector rotateXZ(Vector v, double phi) {
    float cos_t = cos(phi);
    float sin_t = sin(phi);

    return (Vector) {sin_t*v.z + cos_t*v.x, v.y, cos_t*v.z - sin_t*v.x};
}

Vector rotateYZ(Vector v, double theta) {
    float cos_t = cos(theta);
    float sin_t = sin(theta);

    return (Vector) {v.x, sin_t*v.z + cos_t*v.y, cos_t*v.z - sin_t*v.y};
}

Vector rotateXY(Vector v, double a) {
    float cos_t = cos(a);
    float sin_t = sin(a);

    return (Vector) {cos_t*v.x - sin_t*v.y, sin_t*v.x + cos_t*v.y, v.z};
}


float get_longest(Vector* points, int len) {
    float longest = -1;
    for (int i = 0; i < len; i++) {
        float hyp = dot(points[i], points[i]);
        if (longest < hyp)
            longest = hyp;
    }

    return sqrt(longest);
}



void renderInstance(SDL_Renderer* renderer, Camera* camera, Instance* instance, float buffer[500][500]) {
    for (int i = 0; i < 5; i++) {
        float dist = dot(camera->planes[i].normal, instance->position) + camera->planes[i].d;
        //printf("%f %f \n", dist, instance->model->r*instance->scale);
        if (dist < -instance->model->r*instance->scale) {
            //printf("Out of bounds!");
            return;
        }
    }

    Point projs[instance->model->len_v];
    Vector points[instance->model->len_v];

    for (int i = 0; i < instance->model->len_v; i++) {
        Vector pos = add(
            rotateXY(
                rotateYZ(rotateXZ(scale(instance->model->points[i], instance->scale), instance->phi), instance->theta),
                instance->a)
        , instance->position);
        projs[i] = Vec2Point(camera, &pos);
        points[i] = pos;
    }

    for (int i = 0; i < instance->model->len_tr; i++) {
        Vector normal = cross(
            sub(points[instance->model->triangles[i].v3], points[instance->model->triangles[i].v1]),
            sub(points[instance->model->triangles[i].v2], points[instance->model->triangles[i].v1])
        );

        float d = dot(normal, points[instance->model->triangles[i].v2]);
        if (d <= 0)
            continue;

        if (instance->tex != NULL) {
            projs[instance->model->triangles[i].v1].tx = instance->model->triangles[i].v1_tx;
            projs[instance->model->triangles[i].v2].tx = instance->model->triangles[i].v2_tx;
            projs[instance->model->triangles[i].v3].tx = instance->model->triangles[i].v3_tx;

            ShadeTex(renderer, instance->tex,
                &projs[instance->model->triangles[i].v1], 
                &projs[instance->model->triangles[i].v2], 
                &projs[instance->model->triangles[i].v3], instance->model->triangles[i].color,
                normal,
                camera->lights,
                camera->len_l,
                buffer);
            continue;
        }

        ShadeTriangleSmart(renderer, 
            &projs[instance->model->triangles[i].v1], 
            &projs[instance->model->triangles[i].v2], 
            &projs[instance->model->triangles[i].v3], 
            instance->model->triangles[i].color, 
            normal,
            camera->lights,
            camera->len_l,
            buffer);
    }
}