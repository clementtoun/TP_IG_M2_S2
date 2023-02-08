#include "Camera.h"
#include <QtMath>

Camera::Camera(QVector3D position, QVector3D up, QVector3D look, float fov, float aspect, float zoom) {
    setObjectName("Untitled Scene");
    _fov = fov;
    _aspect = aspect;
    _near = NEAR;
    _far = FAR;
    _position = position;
    _front = look-position;
    _up = up;
    _zoom = zoom;
}

Camera::~Camera() = default;

QMatrix4x4 Camera::viewmatrix() const {
    QMatrix4x4 mat;
    mat.lookAt(_position, _position + _front, _up);
    return mat;
}

QMatrix4x4 Camera::projectionmatrix() const {
    QMatrix4x4 mat;
    mat.perspective(_fov, _aspect, _near, _far);
    return mat;
}

void Camera::processmouseclick(int button, float xpos, float ypos) {
    _mousebutton = button;
    _mousestartx = xpos;
    _mousestarty = ypos;
}

float &Camera::zoom() {
    return _zoom;
}

QVector3D &Camera::position() {
    return _position;
}

void Camera::setposition(QVector3D position) {
    _position = position;
}

void Camera::setviewport(QVector4D viewport) {
    _viewport = viewport;
}

void Camera::setAspect(float aspect) {
    _aspect = aspect;
}

QVector3D &Camera::front() {
    return _front;
}

void Camera::processmousescroll(float yoffset) {

}

/*------------------------------------------------------------------------------------------------------------------------*/

// A camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
// Constructor with vectors
EulerCamera::EulerCamera(QVector3D position, QVector3D up, float yaw, float pitch, float fov, float aspect) :
        Camera(position, up, QVector3D(0.f, 0.f, 0.f), fov, aspect, ZOOM) {
    _worldup = _up;
    _yaw = yaw;
    _pitch = pitch;
    _movementspeed = SPEED;
    _mousesensitivity = SENSITIVTY;
    _mol_speed = Mol_SPEED;
    updatecameravectors();
}

EulerCamera::~EulerCamera() = default;

void EulerCamera::processmousescroll(float delta) {
    _position += _mol_speed*delta*_front;
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void EulerCamera::processkeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = _movementspeed * deltaTime;
    if (direction == FORWARD) {
        QVector3D front_xz = QVector3D(_front.x(), 0.0f, _front.z());
        front_xz.normalize();
        _position += front_xz * velocity;
    }
    if (direction == BACKWARD) {
        QVector3D front_xz = QVector3D(_front.x(), 0.0f, _front.z());
        front_xz.normalize();
        _position -= front_xz * velocity;
    }
    if (direction == LEFT)
        _position -= _right * velocity;
    if (direction == RIGHT)
        _position += _right * velocity;
    if (direction == UP)
        _position += QVector3D(0.0f,1.0f,0.0f) * velocity;
    if (direction == DOWN)
        _position -= QVector3D(0.0f,1.0f,0.0f) * velocity;
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void EulerCamera::processmousemovement(int button, float xpos, float ypos) {

    if(button == 1) {
        float xoffset = xpos - _mousestartx;
        float yoffset = _mousestarty - ypos;
        _mousestartx = xpos;
        _mousestarty = ypos;
        xoffset *= _mousesensitivity;
        yoffset *= _mousesensitivity;

        _yaw += xoffset;
        _pitch += yoffset;

        _yaw = _yaw > 360.f || _yaw < -360.f ? yoffset : _yaw;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (_pitch > 89.0f)
            _pitch = 89.0f;
        if (_pitch < -89.0f)
            _pitch = -89.0f;

        // Update Front, Right and Up Vectors using the updated Eular angles
        updatecameravectors();
    }
}

// Calculates the front vector from the Camera's (updated) Eular Angles
void EulerCamera::updatecameravectors() {
    // Calculate the new Front vector
    QVector3D front;
    front.setX(qCos(qDegreesToRadians(_yaw)) * qCos(qDegreesToRadians(_pitch)));
    front.setY(qSin(qDegreesToRadians(_pitch)));
    front.setZ(qSin(qDegreesToRadians(_yaw)) * qCos(qDegreesToRadians(_pitch)));
    _front = front.normalized();
    // Also re-calculate the Right and Up vector
    _right = QVector3D::crossProduct(_front, _worldup).normalized();  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    _up    = QVector3D::crossProduct(_right, _front).normalized();
}

/*------------------------------------------------------------------------------------------------------------------------*/
/*                                            Trackball Camera                                                            */
/*                                  Always rotate around point (0, 0, 0)                                                  */
/*                                      Accumulate error on roll                                                          */
/*------------------------------------------------------------------------------------------------------------------------*/
TrackballCamera::TrackballCamera(QVector3D position, QVector3D up, float yaw, float pitch, float fov, float aspect) :
        Camera(position, up, QVector3D(0.f, 0.f, 0.f), fov, aspect, ZOOM) {
    _worldup = _up;
    _yaw = yaw;
    _pitch = pitch;
    _movementspeed = SPEED;
    _mousesensitivity = SENSITIVTY;
    _mol_speed = Mol_SPEED;
    updatecameravectors();
}

TrackballCamera::~TrackballCamera() = default;


// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void TrackballCamera::processkeyboard(Camera_Movement direction, float deltaTime) {
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void TrackballCamera::processmousemovement(int button, float xpos, float ypos) {

    if(button == 1) {
        float xoffset = xpos - _mousestartx;
        float yoffset = _mousestarty - ypos;
        _mousestartx = xpos;
        _mousestarty = ypos;
        xoffset *= _mousesensitivity;
        yoffset *= _mousesensitivity;

        _yaw += xoffset;
        _pitch += yoffset;

        _pitch = _pitch > 360.f || _pitch < -360.f ? yoffset : _pitch;
        _yaw = _yaw > 360.f || _yaw < -360.f ? yoffset : _yaw;

        updatecameravectors();
    }
}

// Calculates the front vector from the Camera's (updated) Eular Angles
void TrackballCamera::updatecameravectors() {
    // Calculate the new Front vector
    QVector3D front;
    front.setX(qCos(qDegreesToRadians(_yaw)) * qCos(qDegreesToRadians(_pitch)));
    front.setY(qSin(qDegreesToRadians(_pitch)));
    front.setZ(qSin(qDegreesToRadians(_yaw)) * qCos(qDegreesToRadians(_pitch)));
    _front = front.normalized();
    // Also re-calculate the Right and Up vector
    _right = QVector3D::crossProduct(_front, _worldup).normalized();  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    _up = QVector3D::crossProduct(_right, _front).normalized();
    _up = qCos(qDegreesToRadians(_pitch)) > 0.0 ? _up : -_up;

    float dist = _position.length();
    _position =  _front * -dist;
}

void TrackballCamera::processmousescroll(float delta) {
    _position += _mol_speed*delta*_front;
}
