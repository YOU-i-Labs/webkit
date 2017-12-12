/*
 * Copyright (C) 2017 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#if ENABLE(SERVICE_WORKER)

#include "ExceptionOr.h"
#include "ServiceWorkerThreadProxy.h"
#include <wtf/HashMap.h>

namespace WebCore {

class SerializedScriptValue;
struct ServiceWorkerClientIdentifier;

class SWContextManager {
public:
    WEBCORE_EXPORT static SWContextManager& singleton();

    class Connection {
    public:
        virtual ~Connection() { }

        virtual void postMessageToServiceWorkerClient(const ServiceWorkerClientIdentifier& destinationIdentifier, Ref<SerializedScriptValue>&& message, uint64_t sourceServiceWorkerIdentifier, const String& sourceOrigin) = 0;
    };

    WEBCORE_EXPORT void setConnection(std::unique_ptr<Connection>&&);
    WEBCORE_EXPORT Connection* connection() const;

    WEBCORE_EXPORT void registerServiceWorkerThread(Ref<ServiceWorkerThreadProxy>&&);
    WEBCORE_EXPORT ServiceWorkerThreadProxy* serviceWorkerThreadProxy(uint64_t) const;
    WEBCORE_EXPORT void postMessageToServiceWorkerGlobalScope(uint64_t destinationServiceWorkerIdentifier, Ref<SerializedScriptValue>&& message, const ServiceWorkerClientIdentifier& sourceIdentifier, const String& sourceOrigin);

private:
    SWContextManager() = default;

    HashMap<uint64_t, RefPtr<ServiceWorkerThreadProxy>> m_workerMap;
    std::unique_ptr<Connection> m_connection;
};

} // namespace WebCore

#endif // ENABLE(SERVICE_WORKER)
