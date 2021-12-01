/*  By downloading or using this file, the user agrees to be bound by the terms of the license 
 *  agreement located in the LICENSE file in the root of this project
 *  as an original contracting party.
 */

/*
 *  Description         : OcaLiteLevelSensor
 *
 */
// ---- Include system wide include files ----
#include <assert.h>


// ---- FileInfo Macro ----

// ---- Include local include files ----
#include "OcaLiteLevelSensor.h"

// ---- Helper types and constants ----

static const ::OcaUint16        classID[]   = {OCA_LEVEL_SENSOR_CLASSID};
const ::OcaLiteClassID          OcaLiteLevelSensor::CLASS_ID(static_cast< ::OcaUint16>(sizeof(classID) / sizeof(classID[0])), classID);

/** Defines the version increment of this class compared to its base class. */
#define CLASS_VERSION_INCREMENT     0

// ---- Helper functions ----

// ---- Local data ----

// ---- Class Implementation ----

// ---- Helper functions ----

// ---- Local data ----

// ---- Class Implementation ----
OcaLiteLevelSensor::OcaLiteLevelSensor(::OcaONo objectNumber, ::OcaBoolean lockable, const ::OcaLiteString& role, const ::OcaLiteList< ::OcaLitePort>& ports, ::OcaDB minRefLevel, ::OcaDB maxRefLevel)
    : ::OcaLiteSensor(objectNumber, lockable, role, ports),
      m_minReading(minRefLevel),
      m_maxReading(maxRefLevel)
{
    assert(minRefLevel <= maxRefLevel);
}

::OcaLiteStatus OcaLiteLevelSensor::GetReading(::OcaDB& reading, ::OcaDB& minReading, ::OcaDB& maxReading) const
{
    minReading = m_minReading;
    maxReading = m_maxReading;
    return GetReadingValue(reading);
}

::OcaLiteStatus OcaLiteLevelSensor::Execute(const ::IOcaLiteReader& reader, const ::IOcaLiteWriter& writer, ::OcaSessionID sessionID, const ::OcaLiteMethodID& methodID,
                                    ::OcaUint32 parametersSize, const ::OcaUint8* parameters, ::OcaUint8** response)
{
    ::OcaLiteStatus rc(OCASTATUS_PARAMETER_ERROR);
    if (!IsLocked(sessionID))
    {
        if (methodID.GetDefLevel() == CLASS_ID.GetFieldCount())
        {
            ::OcaUint8* responseBuffer(NULL);
            const ::OcaUint8* pCmdParameters(parameters);
            ::OcaUint32 bytesLeft(parametersSize);

            switch (methodID.GetMethodIndex())
            {
            case GET_READING:
                {
                    ::OcaUint8 numberOfParameters(0);
                    if (reader.Read(bytesLeft, &pCmdParameters, numberOfParameters) &&
                        (0 == numberOfParameters))
                    {
                        ::OcaDB reading;
                        ::OcaDB minReading;
                        ::OcaDB maxReading;
                        rc = GetReading(reading, minReading, maxReading);
                        if (OCASTATUS_OK == rc)
                        {
                            ::OcaUint32 responseSize(::GetSizeValue< ::OcaUint8>(static_cast< ::OcaUint8>(3), writer) + 
                                                     ::GetSizeValue< ::OcaDB>(reading, writer) + 
                                                     ::GetSizeValue< ::OcaDB>(minReading, writer) + 
                                                     ::GetSizeValue< ::OcaDB>(maxReading, writer));
                            responseBuffer = ::OcaLiteCommandHandler::GetInstance().GetResponseBuffer(responseSize);
                            if (NULL != responseBuffer)
                            {
                                ::OcaUint8* pResponse(responseBuffer);
                                writer.Write(static_cast< ::OcaUint8>(3/*NrParameters*/), &pResponse);
                                ::MarshalValue< ::OcaDB>(reading, &pResponse, writer);
                                ::MarshalValue< ::OcaDB>(minReading, &pResponse, writer);
                                ::MarshalValue< ::OcaDB>(maxReading, &pResponse, writer);

                                *response = responseBuffer;
                            }
                            else
                            {
                                rc = OCASTATUS_BUFFER_OVERFLOW;
                            }
                        }
                    }
                }
                break;
            default:
                rc = OCASTATUS_BAD_METHOD;
                break;
            }
        }
        else
        {
            // Should be executed on higher level
            rc = OcaLiteSensor::Execute(reader, writer, sessionID, methodID, parametersSize, parameters, response);
        }
    }
    else
    {
        rc = OCASTATUS_LOCKED;
    }

    return rc;
}

::OcaClassVersionNumber OcaLiteLevelSensor::GetClassVersion() const
{
    return static_cast< ::OcaClassVersionNumber>(static_cast<int>(OcaLiteSensor::GetClassVersion()) + CLASS_VERSION_INCREMENT);
}

void OcaLiteLevelSensor::ReadingChanged(::OcaDB reading)
{
    ::OcaLitePropertyID propertyID(CLASS_ID.GetFieldCount(), static_cast< ::OcaUint16>(OCA_PROP_READING));
    ::OcaLitePropertyChangedEventData< ::OcaDB> eventData(GetObjectNumber(),
                                                      propertyID,
                                                      reading,
                                                      OCAPROPERTYCHANGETYPE_CURRENT_CHANGED);
    PropertyChanged(eventData, propertyID);
}
