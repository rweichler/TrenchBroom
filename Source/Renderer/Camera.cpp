/*
 Copyright (C) 2010-2012 Kristian Duske
 
 This file is part of TrenchBroom.
 
 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with TrenchBroom.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Camera.h"

#include <algorithm>

namespace TrenchBroom {
    namespace Renderer {
        void Camera::validate() const {
            float vFrustum = static_cast<float>(tan(m_fieldOfVision * Math::Pi / 360.0)) * 0.75f * m_nearPlane;
            float hFrustum = vFrustum * m_viewport.width / m_viewport.height;
            
            float depth = m_farPlane - m_nearPlane;
            m_matrix.set(m_nearPlane / hFrustum,    0.0f,                   0.0f,                                   0.0f,
                         0.0f,                      m_nearPlane / vFrustum, 0.0f,                                   0.0f,
                         0.0f,                      0.0f,                   -(m_farPlane + m_nearPlane) / depth,    -2.0f * (m_farPlane * m_nearPlane) / depth,
                         0.0f,                      0.0f,                   -1.0f,                                  0.0f);
            
            const Vec3f& f = m_direction;
            Vec3f s = f.crossed(m_up);
            Vec3f u = s.crossed(f);
            
            Mat4f modelView( s.x,  s.y,  s.z, 0.0f,
                             u.x,  u.y,  u.z, 0.0f,
                            -f.x, -f.y, -f.z, 0.0f,
                            0.0f, 0.0f, 0.0f, 1.0f);
            modelView.translate(-1.0f * m_position);
            
            m_matrix *= modelView;
            
            bool invertible;
            m_invertedMatrix = m_matrix.inverted(invertible);
            assert(invertible);
        }
        
        Camera::Camera(float fieldOfVision, float nearPlane, float farPlane, const Vec3f& position, const Vec3f& direction) :
        m_fieldOfVision(fieldOfVision),
        m_nearPlane(nearPlane),
        m_farPlane(farPlane),
        m_position(position),
        m_direction(direction),
        m_valid(false) {
            if (m_direction.equals(Vec3f::PosZ)) {
                m_right = Vec3f::NegY;
                m_up = Vec3f::NegX;
            } else if (m_direction.equals(Vec3f::NegZ)) {
                m_right = Vec3f::NegY;
                m_up = Vec3f::PosX;
            } else {
                m_right = m_direction.crossed(Vec3f::PosZ);
                m_up = m_right.crossed(m_direction);
            }
        }
        
        void Camera::update(int x, int y, int width, int height) {
            setViewport(x, y, width, height);
            
            if (!m_valid)
                validate();
            
            glViewport(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
            glMatrixMode(GL_PROJECTION);
            glLoadMatrixf(m_matrix.v);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        }

        const Vec3f Camera::defaultPoint() const {
            return defaultPoint(m_direction);
        }
        
        const Vec3f Camera::defaultPoint(const Vec3f& direction) const {
            return m_position + direction * 256.0f;
        }

        const Vec3f Camera::defaultPoint(float x, float y) const {
            const Vec3f point = unproject(x, y, 0.5f);
            return defaultPoint((point - m_position).normalized());
        }

        const Vec3f Camera::project(const Vec3f& point) const {
            if (!m_valid)
                validate();
            
            Vec3f win = m_matrix * point;
            win.x = m_viewport.x + (m_viewport.width  * (win.x + 1.0f)) / 2.0f;
            win.y = m_viewport.y + (m_viewport.height * (win.y + 1.0f)) / 2.0f;
            win.z = (win.z + 1.0f) / 2.0f;
            return win;
        }

        const Vec3f Camera::unproject(float x, float y, float depth) const {
            if (!m_valid)
                validate();
            
            Vec3f normalized;
            normalized.x = 2.0f * (x - m_viewport.x) / m_viewport.width  - 1.0f;
            normalized.y = 2.0f * (m_viewport.height - y - m_viewport.y) / m_viewport.height - 1.0f;
            normalized.z = 2.0f * depth - 1.0f;
            
            return m_invertedMatrix * normalized;
        }

        const Ray Camera::pickRay(float x, float y) const {
            Vec3f direction = (unproject(x, y, 0.5f) - m_position).normalized();
            return Ray(m_position, direction);
        }

        const Mat4f& Camera::matrix() const {
            return m_matrix;
        }

        const Mat4f Camera::billboardMatrix() const {
            Vec3f bbLook = m_direction * -1.0f;
            Vec3f bbUp = m_up;
            Vec3f bbRight = bbUp.crossed(bbLook);

            return Mat4f(bbRight.x,  bbUp.x,     bbLook.x,   0.0f,
                         bbRight.y,  bbUp.y,     bbLook.y,   0.0f,
                         bbRight.z,  bbUp.z,     bbLook.z,   0.0f,
                         0.0f,       0.0f,       0.0f,       1.0f);
        }

        float Camera::distanceTo(const Vec3f& point) const {
            return sqrt(squaredDistanceTo(point));
        }
        
        float Camera::squaredDistanceTo(const Vec3f& point) const {
            return (point - m_position).lengthSquared();
        }

        void Camera::moveTo(Vec3f position) {
            m_position = position;
        }
        
        void Camera::moveBy(float forward, float right, float up) {
            m_position += m_direction * forward;
            m_position += m_right * right;
            m_position += m_up * up;
        }
        
        void Camera::lookAt(Vec3f point, Vec3f up) {
            setDirection((point - m_position).normalized(), up);
        }
        
        void Camera::setDirection(Vec3f direction, Vec3f up) {
            m_direction = direction;
            m_right = (m_direction.crossed(up)).normalized();
            m_up = m_right.crossed(m_direction);
        }
        
        void Camera::rotate(float yawAngle, float pitchAngle) {
            if (yawAngle == 0.0f && pitchAngle == 0.0f) return;
            
            Quat rotation = Quat(yawAngle, Vec3f::PosZ) * Quat(pitchAngle, m_right);
            Vec3f newDirection = rotation * m_direction;
            Vec3f newUp = rotation * m_up;

            if (newUp.z < 0.0f) {
                newUp.z = 0.0f;
                newDirection.x = 0.0f;
                newDirection.y = 0.0f;
            }
            
            setDirection(newDirection, newUp);
        }
        
        void Camera::orbit(Vec3f center, float hAngle, float vAngle) {
            if (hAngle == 0.0f && vAngle == 0.0f) return;
            
            Quat rotation = Quat(hAngle, Vec3f::PosZ) * Quat(vAngle, m_right);
            Vec3f newDirection = rotation * m_direction;
            Vec3f newUp = rotation * m_up;
            Vec3f offset = m_position - center;
            
            if (newUp.z < 0.0f) {
                newUp = m_up;
                newDirection.x = 0.0f;
                newDirection.y = 0.0f;
                newDirection.normalize();
                
                // correct rounding errors
                float cos = (std::max)(-1.0f, (std::min)(1.0f, m_direction.dot(newDirection)));
                float angle = acosf(cos);
                if (!Math::zero(angle)) {
                    Vec3f axis = (m_direction.crossed(newDirection)).normalized();
                    rotation = Quat(angle, axis);
                    offset = rotation * offset;
                    newUp = rotation * newUp;
                }
            } else {
                offset = rotation * offset;
            }
            
            setDirection(newDirection, newUp);
            moveTo(offset + center);
        }
    }
}