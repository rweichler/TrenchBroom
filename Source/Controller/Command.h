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

#ifndef __TrenchBroom__Command__
#define __TrenchBroom__Command__

#include <wx/cmdproc.h>

namespace TrenchBroom {
    namespace Model {
        class MapDocument;
    }
    
    namespace Controller {
        class Command : public wxCommand {
        public:
            typedef enum {
                LoadMap,
                ClearMap,
                ChangeEditState
            } Type;
        private:
            Type m_type;
            Model::MapDocument& m_document;
        protected:
            inline Model::MapDocument& document() const {
                return m_document;
            }
        public:
            Command(Type type, Model::MapDocument& document, bool undoable, const wxString& name) :
            wxCommand(undoable, name),
            m_type(type),
            m_document(document) {}
            
            inline Type type() const {
                return m_type;
            }
            
            bool Do() {
                return true;
            }
            
            bool Undo() {
                return true;
            }
        };
    }
}

#endif /* defined(__TrenchBroom__Command__) */
