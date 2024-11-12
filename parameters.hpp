#include "gl_wrappers.hpp"

namespace sim_2d {

struct SimParams {
    int stepsPerFrame = (int)(30);
    float dt = (float)(0.0005F);
    float xMin = (float)(0.0F);
    float xMax = (float)(20.0F);
    float yMin = (float)(0.0F);
    float yMax = (float)(20.0F);
    float springConst1 = (float)(600.0F);
    float springConst2 = (float)(600.0F);
    float dragConst = (float)(0.0002F);
    float restWidth = (float)(12.0F);
    float restHeight = (float)(6.0F);
    float wallForceConst = (float)(10000.0F);
    float wallFriction = (float)(1.0F);
    float g = (float)(9.81F);
    int wCount = (int)(64);
    int hCount = (int)(32);
    float m = (float)(0.00048828125F);
    Vec2 initialPosition = (Vec2)(Vec2 {.ind={10.0, 10.0}});
    Vec2 initialVelocity = (Vec2)(Vec2 {.ind={10.0, 10.0}});
    float initialAngle = (float)(1.5707963267948966F);
    float initialAngularVelocity = (float)(-10.0F);
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
            stepsPerFrame = val.i32;
            break;
            case DT:
            dt = val.f32;
            break;
            case X_MIN:
            xMin = val.f32;
            break;
            case X_MAX:
            xMax = val.f32;
            break;
            case Y_MIN:
            yMin = val.f32;
            break;
            case Y_MAX:
            yMax = val.f32;
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
            case REST_WIDTH:
            restWidth = val.f32;
            break;
            case REST_HEIGHT:
            restHeight = val.f32;
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
            case W_COUNT:
            wCount = val.i32;
            break;
            case H_COUNT:
            hCount = val.i32;
            break;
            case M:
            m = val.f32;
            break;
            case INITIAL_POSITION:
            initialPosition = val.vec2;
            break;
            case INITIAL_VELOCITY:
            initialVelocity = val.vec2;
            break;
            case INITIAL_ANGLE:
            initialAngle = val.f32;
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
            case X_MIN:
            return {(float)xMin};
            case X_MAX:
            return {(float)xMax};
            case Y_MIN:
            return {(float)yMin};
            case Y_MAX:
            return {(float)yMax};
            case SPRING_CONST1:
            return {(float)springConst1};
            case SPRING_CONST2:
            return {(float)springConst2};
            case DRAG_CONST:
            return {(float)dragConst};
            case REST_WIDTH:
            return {(float)restWidth};
            case REST_HEIGHT:
            return {(float)restHeight};
            case WALL_FORCE_CONST:
            return {(float)wallForceConst};
            case WALL_FRICTION:
            return {(float)wallFriction};
            case G:
            return {(float)g};
            case W_COUNT:
            return {(int)wCount};
            case H_COUNT:
            return {(int)hCount};
            case M:
            return {(float)m};
            case INITIAL_POSITION:
            return {(Vec2)initialPosition};
            case INITIAL_VELOCITY:
            return {(Vec2)initialVelocity};
            case INITIAL_ANGLE:
            return {(float)initialAngle};
            case INITIAL_ANGULAR_VELOCITY:
            return {(float)initialAngularVelocity};
        }
    return Uniform(0);
    }
};

}
