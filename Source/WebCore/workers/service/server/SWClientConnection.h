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

#include "ServiceWorkerJob.h"
#include <wtf/HashMap.h>
#include <wtf/ThreadSafeRefCounted.h>

namespace WebCore {

class ResourceError;
class SecurityOrigin;
class SerializedScriptValue;
class SharedBuffer;
struct ExceptionData;
struct ServiceWorkerFetchResult;
struct ServiceWorkerRegistrationData;

class SWClientConnection : public ThreadSafeRefCounted<SWClientConnection> {
public:
    WEBCORE_EXPORT SWClientConnection();
    WEBCORE_EXPORT virtual ~SWClientConnection();

    void scheduleJob(ServiceWorkerJob&);
    void finishedFetchingScript(ServiceWorkerJob&, const String&);
    void failedFetchingScript(ServiceWorkerJob&, const ResourceError&);
    virtual void postMessageToServiceWorkerGlobalScope(uint64_t destinationServiceWorkerIdentifier, Ref<SerializedScriptValue>&&, ScriptExecutionContext& source) = 0;

    virtual uint64_t identifier() const = 0;
    virtual bool hasServiceWorkerRegisteredForOrigin(const SecurityOrigin&) const = 0;

protected:
    WEBCORE_EXPORT void jobRejectedInServer(uint64_t jobIdentifier, const ExceptionData&);
    WEBCORE_EXPORT void registrationJobResolvedInServer(uint64_t jobIdentifier, ServiceWorkerRegistrationData&&);
    WEBCORE_EXPORT void unregistrationJobResolvedInServer(uint64_t jobIdentifier, bool unregistrationResult);
    WEBCORE_EXPORT void startScriptFetchForServer(uint64_t jobIdentifier);
    WEBCORE_EXPORT void postMessageToServiceWorkerClient(uint64_t destinationScriptExecutionContextIdentifier, Ref<SerializedScriptValue>&& message, uint64_t sourceServiceWorkerIdentifier, const String& sourceOrigin);

private:
    virtual void scheduleJobInServer(const ServiceWorkerJobData&) = 0;
    virtual void finishFetchingScriptInServer(const ServiceWorkerFetchResult&) = 0;

    HashMap<uint64_t, RefPtr<ServiceWorkerJob>> m_scheduledJobs;
};

} // namespace WebCore

#endif // ENABLE(SERVICE_WORKER)
