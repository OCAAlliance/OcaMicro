/*  By downloading or using this file, the user agrees to be bound by the terms of the license 
 *  agreement located at http://ocaalliance.com/EULA as an original contracting party.
 */

/*
 *  Description         : The OcaLiteNetworkManager.
 *
 */

// ---- Include system wide include files ----
#include <OCC/ControlClasses/Agents/OcaLiteNetwork.h>
#include <OCC/ControlClasses/Networks/OcaLiteMediaTransportNetwork.h>
#include <OCC/ControlClasses/Workers/BlocksAndMatrices/OcaLiteBlock.h>
#include <OCC/ControlDataTypes/OcaLiteMethodID.h>
#include <OCC/ControlDataTypes/OcaLitePropertyChangedEventData.h>
#include <OCF/OcaLiteCommandHandler.h>
#include <OCF/Messages/OcaLiteMessageResponse.h>

// ---- Include local include files ----
#include "OcaLiteNetworkManager.h"

// ---- Helper types and constants ----

static const ::OcaUint16        classID[]   = {OCA_NETWORKMANAGER_CLASSID};
const ::OcaLiteClassID          OcaLiteNetworkManager::CLASS_ID(static_cast< ::OcaUint16>(sizeof(classID) / sizeof(classID[0])), classID);

/** Defines the version increment of this class compared to its base class. */
#define CLASS_VERSION_INCREMENT     0

// ---- Helper functions ----

// ---- Local data ----

// ---- Class Implementation ----

::OcaLiteNetworkManager* OcaLiteNetworkManager::m_pSingleton(NULL);
const ::OcaONo OcaLiteNetworkManager::OBJECT_NUMBER(static_cast< ::OcaONo>(6));

OcaLiteNetworkManager::OcaLiteNetworkManager()
  : ::OcaLiteManager(OBJECT_NUMBER, ::OcaLiteString("NetworkManager"), ::OcaLiteString("NetworkManager")),
     m_networks(),
     m_streamNetwork(OCA_INVALID_ONO),
     m_mediaTransportNetwork(OCA_INVALID_ONO)
{
}

OcaLiteNetworkManager::~OcaLiteNetworkManager()
{
}

::OcaLiteNetworkManager& OcaLiteNetworkManager::GetInstance()
{
    if (NULL == OcaLiteNetworkManager::m_pSingleton)
    {
        OcaLiteNetworkManager::m_pSingleton = new ::OcaLiteNetworkManager;
    }

    return *OcaLiteNetworkManager::m_pSingleton;
}

void OcaLiteNetworkManager::FreeInstance()
{
    if (OcaLiteNetworkManager::m_pSingleton != NULL)
    {
        delete m_pSingleton;
        m_pSingleton = NULL;
    }
}

::OcaLiteStatus OcaLiteNetworkManager::GetNetworks(::OcaLiteList< ::OcaONo>& networks) const
{
    networks = m_networks;
    return OCASTATUS_OK;
}

::OcaLiteStatus OcaLiteNetworkManager::GetStreamNetworks(::OcaLiteList< ::OcaONo>& networks) const
{
    networks.Clear();
    if (OCA_INVALID_ONO != m_streamNetwork)
    {
        networks.Add(m_streamNetwork);
    }
    return OCASTATUS_OK;
}

::OcaLiteStatus OcaLiteNetworkManager::GetMediaTransportNetworks(::OcaLiteList< ::OcaONo>& networks) const
{
    networks.Clear();
    if (OCA_INVALID_ONO != m_mediaTransportNetwork)
    {
        networks.Add(m_mediaTransportNetwork);
    }
    return OCASTATUS_OK;
}

::OcaLiteNetwork* OcaLiteNetworkManager::GetNetwork(::OcaONo networkONo) const
{
    ::OcaLiteNetwork* pOcaLiteNetwork(NULL);

    if (OCA_INVALID_ONO != networkONo)
    {
        ::OcaLiteRoot* pOcaRoot(OcaLiteBlock::GetRootBlock().GetObject(networkONo));
        if (NULL != pOcaRoot)
        {
            pOcaLiteNetwork = static_cast< ::OcaLiteNetwork*>(pOcaRoot);
        }
    }

    return pOcaLiteNetwork;
}

::OcaBoolean OcaLiteNetworkManager::AddNetwork(const ::OcaLiteAgent& network)
{
    ::OcaBoolean bSuccess(static_cast< ::OcaBoolean>(false));

    if (!m_networks.Contains(network.GetObjectNumber()))
    {
        m_networks.Add(network.GetObjectNumber());
        bSuccess = static_cast< ::OcaBoolean>(true);
        // Don't send a property changed event. We assume this never happens in an online device.
    }

    return bSuccess;
}

void OcaLiteNetworkManager::RemoveNetwork(const ::OcaLiteAgent& network)
{
    if (m_networks.Contains(network.GetObjectNumber()))
    {
        m_networks.RemoveElement(network.GetObjectNumber());

        // Don't send a property changed event. We assume this never happens in an online device.
    }
}

::OcaBoolean OcaLiteNetworkManager::AddStreamNetwork(const ::OcaLiteAgent& network)
{
    ::OcaBoolean bSuccess(static_cast< ::OcaBoolean>(false));

    if (OCA_INVALID_ONO == m_streamNetwork)
    {
        m_streamNetwork = network.GetObjectNumber();
        bSuccess = static_cast< ::OcaBoolean>(true);

        // Don't send a property changed event. We assume this never happens in an online device.
    }

    return bSuccess;
}

void OcaLiteNetworkManager::RemoveStreamNetwork(const ::OcaLiteAgent& network)
{
    if (m_streamNetwork == network.GetObjectNumber())
    {
        m_streamNetwork = OCA_INVALID_ONO;

        // Don't send a property changed event. We assume this never happens in an online device.
    }
}

::OcaBoolean OcaLiteNetworkManager::AddMediaTransportNetwork(const ::OcaLiteMediaTransportNetwork& network)
{
    ::OcaBoolean bSuccess(static_cast< ::OcaBoolean>(false));

    if (OCA_INVALID_ONO == m_mediaTransportNetwork)
    {
        m_mediaTransportNetwork = network.GetObjectNumber();
        bSuccess = static_cast< ::OcaBoolean>(true);

        // Don't send a property changed event. We assume this never happens in an online device.
    }

    return bSuccess;
}


void OcaLiteNetworkManager::RemoveMediaTransportNetwork(const ::OcaLiteMediaTransportNetwork& network)
{
    if (m_mediaTransportNetwork == network.GetObjectNumber())
    {
        m_mediaTransportNetwork = OCA_INVALID_ONO;

        // Don't send a property changed event. We assume this never happens in an online device.
    }
}

::OcaLiteStatus OcaLiteNetworkManager::Execute(const ::IOcaLiteReader& reader, const ::IOcaLiteWriter& writer, ::OcaSessionID sessionID, const ::OcaLiteMethodID& methodID,
                                               ::OcaUint32 parametersSize, const ::OcaUint8* parameters, ::OcaUint8** response)
{
    ::OcaLiteStatus rc(OCASTATUS_OK);

    if (!IsLocked(sessionID))
    {
        if (methodID.GetDefLevel() == CLASS_ID.GetFieldCount())
        {
            ::OcaUint8* responseBuffer(NULL);
            const ::OcaUint8* pCmdParameters(parameters);
            ::OcaUint32 bytesLeft(parametersSize);

            switch (methodID.GetMethodIndex())
            {
            case GET_NETWORKS:
                {
                    ::OcaUint8 numberOfParameters(0);
                    if (reader.Read(bytesLeft, &pCmdParameters, numberOfParameters) &&
                        (0 == numberOfParameters))
                    {
                        ::OcaLiteList< ::OcaONo> networks;
                        rc = GetNetworks(networks);
                        if (OCASTATUS_OK == rc)
                        {
                            ::OcaUint32 responseSize(::GetSizeValue< ::OcaUint8>(static_cast< ::OcaUint8>(1), writer) +
                                                     networks.GetSize(writer));
                            responseBuffer = ::OcaLiteCommandHandler::GetInstance().GetResponseBuffer(responseSize);
                            if (NULL != responseBuffer)
                            {
                                ::OcaUint8* pResponse(responseBuffer);
                                writer.Write(static_cast< ::OcaUint8>(1/*NrParameters*/), &pResponse);
                                networks.Marshal(&pResponse, writer);

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
            case GET_STREAM_NETWORKS:
                {
                    ::OcaUint8 numberOfParameters(0);
                    if (reader.Read(bytesLeft, &pCmdParameters, numberOfParameters) &&
                        (0 == numberOfParameters))
                    {
                        ::OcaLiteList< ::OcaONo> networks;
                        rc = GetStreamNetworks(networks);
                        if (OCASTATUS_OK == rc)
                        {
                            ::OcaUint32 responseSize(::GetSizeValue< ::OcaUint8>(static_cast< ::OcaUint8>(1), writer) +
                                                     networks.GetSize(writer));
                            responseBuffer = ::OcaLiteCommandHandler::GetInstance().GetResponseBuffer(responseSize);
                            if (NULL != responseBuffer)
                            {
                                ::OcaUint8* pResponse(responseBuffer);
                                writer.Write(static_cast< ::OcaUint8>(1/*NrParameters*/), &pResponse);
                                networks.Marshal(&pResponse, writer);

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
            case GET_MEDIA_TRANSPORT_NETWORKS:
                {
                    ::OcaUint8 numberOfParameters(0);
                    if (reader.Read(bytesLeft, &pCmdParameters, numberOfParameters) &&
                        (0 == numberOfParameters))
                    {
                        ::OcaLiteList< ::OcaONo> networks;
                        rc = GetMediaTransportNetworks(networks);
                        if (OCASTATUS_OK == rc)
                        {
                            ::OcaUint32 responseSize(::GetSizeValue< ::OcaUint8>(static_cast< ::OcaUint8>(1), writer) +
                                networks.GetSize(writer));
                            responseBuffer = ::OcaLiteCommandHandler::GetInstance().GetResponseBuffer(responseSize);
                            if (NULL != responseBuffer)
                            {
                                ::OcaUint8* pResponse(responseBuffer);
                                writer.Write(static_cast< ::OcaUint8>(1/*NrParameters*/), &pResponse);
                                networks.Marshal(&pResponse, writer);

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
            case GET_CONTROL_NETWORKS:
                {
                    ::OcaUint8 numberOfParameters(0);
                    if (reader.Read(bytesLeft, &pCmdParameters, numberOfParameters) &&
                        (0 == numberOfParameters))
                    {
                        // Always return an empty list, OCALite has no awareness of control networks at this time.
                        ::OcaLiteList< ::OcaONo> networks;
                        rc = OCASTATUS_OK;
 
                        ::OcaUint32 responseSize(::GetSizeValue< ::OcaUint8>(static_cast< ::OcaUint8>(1), writer) +
                            networks.GetSize(writer));
                        responseBuffer = ::OcaLiteCommandHandler::GetInstance().GetResponseBuffer(responseSize);
                        if (NULL != responseBuffer)
                        {
                            ::OcaUint8* pResponse(responseBuffer);
                            writer.Write(static_cast< ::OcaUint8>(1/*NrParameters*/), &pResponse);
                            networks.Marshal(&pResponse, writer);

                            *response = responseBuffer;
                        }
                        else
                        {
                            rc = OCASTATUS_BUFFER_OVERFLOW;
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
            rc = OcaLiteManager::Execute(reader, writer, sessionID, methodID, parametersSize, parameters, response);
        }
    }
    else
    {
        rc = OCASTATUS_LOCKED;
    }

    return rc;
}

::OcaClassVersionNumber OcaLiteNetworkManager::GetClassVersion() const
{
    return static_cast< ::OcaClassVersionNumber>(static_cast<int>(OcaLiteManager::GetClassVersion()) + CLASS_VERSION_INCREMENT);
}
