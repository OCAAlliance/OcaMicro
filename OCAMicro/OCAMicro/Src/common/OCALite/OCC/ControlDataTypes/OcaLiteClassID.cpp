/*  By downloading or using this file, the user agrees to be bound by the terms of the license 
 *  agreement located at http://ocaalliance.com/EULA as an original contracting party.
 */

/*
 *  Description         : OcaLiteClassID
 *
 */

// ---- Include system wide include files ----
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <assert.h>
#include <OCF/IOcaLiteReader.h>
#include <OCF/IOcaLiteWriter.h>

// ---- FileInfo Macro ----

// ---- Include local include files ----
#include "OcaLiteClassID.h"

// ---- Helper types and constants ----

// ---- Helper functions ----

// ---- Local data ----

// ---- Class Implementation ----

OcaLiteClassID::OcaLiteClassID()
    : ::IOcaLiteMarshal(),
      m_fieldCount(static_cast< ::OcaUint16>(0)),
      m_defLevel(static_cast< ::OcaUint16>(0))
{
    ::memset(m_fields, 0, sizeof(m_fields));
}

OcaLiteClassID::OcaLiteClassID(::OcaUint16 fieldCount, const ::OcaUint16* fields)
    : ::IOcaLiteMarshal(),
      m_fieldCount(fieldCount),
      m_defLevel(DetermineDefLevel(fieldCount, fields))
{
    assert(AreFieldsValid(fieldCount, fields));
    assert(static_cast< ::OcaUint16>(0) < fieldCount);
    assert(fieldCount <= static_cast< ::OcaUint16>(OCA_CLASS_ID_SIZE));
    assert(static_cast< ::OcaUint16>(1) == fields[0]);

    // Copy the fields to the member
    ::memcpy(m_fields, fields, static_cast<size_t>(fieldCount) * sizeof(::OcaUint16));
}

OcaLiteClassID::OcaLiteClassID(const ::OcaLiteClassID& source)
    : ::IOcaLiteMarshal(source),
      m_fieldCount(source.m_fieldCount),
      m_defLevel(source.m_defLevel)
{
    // Copy the fields to the member
    ::memcpy(m_fields, source.m_fields, static_cast<size_t>(m_fieldCount) * sizeof(::OcaUint16));
}

#ifdef _WIN32
OcaLiteClassID::OcaLiteClassID(const std::string& source)
{
    ::memset(m_fields, 0, sizeof(m_fields));
    m_fieldCount = static_cast<int>(std::count(source.begin(), source.end(), '.') + 1);

    size_t index = 0;
    const char* str(source.c_str());
    while (*str) {
        if (isdigit((unsigned char)*str)) {
            m_fields[index] *= 10;
            m_fields[index] += *str - '0';
        }
        else {
            index++;
        }
        str++;
    }
}
#endif

OcaLiteClassID::~OcaLiteClassID()
{
}

bool OcaLiteClassID::GetParent(::OcaLiteClassID& parent) const
{
    bool result(false);

    if ((this != &parent) &&
        (m_fieldCount > static_cast< ::OcaUint16>(1)))
    {
        // Calculate the new field count and definition level
        parent.m_fieldCount = static_cast<::OcaUint16>(m_fieldCount - static_cast<::OcaUint16>(1));
        if (parent.m_fieldCount >= static_cast<::OcaUint16>(4))
        {
            if (OCA_CLASS_ID_PROPRIETARY_CLASS_FIELD == m_fields[static_cast<size_t>(parent.m_fieldCount) - 3])
            {
                // It is a proprietary class in the AES70-2018 format, so skip the proprietary identifier to get the real parent
                parent.m_fieldCount = static_cast<::OcaUint16>(parent.m_fieldCount - static_cast<::OcaUint16>(3));
            }
        }
        parent.m_defLevel = DetermineDefLevel(parent.m_fieldCount, m_fields);

        // Copy the fields
        ::memset(parent.m_fields, 0, sizeof(parent.m_fields));
        ::memcpy(parent.m_fields, m_fields, static_cast<size_t>(parent.m_fieldCount) * sizeof(::OcaUint16));
        assert(AreFieldsValid(parent.m_fieldCount, parent.m_fields));

        result = true;
    }

    return result;
}

::OcaBoolean OcaLiteClassID::IsValid() const
{
    return static_cast< ::OcaBoolean>(AreFieldsValid(m_fieldCount, m_fields));
}

::OcaLiteClassID& OcaLiteClassID::operator=(const ::OcaLiteClassID& source)
{
    if (this != &source)
    {
        m_fieldCount = source.m_fieldCount;
        m_defLevel = source.m_defLevel;
        ::memcpy(m_fields, source.m_fields, static_cast<size_t>(m_fieldCount) * sizeof(::OcaUint16));
    }

    return *this;
}

bool OcaLiteClassID::operator==(const ::OcaLiteClassID& rhs) const
{
    bool result(m_fieldCount == rhs.m_fieldCount);
    // No need to check the definition level, as this is implied by the fields
    result = result && (0 == ::memcmp(m_fields, rhs.m_fields, static_cast<size_t>(m_fieldCount) * sizeof(::OcaUint16)));

    return result;
}

bool OcaLiteClassID::operator!=(const ::OcaLiteClassID& rhs) const
{
    return !(operator==(rhs));
}

bool OcaLiteClassID::operator<(const ::OcaLiteClassID& rhs) const
{
    bool bLessThan(m_fieldCount < rhs.m_fieldCount);

    if (!bLessThan &&
        (m_fieldCount == rhs.m_fieldCount))
    {
        bLessThan = true;
        for (::OcaUint16 field(static_cast< ::OcaUint16>(0)); bLessThan && (field < m_fieldCount); field++)
        {
            bLessThan = (m_fields[field] < rhs.m_fields[field]);
        }
    }

    return bLessThan;
}

bool OcaLiteClassID::operator<=(const ::OcaLiteClassID& rhs) const
{
    return !(operator>(rhs));
}

bool OcaLiteClassID::operator>(const ::OcaLiteClassID& rhs) const
{
    bool bGreaterThan(m_fieldCount > rhs.m_fieldCount);

    if (!bGreaterThan &&
        (m_fieldCount == rhs.m_fieldCount))
    {
        bGreaterThan = true;
        for (::OcaUint16 field(static_cast< ::OcaUint16>(0)); bGreaterThan && (field < m_fieldCount); field++)
        {
            bGreaterThan = (m_fields[field] > rhs.m_fields[field]);
        }
    }

    return bGreaterThan;
}

bool OcaLiteClassID::operator>=(const ::OcaLiteClassID& rhs) const
{
    return !(operator<(rhs));
}

void OcaLiteClassID::Marshal(::OcaUint8** destination, const ::IOcaLiteWriter& writer) const
{
    assert(NULL != m_fields);

    writer.Write(m_fieldCount, destination);
    for (::OcaUint16 i(static_cast< ::OcaUint16>(0)); i < m_fieldCount; i++)
    {
        writer.Write(m_fields[i], destination);
    }
}

bool OcaLiteClassID::Unmarshal(::OcaUint32& bytesLeft, const ::OcaUint8** source, const ::IOcaLiteReader& reader)
{
    m_fieldCount = static_cast< ::OcaUint16>(0);

    bool result(reader.Read(bytesLeft, source, m_fieldCount));
    result = result && (m_fieldCount > static_cast< ::OcaUint16>(0));
    result = result && (m_fieldCount <= static_cast< ::OcaUint16>(OCA_CLASS_ID_SIZE));

    if (result)
    {
        for (::OcaUint16 i(static_cast< ::OcaUint16>(0)); (i < m_fieldCount) && result; i++)
        {
            result = result && reader.Read(bytesLeft, source, m_fields[i]);
        }
        m_defLevel = DetermineDefLevel(m_fieldCount, m_fields);
    }
    result = result && (static_cast< ::OcaUint16>(1) == m_fields[0]);

    // Clear all unmarshalled data if unmarshalling failed
    if (!result)
    {
        m_fieldCount = static_cast< ::OcaUint16>(0);
        m_defLevel = static_cast< ::OcaUint16>(0);
    }

    return result;
}

::OcaUint32 OcaLiteClassID::GetSize(const ::IOcaLiteWriter& writer) const
{
    assert(NULL != m_fields);

    ::OcaUint32 length(writer.GetSize(m_fieldCount));
    for (::OcaUint16 i(static_cast< ::OcaUint16>(0)); i < m_fieldCount; i++)
    {
        length += writer.GetSize(m_fields[i]);
    }

    return length;
}

bool OcaLiteClassID::AreFieldsValid(::OcaUint16 fieldCount, const ::OcaUint16* fields)
{
    bool result(fieldCount > static_cast< ::OcaUint16>(0));
    result = result && ((NULL != fields) && (static_cast< ::OcaUint16>(1) == fields[0]));

    // Check if any derivate of a proprietary (test) class also is a proprietary (test) class
    bool proprietaryClass(false);
    bool testClass(false);
    bool proprietaryFieldPresent(false);
    for (int i(0); result && (i < static_cast<int>(fieldCount)); i++)
    {
        assert(NULL != fields);

        if (fields[i] != OCA_CLASS_ID_PROPRIETARY_CLASS_FIELD)
        {
            if (proprietaryClass &&
                !proprietaryFieldPresent)
            {
                if ((fields[i] & OCA_CLASS_ID_PROPRIETARY_CLASS_FIELD_MASK) != OCA_CLASS_ID_PROPRIETARY_CLASS_FIELD_MASK)
                {
                    // The field should also indicate it's a proprietary class
                    result = false;
                }
                else
                {
                    if (testClass &&
                        ((fields[i] & OCA_CLASS_ID_PROPRIETARY_TEST_CLASS_FIELD_MASK) != OCA_CLASS_ID_PROPRIETARY_TEST_CLASS_FIELD_MASK))
                    {
                        // The field should also indicate it's a proprietary test class
                        result = false;
                    }
                }
            }
        }
        else
        {
            // There should be at least three more fields (2 for the OUI and 1 additional field)
            result = ((static_cast<int>(fieldCount) - i) >= 3) && !proprietaryClass;
            proprietaryFieldPresent = true;
        }

        proprietaryClass = ((fields[i] & OCA_CLASS_ID_PROPRIETARY_CLASS_FIELD_MASK) == OCA_CLASS_ID_PROPRIETARY_CLASS_FIELD_MASK) && (fields[i] != OCA_CLASS_ID_PROPRIETARY_CLASS_FIELD);
        testClass = ((fields[i] & OCA_CLASS_ID_PROPRIETARY_TEST_CLASS_FIELD_MASK) == OCA_CLASS_ID_PROPRIETARY_TEST_CLASS_FIELD_MASK) && (fields[i] != OCA_CLASS_ID_PROPRIETARY_CLASS_FIELD);
    }

    static_cast<void>(testClass);
    static_cast<void>(proprietaryFieldPresent);

    return result;
}

::OcaUint16 OcaLiteClassID::DetermineDefLevel(::OcaUint16 fieldCount, const ::OcaUint16* fields)
{
    ::OcaUint16 result(static_cast< ::OcaUint16>(0));

    if (AreFieldsValid(fieldCount, fields))
    {
        assert(NULL != fields);

        result = fieldCount;

        // If there is the proprietary field indicator, adjust the definition level
        for (int i(0); i < static_cast<int>(fieldCount); i++)
        {
            if (OCA_CLASS_ID_PROPRIETARY_CLASS_FIELD == fields[i])
            {
                assert(fieldCount >= static_cast< ::OcaUint16>(5));
                result = static_cast< ::OcaUint16>(fieldCount - static_cast< ::OcaUint16>(3));
                break;
            }
        }
    }

    return result;
}

