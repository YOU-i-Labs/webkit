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

#include "SWServer.h"
#include "ServiceWorkerClientIdentifier.h"
#include "ServiceWorkerRegistrationData.h"
#include "ServiceWorkerTypes.h"
#include <wtf/HashCountedSet.h>
#include <wtf/MonotonicTime.h>

namespace WebCore {

class SWServer;
class SWServerWorker;
enum class ServiceWorkerRegistrationState;
enum class ServiceWorkerState;
struct ExceptionData;
struct ServiceWorkerFetchResult;

class SWServerRegistration {
public:
    SWServerRegistration(SWServer&, const ServiceWorkerRegistrationKey&, ServiceWorkerUpdateViaCache, const URL& scopeURL, const URL& scriptURL);
    ~SWServerRegistration();

    const ServiceWorkerRegistrationKey& key() const { return m_registrationKey; }
    ServiceWorkerRegistrationIdentifier identifier() const { return m_identifier; }

    SWServerWorker* getNewestWorker();
    WEBCORE_EXPORT ServiceWorkerRegistrationData data() const;

    bool isUninstalling() const { return m_uninstalling; }
    void setIsUninstalling(bool value) { m_uninstalling = value; }

    void setLastUpdateTime(double time) { m_lastUpdateTime = time; }
    ServiceWorkerUpdateViaCache updateViaCache() const { return m_updateViaCache; }

    void updateRegistrationState(ServiceWorkerRegistrationState, SWServerWorker*);
    void updateWorkerState(SWServerWorker&, ServiceWorkerState);
    void fireUpdateFoundEvent();

    void addClientServiceWorkerRegistration(SWServerConnectionIdentifier);
    void removeClientServiceWorkerRegistration(SWServerConnectionIdentifier);

    SWServerWorker* installingWorker() const { return m_installingWorker.get(); }
    SWServerWorker* waitingWorker() const { return m_waitingWorker.get(); }
    SWServerWorker* activeWorker() const { return m_activeWorker.get(); }

    MonotonicTime creationTime() const { return m_creationTime; }

    bool hasClientsUsingRegistration() const { return !m_clientsUsingRegistration.isEmpty(); }
    void addClientUsingRegistration(const ServiceWorkerClientIdentifier&);
    void removeClientUsingRegistration(const ServiceWorkerClientIdentifier&);
    void unregisterServerConnection(SWServerConnectionIdentifier);

    void notifyClientsOfControllerChange();

private:
    void forEachConnection(const WTF::Function<void(SWServer::Connection&)>&);

    ServiceWorkerRegistrationIdentifier m_identifier;
    ServiceWorkerRegistrationKey m_registrationKey;
    ServiceWorkerUpdateViaCache m_updateViaCache;
    URL m_scopeURL;
    URL m_scriptURL;

    bool m_uninstalling { false };
    RefPtr<SWServerWorker> m_installingWorker;
    RefPtr<SWServerWorker> m_waitingWorker;
    RefPtr<SWServerWorker> m_activeWorker;

    double m_lastUpdateTime { 0 };
    
    HashCountedSet<SWServerConnectionIdentifier> m_connectionsWithClientRegistrations;
    SWServer& m_server;

    MonotonicTime m_creationTime;
    HashMap<SWServerConnectionIdentifier, HashSet<uint64_t /* scriptExecutionContextIdentifier */>> m_clientsUsingRegistration;
};

} // namespace WebCore

#endif // ENABLE(SERVICE_WORKER)
