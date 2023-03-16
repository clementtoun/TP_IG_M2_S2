#ifndef TP_IG_M2_S2_CAMERA_H
#define TP_IG_M2_S2_CAMERA_H

#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>

enum Camera_Movement {
    LEFT = 0,
    FORWARD,
    RIGHT,
    BACKWARD,
    UP,
    DOWN
};

constexpr float FOV      =  45.0f;
constexpr float ASPECT     =  16.0f/9.0f;
constexpr float NEAR      =  0.1f;
constexpr float FAR     =  500.0f;

/*------------------------------------------------------------------------------------------------------------------------*/
/*                                                Abstract Camera                                                         */
/*------------------------------------------------------------------------------------------------------------------------*/

class Camera : public QObject {

public:

    explicit Camera(QVector3D position = QVector3D(0.f, 0.f, 1.f), QVector3D up = QVector3D(0.f, 1.f, 0.f), QVector3D look = QVector3D(0.f, 0.f, 0.f),
                    float fov=FOV, float aspect=ASPECT, float zoom=45.f);

    virtual ~Camera();

    virtual float getYaw() const = 0;
    virtual float getPitch() const = 0;

    virtual // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    QMatrix4x4 viewmatrix() const;
    QMatrix4x4 projectionmatrix() const;
    float &zoom();
    QVector3D &position();
    QVector3D &front();

    void setposition(QVector3D position);
    void setAspect(float aspect);
    float getAspect() const;

    float getNear() const;

    float getFar() const;

    void setviewport(QVector4D viewport);

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    virtual void processkeyboard(Camera_Movement direction, float deltaTime) {};
    // Processes input received from a mouse input system.
    virtual void processmouseclick(int button, float xpos, float ypos);
    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    virtual void processmousemovement(int button, float xpos, float ypos) {};
    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    virtual void processmousescroll(float yoffset);

protected:

    float _fov;
    float _aspect;
    float _near;
    float _far;

    QVector3D _position{};
    QVector3D _front{};
    QVector3D _up{};
    float _zoom;

    QVector4D _viewport{};

    // mouse movement
    int _mousebutton{};
    float _mousestartx{};
    float _mousestarty{};

    // Camera options
    float _movementspeed{};
    float _mousesensitivity{};
    float _mol_speed{};
};


/*------------------------------------------------------------------------------------------------------------------------*/
/*                                       Euler Camera : Yaw, Pitch, Roll                                                  */
/*------------------------------------------------------------------------------------------------------------------------*/

// from learnopenGL tutorial
// Default camera values
constexpr float YAW        = -90.0f;
constexpr float PITCH      =  0.0f;
constexpr float SPEED      =  0.01f;
constexpr float SENSITIVTY =  0.10f;
constexpr float ZOOM       =  45.0f;
constexpr float Mol_SPEED = 0.1f;

// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class EulerCamera : public Camera {

public:

    // Constructor with vectors (default constructor)
    explicit EulerCamera(QVector3D position = QVector3D(0.0f, 0.0f, 0.0f), QVector3D up = QVector3D(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, float fov = FOV, float aspect = ASPECT);

    ~EulerCamera() override;

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void processkeyboard(Camera_Movement direction, float deltaTime) override;

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void processmousemovement(int button, float xpos, float ypos) override;

    void processmousescroll(float delta) override;

    float getYaw() const;

    float getPitch() const;

private:
    // Calculates the front vector from the Camera's (updated) Eular Angles
    void updatecameravectors();

    // Camera Attributes
    QVector3D _right{};
    QVector3D _worldup{};
    // Eular Angles
    float _yaw;
    float _pitch;
};


/*------------------------------------------------------------------------------------------------------------------------*/
/*                                            Trackball Camera                                                            */
/*                                  Always rotate around point (0, 0, 0)                                                  */
/*                                      Accumulate error on roll                                                          */
/*------------------------------------------------------------------------------------------------------------------------*/
class TrackballCamera : public Camera {

public:

    // Constructor with vectors (default constructor)
    explicit TrackballCamera(QVector3D position = QVector3D(0.0f, 0.0f, 0.0f), QVector3D up = QVector3D(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, float fov = FOV, float aspect = ASPECT);

    ~TrackballCamera() override;

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void processkeyboard(Camera_Movement direction, float deltaTime) override;

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void processmousemovement(int button, float xpos, float ypos) override;

    void processmousescroll(float yoffset) override;

    float getYaw() const;

    float getPitch() const;

private:
    // Calculates the front vector from the Camera's (updated) Eular Angles
    void updatecameravectors();

    // Camera Attributes
    QVector3D _right{};
    QVector3D _worldup{};
    // Eular Angles
    float _yaw;
    float _pitch;
};


#endif //TP_IG_M2_S2_CAMERA_H
