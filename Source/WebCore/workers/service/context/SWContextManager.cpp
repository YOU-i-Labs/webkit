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

#include "config.h"

#if ENABLE(SERVICE_WORKER)
#include "SWContextManager.h"

namespace WebCore {

SWContextManager& SWContextManager::singleton()
{
    static SWContextManager* sharedManager = new SWContextManager;
    return *sharedManager;
}

void SWContextManager::setConnection(std::unique_ptr<Connection>&& connection)
{
    ASSERT(!m_connection);
    m_connection = WTFMove(connection);
}

auto SWContextManager::connection() const -> Connection*
{
    return m_connection.get();
}

void SWContextManager::registerServiceWorkerThread(Ref<ServiceWorkerThreadProxy>&& serviceWorkerThreadProxy)
{
    auto serviceWorkerIdentifier = serviceWorkerThreadProxy->identifier();
    auto result = m_workerMap.add(serviceWorkerIdentifier, WTFMove(serviceWorkerThreadProxy));
    ASSERT_UNUSED(result, result.isNewEntry);
}

ServiceWorkerThreadProxy* SWContextManager::serviceWorkerThreadProxy(uint64_t serviceWorkerIdentifier) const
{
    return m_workerMap.get(serviceWorkerIdentifier);
}

void SWContextManager::postMessageToServiceWorkerGlobalScope(uint64_t destinationServiceWorkerIdentifier, Ref<SerializedScriptValue>&& message, const ServiceWorkerClientIdentifier& sourceIdentifier, const String& sourceOrigin)
{
    auto* serviceWorker = m_workerMap.get(destinationServiceWorkerIdentifier);
    if (!serviceWorker)
        return;

    // FIXME: We should pass valid MessagePortChannels.
    serviceWorker->thread().postMessageToServiceWorkerGlobalScope(WTFMove(message), nullptr, sourceIdentifier, sourceOrigin);
}

} // namespace WebCore

#endif
