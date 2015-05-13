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

#if ENABLE(DATABASE)

#ifndef V8SQLStatementErrorCallback_h
#define V8SQLStatementErrorCallback_h

#include "ActiveDOMCallback.h"
#include "SQLStatementErrorCallback.h"
#include "WorldContextHandle.h"
#include <v8.h>
#include <wtf/Forward.h>

namespace WebCore {

class ScriptExecutionContext;

class V8SQLStatementErrorCallback : public SQLStatementErrorCallback, public ActiveDOMCallback {
public:
    static PassRefPtr<V8SQLStatementErrorCallback> create(v8::Local<v8::Value> value, ScriptExecutionContext* context)
    {
        ASSERT(value->IsObject());
        ASSERT(context);
        return adoptRef(new V8SQLStatementErrorCallback(value->ToObject(), context));
    }

    virtual ~V8SQLStatementErrorCallback();

    // Functions
    virtual bool handleEvent(SQLTransaction* transaction, SQLError* error);

private:
    V8SQLStatementErrorCallback(v8::Local<v8::Object>, ScriptExecutionContext*);

    v8::Persistent<v8::Object> m_callback;
    WorldContextHandle m_worldContext;
};

}

#endif // V8SQLStatementErrorCallback_h

#endif // ENABLE(DATABASE)
