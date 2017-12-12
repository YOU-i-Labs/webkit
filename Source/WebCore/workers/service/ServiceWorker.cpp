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
#include "ServiceWorker.h"

#if ENABLE(SERVICE_WORKER)

#include "EventNames.h"
#include "MessagePort.h"
#include "SWClientConnection.h"
#include "ScriptExecutionContext.h"
#include "SerializedScriptValue.h"
#include "ServiceWorkerProvider.h"
#include <runtime/JSCJSValueInlines.h>

namespace WebCore {

ServiceWorker::ServiceWorker(ScriptExecutionContext& context, uint64_t serviceWorkerIdentifier, const URL& scriptURL)
    : ContextDestructionObserver(&context)
    , m_identifier(serviceWorkerIdentifier)
    , m_scriptURL(scriptURL)
{
}

void ServiceWorker::setState(State state)
{
    m_state = state;
    dispatchEvent(Event::create(eventNames().statechangeEvent, false, false));
}

ExceptionOr<void> ServiceWorker::postMessage(ScriptExecutionContext& context, JSC::JSValue messageValue, Vector<JSC::Strong<JSC::JSObject>>&& transfer)
{
    if (state() == State::Redundant)
        return Exception { InvalidStateError, ASCIILiteral("Service Worker state is redundant") };

    // FIXME: Invoke Run Service Worker algorithm with serviceWorker as the argument.

    auto* execState = context.execState();
    ASSERT(execState);

    Vector<RefPtr<MessagePort>> ports;
    auto message = SerializedScriptValue::create(*execState, messageValue, WTFMove(transfer), ports, SerializationContext::WorkerPostMessage);
    if (message.hasException())
        return message.releaseException();

    // Disentangle the port in preparation for sending it to the remote context.
    auto channelsOrException = MessagePort::disentanglePorts(WTFMove(ports));
    if (channelsOrException.hasException())
        return channelsOrException.releaseException();

    // FIXME: Support sending the channels.
    auto channels = channelsOrException.releaseReturnValue();
    if (channels && !channels->isEmpty())
        return Exception { NotSupportedError, ASCIILiteral("Passing MessagePort objects to postMessage is not yet supported") };

    auto& swConnection = ServiceWorkerProvider::singleton().serviceWorkerConnectionForSession(context.sessionID());
    swConnection.postMessageToServiceWorkerGlobalScope(m_identifier, message.releaseReturnValue(), context);

    return { };
}

EventTargetInterface ServiceWorker::eventTargetInterface() const
{
    return ServiceWorkerEventTargetInterfaceType;
}

ScriptExecutionContext* ServiceWorker::scriptExecutionContext() const
{
    return ContextDestructionObserver::scriptExecutionContext();
}

} // namespace WebCore

#endif // ENABLE(SERVICE_WORKER)
