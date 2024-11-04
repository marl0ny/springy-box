#include "gl_wrappers.hpp"

    namespace sim_2d {

    struct UniformParams {
    Uniform stepsPerFrame = Uniform((int)(15));
    Uniform dt = Uniform((float)(0.0005F));
    Uniform xMin = Uniform((float)(0.0F));
    Uniform xMax = Uniform((float)(20.0F));
    Uniform yMin = Uniform((float)(0.0F));
    Uniform yMax = Uniform((float)(20.0F));
    Uniform springConst1 = Uniform((float)(600.0F));
    Uniform springConst2 = Uniform((float)(600.0F));
    Uniform dragConst = Uniform((float)(0.0002F));
    Uniform restWidth = Uniform((float)(12.0F));
    Uniform restHeight = Uniform((float)(6.0F));
    Uniform wallForceConst = Uniform((float)(10000.0F));
    Uniform wallFriction = Uniform((float)(1.0F));
    Uniform g = Uniform((float)(9.81F));
    Uniform wCount = Uniform((int)(64));
    Uniform hCount = Uniform((int)(32));
    Uniform m = Uniform((float)(0.00048828125F));
    Uniform initialPosition = Uniform((Vec2)(Vec2 {.ind={10.0, 10.0}}));
    Uniform initialVelocity = Uniform((Vec2)(Vec2 {.ind={10.0, 10.0}}));
    Uniform initialAngle = Uniform((float)(1.5707963267948966F));
    Uniform initialAngularVelocity = Uniform((float)(-10.0F));
    enum {
        STEPS_PER_FRAME=0,
        DT=1,
        X_MIN=2,
        X_MAX=3,
        Y_MIN=4,
        Y_MAX=5,
        SPRING_CONST1=6,
        SPRING_CONST2=7,
        DRAG_CONST=8,
        REST_WIDTH=9,
        REST_HEIGHT=10,
        WALL_FORCE_CONST=11,
        WALL_FRICTION=12,
        G=13,
        W_COUNT=14,
        H_COUNT=15,
        M=16,
        INITIAL_POSITION=17,
        INITIAL_VELOCITY=18,
        INITIAL_ANGLE=19,
        INITIAL_ANGULAR_VELOCITY=20,
    };
    void set(int enum_val, Uniform val) {
        switch(enum_val) {
            case STEPS_PER_FRAME:
            stepsPerFrame = val;
            break;
            case DT:
            dt = val;
            break;
            case X_MIN:
            xMin = val;
            break;
            case X_MAX:
            xMax = val;
            break;
            case Y_MIN:
            yMin = val;
            break;
            case Y_MAX:
            yMax = val;
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
            case REST_WIDTH:
            restWidth = val;
            break;
            case REST_HEIGHT:
            restHeight = val;
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
            case W_COUNT:
            wCount = val;
            break;
            case H_COUNT:
            hCount = val;
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
            case INITIAL_ANGLE:
            initialAngle = val;
            break;
            case INITIAL_ANGULAR_VELOCITY:
            initialAngularVelocity = val;
            break;
        }
    }
};

}
