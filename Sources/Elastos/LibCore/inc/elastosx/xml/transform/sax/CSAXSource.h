
#ifndef  __ELASTOSX_XML_TRANSFORM_SAX_CSAXSOURCE_H__
#define  __ELASTOSX_XML_TRANSFORM_SAX_CSAXSOURCE_H__

#include "_Elastosx_Xml_Transform_Sax_CSAXSource.h"
#include "Object.h"

using Elastos::Core::Object;
using Org::Xml::Sax::IXMLReader;
using Org::Xml::Sax::IInputSource;

namespace Elastosx {
namespace Xml {
namespace Transform {
namespace Sax {

/**
 * <p>Acts as an holder for SAX-style Source.</p>
 *
 * <p>Note that XSLT requires namespace support. Attempting to transform an
 * input source that is not
 * generated with a namespace-aware parser may result in errors.
 * Parsers can be made namespace aware by calling the
 * {@link javax.xml.parsers.SAXParserFactory#setNamespaceAware(boolean awareness)} method.</p>
 *
 * @author <a href="mailto:Jeff.Suttor@Sun.com">Jeff Suttor</a>
 * @version $Revision: 446598 $, $Date: 2006-09-15 05:55:40 -0700 (Fri, 15 Sep 2006) $
 */
CarClass(CSAXSource)
    , public Object
    , public ISAXSource
    , public ISource
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    /**
     * <p>Zero-argument default constructor.  If this constructor is used, and
     * no SAX source is set using
     * {@link #setInputSource(InputSource inputSource)} , then the
     * <code>Transformer</code> will
     * create an empty source {@link org.xml.sax.InputSource} using
     * {@link org.xml.sax.InputSource#InputSource() new InputSource()}.</p>
     *
     * @see javax.xml.transform.Transformer#transform(Source xmlSource, Result outputTarget)
     */
    CSAXSource();

    /**
     * Create a <code>SAXSource</code>, using an {@link org.xml.sax.XMLReader}
     * and a SAX InputSource. The {@link javax.xml.transform.Transformer}
     * or {@link javax.xml.transform.sax.SAXTransformerFactory} will set itself
     * to be the reader's {@link org.xml.sax.ContentHandler}, and then will call
     * reader.parse(inputSource).
     *
     * @param reader An XMLReader to be used for the parse.
     * @param inputSource A SAX input source reference that must be non-null
     * and that will be passed to the reader parse method.
     */
    CARAPI constructor(
        /* [in] */ IXMLReader* reader,
        /* [in] */ IInputSource* inputSource);

    /**
     * Create a <code>SAXSource</code>, using a SAX <code>InputSource</code>.
     * The {@link javax.xml.transform.Transformer} or
     * {@link javax.xml.transform.sax.SAXTransformerFactory} creates a
     * reader via {@link org.xml.sax.helpers.XMLReaderFactory}
     * (if setXMLReader is not used), sets itself as
     * the reader's {@link org.xml.sax.ContentHandler}, and calls
     * reader.parse(inputSource).
     *
     * @param inputSource An input source reference that must be non-null
     * and that will be passed to the parse method of the reader.
     */
    CARAPI constructor(
        /* [in] */ IInputSource* inputSource);

    /**
     * Set the XMLReader to be used for the Source.
     *
     * @param reader A valid XMLReader or XMLFilter reference.
     */
    CARAPI SetXMLReader(
        /* [in] */ IXMLReader* reader);

    /**
     * Get the XMLReader to be used for the Source.
     *
     * @return A valid XMLReader or XMLFilter reference, or null.
     */
    CARAPI GetXMLReader(
        /* [out] */ IXMLReader** reader);

    /**
     * Set the SAX InputSource to be used for the Source.
     *
     * @param inputSource A valid InputSource reference.
     */
    CARAPI SetInputSource(
        /* [in] */ IInputSource* inputSource);

    /**
     * Get the SAX InputSource to be used for the Source.
     *
     * @return A valid InputSource reference, or null.
     */
    CARAPI GetInputSource(
        /* [out] */ IInputSource** is);

    /**
     * Set the system identifier for this Source.  If an input source
     * has already been set, it will set the system ID or that
     * input source, otherwise it will create a new input source.
     *
     * <p>The system identifier is optional if there is a byte stream
     * or a character stream, but it is still useful to provide one,
     * since the application can use it to resolve relative URIs
     * and can include it in error messages and warnings (the parser
     * will attempt to open a connection to the URI only if
     * no byte stream or character stream is specified).</p>
     *
     * @param systemId The system identifier as a URI string.
     */
    CARAPI SetSystemId(
        /* [in] */ const String& systemId);

    /**
     * <p>Get the base ID (URI or system ID) from where URIs
     * will be resolved.</p>
     *
     * @return Base URL for the <code>Source</code>, or <code>null</code>.
     */
    CARAPI GetSystemId(
        /* [out] */ String* id);

    /**
     * Attempt to obtain a SAX InputSource object from a Source
     * object.
     *
     * @param source Must be a non-null Source reference.
     *
     * @return An InputSource, or null if Source can not be converted.
     */
    static CARAPI_(AutoPtr<IInputSource>) SourceToInputSource(
        /* [in] */ ISource* source);

private:
    /**
     * The XMLReader to be used for the source tree input. May be null.
     */
    AutoPtr<IXMLReader> mReader;

    /**
     * <p>The SAX InputSource to be used for the source tree input.
     * Should not be <code>null<code>.</p>
     */
    AutoPtr<IInputSource> mInputSource;
};

} // namespace Sax
} // namespace Transform
} // namespace Xml
} // namespace Elastosx

#endif  // __ELASTOSX_XML_TRANSFORM_SAX_CSAXSOURCE_H__