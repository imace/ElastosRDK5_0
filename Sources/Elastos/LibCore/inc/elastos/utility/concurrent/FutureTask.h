
#ifndef __ELASTOS_UTILITY_FUTURETASK_H__
#define __ELASTOS_UTILITY_FUTURETASK_H__

#include "Thread.h"

using Elastos::Core::IRunnable;
using Elastos::Core::IThread;
using Elastos::Core::Thread;

namespace Elastos {
namespace Utility {
namespace Concurrent {

class FutureTask
    : public Object
    , public IRunnableFuture
    , public IRunnable
{
public:
    /**
     * Simple linked list nodes to record waiting threads in a Treiber
     * stack.  See other classes such as Phaser and SynchronousQueue
     * for more detailed explanation.
     */
    class WaitNode
        : public Object
    {
    public:
        WaitNode()
        {
            mThread = Thread::GetCurrentThread();
        }

    public:
        AutoPtr<IThread> mThread;
        AutoPtr<WaitNode> mNext;
    };

public:
    CAR_INTERFACE_DECL()

    FutureTask();

    /**
     * Creates a {@code FutureTask} that will, upon running, execute the
     * given {@code Callable}.
     *
     * @param  callable the callable task
     * @throws NullPointerException if the callable is null
     */
    FutureTask(
        /* [in] */ ICallable* callable);

    /**
     * Creates a {@code FutureTask} that will, upon running, execute the
     * given {@code Runnable}, and arrange that {@code get} will return the
     * given result on successful completion.
     *
     * @param runnable the runnable task
     * @param result the result to return on successful completion. If
     * you don't need a particular result, consider using
     * constructions of the form:
     * {@code Future<?> f = new FutureTask<Void>(runnable, null)}
     * @throws NullPointerException if the runnable is null
     */
    FutureTask(
        /* [in] */ IRunnable* runnable,
        /* [in] */ IInterface* result);

    virtual CARAPI IsCancelled(
        /* [out] */ Boolean* isCancelled);

    virtual CARAPI IsDone(
        /* [out] */ Boolean* isDone);

    virtual CARAPI Cancel(
        /* [in] */ Boolean mayInterruptIfRunning,
        /* [out] */ Boolean* result);

    virtual CARAPI Get(
        /* [out] */ IInterface** result);

    virtual CARAPI Get(
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ IInterface** result);

    virtual CARAPI Run();


    /**
     * Sets the result of this future to the given value unless
     * this future has already been set or has been cancelled.
     *
     * <p>This method is invoked internally by the {@link #run} method
     * upon successful completion of the computation.
     *
     * @param v the value
     */
    virtual CARAPI_(void) Set(
        /* [in] */ IInterface* v);

    /**
     * Causes this future to report an {@link ExecutionException}
     * with the given throwable as its cause, unless this future has
     * already been set or has been cancelled.
     *
     * <p>This method is invoked internally by the {@link #run} method
     * upon failure of the computation.
     *
     * @param t the cause of failure
     */
    virtual CARAPI_(void) SetException(
        /* [in] */ ECode ec);

    CARAPI constructor(
        /* [in] */ ICallable* callable);

    CARAPI constructor(
        /* [in] */ IRunnable* runnable,
        /* [in] */ IInterface* result);

    /**
     * Protected method invoked when this task transitions to state
     * {@code isDone} (whether normally or via cancellation). The
     * default implementation does nothing.  Subclasses may override
     * this method to invoke completion callbacks or perform
     * bookkeeping. Note that you can query status inside the
     * implementation of this method to determine whether this task
     * has been cancelled.
     */
    virtual CARAPI_(void) Done();

    /**
     * Executes the computation without setting its result, and then
     * resets this future to initial state, failing to do so if the
     * computation encounters an exception or is cancelled.  This is
     * designed for use with tasks that intrinsically execute more
     * than once.
     *
     * @return true if successfully run and reset
     */
    virtual CARAPI_(Boolean) RunAndReset();

private:
    /**
     * Returns result or throws exception for completed task.
     *
     * @param s completed state value
     */
    CARAPI Report(
        /* [in] */ Int32 s,
        /* [out] */ IInterface** result);

    /**
     * Ensures that any interrupt from a possible cancel(true) is only
     * delivered to a task while in run or runAndReset.
     */
    CARAPI_(void) HandlePossibleCancellationInterrupt(
        /* [in] */ Int32 s);

    /**
     * Removes and signals all waiting threads, invokes done(), and
     * nulls out callable.
     */
    CARAPI_(void) FinishCompletion();

    /**
     * Awaits completion or aborts on interrupt or timeout.
     *
     * @param timed true if use timed waits
     * @param nanos time to wait, if timed
     * @return state upon completion
     */
    CARAPI AwaitDone(
        /* [in] */ Boolean timed,
        /* [in] */ Int64 nanos,
        /* [out] */ Int32* state);

    /**
     * Tries to unlink a timed-out or interrupted wait node to avoid
     * accumulating garbage.  Internal nodes are simply unspliced
     * without CAS since it is harmless if they are traversed anyway
     * by releasers.  To avoid effects of unsplicing from already
     * removed nodes, the list is retraversed in case of an apparent
     * race.  This is slow when there are a lot of nodes, but we don't
     * expect lists to be long enough to outweigh higher-overhead
     * schemes.
     */
    CARAPI_(void) RemoveWaiter(
        /* [in] */ WaitNode* node);

private:
    /*
     * Revision notes: This differs from previous versions of this
     * class that relied on AbstractQueuedSynchronizer, mainly to
     * avoid surprising users about retaining interrupt status during
     * cancellation races. Sync control in the current design relies
     * on a "state" field updated via CAS to track completion, along
     * with a simple Treiber stack to hold waiting threads.
     *
     * Style note: As usual, we bypass overhead of using
     * AtomicXFieldUpdaters and instead directly use Unsafe intrinsics.
     */

    /**
     * The run state of this task, initially NEW.  The run state
     * transitions to a terminal state only in methods set,
     * setException, and cancel.  During completion, state may take on
     * transient values of COMPLETING (while outcome is being set) or
     * INTERRUPTING (only while interrupting the runner to satisfy a
     * cancel(true)). Transitions from these intermediate to final
     * states use cheaper ordered/lazy writes because values are unique
     * and cannot be further modified.
     *
     * Possible state transitions:
     * NEW -> COMPLETING -> NORMAL
     * NEW -> COMPLETING -> EXCEPTIONAL
     * NEW -> CANCELLED
     * NEW -> INTERRUPTING -> INTERRUPTED
     */
    volatile Int32 mState;
    static const Int32 NEW;
    static const Int32 COMPLETING;
    static const Int32 NORMAL;
    static const Int32 EXCEPTIONAL;
    static const Int32 CANCELLED;
    static const Int32 INTERRUPTING;
    static const Int32 INTERRUPTED;

    /** The underlying callable; nulled out after running */
    AutoPtr<ICallable> mCallable;
    /** The result to return or exception to throw from get() */
    AutoPtr<IInterface> mOutcome; // non-volatile, protected by state reads/writes
    /** The thread running the callable; CASed during run() */
    AutoPtr<IThread> mRunner;
    /** Treiber stack of waiting threads */
    AutoPtr<WaitNode> mWaiters;

    // Unsafe mechanics
//    static sun.misc.Unsafe UNSAFE;
    static const Int64 mStateOffset;
    static const Int64 mRunnerOffset;
    static const Int64 mWaitersOffset;
    // static {
    //     try {
    //         UNSAFE = sun.misc.Unsafe.getUnsafe();
    //         Class<?> k = FutureTask.class;
    //         stateOffset = UNSAFE.objectFieldOffset
    //             (k.getDeclaredField("state"));
    //         runnerOffset = UNSAFE.objectFieldOffset
    //             (k.getDeclaredField("runner"));
    //         waitersOffset = UNSAFE.objectFieldOffset
    //             (k.getDeclaredField("waiters"));
    //     } catch (Exception e) {
    //         throw new Error(e);
    //     }
    // }
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_FUTURETASK_H__
