/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef V8HTMLOptGroupElement_h
#define V8HTMLOptGroupElement_h

#include "HTMLOptGroupElement.h"
#include "V8DOMWrapper.h"
#include "WrapperTypeInfo.h"
#include <wtf/text/StringHash.h>
#include <v8.h>
#include <wtf/HashMap.h>

namespace WebCore {

class V8HTMLOptGroupElement {

public:
    static bool HasInstance(v8::Handle<v8::Value> value);
    static v8::Persistent<v8::FunctionTemplate> GetRawTemplate();
    static v8::Persistent<v8::FunctionTemplate> GetTemplate();
    static HTMLOptGroupElement* toNative(v8::Handle<v8::Object> object)
    {
        return reinterpret_cast<HTMLOptGroupElement*>(object->GetPointerFromInternalField(v8DOMWrapperObjectIndex));
    }
    inline static v8::Handle<v8::Object> wrap(HTMLOptGroupElement*);
    static void derefObject(void*);
    static WrapperTypeInfo info;
    static const int eventListenerCacheIndex = v8DefaultWrapperInternalFieldCount + 0;
    static const int internalFieldCount = v8DefaultWrapperInternalFieldCount + 1;
private:
    static v8::Handle<v8::Object> wrapSlow(HTMLOptGroupElement*);
};


v8::Handle<v8::Object> V8HTMLOptGroupElement::wrap(HTMLOptGroupElement* impl)
{
        v8::Handle<v8::Object> wrapper = V8DOMWrapper::getWrapper(impl);
        if (!wrapper.IsEmpty())
            return wrapper;
    return V8HTMLOptGroupElement::wrapSlow(impl);
}

inline v8::Handle<v8::Value> toV8(HTMLOptGroupElement* impl)
{
    if (!impl)
        return v8::Null();
    return V8HTMLOptGroupElement::wrap(impl);
}
inline v8::Handle<v8::Value> toV8(PassRefPtr< HTMLOptGroupElement > impl)
{
    return toV8(impl.get());
}
}

#endif // V8HTMLOptGroupElement_h
