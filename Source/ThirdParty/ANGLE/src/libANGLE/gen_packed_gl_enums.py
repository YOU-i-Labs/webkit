# Copyright 2016 The ANGLE Project Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# gen_packed_gl_enums.py:
#  Code generation for the packed GL enums.

import datetime, json, os, sys
from collections import namedtuple

Enum = namedtuple('Enum', ['name', 'values', 'max_value'])
EnumValue = namedtuple('EnumValue', ['name', 'gl_name', 'value'])

kJsonFileName = "packed_gl_enums.json"

def load_enums(path):
    with open(path) as map_file:
        enums_dict = json.loads(map_file.read())

    enums = []
    for (enum_name, values_dict) in enums_dict.iteritems():

        values = []
        i = 0
        for (value_name, value_gl_name) in sorted(values_dict.iteritems()):
            values.append(EnumValue(value_name, value_gl_name, i))
            i += 1

        assert(i < 255) # This makes sure enums fit in the uint8_t
        enums.append(Enum(enum_name, values, i))

    enums.sort(key=lambda enum: enum.name)
    return enums

header_template = """// GENERATED FILE - DO NOT EDIT.
// Generated by {script_name} using data from {data_source_name}.
//
// Copyright {copyright_year} The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// PackedGLEnums_autogen.h:
//   Declares ANGLE-specific enums classes for GLEnum and functions operating
//   on them.

#ifndef LIBANGLE_PACKEDGLENUMS_AUTOGEN_H_
#define LIBANGLE_PACKEDGLENUMS_AUTOGEN_H_

#include <angle_gl.h>

#include <cstdint>

namespace gl
{{

template<typename Enum>
Enum FromGLenum(GLenum from);
{content}
}}  // namespace gl

#endif // LIBANGLE_PACKEDGLENUMS_AUTOGEN_H_
"""

enum_declaration_template = """
enum class {enum_name} : uint8_t
{{
{value_declarations}

    InvalidEnum = {max_value},
    EnumCount = {max_value},
}};

template<>
{enum_name} FromGLenum<{enum_name}>(GLenum from);
GLenum ToGLenum({enum_name} from);
"""

def write_header(enums, path):
    content = ['']

    for enum in enums:
        value_declarations = []
        for value in enum.values:
            value_declarations.append('    ' + value.name + ' = ' + str(value.value) + ',')

        content.append(enum_declaration_template.format(
            enum_name = enum.name,
            max_value = str(enum.max_value),
            value_declarations = '\n'.join(value_declarations)
        ))

    header = header_template.format(
        content = ''.join(content),
        copyright_year = datetime.date.today().year,
        data_source_name = kJsonFileName,
        script_name = sys.argv[0]
    )

    with (open(path, 'wt')) as f:
        f.write(header)

cpp_template = """// GENERATED FILE - DO NOT EDIT.
// Generated by {script_name} using data from {data_source_name}.
//
// Copyright {copyright_year} The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// PackedGLEnums_autogen.cpp:
//   Implements ANGLE-specific enums classes for GLEnum and functions operating
//   on them.

#include "common/debug.h"
#include "libANGLE/PackedGLEnums_autogen.h"

namespace gl
{{
{content}
}}  // namespace gl
"""

enum_implementation_template = """
template<>
{enum_name} FromGLenum<{enum_name}>(GLenum from)
{{
    switch(from)
    {{
{from_glenum_cases}
        default: return {enum_name}::InvalidEnum;
    }}
}}

GLenum ToGLenum({enum_name} from)
{{
    switch(from)
    {{
{to_glenum_cases}
        default: UNREACHABLE(); return GL_NONE;
    }}
}}
"""

def write_cpp(enums, path):
    content = ['']

    for enum in enums:
        from_glenum_cases = []
        to_glenum_cases = []
        for value in enum.values:
            qualified_name = enum.name + '::' + value.name
            from_glenum_cases.append('        case ' + value.gl_name + ': return ' + qualified_name + ';')
            to_glenum_cases.append('        case ' + qualified_name + ': return ' + value.gl_name + ';')

        content.append(enum_implementation_template.format(
            enum_name = enum.name,
            from_glenum_cases = '\n'.join(from_glenum_cases),
            max_value = str(enum.max_value),
            to_glenum_cases = '\n'.join(to_glenum_cases)
        ))

    cpp = cpp_template.format(
        content = ''.join(content),
        copyright_year = datetime.date.today().year,
        data_source_name = kJsonFileName,
        script_name = sys.argv[0]
    )

    with (open(path, 'wt')) as f:
        f.write(cpp)

if __name__ == '__main__':
    path_prefix = os.path.dirname(os.path.realpath(__file__)) + os.path.sep
    enums = load_enums(path_prefix + kJsonFileName)

    write_header(enums, path_prefix + 'PackedGLEnums_autogen.h')
    write_cpp(enums, path_prefix + 'PackedGLEnums_autogen.cpp')
