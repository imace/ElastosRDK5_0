
#include "FutureTask.h"
#include <cutils/atomic.h>
#include <cutils/atomic-inline.h>
#include "LockSupport.h"
#include "Executors.h"
#include "CInteger32.h"
#include "CExecutors.h"
#include "CLockSupport.h"
#include "CSystem.h"

using Elastos::Core::ISystem;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Concurrent::Locks::ILockSupport;
using Elastos::Utility::Concurrent::Locks::CLockSupport;
using Elastos::Utility::Concurrent::IExecutors;
using Elastos::Utility::Concurrent::CExecutors;

namespace Elastos {
namespace Utility {
namespace Concurrent {

CAR_INTERFACE_IMPL_2(FutureTask, Object, IRunnableFuture, IRunnable)

const Int32 FutureTask::NEW          = 0;
const Int32 FutureTask::COMPLETING   = 1;
const Int32 FutureTask::NORMAL       = 2;
const Int32 FutureTask::EXCEPTIONAL  = 3;
const Int32 FutureTask::CANCELLED    = 4;
const Int32 FutureTask::INTERRUPTING = 5;
const Int32 FutureTask::INTERRUPTED  = 6;

const Int64 FutureTask::mStateOffset;
const Int64 FutureTask::mRunnerOffset;
const Int64 FutureTask::mWaitersOffset;

FutureTask::FutureTask()
{}

FutureTask::FutureTask(
    /* [in] */ ICallable* callable)
{
    ASSERT_SUCCEEDED(constructor(callable));
}

FutureTask::FutureTask(
    /* [in] */ IRunnable* runnable,
    /* [in] */ IInterface* result)
{
    ASSERT_SUCCEEDED(constructor(runnable, result));
}

ECode FutureTask::constructor(
    /* [in] */ ICallable* callable)
{
    if (callable == NULL) return E_NULL_POINTER_EXCEPTION;
    mCallable = callable;
    mState = NEW;       // ensure visibility of callable
    return NOERROR;
}

ECode FutureTask::constructor(
    /* [in] */ IRunnable* runnable,
    /* [in] */ IInterface* result)
{
#ifdef ELASTOS_UTILITY_CONCURRENT
    FAIL_RETURN(Executors::Callable(runnable, result, (ICallable**)&mCallable));
#else
    AutoPtr<IExecutors> executors;
    CExecutors::AcquireSingleton((IExecutors**)&executors);
    FAIL_RETURN(executors->Callable(runnable, result, (ICallable**)&mCallable));
#endif
    mState = NEW;       // ensure visibility of callable
    return NOERROR;
}

ECode FutureTask::Report(
    /* [in] */ Int32 s,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> x = mOutcome;
    if (s == NORMAL) {
        *result = x;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    if (s >= CANCELLED) return E_CANCELLATION_EXCEPTION;
    return E_EXECUTION_EXCEPTION;
}

ECode FutureTask::IsCancelled(
    /* [out] */ Boolean* isCancelled)
{
    VALIDATE_NOT_NULL(isCancelled)

    *isCancelled = mState >= CANCELLED;
    return NOERROR;
}

ECode FutureTask::IsDone(
    /* [out] */ Boolean* isDone)
{
    VALIDATE_NOT_NULL(isDone)

    *isDone = mState != NEW;
    return NOERROR;
}

static Boolean CompareAndSwapInt32(volatile int32_t* address, Int32 expect, Int32 update)
{
    // Note: android_atomic_release_cas() returns 0 on success, not failure.
    int ret = android_atomic_release_cas(expect, update, address);

    return (ret == 0);
}

static Boolean CompareAndSwapObject(volatile int32_t* address, IInterface* expect, IInterface* update)
{
    // Note: android_atomic_cmpxchg() returns 0 on success, not failure.
    int ret = android_atomic_release_cas((int32_t)expect,
            (int32_t)update, address);
    if (ret == 0) {
        REFCOUNT_ADD(update)
    }
    return (ret == 0);
}

static void PutOrderedInt32(volatile int32_t* address, Int32 newValue)
{
    ANDROID_MEMBAR_STORE();
    *address = newValue;
}

ECode FutureTask::Cancel(
    /* [in] */ Boolean mayInterruptIfRunning,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (!(mState == NEW &&
          CompareAndSwapInt32((volatile int32_t*)&mStateOffset, NEW,
              mayInterruptIfRunning ? INTERRUPTING : CANCELLED))) {
        *result = FALSE;
        return NOERROR;
    }
//    try {    // in case call to interrupt throws exception
        if (mayInterruptIfRunning) {
//            try {
                AutoPtr<IThread> t = mRunner;
                if (t != NULL)
                    t->Interrupt();
//            } finally { // final state
                PutOrderedInt32((volatile int32_t*)&mStateOffset, INTERRUPTED);
//            }
        }
//    } finally {
        FinishCompletion();
//    }
    *result = TRUE;
    return NOERROR;
}

ECode FutureTask::Get(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 s = mState;
    if (s <= COMPLETING) {
        AwaitDone(FALSE, 0ll, &s);
    }
    return Report(s, result);
}

ECode FutureTask::Get(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    if (unit == NULL) return E_NULL_POINTER_EXCEPTION;
    Int32 s = mState;
    if (s <= COMPLETING) {
        Int64 nanos;
        unit->ToNanos(timeout, &nanos);
        AwaitDone(TRUE, nanos, &s);
        if (s <=  COMPLETING) return E_TIMEOUT_EXCEPTION;
    }
    return Report(s, result);
}

void FutureTask::Done()
{}

void FutureTask::Set(
    /* [in] */ IInterface* v)
{
    if (CompareAndSwapInt32((volatile int32_t*)&mState, NEW, COMPLETING)) {
        mOutcome = v;
        PutOrderedInt32((volatile int32_t*)&mState, NORMAL); // final state
        FinishCompletion();
    }
}

void FutureTask::SetException(
    /* [in] */ ECode ec)
{
    if (CompareAndSwapInt32((volatile int32_t*)&mState, NEW, COMPLETING)) {
        AutoPtr<IInteger32> ex;
        CInteger32::New(ec, (IInteger32**)&ex);
        mOutcome = ex;
        PutOrderedInt32((volatile int32_t*)&mState, EXCEPTIONAL); // final state
        FinishCompletion();
    }
}

ECode FutureTask::Run()
{
    if (mState != NEW) return NOERROR;

    AutoPtr<IThread> t = Thread::GetCurrentThread();
    if (!CompareAndSwapObject((volatile int32_t*)&mRunner, NULL, t.Get())) {
        return NOERROR;
    }
    // try {
    AutoPtr<ICallable> c = mCallable;
    if (c != NULL && mState == NEW) {
        AutoPtr<IInterface> result;
        Boolean ran;
        // try {
        ECode ec = c->Call((IInterface**)&result);
        ran = TRUE;
        if (FAILED(ec)) {
            result = NULL;
            ran = FALSE;
            SetException(ec);
        }
        // } catch (Throwable ex) {
        //     result = null;
        //     ran = false;
        //     setException(ex);
        // }
        if (ran) {
            Set(result);
        }
    }
    // } finally {
    // runner must be non-null until state is settled to
    // prevent concurrent calls to run()
    mRunner = NULL;
    // state must be re-read after nulling runner to prevent
    // leaked interrupts
    Int32 s = mState;
    if (s >= INTERRUPTING) {
        HandlePossibleCancellationInterrupt(s);
    }
    // }
    return NOERROR;
}

Boolean FutureTask::RunAndReset()
{
    if (mState != NEW) return FALSE;

    AutoPtr<IThread> t = Thread::GetCurrentThread();
    if (!CompareAndSwapObject((volatile int32_t*)&mRunner, NULL, t.Get())) {
        return FALSE;
    }
    Boolean ran = FALSE;
    Int32 s = mState;
    // try {
    AutoPtr<ICallable> c = mCallable;
    if (c != NULL && s == NEW) {
        // try {
        AutoPtr<IInterface> result;
        ECode ec = c->Call((IInterface**)&result); // don't set result
        ran = TRUE;
        if (FAILED(ec)) {
            SetException(ec);
        }
        // } catch (Throwable ex) {
        //     setException(ex);
        // }
    }
    // } finally {
    // runner must be non-null until state is settled to
    // prevent concurrent calls to run()
    mRunner = NULL;
    // state must be re-read after nulling runner to prevent
    // leaked interrupts
    s = mState;
    if (s >= INTERRUPTING) {
        HandlePossibleCancellationInterrupt(s);
    }
    // }
    return ran && s == NEW;
}

void FutureTask::HandlePossibleCancellationInterrupt(
    /* [in] */ Int32 s)
{
    // It is possible for our interrupter to stall before getting a
    // chance to interrupt us.  Let's spin-wait patiently.
    if (s == INTERRUPTING) {
        while (mState == INTERRUPTING) {
            Thread::Yield(); // wait out pending interrupt
        }
    }

    // assert state == INTERRUPTED;

    // We want to clear any interrupt we may have received from
    // cancel(true).  However, it is permissible to use interrupts
    // as an independent mechanism for a task to communicate with
    // its caller, and there is no way to clear only the
    // cancellation interrupt.
    //
    // Thread.interrupted();
}

void FutureTask::FinishCompletion()
{
    // assert state > COMPLETING;
    for (AutoPtr<WaitNode> q; (q = mWaiters) != NULL;) {
        if (CompareAndSwapObject((volatile int32_t*)&mWaiters, q->Probe(EIID_IInterface), NULL)) {
            for (;;) {
                AutoPtr<IThread> t = q->mThread;
                if (t != NULL) {
                    q->mThread = NULL;
#ifdef ELASTOS_UTILITY_CONCURRENT
                    LockSupport::Unpark(t);
#else
                    AutoPtr<ILockSupport> lockSupport;
                    CLockSupport::AcquireSingleton((ILockSupport**)&lockSupport);
                    lockSupport->Unpark(t);
#endif
                }
                AutoPtr<WaitNode> next = q->mNext;
                if (next == NULL) {
                    break;
                }
                q->mNext = NULL; // unlink to help gc
                q = next;
            }
            break;
        }
    }

    Done();

    mCallable = NULL;        // to reduce footprint
}

ECode FutureTask::AwaitDone(
    /* [in] */ Boolean timed,
    /* [in] */ Int64 nanos,
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state)

    AutoPtr<ISystem> system;
#ifdef ELASTOS_CORELIBRARY
    AutoPtr<Elastos::Core::CSystem> cs;
    Elastos::Core::CSystem::AcquireSingletonByFriend((Elastos::Core::CSystem**)&cs);
    system = (ISystem*)cs.Get();
#else
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
#endif

    Int64 deadline = 0ll;
    if (timed) {
        system->GetNanoTime(&deadline);
        deadline += nanos;
    }
    AutoPtr<WaitNode> q;
    Boolean queued = FALSE;
    for (;;) {
        if (Thread::Interrupted()) {
            RemoveWaiter(q);
            return E_INTERRUPTED_EXCEPTION;
        }

        Int32 s = mState;
        if (s > COMPLETING) {
            if (q != NULL) {
                q->mThread = NULL;
            }
            return s;
        }
        else if (s == COMPLETING) { // cannot time out yet
            Thread::Yield();
        }
        else if (q == NULL) {
            q = new WaitNode();
        }
        else if (!queued) {
            queued = CompareAndSwapObject((volatile int32_t*)&mWaiters,
                    (q->mNext = mWaiters)->Probe(EIID_IInterface), q->Probe(EIID_IInterface));
        }
        else if (timed) {
            Int64 now;
            system->GetNanoTime(&now);
            nanos = deadline - now;
            if (nanos <= 0ll) {
                RemoveWaiter(q);
                *state = mState;
                return NOERROR;
            }
#ifdef ELASTOS_UTILITY_CONCURRENT
            LockSupport::ParkNanos((IRunnableFuture*)THIS_PROBE(IRunnableFuture), nanos);
#else
            AutoPtr<ILockSupport> lockSupport;
            CLockSupport::AcquireSingleton((ILockSupport**)&lockSupport);
            lockSupport->ParkNanos((IRunnableFuture*)THIS_PROBE(IRunnableFuture), nanos);
#endif

        }
        else {
#ifdef ELASTOS_UTILITY_CONCURRENT
            LockSupport::Park((IRunnableFuture*)THIS_PROBE(IRunnableFuture));
#else
            AutoPtr<ILockSupport> lockSupport;
            CLockSupport::AcquireSingleton((ILockSupport**)&lockSupport);
            lockSupport->Park((IRunnableFuture*)THIS_PROBE(IRunnableFuture));
#endif
        }
    }
    return NOERROR;
}

void FutureTask::RemoveWaiter(
    /* [in] */ WaitNode* node)
{
    if (node != NULL) {
        node->mThread = NULL;
RETRY:
        for (;;) {          // restart on removeWaiter race
            for (AutoPtr<WaitNode> pred, q = mWaiters, s; q != NULL; q = s) {
                s = q->mNext;
                if (q->mThread != NULL) {
                    pred = q;
                }
                else if (pred != NULL) {
                    pred->mNext = s;
                    if (pred->mThread == NULL) // check for race
                        goto RETRY;
                }
                else if (!CompareAndSwapObject((volatile int32_t*)&mWaiters, q->Probe(EIID_IInterface), s->Probe(EIID_IInterface))) {
                    goto RETRY;
                }
            }
            break;
        }
    }
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
