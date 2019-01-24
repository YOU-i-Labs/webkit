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

#include "SWServerWorker.h"
#include "ServiceWorkerIdentifier.h"
#include "ServiceWorkerJob.h"
#include "ServiceWorkerRegistrationData.h"
#include "ServiceWorkerRegistrationKey.h"
#include "ServiceWorkerTypes.h"
#include <wtf/CrossThreadQueue.h>
#include <wtf/CrossThreadTask.h>
#include <wtf/HashMap.h>
#include <wtf/HashSet.h>
#include <wtf/ObjectIdentifier.h>
#include <wtf/RunLoop.h>
#include <wtf/ThreadSafeRefCounted.h>
#include <wtf/Threading.h>
#include <wtf/UniqueRef.h>

namespace WebCore {

class SWOriginStore;
class SWServerJobQueue;
class SWServerRegistration;
class SWServerToContextConnection;
enum class ServiceWorkerRegistrationState;
enum class ServiceWorkerState;
struct ExceptionData;
struct ServiceWorkerContextData;
struct ServiceWorkerFetchResult;
struct ServiceWorkerRegistrationData;

class SWServer {
public:
    class Connection {
    friend class SWServer;
    public:
        WEBCORE_EXPORT virtual ~Connection();

        using Identifier = SWServerConnectionIdentifier;
        Identifier identifier() const { return m_identifier; }

        WEBCORE_EXPORT void didResolveRegistrationPromise(const ServiceWorkerRegistrationKey&);
        const SWServerRegistration* doRegistrationMatching(const SecurityOriginData& topOrigin, const URL& clientURL) const { return m_server.doRegistrationMatching(topOrigin, clientURL); }

        // Messages to the client WebProcess
        virtual void updateRegistrationStateInClient(ServiceWorkerRegistrationIdentifier, ServiceWorkerRegistrationState, const std::optional<ServiceWorkerData>&) = 0;
        virtual void updateWorkerStateInClient(ServiceWorkerIdentifier, ServiceWorkerState) = 0;
        virtual void fireUpdateFoundEvent(ServiceWorkerRegistrationIdentifier) = 0;
        virtual void notifyClientsOfControllerChange(const HashSet<uint64_t>& scriptExecutionContexts, const ServiceWorkerData& newController) = 0;

    protected:
        WEBCORE_EXPORT explicit Connection(SWServer&);
        SWServer& server() { return m_server; }

        WEBCORE_EXPORT void scheduleJobInServer(const ServiceWorkerJobData&);
        WEBCORE_EXPORT void finishFetchingScriptInServer(const ServiceWorkerFetchResult&);
        WEBCORE_EXPORT void addServiceWorkerRegistrationInServer(const ServiceWorkerRegistrationKey&, ServiceWorkerRegistrationIdentifier);
        WEBCORE_EXPORT void removeServiceWorkerRegistrationInServer(const ServiceWorkerRegistrationKey&, ServiceWorkerRegistrationIdentifier);
        WEBCORE_EXPORT void serviceWorkerStartedControllingClient(ServiceWorkerIdentifier, uint64_t scriptExecutionContextIdentifier);
        WEBCORE_EXPORT void serviceWorkerStoppedControllingClient(ServiceWorkerIdentifier, uint64_t scriptExecutionContextIdentifier);

    private:
        // Messages to the client WebProcess
        virtual void rejectJobInClient(const ServiceWorkerJobDataIdentifier&, const ExceptionData&) = 0;
        virtual void resolveRegistrationJobInClient(const ServiceWorkerJobDataIdentifier&, const ServiceWorkerRegistrationData&, ShouldNotifyWhenResolved) = 0;
        virtual void resolveUnregistrationJobInClient(const ServiceWorkerJobDataIdentifier&, const ServiceWorkerRegistrationKey&, bool registrationResult) = 0;
        virtual void startScriptFetchInClient(const ServiceWorkerJobDataIdentifier&) = 0;

        SWServer& m_server;
        Identifier m_identifier;
    };

    WEBCORE_EXPORT explicit SWServer(UniqueRef<SWOriginStore>&&);
    WEBCORE_EXPORT ~SWServer();

    WEBCORE_EXPORT void clearAll();
    WEBCORE_EXPORT void clear(const SecurityOrigin&);


    SWServerRegistration* getRegistration(const ServiceWorkerRegistrationKey&);
    void addRegistration(std::unique_ptr<SWServerRegistration>&&);
    void removeRegistration(const ServiceWorkerRegistrationKey&);
    WEBCORE_EXPORT Vector<ServiceWorkerRegistrationData> getRegistrations(const SecurityOriginData& topOrigin, const URL& clientURL);

    void scheduleJob(const ServiceWorkerJobData&);
    void rejectJob(const ServiceWorkerJobData&, const ExceptionData&);
    void resolveRegistrationJob(const ServiceWorkerJobData&, const ServiceWorkerRegistrationData&, ShouldNotifyWhenResolved);
    void resolveUnregistrationJob(const ServiceWorkerJobData&, const ServiceWorkerRegistrationKey&, bool unregistrationResult);
    void startScriptFetch(const ServiceWorkerJobData&);

    void postTask(CrossThreadTask&&);
    void postTaskReply(CrossThreadTask&&);

    void updateWorker(Connection&, const ServiceWorkerJobDataIdentifier&, const ServiceWorkerRegistrationKey&, const URL&, const String& script, WorkerType);
    void terminateWorker(SWServerWorker&);
    void fireInstallEvent(SWServerWorker&);
    void fireActivateEvent(SWServerWorker&);
    SWServerWorker* workerByID(ServiceWorkerIdentifier identifier) const { return m_workersByID.get(identifier); }
    
    Connection* getConnection(SWServerConnectionIdentifier identifier) { return m_connections.get(identifier); }
    SWOriginStore& originStore() { return m_originStore; }

    void scriptContextFailedToStart(const ServiceWorkerJobDataIdentifier&, SWServerWorker&, const String& message);
    void scriptContextStarted(const ServiceWorkerJobDataIdentifier&, SWServerWorker&);
    void didFinishInstall(const ServiceWorkerJobDataIdentifier&, SWServerWorker&, bool wasSuccessful);
    void didFinishActivation(SWServerWorker&);
    void workerContextTerminated(SWServerWorker&);

    WEBCORE_EXPORT void serverToContextConnectionCreated();
    
    WEBCORE_EXPORT static HashSet<SWServer*>& allServers();

private:
    void registerConnection(Connection&);
    void unregisterConnection(Connection&);

    void taskThreadEntryPoint();
    void handleTaskRepliesOnMainThread();

    void scriptFetchFinished(Connection&, const ServiceWorkerFetchResult&);

    void didResolveRegistrationPromise(Connection&, const ServiceWorkerRegistrationKey&);

    void addClientServiceWorkerRegistration(Connection&, const ServiceWorkerRegistrationKey&, ServiceWorkerRegistrationIdentifier);
    void removeClientServiceWorkerRegistration(Connection&, const ServiceWorkerRegistrationKey&, ServiceWorkerRegistrationIdentifier);
    void serviceWorkerStartedControllingClient(Connection&, ServiceWorkerIdentifier, uint64_t scriptExecutionContextIdentifier);
    void serviceWorkerStoppedControllingClient(Connection&, ServiceWorkerIdentifier, uint64_t scriptExecutionContextIdentifier);

    WEBCORE_EXPORT const SWServerRegistration* doRegistrationMatching(const SecurityOriginData& topOrigin, const URL& clientURL) const;

    void installContextData(const ServiceWorkerContextData&);

    HashMap<SWServerConnectionIdentifier, Connection*> m_connections;
    HashMap<ServiceWorkerRegistrationKey, std::unique_ptr<SWServerRegistration>> m_registrations;
    HashMap<ServiceWorkerRegistrationKey, std::unique_ptr<SWServerJobQueue>> m_jobQueues;

    HashMap<ServiceWorkerIdentifier, Ref<SWServerWorker>> m_workersByID;

    RefPtr<Thread> m_taskThread;
    Lock m_taskThreadLock;

    CrossThreadQueue<CrossThreadTask> m_taskQueue;
    CrossThreadQueue<CrossThreadTask> m_taskReplyQueue;

    Lock m_mainThreadReplyLock;
    bool m_mainThreadReplyScheduled { false };
    UniqueRef<SWOriginStore> m_originStore;
    Deque<ServiceWorkerContextData> m_pendingContextDatas;
};

} // namespace WebCore

#endif // ENABLE(SERVICE_WORKER)
