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
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __TrenchBroom__ModelFilter__
#define __TrenchBroom__ModelFilter__

#include "Model/ModelTypes.h"

namespace TrenchBroom {
    namespace Model {
        class ModelFilter {
        public:
            virtual bool visible(const Object* object) const;
            virtual bool visible(const BrushFace* face) const;
            virtual bool pickable(const Object* object) const;
            virtual bool pickable(const BrushFace* face) const;
            virtual bool selectable(const Object* object) const;
            virtual bool selectable(const BrushFace* face) const;
        };
    }
}

#endif /* defined(__TrenchBroom__ModelFilter__) */