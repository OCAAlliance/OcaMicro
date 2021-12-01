/*  By downloading or using this file, the user agrees to be bound by the terms of the license
 *  agreement located in the LICENSE file in the root of this project
 *  as an original contracting party.
 *
 *  Description         : OcaLiteMediaSinkConnector
 *
 */

// ---- Include system wide include files ----

// ---- FileInfo Macro ----

// ---- Include local include files ----
#include "OcaLiteMediaSinkConnector.h"

// ---- Helper types and constants ----

// ---- Helper functions ----

// ---- Local data ----

// ---- Class Implementation ----

OcaLiteMediaSinkConnector::OcaLiteMediaSinkConnector()
    : ::IOcaLiteMarshal(),
    m_IDInternal(static_cast< ::OcaLiteMediaConnectorID>(0)),
    m_IDExternal(),
    m_connection(),
    m_coding(),
    m_pinCount(static_cast< ::OcaUint16>(0)),
    m_channelPinMap(),
    m_alignmentLevel(),
    m_alignmentGain()
{
}

OcaLiteMediaSinkConnector::OcaLiteMediaSinkConnector(::OcaLiteMediaConnectorID IDInternal, ::OcaLiteString IDExternal, ::OcaLiteMediaConnection connection,
                                                     ::OcaLiteMediaCoding coding, ::OcaUint16 pinCount,
                                                     ::OcaLiteMultiMap< ::OcaUint16, ::OcaLitePortID> channelPinMap,
                                                     ::OcaDBfs alignmentLevel,
                                                     ::OcaDB alignmentGain)
    : ::IOcaLiteMarshal(),
    m_IDInternal(IDInternal),
    m_IDExternal(IDExternal),
    m_connection(connection),
    m_coding(coding),
    m_pinCount(pinCount),
    m_channelPinMap(channelPinMap),
    m_alignmentLevel(alignmentLevel),
    m_alignmentGain(alignmentGain)
{
}

OcaLiteMediaSinkConnector::OcaLiteMediaSinkConnector(const ::OcaLiteMediaSinkConnector& source)
    : ::IOcaLiteMarshal(source),
    m_IDInternal(source.m_IDInternal),
    m_IDExternal(source.m_IDExternal),
    m_connection(source.m_connection),
    m_coding(source.m_coding),
    m_pinCount(source.m_pinCount),
    m_channelPinMap(source.m_channelPinMap),
    m_alignmentLevel(source.m_alignmentLevel),
    m_alignmentGain(source.m_alignmentGain)
{
}

OcaLiteMediaSinkConnector::~OcaLiteMediaSinkConnector()
{
}

::OcaLiteMediaSinkConnector& OcaLiteMediaSinkConnector::operator=(const ::OcaLiteMediaSinkConnector& source)
{
    if (this != &source)
    {
        m_IDInternal = source.m_IDInternal;
        m_IDExternal = source.m_IDExternal;
        m_connection = source.m_connection;
        m_coding = source.m_coding;
        m_pinCount = source.m_pinCount;
        m_channelPinMap = source.m_channelPinMap;
        m_alignmentLevel = source.m_alignmentLevel;
        m_alignmentGain = source.m_alignmentGain;
    }

    return *this;
}

bool OcaLiteMediaSinkConnector::operator==(const ::OcaLiteMediaSinkConnector& rhs) const
{
    bool bResult(m_IDInternal == rhs.m_IDInternal);
    bResult = bResult && (m_IDExternal == rhs.m_IDExternal);
    bResult = bResult && (m_connection == rhs.m_connection);
    bResult = bResult && (m_coding == rhs.m_coding);
    bResult = bResult && (m_pinCount == rhs.m_pinCount);
    bResult = bResult && (m_channelPinMap == rhs.m_channelPinMap);
    bResult = bResult && ::CompareValue< ::OcaDBfs>(m_alignmentLevel, rhs.m_alignmentLevel);
    bResult = bResult && ::CompareValue< ::OcaDB>(m_alignmentGain, rhs.m_alignmentGain);
    return bResult;
}

bool OcaLiteMediaSinkConnector::operator!=(const ::OcaLiteMediaSinkConnector& rhs) const
{
    return !(operator==(rhs));
}

void OcaLiteMediaSinkConnector::Marshal(::OcaUint8** destination, const ::IOcaLiteWriter& writer) const
{
    writer.Write(m_IDInternal, destination);
    m_IDExternal.Marshal(destination, writer);
    m_connection.Marshal(destination, writer);
    m_coding.Marshal(destination, writer);
    writer.Write(m_pinCount, destination);
    m_channelPinMap.Marshal(destination, writer);
    writer.Write(m_alignmentLevel, destination);
    writer.Write(m_alignmentGain, destination);
}

bool OcaLiteMediaSinkConnector::Unmarshal(::OcaUint32& bytesLeft, const ::OcaUint8** source, const ::IOcaLiteReader& reader)
{
    bool result(reader.Read(bytesLeft, source, m_IDInternal));
    result = result && m_IDExternal.Unmarshal(bytesLeft, source, reader);
    result = result && m_connection.Unmarshal(bytesLeft, source, reader);
    result = result && m_coding.Unmarshal(bytesLeft, source, reader);
    result = result && reader.Read(bytesLeft, source, m_pinCount);
    result = result && m_channelPinMap.Unmarshal(bytesLeft, source, reader);
    result = result && reader.Read(bytesLeft, source, m_alignmentLevel);
    result = result && reader.Read(bytesLeft, source, m_alignmentGain);
    if (!result)
    {
        (*this) = ::OcaLiteMediaSinkConnector();
    }

    return result;
}

::OcaUint32 OcaLiteMediaSinkConnector::GetSize(const ::IOcaLiteWriter& writer) const
{
    ::OcaUint32 length(writer.GetSize(m_IDInternal));
    length += m_IDExternal.GetSize(writer);
    length += m_connection.GetSize(writer);
    length += m_coding.GetSize(writer);
    length += writer.GetSize(m_pinCount);
    length += m_channelPinMap.GetSize(writer);
    length += writer.GetSize(m_alignmentLevel);
    length += writer.GetSize(m_alignmentGain);
    return length;
}
