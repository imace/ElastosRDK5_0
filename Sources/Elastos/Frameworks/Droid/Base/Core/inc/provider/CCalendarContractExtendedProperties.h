
#ifndef __CCALENDARCONTRACTEXTENDEDPROPERTIES_H__
#define __CCALENDARCONTRACTEXTENDEDPROPERTIES_H__

#include "_CCalendarContractExtendedProperties.h"
#include "CalendarContractExtendedProperties.h"

namespace Elastos {
namespace Droid {
namespace Provider {

/**
 * Fields for accessing the Extended Properties. This is a generic set of
 * name/value pairs for use by sync adapters to add extra
 * information to events. There are three writable columns and all three
 * must be present when inserting a new value. They are:
 * <ul>
 * <li>{@link #EVENT_ID}</li>
 * <li>{@link #NAME}</li>
 * <li>{@link #VALUE}</li>
 * </ul>
 */
CarClass(CCalendarContractExtendedProperties)
{
public:
    /**
     * This utility class cannot be instantiated
     */
    CARAPI constructor();
    // TODO: fill out this class when we actually start utilizing extendedproperties
    // in the calendar application.

    /**
     * The content:// style URL for the top-level calendar authority
     */
    CARAPI GetCONTENTURI(
        /* [out] */ IUri** uri);
};

} //Provider
} //Droid
} //Elastos

#endif //__CCALENDARCONTRACTEXTENDEDPROPERTIES_H__