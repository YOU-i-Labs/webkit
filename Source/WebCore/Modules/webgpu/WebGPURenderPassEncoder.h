/*
 * Copyright (C) 2018 Apple Inc. All rights reserved.
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

#if ENABLE(WEBGPU)

#include "WebGPUProgrammablePassEncoder.h"

#include <wtf/RefPtr.h>
#include <wtf/Vector.h>

namespace WebCore {

class GPUProgrammablePassEncoder;
class GPURenderPassEncoder;
class WebGPUBuffer;

class WebGPURenderPassEncoder final : public WebGPUProgrammablePassEncoder {
public:
    static Ref<WebGPURenderPassEncoder> create(Ref<WebGPUCommandBuffer>&&, Ref<GPURenderPassEncoder>&&);

    // FIXME: Last argument should be Vector<unsigned long>. Why is the generated code incorrectly assuming the IDL wants a sequence<unsigned int>?
    void setVertexBuffers(unsigned long, Vector<RefPtr<WebGPUBuffer>>&&, Vector<unsigned>&&);
    void draw(unsigned long, unsigned long, unsigned long, unsigned long);

private:
    WebGPURenderPassEncoder(Ref<WebGPUCommandBuffer>&&, Ref<GPURenderPassEncoder>&&);

    GPUProgrammablePassEncoder& passEncoder() const final;

    Ref<GPURenderPassEncoder> m_passEncoder;
};

} // namespace WebCore

#endif // ENABLE(WEBGPU)
