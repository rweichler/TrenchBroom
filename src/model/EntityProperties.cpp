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

#include "EntityProperties.h"

#include "Exceptions.h"

namespace TrenchBroom {
    namespace Model {
        const EntityPropertyList& EntityProperties::properties() const {
            return m_properties;
        }
        
        void EntityProperties::addOrUpdateProperty(const PropertyKey& key, const PropertyValue& value) {
            EntityPropertyList::iterator it = findProperty(key);
            if (it != m_properties.end())
                it->value = value;
            else
                m_properties.push_back(EntityProperty(key, value));
        }

        bool EntityProperties::hasProperty(const PropertyKey& key) const {
            return findProperty(key) != m_properties.end();
        }

        const PropertyValue* EntityProperties::property(const PropertyKey& key) const {
            EntityPropertyList::const_iterator it = findProperty(key);
            if (it == m_properties.end())
                return NULL;
            return &it->value;
        }

        EntityPropertyList::const_iterator EntityProperties::findProperty(const PropertyKey& key) const {
            EntityPropertyList::const_iterator it, end;
            for (it = m_properties.begin(), end = m_properties.end(); it != end; ++it) {
                const EntityProperty& property = *it;
                if (property.key == key)
                    return it;
            }
            
            return end;
        }
        
        EntityPropertyList::iterator EntityProperties::findProperty(const PropertyKey& key) {
            EntityPropertyList::iterator it, end;
            for (it = m_properties.begin(), end = m_properties.end(); it != end; ++it) {
                const EntityProperty& property = *it;
                if (property.key == key)
                    return it;
            }
            
            return end;
        }
    }
}