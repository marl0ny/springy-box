#include "gl_wrappers.hpp"

namespace sim_3d {

struct UniformParams {
    Uniform stepsPerFrame = Uniform((int)(30));
    Uniform dt = Uniform((float)(0.0005F));
    Uniform xyzMin = Uniform((Vec3)(Vec3 {.ind={0.0, 0.0, 0.0}}));
    Uniform xyzMax = Uniform((Vec3)(Vec3 {.ind={20.0, 20.0, 20.0}}));
    Uniform springConst1 = Uniform((float)(100.0F));
    Uniform springConst2 = Uniform((float)(25.0F));
    Uniform dragConst = Uniform((float)(0.0002F));
    Uniform springCountDimensions = Uniform((IVec3)(IVec3 {.ind={16, 8, 16}}));
    Uniform restDimensions = Uniform((Vec3)(Vec3 {.ind={12.0, 6.0, 12.0}}));
    Uniform wallForceConst = Uniform((float)(10000.0F));
    Uniform wallFriction = Uniform((float)(1.0F));
    Uniform g = Uniform((float)(9.81F));
    Uniform m = Uniform((float)(0.00048828125F));
    Uniform initialPosition = Uniform((Vec3)(Vec3 {.ind={10.0, 12.0, 10.0}}));
    Uniform initialVelocity = Uniform((Vec3)(Vec3 {.ind={0.0, 0.0, 0.0}}));
    Uniform initialAxis = Uniform((Vec3)(Vec3 {.ind={1.0, 2.0, -1.0}}));
    Uniform initialAngularVelocity = Uniform((float)(0.0F));
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
            stepsPerFrame = val;
            break;
            case DT:
            dt = val;
            break;
            case XYZ_MIN:
            xyzMin = val;
            break;
            case XYZ_MAX:
            xyzMax = val;
            break;
            case SPRING_CONST1:
            springConst1 = val;
            break;
            case SPRING_CONST2:
            springConst2 = val;
            break;
            case DRAG_CONST:
            dragConst = val;
            break;
            case SPRING_COUNT_DIMENSIONS:
            springCountDimensions = val;
            break;
            case REST_DIMENSIONS:
            restDimensions = val;
            break;
            case WALL_FORCE_CONST:
            wallForceConst = val;
            break;
            case WALL_FRICTION:
            wallFriction = val;
            break;
            case G:
            g = val;
            break;
            case M:
            m = val;
            break;
            case INITIAL_POSITION:
            initialPosition = val;
            break;
            case INITIAL_VELOCITY:
            initialVelocity = val;
            break;
            case INITIAL_AXIS:
            initialAxis = val;
            break;
            case INITIAL_ANGULAR_VELOCITY:
            initialAngularVelocity = val;
            break;
        }
    }
    Uniform get(int enum_val) const {
        switch(enum_val) {
            case STEPS_PER_FRAME:
            return stepsPerFrame;
            case DT:
            return dt;
            case XYZ_MIN:
            return xyzMin;
            case XYZ_MAX:
            return xyzMax;
            case SPRING_CONST1:
            return springConst1;
            case SPRING_CONST2:
            return springConst2;
            case DRAG_CONST:
            return dragConst;
            case SPRING_COUNT_DIMENSIONS:
            return springCountDimensions;
            case REST_DIMENSIONS:
            return restDimensions;
            case WALL_FORCE_CONST:
            return wallForceConst;
            case WALL_FRICTION:
            return wallFriction;
            case G:
            return g;
            case M:
            return m;
            case INITIAL_POSITION:
            return initialPosition;
            case INITIAL_VELOCITY:
            return initialVelocity;
            case INITIAL_AXIS:
            return initialAxis;
            case INITIAL_ANGULAR_VELOCITY:
            return initialAngularVelocity;
        }
    return Uniform(0);
    }
};

}
