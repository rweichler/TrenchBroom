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

#include <gtest/gtest.h>

#include "BBox.h"
#include "Vec.h"

#include "TestUtils.h"

TEST(BBoxTest, ConstructBBox3fWithDefaults) {
    const BBox3f bounds;
    ASSERT_EQ(Vec3f::Null, bounds.min);
    ASSERT_EQ(Vec3f::Null, bounds.max);
}

TEST(BBoxTest, ConstructBBox3fWithMinAndMax) {
    const Vec3f min(-1.0f, -2.0f, -3.0f);
    const Vec3f max( 1.0f,  2.0f,  3.0f);
    
    const BBox3f bounds(min, max);
    ASSERT_EQ(min, bounds.min);
    ASSERT_EQ(max, bounds.max);
}

TEST(BBoxTest, ConstructBBox3fWithCenterAndSize) {
    const Vec3f center(-1.0f, -2.0f, -3.0f);
    const float size = 12.32323f;
    
    const BBox3f bounds(center, size);
    ASSERT_VEC_EQ(Vec3f(center.x() - size, center.y() - size, center.z() - size), bounds.min);
    ASSERT_VEC_EQ(Vec3f(center.x() + size, center.y() + size, center.z() + size), bounds.max);
}

TEST(BBoxTest, OperatorEquals) {
    const Vec3f min(-1.0f, -2.0f, -3.0f);
    const Vec3f max( 1.0f,  2.0f,  3.0f);
    
    const BBox3f bounds1(min, max);
    const BBox3f bounds2(min, max);

    ASSERT_TRUE(bounds1 == bounds2);
}

TEST(BBoxTest, GetCenter) {
    const Vec3f min(-1.0f, -2.0f, -3.0f);
    const Vec3f max( 1.0f,  3.0f,  5.0f);
    const BBox3f bounds(min, max);
    
    ASSERT_EQ(Vec3f(0.0f, 0.5f, 1.0f), bounds.center());
}

TEST(BBoxTest, GetSize) {
    const Vec3f min(-1.0f, -2.0f, -3.0f);
    const Vec3f max( 1.0f,  3.0f,  5.0f);
    const BBox3f bounds(min, max);
    
    ASSERT_EQ(Vec3f(2.0f, 5.0f, 8.0f), bounds.size());
}

TEST(BBoxTest, GetVertex) {
    const Vec3f min(-1.0f, -2.0f, -3.0f);
    const Vec3f max( 1.0f,  3.0f,  5.0f);
    const BBox3f bounds(min, max);

    ASSERT_VEC_EQ(Vec3f(-1.0f, -2.0f, -3.0f), bounds.vertex(BBox3f::Min, BBox3f::Min, BBox3f::Min));
    ASSERT_VEC_EQ(Vec3f(-1.0f, -2.0f,  5.0f), bounds.vertex(BBox3f::Min, BBox3f::Min, BBox3f::Max));
    ASSERT_VEC_EQ(Vec3f(-1.0f,  3.0f, -3.0f), bounds.vertex(BBox3f::Min, BBox3f::Max, BBox3f::Min));
    ASSERT_VEC_EQ(Vec3f(-1.0f,  3.0f,  5.0f), bounds.vertex(BBox3f::Min, BBox3f::Max, BBox3f::Max));
    ASSERT_VEC_EQ(Vec3f( 1.0f, -2.0f, -3.0f), bounds.vertex(BBox3f::Max, BBox3f::Min, BBox3f::Min));
    ASSERT_VEC_EQ(Vec3f( 1.0f, -2.0f,  5.0f), bounds.vertex(BBox3f::Max, BBox3f::Min, BBox3f::Max));
    ASSERT_VEC_EQ(Vec3f( 1.0f,  3.0f, -3.0f), bounds.vertex(BBox3f::Max, BBox3f::Max, BBox3f::Min));
    ASSERT_VEC_EQ(Vec3f( 1.0f,  3.0f,  5.0f), bounds.vertex(BBox3f::Max, BBox3f::Max, BBox3f::Max));
}

TEST(BBoxTest, MergeWithBBox) {
    BBox3f bounds1(Vec3f(-12.0f, -3.0f, 4.0f), Vec3f(7.0f, 8.0f, 9.0f));
    const BBox3f bounds2(Vec3f(-10.0f, -5.0f, 3.0f), Vec3f(9.0f, 9.0f, 5.0f));
    const BBox3f merged( Vec3f(-12.0f, -5.0f, 3.0f), Vec3f(9.0f, 9.0f, 9.0f));
    
    ASSERT_EQ(merged, bounds1.mergeWith(bounds2));
}

TEST(BBoxTest, MergedWithBBox) {
    const BBox3f bounds1(Vec3f(-12.0f, -3.0f, 4.0f), Vec3f(7.0f, 8.0f, 9.0f));
    const BBox3f bounds2(Vec3f(-10.0f, -5.0f, 3.0f), Vec3f(9.0f, 9.0f, 5.0f));
    const BBox3f merged( Vec3f(-12.0f, -5.0f, 3.0f), Vec3f(9.0f, 9.0f, 9.0f));
    
    ASSERT_EQ(merged, bounds1.mergedWith(bounds2));
}

TEST(BBoxTest, MergeWithVec) {
    BBox3f bounds(Vec3f(-12.0f, -3.0f, 4.0f), Vec3f(7.0f, 8.0f,  9.0f));
    const Vec3f  vec(-10.0f, -6.0f, 10.0f);
    const BBox3f merged(Vec3f(-12.0f, -6.0f, 4.0f), Vec3f(7.0f, 8.0f, 10.0f));

    ASSERT_EQ(merged, bounds.mergeWith(vec));
}

TEST(BBoxTest, MergedWithVec) {
    const BBox3f bounds(Vec3f(-12.0f, -3.0f, 4.0f), Vec3f(7.0f, 8.0f,  9.0f));
    const Vec3f  vec(-10.0f, -6.0f, 10.0f);
    const BBox3f merged(Vec3f(-12.0f, -6.0f, 4.0f), Vec3f(7.0f, 8.0f, 10.0f));
    
    ASSERT_EQ(merged, bounds.mergedWith(vec));
}

TEST(BBoxTest, TranslateToOrigin) {
          BBox3f bounds    (Vec3f(-12.0f, -3.0f,  4.0f), Vec3f( 8.0f, 9.0f, 8.0f));
    const BBox3f translated(Vec3f(-10.0f, -6.0f, -2.0f), Vec3f(10.0f, 6.0f, 2.0f));
    ASSERT_EQ(translated, bounds.translateToOrigin());
}

TEST(BBoxTest, TranslatedToOrigin) {
    const BBox3f bounds    (Vec3f(-12.0f, -3.0f,  4.0f), Vec3f( 8.0f, 9.0f, 8.0f));
    const BBox3f translated(Vec3f(-10.0f, -6.0f, -2.0f), Vec3f(10.0f, 6.0f, 2.0f));
    ASSERT_EQ(translated, bounds.translatedToOrigin());
}

TEST(BBoxTest, Repair) {
          BBox3f bounds  (Vec3f( 3.0f, 4.0f, 0.0f), Vec3f(-1.0f, 0.0f, 1.0f));
    const BBox3f repaired(Vec3f(-1.0f, 0.0f, 0.0f), Vec3f( 3.0f, 4.0f, 1.0f));
    ASSERT_EQ(repaired, bounds.repair());
}

TEST(BBoxTest, Repaired) {
    const BBox3f bounds  (Vec3f( 3.0f, 4.0f, 0.0f), Vec3f(-1.0f, 0.0f, 1.0f));
    const BBox3f repaired(Vec3f(-1.0f, 0.0f, 0.0f), Vec3f( 3.0f, 4.0f, 1.0f));
    ASSERT_EQ(repaired, bounds.repaired());
}

TEST(BBoxTest, ContainsPoint) {
    const BBox3f bounds(Vec3f(-12.0f, -3.0f,  4.0f), Vec3f( 8.0f, 9.0f, 8.0f));
    ASSERT_TRUE(bounds.contains(Vec3f(2.0f, 1.0f, 7.0f)));
    ASSERT_TRUE(bounds.contains(Vec3f(-12.0f, -3.0f, 7.0f)));
    ASSERT_FALSE(bounds.contains(Vec3f(-13.0f, -3.0f, 7.0f)));
}

TEST(BBoxTest, RelativePosition) {
    const BBox3f bounds(Vec3f(-12.0f, -3.0f,  4.0f), Vec3f( 8.0f, 9.0f, 8.0f));
    const Vec3f point1(-1.0f, 0.0f, 0.0f);
    const BBox3f::RelativePosition pos1 = bounds.relativePosition(point1);
    ASSERT_EQ(BBox3f::RelativePosition::Within, pos1[0]);
    ASSERT_EQ(BBox3f::RelativePosition::Within, pos1[1]);
    ASSERT_EQ(BBox3f::RelativePosition::Less,   pos1[2]);
}

TEST(BBoxTest, ContainsBBox) {
    const BBox3f bounds1(Vec3f(-12.0f, -3.0f,  4.0f), Vec3f( 8.0f, 9.0f, 8.0f));
    const BBox3f bounds2(Vec3f(-10.0f, -2.0f,  5.0f), Vec3f( 7.0f, 8.0f, 7.0f));
    const BBox3f bounds3(Vec3f(-13.0f, -2.0f,  5.0f), Vec3f( 7.0f, 8.0f, 7.0f));
    ASSERT_TRUE(bounds1.contains(bounds1));
    ASSERT_TRUE(bounds1.contains(bounds2));
    ASSERT_FALSE(bounds1.contains(bounds3));
}

TEST(BBoxTest, IntersectsBBox) {
    const BBox3f bounds1(Vec3f(-12.0f, -3.0f,  4.0f), Vec3f(  8.0f,  9.0f,  8.0f));
    const BBox3f bounds2(Vec3f(-10.0f, -2.0f,  5.0f), Vec3f(  7.0f,  8.0f,  7.0f));
    const BBox3f bounds3(Vec3f(-13.0f, -2.0f,  5.0f), Vec3f(  7.0f,  8.0f,  7.0f));
    const BBox3f bounds4(Vec3f(-15.0f, 10.0f,  9.0f), Vec3f(-13.0f, 12.0f, 10.0f));
    const BBox3f bounds5(Vec3f(-15.0f, 10.0f,  9.0f), Vec3f(-12.0f, 12.0f, 10.0f));
    ASSERT_TRUE(bounds1.intersects(bounds1));
    ASSERT_TRUE(bounds1.intersects(bounds2));
    ASSERT_TRUE(bounds1.intersects(bounds3));
    ASSERT_FALSE(bounds1.intersects(bounds4));
    ASSERT_FALSE(bounds1.intersects(bounds5));
}

TEST(BBoxTest, Expand) {
          BBox3f bounds  (Vec3f(-12.0f, -3.0f,  4.0f), Vec3f( 8.0f,  9.0f,  8.0f));
    const BBox3f expanded(Vec3f(-14.0f, -5.0f,  2.0f), Vec3f(10.0f, 11.0f, 10.0f));
    ASSERT_EQ(expanded, bounds.expand(2.0f));
}

TEST(BBoxTest, Expanded) {
    const BBox3f bounds  (Vec3f(-12.0f, -3.0f,  4.0f), Vec3f( 8.0f,  9.0f,  8.0f));
    const BBox3f expanded(Vec3f(-14.0f, -5.0f,  2.0f), Vec3f(10.0f, 11.0f, 10.0f));
    ASSERT_EQ(expanded, bounds.expanded(2.0f));
}

TEST(BBoxTest, Translate) {
          BBox3f bounds    (Vec3f(-12.0f, -3.0f,  4.0f), Vec3f( 8.0f, 9.0f, 8.0f));
    const BBox3f translated(Vec3f(-10.0f, -4.0f,  1.0f), Vec3f(10.0f, 8.0f, 5.0f));
    ASSERT_EQ(translated, bounds.translate(Vec3f(2.0f, -1.0f, -3.0f)));
}

TEST(BBoxTest, Translated) {
    const BBox3f bounds    (Vec3f(-12.0f, -3.0f,  4.0f), Vec3f( 8.0f, 9.0f, 8.0f));
    const BBox3f translated(Vec3f(-10.0f, -4.0f,  1.0f), Vec3f(10.0f, 8.0f, 5.0f));
    ASSERT_EQ(translated, bounds.translated(Vec3f(2.0f, -1.0f, -3.0f)));
}
