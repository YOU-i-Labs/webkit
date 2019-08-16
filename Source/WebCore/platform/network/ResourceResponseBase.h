/*
 * Copyright (C) 2006, 2008, 2016 Apple Inc. All rights reserved.
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#pragma once

#include "CacheValidation.h"
#include "CertificateInfo.h"
#include "HTTPHeaderMap.h"
#include "NetworkLoadMetrics.h"
#include "ParsedContentRange.h"
#include "URL.h"
#include <wtf/SHA1.h>

namespace WebCore {

class ResourceResponse;

#if ENABLE(NOSNIFF)
bool isScriptAllowedByNosniff(const ResourceResponse&);
#endif

// Do not use this class directly, use the class ResponseResponse instead
class ResourceResponseBase {
    WTF_MAKE_FAST_ALLOCATED;
public:
    enum class Type;

    struct CrossThreadData {
        CrossThreadData(const CrossThreadData&) = delete;
        CrossThreadData& operator=(const CrossThreadData&) = delete;
        CrossThreadData() = default;
        CrossThreadData(CrossThreadData&&) = default;

        URL url;
        String mimeType;
        long long expectedContentLength;
        String textEncodingName;
        int httpStatusCode;
        String httpStatusText;
        String httpVersion;
        HTTPHeaderMap httpHeaderFields;
        NetworkLoadMetrics networkLoadMetrics;
        Type type;
        bool isRedirected;
    };

    CrossThreadData crossThreadData() const;
    static ResourceResponse fromCrossThreadData(CrossThreadData&&);

    enum class Tainting { Basic, Cors, Opaque };
    static ResourceResponse filterResponse(const ResourceResponse&, Tainting);

    bool isNull() const { return m_isNull; }
    WEBCORE_EXPORT bool isHTTP() const;
    bool isSuccessful() const;

    WEBCORE_EXPORT const URL& url() const;
    WEBCORE_EXPORT void setURL(const URL&);

    WEBCORE_EXPORT const String& mimeType() const;
    WEBCORE_EXPORT void setMimeType(const String& mimeType);

    WEBCORE_EXPORT long long expectedContentLength() const;
    WEBCORE_EXPORT void setExpectedContentLength(long long expectedContentLength);

    WEBCORE_EXPORT const String& textEncodingName() const;
    WEBCORE_EXPORT void setTextEncodingName(const String& name);

    WEBCORE_EXPORT int httpStatusCode() const;
    WEBCORE_EXPORT void setHTTPStatusCode(int);

    WEBCORE_EXPORT const String& httpStatusText() const;
    WEBCORE_EXPORT void setHTTPStatusText(const String&);

    WEBCORE_EXPORT const String& httpVersion() const;
    WEBCORE_EXPORT void setHTTPVersion(const String&);
    WEBCORE_EXPORT bool isHTTP09() const;

    WEBCORE_EXPORT const HTTPHeaderMap& httpHeaderFields() const;

    String httpHeaderField(const String& name) const;
    WEBCORE_EXPORT String httpHeaderField(HTTPHeaderName) const;
    WEBCORE_EXPORT void setHTTPHeaderField(const String& name, const String& value);
    WEBCORE_EXPORT void setHTTPHeaderField(HTTPHeaderName, const String& value);

    void addHTTPHeaderField(HTTPHeaderName, const String& value);
    void addHTTPHeaderField(const String& name, const String& value);

    // Instead of passing a string literal to any of these functions, just use a HTTPHeaderName instead.
    template<size_t length> String httpHeaderField(const char (&)[length]) const = delete;
    template<size_t length> void setHTTPHeaderField(const char (&)[length], const String&) = delete;
    template<size_t length> void addHTTPHeaderField(const char (&)[length], const String&) = delete;

    bool isMultipart() const { return mimeType() == "multipart/x-mixed-replace"; }

    WEBCORE_EXPORT bool isAttachment() const;
    WEBCORE_EXPORT bool isAttachmentWithFilename() const;
    WEBCORE_EXPORT String suggestedFilename() const;
    WEBCORE_EXPORT static String sanitizeSuggestedFilename(const String&);

    WEBCORE_EXPORT void includeCertificateInfo() const;
    const std::optional<CertificateInfo>& certificateInfo() const { return m_certificateInfo; };
    
    // These functions return parsed values of the corresponding response headers.
    WEBCORE_EXPORT bool cacheControlContainsNoCache() const;
    WEBCORE_EXPORT bool cacheControlContainsNoStore() const;
    WEBCORE_EXPORT bool cacheControlContainsMustRevalidate() const;
    WEBCORE_EXPORT bool cacheControlContainsImmutable() const;
    WEBCORE_EXPORT bool hasCacheValidatorFields() const;
    WEBCORE_EXPORT std::optional<std::chrono::microseconds> cacheControlMaxAge() const;
    WEBCORE_EXPORT std::optional<std::chrono::system_clock::time_point> date() const;
    WEBCORE_EXPORT std::optional<std::chrono::microseconds> age() const;
    WEBCORE_EXPORT std::optional<std::chrono::system_clock::time_point> expires() const;
    WEBCORE_EXPORT std::optional<std::chrono::system_clock::time_point> lastModified() const;
    ParsedContentRange& contentRange() const;

    enum class Source { Unknown, Network, DiskCache, DiskCacheAfterValidation, MemoryCache, MemoryCacheAfterValidation };
    WEBCORE_EXPORT Source source() const;
    void setSource(Source source) { m_source = source; }

    const std::optional<SHA1::Digest>& cacheBodyKey() const { return m_cacheBodyKey; }
    void setCacheBodyKey(const SHA1::Digest& key) { m_cacheBodyKey = key; }

    // FIXME: This should be eliminated from ResourceResponse.
    // Network loading metrics should be delivered via didFinishLoad
    // and should not be part of the ResourceResponse.
    NetworkLoadMetrics& deprecatedNetworkLoadMetrics() const { return m_networkLoadMetrics; }

    // The ResourceResponse subclass may "shadow" this method to provide platform-specific memory usage information
    unsigned memoryUsage() const
    {
        // average size, mostly due to URL and Header Map strings
        return 1280;
    }

    enum class Type { Basic, Cors, Default, Error, Opaque, Opaqueredirect };
    Type type() const { return m_type; }
    void setType(Type type) { m_type = type; }
    bool isRedirected() const { return m_isRedirected; }
    void setRedirected(bool isRedirected) { m_isRedirected = isRedirected; }

    static bool compare(const ResourceResponse&, const ResourceResponse&);

    template<class Encoder> void encode(Encoder&) const;
    template<class Decoder> static bool decode(Decoder&, ResourceResponseBase&);

protected:
    enum InitLevel {
        Uninitialized,
        CommonFieldsOnly,
        AllFields
    };

    WEBCORE_EXPORT ResourceResponseBase();
    WEBCORE_EXPORT ResourceResponseBase(const URL&, const String& mimeType, long long expectedLength, const String& textEncodingName);

    WEBCORE_EXPORT void lazyInit(InitLevel) const;

    // The ResourceResponse subclass should shadow these functions to lazily initialize platform specific fields
    void platformLazyInit(InitLevel) { }
    CertificateInfo platformCertificateInfo() const { return CertificateInfo(); };
    String platformSuggestedFileName() const { return String(); }

    static bool platformCompare(const ResourceResponse&, const ResourceResponse&) { return true; }

private:
    void parseCacheControlDirectives() const;
    void updateHeaderParsedState(HTTPHeaderName);

protected:
    bool m_isNull;
    URL m_url;
    AtomicString m_mimeType;
    long long m_expectedContentLength;
    AtomicString m_textEncodingName;
    AtomicString m_httpStatusText;
    AtomicString m_httpVersion;
    HTTPHeaderMap m_httpHeaderFields;
    mutable NetworkLoadMetrics m_networkLoadMetrics;

    mutable std::optional<CertificateInfo> m_certificateInfo;

    int m_httpStatusCode;

private:
    mutable std::optional<std::chrono::microseconds> m_age;
    mutable std::optional<std::chrono::system_clock::time_point> m_date;
    mutable std::optional<std::chrono::system_clock::time_point> m_expires;
    mutable std::optional<std::chrono::system_clock::time_point> m_lastModified;
    mutable ParsedContentRange m_contentRange;
    mutable CacheControlDirectives m_cacheControlDirectives;

    mutable bool m_haveParsedCacheControlHeader { false };
    mutable bool m_haveParsedAgeHeader { false };
    mutable bool m_haveParsedDateHeader { false };
    mutable bool m_haveParsedExpiresHeader { false };
    mutable bool m_haveParsedLastModifiedHeader { false };
    mutable bool m_haveParsedContentRangeHeader { false };

    Source m_source { Source::Unknown };

    std::optional<SHA1::Digest> m_cacheBodyKey;

    Type m_type { Type::Default };
    bool m_isRedirected { false };
};

inline bool operator==(const ResourceResponse& a, const ResourceResponse& b) { return ResourceResponseBase::compare(a, b); }
inline bool operator!=(const ResourceResponse& a, const ResourceResponse& b) { return !(a == b); }

template<class Encoder>
void ResourceResponseBase::encode(Encoder& encoder) const
{
    encoder << m_isNull;
    if (m_isNull)
        return;
    lazyInit(AllFields);

    encoder << m_url;
    encoder << m_mimeType;
    encoder << static_cast<int64_t>(m_expectedContentLength);
    encoder << m_textEncodingName;
    encoder << m_httpStatusText;
    encoder << m_httpVersion;
    encoder << m_httpHeaderFields;

    // We don't want to put the networkLoadMetrics info
    // into the disk cache, because we will never use the old info.
    if (Encoder::isIPCEncoder)
        encoder << m_networkLoadMetrics;

    encoder << m_httpStatusCode;
    encoder << m_certificateInfo;
    encoder.encodeEnum(m_source);
    encoder << m_cacheBodyKey;
    encoder.encodeEnum(m_type);
    encoder << m_isRedirected;
}

template<class Decoder>
bool ResourceResponseBase::decode(Decoder& decoder, ResourceResponseBase& response)
{
    ASSERT(response.m_isNull);
    bool responseIsNull;
    if (!decoder.decode(responseIsNull))
        return false;
    if (responseIsNull)
        return true;

    if (!decoder.decode(response.m_url))
        return false;
    if (!decoder.decode(response.m_mimeType))
        return false;
    int64_t expectedContentLength;
    if (!decoder.decode(expectedContentLength))
        return false;
    response.m_expectedContentLength = expectedContentLength;
    if (!decoder.decode(response.m_textEncodingName))
        return false;
    if (!decoder.decode(response.m_httpStatusText))
        return false;
    if (!decoder.decode(response.m_httpVersion))
        return false;
    if (!decoder.decode(response.m_httpHeaderFields))
        return false;
    // The networkLoadMetrics info is only send over IPC and not stored in disk cache.
    if (Decoder::isIPCDecoder && !decoder.decode(response.m_networkLoadMetrics))
        return false;
    if (!decoder.decode(response.m_httpStatusCode))
        return false;
    if (!decoder.decode(response.m_certificateInfo))
        return false;
    if (!decoder.decodeEnum(response.m_source))
        return false;
    if (!decoder.decode(response.m_cacheBodyKey))
        return false;
    if (!decoder.decodeEnum(response.m_type))
        return false;
    if (!decoder.decode(response.m_isRedirected))
        return false;
    response.m_isNull = false;

    return true;
}

} // namespace WebCore
