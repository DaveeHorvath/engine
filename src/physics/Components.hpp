#ifndef PHYSICS_COMPONENTS
#define PHYSICS_COMPONENTS

struct Transform {
    float pos_x, pos_y, pos_z;
    float scale_x, scale_y, scale_z;
    float rotation_x, rotation_y, rotation_z;
};

struct Rigidbody
{
    float vel_x, vel_y, vel_z;
    float accel_x, accel_y, accel_z;
};


#endif