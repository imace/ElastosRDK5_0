#ifndef __ELASTOS_DROID_OS_PROCESS_H__
#define __ELASTOS_DROID_OS_PROCESS_H__

#include "ext/frameworkext.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::IO::IDataInputStream;
using Elastos::IO::IBufferedWriter;
using Elastos::Droid::Net::ILocalSocket;

namespace Elastos {
namespace Droid {
namespace Os {

class Process
    : public Object
    , public IProcess
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Start a new process.
     *
     * <p>If processes are enabled, a new process is created and the
     * static main() function of a <var>processClass</var> is executed there.
     * The process will continue running after this function returns.
     *
     * <p>If processes are not enabled, a new thread in the caller's
     * process is created and main() of <var>processClass</var> called there.
     *
     * <p>The niceName parameter, if not an empty string, is a custom name to
     * give to the process instead of using processClass.  This allows you to
     * make easily identifyable processes even if you are using the same base
     * <var>processClass</var> to start them.
     *
     * @param processClass The class to use as the process's main entry
     *                     point.
     * @param niceName A more readable name to use for the process.
     * @param uid The user-id under which the process will run.
     * @param gid The group-id under which the process will run.
     * @param gids Additional group-ids associated with the process.
     * @param debugFlags Additional flags.
     * @param targetSdkVersion The target SDK version for the app.
     * @param seInfo null-ok SE Android information for the new process.
     * @param zygoteArgs Additional arguments to supply to the zygote process.
     *
     * @return An object that describes the result of the attempt to start the process.
     * @throws RuntimeException on fatal start failure
     *
     * {@hide}
     */
    static CARAPI Start(
        /* [in] */ const String& processClass,
        /* [in] */ const String& niceName,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 gid,
        /* [in] */ ArrayOf<Int32>* gids,
        /* [in] */ Int32 debugFlags,
        /* [in] */ Int32 mountExternal,
        /* [in] */ Int32 targetSdkVersion,
        /* [in] */ const String& seInfo,
        /* [in] */ ArrayOf<String>* zygoteArgs,
        /* [out] */ IProcessStartResult** result);

    /**
     * Returns elapsed milliseconds of the time this process has run.
     * @return  Returns the number of milliseconds this process has return.
     */
    static CARAPI_(Int64) GetElapsedCpuTime();

    /**
     * Returns the identifier of this process, which can be used with
     * {@link #killProcess} and {@link #sendSignal}.
     */
    static CARAPI_(Int32) MyPid();

    /**
     * Returns the identifier of the calling thread, which be used with
     * {@link #setThreadPriority(int, int)}.
     */
    static CARAPI_(Int32) MyTid();

    /**
     * Returns the identifier of this process's user.
     */
    static CARAPI_(Int32) MyUid();

    /**
     * Returns this process's user handle.  This is the
     * user the process is running under.  It is distinct from
     * {@link #myUid()} in that a particular user will have multiple
     * distinct apps running under it each with their own uid.
     */
    static CARAPI MyUserHandle(
        /* [out] */ IUserHandle** userHandle);

    /**
     * Returns whether the current process is in an isolated sandbox.
     * @hide
     */
    static CARAPI_(Boolean) IsIsolated();

    /**
     * Returns the UID assigned to a particular user name, or -1 if there is
     * none.  If the given string consists of only numbers, it is converted
     * directly to a uid.
     */
    static CARAPI_(Int32) GetUidForName(
        /* [in] */ const String& name);

    /**
     * Returns the GID assigned to a particular user name, or -1 if there is
     * none.  If the given string consists of only numbers, it is converted
     * directly to a gid.
     */
    static CARAPI_(Int32) GetGidForName(
        /* [in] */ const String& name);

    /**
     * Returns a uid for a currently running process.
     * @param pid the process id
     * @return the uid of the process, or -1 if the process is not running.
     * @hide pending API council review
     */
    static CARAPI_(Int32) GetUidForPid(
        /* [in] */ Int32 pid);

    /**
     * Returns the parent process id for a currently running process.
     * @param pid the process id
     * @return the parent process id of the process, or -1 if the process is not running.
     * @hide
     */
    static CARAPI_(Int32) GetParentPid(
        /* [in] */ Int32 pid);

    /**
     * Returns the thread group leader id for a currently running thread.
     * @param tid the thread id
     * @return the thread group leader id of the thread, or -1 if the thread is not running.
     *         This is same as what getpid(2) would return if called by tid.
     * @hide
     */
    static CARAPI_(Int32) GetThreadGroupLeader(
        /* [in] */ Int32 pid);

    /**
     * Set the priority of a thread, based on Linux priorities.
     *
     * @param tid The identifier of the thread/process to change.
     * @param priority A Linux priority level, from -20 for highest scheduling
     * priority to 19 for lowest scheduling priority.
     *
     * @throws IllegalArgumentException Throws IllegalArgumentException if
     * <var>tid</var> does not exist.
     * @throws SecurityException Throws SecurityException if your process does
     * not have permission to modify the given thread, or to use the given
     * priority.
     */
    static CARAPI SetThreadPriority(
        /* [in] */ Int32 tid,
        /* [in] */ Int32 priority);

    /**
     * Call with 'false' to cause future calls to {@link #setThreadPriority(int)} to
     * throw an exception if passed a background-level thread priority.  This is only
     * effective if the JNI layer is built with GUARD_THREAD_PRIORITY defined to 1.
     *
     * @hide
     */
    static CARAPI SetCanSelfBackground(
        /* [in] */ Boolean backgroundOk);

    /**
     * Sets the scheduling group for a thread.
     * @hide
     * @param tid The indentifier of the thread/process to change.
     * @param group The target group for this thread/process.
     *
     * @throws IllegalArgumentException Throws IllegalArgumentException if
     * <var>tid</var> does not exist.
     * @throws SecurityException Throws SecurityException if your process does
     * not have permission to modify the given thread, or to use the given
     * priority.
     */
    static CARAPI SetThreadGroup(
        /* [in] */ Int32 tid,
        /* [in] */ Int32 group);

    /**
     * Sets the scheduling group for a process and all child threads
     * @hide
     * @param pid The indentifier of the process to change.
     * @param group The target group for this process.
     *
     * @throws IllegalArgumentException Throws IllegalArgumentException if
     * <var>tid</var> does not exist.
     * @throws SecurityException Throws SecurityException if your process does
     * not have permission to modify the given thread, or to use the given
     * priority.
     */
    static CARAPI SetProcessGroup(
        /* [in] */ Int32 pid,
        /* [in] */ Int32 group);

    /**
     * Set the priority of the calling thread, based on Linux priorities.  See
     * {@link #setThreadPriority(int, int)} for more information.
     *
     * @param priority A Linux priority level, from -20 for highest scheduling
     * priority to 19 for lowest scheduling priority.
     *
     * @throws IllegalArgumentException Throws IllegalArgumentException if
     * <var>tid</var> does not exist.
     * @throws SecurityException Throws SecurityException if your process does
     * not have permission to modify the given thread, or to use the given
     * priority.
     *
     * @see #setThreadPriority(int, int)
     */
    static CARAPI SetThreadPriority(
        /* [in] */ Int32 priority);

    /**
     * Return the current priority of a thread, based on Linux priorities.
     *
     * @param tid The identifier of the thread/process to change.
     *
     * @return Returns the current priority, as a Linux priority level,
     * from -20 for highest scheduling priority to 19 for lowest scheduling
     * priority.
     *
     * @throws IllegalArgumentException Throws IllegalArgumentException if
     * <var>tid</var> does not exist.
     */
    static CARAPI GetThreadPriority(
        /* [in] */ Int32 tid,
        /* [out] */ Int32* priority);

    /**
     * Set the scheduling policy and priority of a thread, based on Linux.
     *
     * @param tid The identifier of the thread/process to change.
     * @param policy A Linux scheduling policy such as SCHED_OTHER etc.
     * @param priority A Linux priority level in a range appropriate for the given policy.
     *
     * @throws IllegalArgumentException Throws IllegalArgumentException if
     * <var>tid</var> does not exist, or if <var>priority</var> is out of range for the policy.
     * @throws SecurityException Throws SecurityException if your process does
     * not have permission to modify the given thread, or to use the given
     * scheduling policy or priority.
     *
     * {@hide}
     */
    static CARAPI SetThreadScheduler(
            /* [in] */ Int32 tid,
            /* [in] */ Int32 policy,
            /* [in] */ Int32 priority);

    /**
     * Determine whether the current environment supports multiple processes.
     *
     * @return Returns true if the system can run in multiple processes, else
     * false if everything is running in a single process.
     */
    static CARAPI_(Boolean) SupportsProcesses();

    /**
     * Set the out-of-memory badness adjustment for a process.
     *
     * @param pid The process identifier to set.
     * @param amt Adjustment value -- linux allows -16 to +15.
     *
     * @return Returns true if the underlying system supports this
     *         feature, else false.
     *
     * {@hide}
     */
    static CARAPI_(Boolean) SetOomAdj(
        /* [in] */ Int32 pid,
        /* [in] */ Int32 amt);

    /**
     * Change this process's argv[0] parameter.  This can be useful to show
     * more descriptive information in things like the 'ps' command.
     *
     * @param text The new name of this process.
     *
     * {@hide}
     */
    static CARAPI SetArgV0(
        /* [in] */ const String& name);

    /**
     * Kill the process with the given PID.
     * Note that, though this API allows us to request to
     * kill any process based on its PID, the kernel will
     * still impose standard restrictions on which PIDs you
     * are actually able to kill.  Typically this means only
     * the process running the caller's packages/application
     * and any additional processes created by that app; packages
     * sharing a common UID will also be able to kill each
     * other's processes.
     */
    static CARAPI KillProcess(
        /* [in] */ Int32 pid);

    /** @hide */
    static CARAPI_(Int32) SetUid(
        /* [in] */ Int32 uid);

    /** @hide */
    static CARAPI_(Int32) SetGid(
        /* [in] */ Int32 uid);

    /**
     * Send a signal to the given process.
     *
     * @param pid The pid of the target process.
     * @param signal The signal to send.
     */
    static CARAPI SendSignal(
        /* [in] */ Int32 pid,
        /* [in] */ Int32 signal);

    /**
     * @hide
     * Private impl for avoiding a log message...  DO NOT USE without doing
     * your own log, or the Android Illuminati will find you some night and
     * beat you up.
     */
    static CARAPI KillProcessQuiet(
        /* [in] */ Int32 pid);

    /**
     * @hide
     * Private impl for avoiding a log message...  DO NOT USE without doing
     * your own log, or the Android Illuminati will find you some night and
     * beat you up.
     */
    static CARAPI SendSignalQuiet(
        /* [in] */ Int32 pid,
        /* [in] */ Int32 signal);

    /** @hide */
    static CARAPI_(Int64) GetFreeMemory();

    /** @hide */
    static CARAPI_(Int64) GetTotalMemory();

    /** @hide */
    static CARAPI ReadProcLines(
        /* [in] */ const String& path,
        /* [in] */ const ArrayOf<String>& reqFields,
        /* [out] */ ArrayOf<Int64>* outSizes);

    /** @hide */
    static CARAPI GetPids(
        /* [in] */ const String& path,
        /* [in] */ ArrayOf<Int32>* lastArray,
        /* [out, callee] */ ArrayOf<Int32>** pids);

    /** @hide */
    static CARAPI ReadProcFile(
        /* [in] */ const String& file,
        /* [in] */ const ArrayOf<Int32>& format,
        /* [out] */ ArrayOf<String>* outStrings,
        /* [out] */ ArrayOf<Int64>* outInt64s,
        /* [out] */ ArrayOf<Float>* outFloats,
        /* [out] */ Boolean* result);

    /** @hide */
    static CARAPI ParseProcLine(
        /* [in] */ const ArrayOf<Byte>* buffer,
        /* [in] */ Int32 startIndex,
        /* [in] */ Int32 endIndex,
        /* [in] */ const ArrayOf<Int32>& format,
        /* [out] */ ArrayOf<String>* outStrings,
        /* [out] */ ArrayOf<Int64>* outInt64s,
        /* [out] */ ArrayOf<Float>* outFloats,
        /* [out] */ Boolean* result);

    /** @hide */
    static CARAPI GetPidsForCommands(
        /* [in] */ const ArrayOf<String>& cmds,
        /* [out] */ ArrayOf<Int32>** pids);

    /**
    * Gets the total Pss value for a given process, in bytes.
    *
    * @param pid the process to the Pss for
    * @return the total Pss value for the given process in bytes,
    *  or -1 if the value cannot be determined
    * @hide
    */
    static CARAPI_(Int64) GetPss(
        /* [in] */ Int32 pid);

private:
    /**
     * Tries to open socket to Zygote process if not already open. If
     * already open, does nothing.  May block and retry.
     */
    static CARAPI OpenZygoteSocketIfNeeded();

    static CARAPI OpenJavaZygoteSocketIfNeeded();

    /**
     * Sends an argument list to the zygote process, which starts a new child
     * and returns the child's pid. Please note: the present implementation
     * replaces newlines in the argument list with spaces.
     * @param args argument list
     * @return An object that describes the result of the attempt to start the process.
     * @throws ZygoteStartFailedEx if process start failed for any reason
     */
    static CARAPI ZygoteSendArgsAndGetResult(
        /* [in] */ List<String>& args,
        /* [out] */ IProcessStartResult** result);

    static CARAPI JavaZygoteSendArgsAndGetResult(
        /* [in] */ List<String>& args,
        /* [out] */ IProcessStartResult** result);

    /**
     * Starts a new process via the zygote mechanism.
     *
     * @param processClass Class name whose static main() to run
     * @param niceName 'nice' process name to appear in ps
     * @param uid a POSIX uid that the new process should setuid() to
     * @param gid a POSIX gid that the new process shuold setgid() to
     * @param gids null-ok; a list of supplementary group IDs that the
     * new process should setgroup() to.
     * @param debugFlags Additional flags.
     * @param targetSdkVersion The target SDK version for the app.
     * @param seInfo null-ok SE Android information for the new process.
     * @param extraArgs Additional arguments to supply to the zygote process.
     * @return An object that describes the result of the attempt to start the process.
     * @throws ZygoteStartFailedEx if process start failed for any reason
     */
    static CARAPI StartViaZygote(
        /* [in] */ const String& processClass,
        /* [in] */ const String& niceName,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 gid,
        /* [in] */ ArrayOf<Int32>* gids,
        /* [in] */ Int32 debugFlags,
        /* [in] */ Int32 mountExternal,
        /* [in] */ Int32 targetSdkVersion,
        /* [in] */ const String& seInfo,
        /* [in] */ ArrayOf<String>* zygoteArgs,
        /* [out] */ IProcessStartResult** result);

public:
    // State for communicating with elastos zygote process

    static AutoPtr<ILocalSocket> sZygoteSocket;
    static AutoPtr<IDataInputStream> sZygoteInputStream;
    static AutoPtr<IBufferedWriter> sZygoteWriter;

    /** true if previous elastos zygote open failed */
    static Boolean sPreviousZygoteOpenFailed;

    // Added for android app
    static AutoPtr<ILocalSocket> sJavaZygoteSocket;
    static AutoPtr<IDataInputStream> sJavaZygoteInputStream;
    static AutoPtr<IBufferedWriter> sJavaZygoteWriter;

    /** true if previous android zygote open failed */
    static Boolean sPreviousJavaZygoteOpenFailed;

    /** retry interval for opening a zygote socket */
    static const Int32 ZYGOTE_RETRY_MILLIS = 500;

    /** @hide */
    static const Int32 PROC_TERM_MASK = 0xff;
    /** @hide */
    static const Int32 PROC_ZERO_TERM = 0;
    /** @hide */
    static const Int32 PROC_SPACE_TERM = (Int32)' ';
    /** @hide */
    static const Int32 PROC_TAB_TERM = (Int32)'\t';
    /** @hide */
    static const Int32 PROC_COMBINE = 0x100;
    /** @hide */
    static const Int32 PROC_PARENS = 0x200;
    /** @hide */
    static const Int32 PROC_OUT_STRING = 0x1000;
    /** @hide */
    static const Int32 PROC_OUT_LONG = 0x2000;
    /** @hide */
    static const Int32 PROC_OUT_FLOAT = 0x4000;

private:
    static const String ZYGOTE_SOCKET_ELASTOS;
    static const String ZYGOTE_SOCKET_JAVA;

    static Object sLock;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_PROCESS_H__
