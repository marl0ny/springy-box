#include "gl_wrappers.hpp"

namespace sim_3d {

struct SimParams {
    int stepsPerFrame = (int)(30);
    float dt = (float)(0.0005F);
    Vec3 xyzMin = (Vec3)(Vec3 {.ind={0.0, 0.0, 0.0}});
    Vec3 xyzMax = (Vec3)(Vec3 {.ind={20.0, 20.0, 20.0}});
    float springConst1 = (float)(100.0F);
    float springConst2 = (float)(25.0F);
    float dragConst = (float)(0.0002F);
    IVec3 springCountDimensions = (IVec3)(IVec3 {.ind={32, 15, 32}});
    Vec3 restDimensions = (Vec3)(Vec3 {.ind={12.0, 6.0, 12.0}});
    float wallForceConst = (float)(10000.0F);
    float wallFriction = (float)(1.0F);
    float g = (float)(9.81F);
    float m = (float)(0.00048828125F);
    Vec3 initialPosition = (Vec3)(Vec3 {.ind={10.0, 12.0, 10.0}});
    Vec3 initialVelocity = (Vec3)(Vec3 {.ind={0.0, 0.0, 0.0}});
    Vec3 initialAxis = (Vec3)(Vec3 {.ind={1.0, -1.0, 0.0}});
    float initialAngularVelocity = (float)(0.0F);
    enum {
        STEPS_PER_FRAME=0,
        DT=1,
        XYZ_MIN=2,
        XYZ_MAX=3,
        SPRING_CONST1=4,
        SPRING_CONST2=5,
        DRAG_CONST=6,
        SPRING_COUNT_DIMENSIONS=7,
        REST_DIMENSIONS=8,
        WALL_FORCE_CONST=9,
        WALL_FRICTION=10,
        G=11,
        M=12,
        INITIAL_POSITION=13,
        INITIAL_VELOCITY=14,
        INITIAL_AXIS=15,
        INITIAL_ANGULAR_VELOCITY=16,
    };
    void set(int enum_val, Uniform val) {
        switch(enum_val) {
            case STEPS_PER_FRAME:
            stepsPerFrame = val.i32;
            break;
            case DT:
            dt = val.f32;
            break;
            case XYZ_MIN:
            xyzMin = val.vec3;
            break;
            case XYZ_MAX:
            xyzMax = val.vec3;
            break;
            case SPRING_CONST1:
            springConst1 = val.f32;
            break;
            case SPRING_CONST2:
            springConst2 = val.f32;
            break;
            case DRAG_CONST:
            dragConst = val.f32;
            break;
            case SPRING_COUNT_DIMENSIONS:
            springCountDimensions = val.ivec3;
            break;
            case REST_DIMENSIONS:
            restDimensions = val.vec3;
            break;
            case WALL_FORCE_CONST:
            wallForceConst = val.f32;
            break;
            case WALL_FRICTION:
            wallFriction = val.f32;
            break;
            case G:
            g = val.f32;
            break;
            case M:
            m = val.f32;
            break;
            case INITIAL_POSITION:
            initialPosition = val.vec3;
            break;
            case INITIAL_VELOCITY:
            initialVelocity = val.vec3;
            break;
            case INITIAL_AXIS:
            initialAxis = val.vec3;
            break;
            case INITIAL_ANGULAR_VELOCITY:
            initialAngularVelocity = val.f32;
            break;
        }
    }
    Uniform get(int enum_val) const {
        switch(enum_val) {
            case STEPS_PER_FRAME:
            return {(int)stepsPerFrame};
            case DT:
            return {(float)dt};
            case XYZ_MIN:
            return {(Vec3)xyzMin};
            case XYZ_MAX:
            return {(Vec3)xyzMax};
            case SPRING_CONST1:
            return {(float)springConst1};
            case SPRING_CONST2:
            return {(float)springConst2};
            case DRAG_CONST:
            return {(float)dragConst};
            case SPRING_COUNT_DIMENSIONS:
            return {(IVec3)springCountDimensions};
            case REST_DIMENSIONS:
            return {(Vec3)restDimensions};
            case WALL_FORCE_CONST:
            return {(float)wallForceConst};
            case WALL_FRICTION:
            return {(float)wallFriction};
            case G:
            return {(float)g};
            case M:
            return {(float)m};
            case INITIAL_POSITION:
            return {(Vec3)initialPosition};
            case INITIAL_VELOCITY:
            return {(Vec3)initialVelocity};
            case INITIAL_AXIS:
            return {(Vec3)initialAxis};
            case INITIAL_ANGULAR_VELOCITY:
            return {(float)initialAngularVelocity};
        }
    return Uniform(0);
    }
};

}
