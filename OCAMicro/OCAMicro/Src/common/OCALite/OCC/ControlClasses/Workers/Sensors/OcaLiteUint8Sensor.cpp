/*  By downloading or using this file, the user agrees to be bound by the terms of the license
 *  agreement located in the LICENSE file in the root of this project
 *  as an original contracting party.
 */

/*
 *  Description         : OcaLiteUint8Sensor
 *
 */
// ---- Include system wide include files ----
#include <assert.h>
#include <OCC/ControlDataTypes/OcaLiteMethodID.h>
#include <OCC/ControlDataTypes/OcaLitePropertyChangedEventData.h>
//#include <OCF/OcaLiteMessageParameters.h>
#include <OCF/OcaLiteCommandHandler.h>
#include <OCF/Messages/OcaLiteMessageResponse.h>

// ---- FileInfo Macro ----

// ---- Include local include files ----
#include "OcaLiteUint8Sensor.h"

// ---- Helper types and constants ----

static const ::OcaUint16        classID[]   = {OCA_UINT8_SENSOR_CLASSID};
const ::OcaLiteClassID          OcaLiteUint8Sensor::CLASS_ID(static_cast< ::OcaUint16>(sizeof(classID) / sizeof(classID[0])), classID);

/** Defines the version increment of this class compared to its base class. */
#define CLASS_VERSION_INCREMENT     0

// ---- Helper functions ----

// ---- Local data ----

// ---- Class Implementation ----

OcaLiteUint8Sensor::OcaLiteUint8Sensor(::OcaONo objectNumber,
                                           ::OcaBoolean lockable,
                                           const ::OcaLiteString& role,
                                           const ::OcaLiteList< ::OcaLitePort>& ports,
                                           ::OcaUint8 minReading,
                                           ::OcaUint8 maxReading)
    : ::OcaLiteBasicSensor(objectNumber, lockable, role, ports),
      m_minReading(minReading),
      m_maxReading(maxReading)
{
    assert(minReading <= maxReading);
}

::OcaLiteStatus OcaLiteUint8Sensor::GetReading(::OcaUint8& reading, ::OcaUint8& minReading, ::OcaUint8& maxReading) const
{
    minReading = m_minReading;
    maxReading = m_maxReading;
    ::OcaLiteStatus rc(GetReadingValue(reading));

    return rc;
}

    /**
     * Gets the value of the limit properties.
     *
     * @param[out]   minReading          Lower limit of the sensor
     * @param[out]   maxReading          Upper limit of the sensor
     * @return Indicates whether the operation succeeded.
     */
::OcaLiteStatus  OcaLiteUint8Sensor::GetLimits(::OcaUint8& minReading, ::OcaUint8& maxReading) const
{
    ::OcaLiteStatus rc(OCASTATUS_OK);

    minReading = m_minReading;
    maxReading = m_maxReading;

    return rc;
}

::OcaLiteStatus OcaLiteUint8Sensor::Execute(const ::IOcaLiteReader& reader, const ::IOcaLiteWriter& writer, ::OcaSessionID sessionID, const ::OcaLiteMethodID& methodID,
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
                        ::OcaUint8 reading;
                        ::OcaUint8 minReading;
                        ::OcaUint8 maxReading;
                        rc = GetReading(reading, minReading, maxReading);
                        if (OCASTATUS_OK == rc)
                        {
                            ::OcaUint32 responseSize(::GetSizeValue< ::OcaUint8>(static_cast< ::OcaUint8>(3), writer) +
                                                     ::GetSizeValue< ::OcaUint8>(reading, writer) +
                                                     ::GetSizeValue< ::OcaUint8>(minReading, writer) +
                                                     ::GetSizeValue< ::OcaUint8>(maxReading, writer));
                            responseBuffer = ::OcaLiteCommandHandler::GetInstance().GetResponseBuffer(responseSize);
                            if (NULL != responseBuffer)
                            {
                                ::OcaUint8* pResponse(responseBuffer);
                                writer.Write(static_cast< ::OcaUint8>(3/*NrParameters*/), &pResponse);
                                ::MarshalValue< ::OcaUint8>(reading, &pResponse, writer);
                                ::MarshalValue< ::OcaUint8>(minReading, &pResponse, writer);
                                ::MarshalValue< ::OcaUint8>(maxReading, &pResponse, writer);

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
            rc = OcaLiteBasicSensor::Execute(reader, writer, sessionID, methodID, parametersSize, parameters, response);
        }
    }
    else
    {
        rc = OCASTATUS_LOCKED;
    }

    return rc;
}

::OcaClassVersionNumber OcaLiteUint8Sensor::GetClassVersion() const
{
    return static_cast< ::OcaClassVersionNumber>(static_cast<int>(OcaLiteBasicSensor::GetClassVersion()) + CLASS_VERSION_INCREMENT);
}

void OcaLiteUint8Sensor::ReadingChanged(::OcaUint8 reading)
{
    ::OcaLitePropertyID propertyID(CLASS_ID.GetFieldCount(), static_cast< ::OcaUint16>(OCA_PROP_READING));
    ::OcaLitePropertyChangedEventData< ::OcaUint8> eventData(GetObjectNumber(),
                                                        propertyID,
                                                        reading,
                                                        OCAPROPERTYCHANGETYPE_CURRENT_CHANGED);
    PropertyChanged(eventData, propertyID);
}
