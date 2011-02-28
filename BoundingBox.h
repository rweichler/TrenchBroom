//
//  BoundingBox.h
//  TrenchBroom
//
//  Created by Kristian Duske on 28.01.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class Vector3f;

@interface BoundingBox : NSObject {
    @private
    Vector3f* min;
    Vector3f* max;
    Vector3f* size;
}

- (id)initWithMin:(Vector3f *)theMin max:(Vector3f *)theMax;
- (id)initWithBounds:(BoundingBox *)theBounds;

- (Vector3f *)min;
- (Vector3f *)max;

- (Vector3f *)size;

- (void)merge:(BoundingBox *)theBounds;

@end
