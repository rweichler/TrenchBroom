/*
 Copyright (C) 2010-2013 Kristian Duske
 
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

#include "Grid.h"

#include "Model/BrushFace.h"

namespace TrenchBroom {
    namespace View {
        Grid::Grid(const size_t size) :
        m_size(size),
        m_snap(true),
        m_visible(true) {}

        size_t Grid::size() const {
            return m_size;
        }
        
        void Grid::setSize(const size_t size) {
            assert(size <= MaxSize);
            m_size = size;
        }
        
        void Grid::incSize() {
            if (m_size < MaxSize)
                ++m_size;
        }
        
        void Grid::decSize() {
            if (m_size > 0)
                --m_size;
        }
        
        size_t Grid::actualSize() const {
            if (snap())
                return 1 << m_size;
            return 1;
        }
        
        FloatType Grid::angle() const {
            return static_cast<FloatType>(15.0);
        }
        
        bool Grid::visible() const {
            return m_visible;
        }
        
        void Grid::toggleVisible() {
            m_visible = !m_visible;
        }
        
        bool Grid::snap() const {
            return m_snap;
        }
        
        void Grid::toggleSnap() {
            m_snap = !m_snap;
        }
        
        FloatType Grid::snap(const FloatType f) const {
            if (!snap())
                return f;
            const size_t actSize = actualSize();
            return actSize * Math::round(f / actSize);
        }
        
        FloatType Grid::snapAngle(const FloatType a) const {
            if (!snap())
                return a;
            return angle() * Math::round(a / angle());
        }
        
        FloatType Grid::snapUp(const FloatType f, bool skip) const {
            if (!snap())
                return f;
            const size_t actSize = actualSize();
            const FloatType s = actSize * std::ceil(f / actSize);
            if (skip && s == f)
                return s + actualSize();
            return s;
        }
        
        FloatType Grid::snapDown(const FloatType f, bool skip) const {
            if (!snap())
                return f;
            const size_t actSize = actualSize();
            const FloatType s = actSize * std::floor(f / actSize);
            if (skip && s == f)
                return s - actualSize();
            return s;
        }
        
        FloatType Grid::offset(const FloatType f) const {
            if (!snap())
                return static_cast<FloatType>(0.0);
            return f - snap(f);
        }
        
        Vec3 Grid::snap(const Vec3& p) const {
            if (!snap())
                return p;
            return Vec3(snap(p.x()),
                        snap(p.y()),
                        snap(p.z()));
        }
        
        Vec3 Grid::snapUp(const Vec3& p, const bool skip) const {
            if (!snap())
                return p;
            return Vec3(snapUp(p.x()),
                        snapUp(p.y()),
                        snapUp(p.z()));
        }
        
        Vec3 Grid::snapDown(const Vec3& p, const bool skip) const {
            if (!snap())
                return p;
            return Vec3(snapDown(p.x()),
                        snapDown(p.y()),
                        snapDown(p.z()));
        }
        
        Vec3 Grid::snapTowards(const Vec3& p, const Vec3& d, const bool skip) const {
            if (!snap())
                return p;
            Vec3 result;
            if (    Math::pos(d.x()))    result[0] = snapUp(p.x(), skip);
            else if(Math::neg(d.x()))    result[0] = snapDown(p.x(), skip);
            else                                    result[0] = snap(p.x());
            if (    Math::pos(d.y()))    result[1] = snapUp(p.y(), skip);
            else if(Math::neg(d.y()))    result[1] = snapDown(p.y(), skip);
            else                                    result[1] = snap(p.y());
            if (    Math::pos(d.z()))    result[2] = snapUp(p.z(), skip);
            else if(Math::neg(d.z()))    result[2] = snapDown(p.z(), skip);
            else                                    result[2] = snap(p.z());
            return result;
        }
        
        Vec3 Grid::offset(const Vec3& p) const {
            if (!snap())
                return Vec3::Null;
            return p - snap(p);
        }

        Vec3 Grid::snap(const Vec3& p, const Plane3& onPlane) const {
            Vec3 result;
            switch(onPlane.normal.firstComponent()) {
                case Math::Axis::AX:
                    result[1] = snap(p.y());
                    result[2] = snap(p.z());
                    result[0] = onPlane.xAt(result.yz());
                    break;
                case Math::Axis::AY:
                    result[0] = snap(p.x());
                    result[2] = snap(p.z());
                    result[1] = onPlane.yAt(result.xz());
                    break;
                case Math::Axis::AZ:
                    result[0] = snap(p.x());
                    result[1] = snap(p.y());
                    result[2] = onPlane.zAt(result.xy());
                    break;
            }
            return result;
        }
        
        FloatType Grid::intersectWithRay(const Ray3& ray, const size_t skip) const {
            Vec3 planeAnchor;
            
            for (size_t i = 0; i < 3; ++i)
                planeAnchor[i] = ray.direction[i] > 0.0 ? snapUp(ray.origin[i], true) + skip * actualSize() : snapDown(ray.origin[i], true) - skip * actualSize();
            
            const FloatType distX = Plane3(planeAnchor, Vec3::PosX).intersectWithRay(ray);
            const FloatType distY = Plane3(planeAnchor, Vec3::PosY).intersectWithRay(ray);
            const FloatType distZ = Plane3(planeAnchor, Vec3::PosZ).intersectWithRay(ray);
            
            FloatType dist = distX;
            if (!Math::isnan(distY) && (Math::isnan(dist) || std::abs(distY) < std::abs(dist)))
                dist = distY;
            if (!Math::isnan(distZ) && (Math::isnan(dist) || std::abs(distZ) < std::abs(dist)))
                dist = distZ;
            return dist;
        }
        
        Vec3 Grid::moveDeltaForPoint(const Vec3& point, const BBox3& worldBounds, const Vec3& delta) const {
            const Vec3 newPoint = snap(point + delta);
            Vec3 actualDelta = newPoint - point;

            for (size_t i = 0; i < 3; ++i)
                if ((actualDelta[i] > 0.0) != (delta[i] > 0.0))
                    actualDelta[i] = 0.0;
            return actualDelta;
        }
        
        Vec3 Grid::moveDeltaForBounds(const Model::BrushFace& face, const BBox3& bounds, const BBox3& worldBounds, const Ray3& ray, const Vec3& position) const {
            const Plane3 dragPlane = alignedOrthogonalDragPlane(position, face.boundary().normal);
            
            const Vec3 halfSize = bounds.size() * 0.5;
            FloatType offsetLength = halfSize.dot(dragPlane.normal);
            if (offsetLength < 0.0)
                offsetLength = -offsetLength;
            const Vec3 offset = dragPlane.normal * offsetLength;
            
            const FloatType dist = dragPlane.intersectWithRay(ray);
            const Vec3 newPos = ray.pointAtDistance(dist);
            Vec3 delta = moveDeltaForPoint(bounds.center(), worldBounds, newPos - (bounds.center() - offset));
            
            const Math::Axis::Type a = dragPlane.normal.firstComponent();
            if (dragPlane.normal[a] > 0.0) delta[a] = position[a] - bounds.min[a];
            else delta[a] = position[a] - bounds.max[a];
            
            return delta;
        }
        
        Vec3 Grid::moveDelta(const BBox3& bounds, const BBox3& worldBounds, const Vec3& delta) const {
            Vec3 actualDelta = Vec3::Null;
            for (size_t i = 0; i < 3; ++i) {
                if (!Math::zero(delta[i])) {
                    const FloatType low  = snap(bounds.min[i] + delta[i]) - bounds.min[i];
                    const FloatType high = snap(bounds.max[i] + delta[i]) - bounds.max[i];
                    
                    if (low != 0.0 && high != 0.0)
                        actualDelta[i] = std::abs(high) < std::abs(low) ? high : low;
                    else if (low != 0.0)
                        actualDelta[i] = low;
                    else if (high != 0.0)
                        actualDelta[i] = high;
                    else
                        actualDelta[i] = 0.0;
                }
            }
            
            if (delta.squaredLength() < (delta - actualDelta).squaredLength())
                actualDelta = Vec3::Null;
            return actualDelta;
        }
        
        Vec3 Grid::moveDelta(const Vec3& point, const BBox3& worldBounds, const Vec3& delta) const {
            Vec3 actualDelta = Vec3::Null;
            for (size_t i = 0; i < 3; ++i)
                if (!Math::zero(delta[i]))
                    actualDelta[i] = snap(point[i] + delta[i]) - point[i];
            
            if (delta.squaredLength() < (delta - actualDelta).squaredLength())
                actualDelta = Vec3::Null;
            
            return actualDelta;
        }
        
        Vec3 Grid::moveDelta(const Vec3& delta) const {
            Vec3 actualDelta = Vec3::Null;
            for (unsigned int i = 0; i < 3; i++)
                if (!Math::zero(delta[i]))
                    actualDelta[i] = snap(delta[i]);
            
            if (delta.squaredLength() < (delta - actualDelta).squaredLength())
                actualDelta = Vec3::Null;
            
            return actualDelta;
        }
        
        Vec3 Grid::combineDeltas(const Vec3& delta1, const Vec3& delta2) const {
            if (delta1.squaredLength() < delta2.squaredLength())
                return delta1;
            return delta2;
        }
        
        Vec3 Grid::referencePoint(const BBox3& bounds) {
            return snap(bounds.center());
        }
    }
}