/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
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
 *
 */

#pragma once

#include "ActiveDOMObject.h"
#include "EventTarget.h"
#include "ExceptionOr.h"
#include "MessagePortChannel.h"
#include "MessagePortIdentifier.h"

namespace JSC {
class ExecState;
class JSObject;
class JSValue;
}

namespace WebCore {

class Frame;

class MessagePort final : public ActiveDOMObject, public EventTargetWithInlineData {
public:
    static Ref<MessagePort> create(ScriptExecutionContext& scriptExecutionContext, const MessagePortIdentifier& identifier) { return adoptRef(*new MessagePort(scriptExecutionContext, identifier)); }
    virtual ~MessagePort();

    ExceptionOr<void> postMessage(JSC::ExecState&, JSC::JSValue message, Vector<JSC::Strong<JSC::JSObject>>&&);

    void start();
    void close();

    void entangleWithRemote(RefPtr<MessagePortChannel>&&);

    // Returns nullptr if the passed-in vector is empty.
    static ExceptionOr<std::unique_ptr<MessagePortChannelArray>> disentanglePorts(Vector<RefPtr<MessagePort>>&&);
    static Vector<RefPtr<MessagePort>> entanglePorts(ScriptExecutionContext&, std::unique_ptr<MessagePortChannelArray>&&);
    static RefPtr<MessagePort> existingMessagePortForIdentifier(const MessagePortIdentifier&);

    void messageAvailable();
    bool started() const { return m_started; }

    void dispatchMessages();

    // Returns null if there is no entangled port, or if the entangled port is run by a different thread.
    // This is used solely to enable a GC optimization. Some platforms may not be able to determine ownership
    // of the remote port (since it may live cross-process) - those platforms may always return null.
    MessagePort* locallyEntangledPort() const;

    const MessagePortIdentifier& identifier() const { return m_identifier; }

    void ref() const;
    void deref() const;

    // ActiveDOMObject
    const char* activeDOMObjectName() const final;
    bool canSuspendForDocumentSuspension() const final;
    void contextDestroyed() final;
    void stop() final { close(); }
    bool hasPendingActivity() const final;

    // EventTargetWithInlineData.
    EventTargetInterface eventTargetInterface() const final { return MessagePortEventTargetInterfaceType; }
    ScriptExecutionContext* scriptExecutionContext() const final { return ActiveDOMObject::scriptExecutionContext(); }
    void refEventTarget() final { ref(); }
    void derefEventTarget() final { deref(); }

private:
    explicit MessagePort(ScriptExecutionContext&, const MessagePortIdentifier&);

    bool addEventListener(const AtomicString& eventType, Ref<EventListener>&&, const AddEventListenerOptions&) final;

    RefPtr<MessagePortChannel> disentangle();

    // A port starts out its life entangled, and remains entangled until it is closed or is cloned.
    bool isEntangled() const { return !m_closed && !isNeutered(); }

    // A port gets neutered when it is transferred to a new owner via postMessage().
    bool isNeutered() const { return !m_entangledChannel; }

    RefPtr<MessagePortChannel> m_entangledChannel;
    RefPtr<MessagePort> m_messageProtector;
    bool m_started { false };
    bool m_closed { false };

    MessagePortIdentifier m_identifier;

    mutable std::atomic<unsigned> m_refCount { 1 };
};

} // namespace WebCore
