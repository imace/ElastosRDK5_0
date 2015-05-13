
#include "provider/CCalendarContractAttendees.h"

namespace Elastos {
namespace Droid {
namespace Provider {

ECode CCalendarContractAttendees::constructor()
{
    return NOERROR;
}

ECode CCalendarContractAttendees::GetCONTENTURI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return CalendarContractAttendees::GetCONTENTURI(uri);
}

ECode CCalendarContractAttendees::Query(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Int64 eventId,
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);

    return CalendarContractAttendees::Query(cr, eventId, projection, cursor);
}

} //Provider
} //Droid
} //Elastos