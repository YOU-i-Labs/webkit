/*
 * Copyright (C) 2007-2017 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include <wtf/FileSystem.h>

#include <wtf/EnumTraits.h>
#include <wtf/FileMetadata.h>
#include <wtf/text/CString.h>
#include <wtf/text/StringBuilder.h>
#include <wtf/text/WTFString.h>

namespace WTF {

namespace FileSystemImpl {

bool fileExists(const String& path)
{
    return false;
}

bool deleteFile(const String& path)
{
    return false;
}

PlatformFileHandle openFile(const String& path, FileOpenMode mode)
{
    return invalidPlatformFileHandle;
}

void closeFile(PlatformFileHandle& handle)
{
}

long long seekFile(PlatformFileHandle handle, long long offset, FileSeekOrigin origin)
{
    return 0;
}

bool truncateFile(PlatformFileHandle handle, long long offset)
{
    // ftruncate returns 0 to indicate the success.
    return -1;
}

int writeToFile(PlatformFileHandle handle, const char* data, int length)
{
    return -1;
}

int readFromFile(PlatformFileHandle handle, char* data, int length)
{
    return -1;
}

#if USE(FILE_LOCK)
bool lockFile(PlatformFileHandle handle, OptionSet<FileLockMode> lockMode)
{
    return false;
}

bool unlockFile(PlatformFileHandle handle)
{
    return false;
}
#endif

bool deleteEmptyDirectory(const String& path)
{
    return false;
}

bool getFileSize(const String& path, long long& result)
{
    return false;
}

bool getFileSize(PlatformFileHandle handle, long long& result)
{
    return false;
}

Optional<WallTime> getFileCreationTime(const String& path)
{
    return WTF::nullopt;
}

Optional<WallTime> getFileModificationTime(const String& path)
{
    return WTF::nullopt;
}

static FileMetadata::Type toFileMetataType(struct stat fileInfo)
{
    return FileMetadata::Type::File;
}

static Optional<FileMetadata> fileMetadataUsingFunction(const String& path, int (*statFunc)(const char*, struct stat*))
{
    return WTF::nullopt;
}

Optional<FileMetadata> fileMetadata(const String& path)
{
    return WTF::nullopt;
}

Optional<FileMetadata> fileMetadataFollowingSymlinks(const String& path)
{
    return WTF::nullopt;
}

bool createSymbolicLink(const String& targetPath, const String& symbolicLinkPath)
{
    return false;
}

String pathByAppendingComponent(const String& path, const String& component)
{
    if (path.endsWith('/'))
        return path + component;
    return path + "/" + component;
}

String pathByAppendingComponents(StringView path, const Vector<StringView>& components)
{
    StringBuilder builder;
    builder.append(path);
    for (auto& component : components)
        builder.append('/', component);
    return builder.toString();
}

bool makeAllDirectories(const String& path)
{
    return false;
}

String pathGetFileName(const String& path)
{
    return path.substring(path.reverseFind('/') + 1);
}

String directoryName(const String& path)
{
    return String();
}

Vector<String> listDirectory(const String& path, const String& filter)
{
    Vector<String> entries;
    return entries;
}

String stringFromFileSystemRepresentation(const char* path)
{
    if (!path)
        return String();

    return String::fromUTF8(path);
}

CString fileSystemRepresentation(const String& path)
{
    return path.utf8();
}

bool moveFile(const String& oldPath, const String& newPath)
{
    return false;
}

bool getVolumeFreeSpace(const String& path, uint64_t& freeSpace)
{
    freeSpace = 0;
    return false;
}

String openTemporaryFile(const String& prefix, PlatformFileHandle& handle)
{
    handle = invalidPlatformFileHandle;
    return String();
}

bool hardLink(const String& source, const String& destination)
{
    return false;
}

bool hardLinkOrCopyFile(const String& source, const String& destination)
{
    return false;
}

Optional<int32_t> getFileDeviceId(const CString& fsFile)
{
    return WTF::nullopt;
}

String realPath(const String& filePath)
{
    return filePath;
}

} // namespace FileSystemImpl
} // namespace WTF
