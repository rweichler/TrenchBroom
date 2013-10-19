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

#include "WadTextureLoader.h"

#include "ByteBuffer.h"
#include "Color.h"
#include "Exceptions.h"
#include "Renderer/GL.h"
#include "IO/Wad.h"
#include "Assets/Palette.h"
#include "Assets/FaceTexture.h"
#include "Assets/FaceTextureCollection.h"

namespace TrenchBroom {
    namespace IO {
        WadTextureLoader::WadTextureLoader(const Assets::Palette& palette) :
        m_palette(palette) {}

        Assets::FaceTextureCollection* WadTextureLoader::doLoadTextureCollection(const Path& path) {
            Wad wad(path);
            const WadEntryList mipEntries = wad.entriesWithType(WadEntryType::WEMip);
            const size_t textureCount = mipEntries.size();

            Assets::FaceTextureList textures;
            textures.reserve(textureCount);

            for (size_t i = 0; i < textureCount; ++i) {
                const WadEntry& entry = mipEntries[i];
                const MipSize mipSize = wad.mipSize(entry);
                textures.push_back(new Assets::FaceTexture(entry.name(), mipSize.width, mipSize.height));
            }
            
            return new Assets::FaceTextureCollection(path, textures);
        }

        void WadTextureLoader::doUploadTextureCollection(Assets::FaceTextureCollection* collection) {
            Buffer<unsigned char> buffer(InitialBufferSize);
            Color averageColor;
            
            const IO::Path& path = collection->path();
            Wad wad(path);

            const WadEntryList mipEntries = wad.entriesWithType(WadEntryType::WEMip);
            const Assets::FaceTextureList& textures = collection->textures();
            
            if (mipEntries.size() != textures.size())
                throw AssetException("Found different number of textures in " + path.asString() + " while uploading mip data");
            
            const size_t textureCount = mipEntries.size();
            
            
            typedef std::vector<GLuint> TextureIdList;
            TextureIdList textureIds;
            textureIds.resize(textureCount);
            glEnable(GL_TEXTURE_2D);
            glGenTextures(textureCount, &textureIds[0]);

            for (size_t i = 0; i < textureCount; ++i) {
                const WadEntry& entry = mipEntries[i];
                Assets::FaceTexture* texture = textures[i];
                assert(entry.name() == texture->name());
                
                if (texture->width() * texture->height() > buffer.size())
                    buffer = Buffer<unsigned char>(texture->width() * texture->height());
                
                const GLuint textureId = textureIds[i];
                texture->setTextureId(textureId);

                glBindTexture(GL_TEXTURE_2D, textureId);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                
                for (size_t j = 0; j < 4; ++j) {
                    const MipData mipData = wad.mipData(entry, j);
                    m_palette.indexedToRgb(mipData.begin, mipData.end - mipData.begin, buffer, averageColor);
                    if (j == 0)
                        texture->setAverageColor(averageColor);

                    const size_t divisor = 1 << j;
                    glTexImage2D(GL_TEXTURE_2D, j, GL_RGBA,
                                 static_cast<GLsizei>(texture->width() / divisor),
                                 static_cast<GLsizei>(texture->height() / divisor),
                                 0, GL_RGB, GL_UNSIGNED_BYTE, &buffer[0]);
                }
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }
    }
}