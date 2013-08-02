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

#ifndef __TrenchBroom__HitFilters__
#define __TrenchBroom__HitFilters__

#include "SharedPointer.h"
#include "Model/Picker.h"

namespace TrenchBroom {
    namespace Model {
        class HitFilterChain : public HitFilter {
        private:
            typedef std::tr1::shared_ptr<HitFilter> FilterPtr;
            FilterPtr m_filter;
            FilterPtr m_next;
        public:
            template <class F, class N>
            HitFilterChain(const F& filter, const N& next) :
            m_filter(new F(filter)),
            m_next(new N(next)) {}
            
            bool matches(const Hit& hit) const;
        };
        
        template <class F1, class F2>
        inline HitFilterChain chainHitFilters(const F1& f1, const F2& f2) {
            return HitFilterChain(f1, f2);
        }
        
        template <class F1, class F2, class F3>
        inline HitFilterChain chainHitFilters(const F1& f1, const F2& f2, const F3& f3) {
            return chainHitFilters(f1, chainHitFilters(f2, f3));
        }
        
        class TypedHitFilter : public HitFilter {
        private:
            Hit::HitType m_typeMask;
        public:
            TypedHitFilter(const Hit::HitType typeMask);
            bool matches(const Hit& hit) const;
        };

        class Filter;
        class DefaultHitFilter : public HitFilter {
        private:
            const Filter& m_filter;
        public:
            DefaultHitFilter(const Filter& filter);
            bool matches(const Hit& hit) const;
        };
    }
}

#endif /* defined(__TrenchBroom__DefaultHitFilter__) */
