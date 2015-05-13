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

#ifndef V8DOMWindow_h
#define V8DOMWindow_h

#include "DOMWindow.h"
#include "V8DOMWrapper.h"
#include "WrapperTypeInfo.h"
#include <wtf/text/StringHash.h>
#include <v8.h>
#include <wtf/HashMap.h>

namespace WebCore {

class V8DOMWindow {

public:
    static bool HasInstance(v8::Handle<v8::Value> value);
    static v8::Persistent<v8::FunctionTemplate> GetRawTemplate();
    static v8::Persistent<v8::FunctionTemplate> GetTemplate();
    static DOMWindow* toNative(v8::Handle<v8::Object> object)
    {
        return reinterpret_cast<DOMWindow*>(object->GetPointerFromInternalField(v8DOMWrapperObjectIndex));
    }
    inline static v8::Handle<v8::Object> wrap(DOMWindow*);
    static void derefObject(void*);
    static WrapperTypeInfo info;
    static v8::Persistent<v8::ObjectTemplate> GetShadowObjectTemplate();
    static v8::Handle<v8::Value> openCallback(const v8::Arguments&);
    static v8::Handle<v8::Value> showModalDialogCallback(const v8::Arguments&);
    static v8::Handle<v8::Value> postMessageCallback(const v8::Arguments&);
    static v8::Handle<v8::Value> setTimeoutCallback(const v8::Arguments&);
    static v8::Handle<v8::Value> setIntervalCallback(const v8::Arguments&);
    static v8::Handle<v8::Value> addEventListenerCallback(const v8::Arguments&);
    static v8::Handle<v8::Value> removeEventListenerCallback(const v8::Arguments&);
    static v8::Handle<v8::Value> captureEventsCallback(const v8::Arguments&);
    static v8::Handle<v8::Value> releaseEventsCallback(const v8::Arguments&);
    static v8::Handle<v8::Value> toStringCallback(const v8::Arguments&);
    static void locationAccessorSetter(v8::Local<v8::String> name, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> eventAccessorGetter(v8::Local<v8::String> name, const v8::AccessorInfo& info);
    static void eventAccessorSetter(v8::Local<v8::String> name, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static void openerAccessorSetter(v8::Local<v8::String> name, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> ImageAccessorGetter(v8::Local<v8::String> name, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> OptionAccessorGetter(v8::Local<v8::String> name, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> AudioAccessorGetter(v8::Local<v8::String> name, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> indexedPropertyGetter(uint32_t, const v8::AccessorInfo&);
    static v8::Handle<v8::Boolean> indexedPropertyDeleter(uint32_t, const v8::AccessorInfo&);
    static v8::Handle<v8::Value> namedPropertyGetter(v8::Local<v8::String>, const v8::AccessorInfo&);
    static v8::Handle<v8::Boolean> namedPropertyDeleter(v8::Local<v8::String>, const v8::AccessorInfo&);
    static v8::Handle<v8::Array> namedPropertyEnumerator(const v8::AccessorInfo&);
    static v8::Handle<v8::Integer> namedPropertyQuery(v8::Local<v8::String>, const v8::AccessorInfo&);
    static const int eventListenerCacheIndex = v8DefaultWrapperInternalFieldCount + 0;
    static const int enteredIsolatedWorldIndex = v8DefaultWrapperInternalFieldCount + 1;
    static const int internalFieldCount = v8DefaultWrapperInternalFieldCount + 2;
    static bool namedSecurityCheck(v8::Local<v8::Object> host, v8::Local<v8::Value> key, v8::AccessType, v8::Local<v8::Value> data);
    static bool indexedSecurityCheck(v8::Local<v8::Object> host, uint32_t index, v8::AccessType, v8::Local<v8::Value> data);
private:
    static v8::Handle<v8::Object> wrapSlow(DOMWindow*);
};


v8::Handle<v8::Object> V8DOMWindow::wrap(DOMWindow* impl)
{
        v8::Handle<v8::Object> wrapper = getDOMObjectMap().get(impl);
        if (!wrapper.IsEmpty())
            return wrapper;
    return V8DOMWindow::wrapSlow(impl);
}

v8::Handle<v8::Value> toV8(DOMWindow*);
inline v8::Handle<v8::Value> toV8(PassRefPtr< DOMWindow > impl)
{
    return toV8(impl.get());
}
}

#endif // V8DOMWindow_h
