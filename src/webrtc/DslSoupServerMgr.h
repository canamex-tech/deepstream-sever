/*
The MIT License

Copyright (c) 2019-2021, Prominence AI, Inc.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in-
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef _DSL_SOUP_SERVER_H
#define _DSL_SOUP_SERVER_H

#include <gst/sdp/sdp.h>
#include <libsoup/soup.h>
#include <json-glib/json-glib.h>

#include "Dsl.h"
#include "DslApi.h"

#define GST_USE_UNSTABLE_API
#include <gst/webrtc/webrtc.h>

namespace DSL
{
    class SignalingTransceiver
    {
    public:
        /**
         * @brief Ctor for the SignalingTransceiver class
         */
        SignalingTransceiver();

        /**
         * @brief Dtor for the SignalingTransceiver class
         */
        ~SignalingTransceiver();

        /**
         * @brief Gets the current connection for the SignalingTransceiver.
         * @return pointer to Websocket Connection, NULL when not connected.
         */
        virtual const SoupWebsocketConnection* GetConnection();

        /**
         * @brief Sets the current connection for the SignalingTransceiver.
         * @param[in] pConnection pointer to Websocket Connection object, 
         */
        virtual void SetConnection(SoupWebsocketConnection* pConnection);

        virtual bool IsConnected();

        /**
         * @brief Clears the current connection for the SignalingTransceiver.
         */
        virtual void ClearConnection();

        /**
         * @brief Called when a Websocket is closed
         * @param[in] pConnection unique connection that closed
         */
        virtual void OnClosed(SoupWebsocketConnection* pConnection);

        /**
         * @brief Called on incomming message from a Websocket Connection
         * @param[in] pConnection unique connection for this message
         */
        virtual void OnMessage(SoupWebsocketConnection* pConnection, 
            SoupWebsocketDataType dataType, GBytes* message);

    protected:

        /**
         * @brief mutex to protect mutual access to transceiver data
         */
        GMutex m_transceiverMutex;

        /** 
         * @brief Client's unique Websocket connection, NULL until connection established.
         */
        SoupWebsocketConnection* m_pConnection;

        /**
         *@brief Current connection state, one of DSL_SOCKET_CONNECTION_STATE_*, 
         * The state is set to DSL_SOCKET_CONNECTION_STATE_NONE on Transceiver creation
         */
        uint m_connectionState;

        /** 
         * @brief Handler Id for the Websocket closed Signal Handler, 0 when not set
         */
        gulong m_closedSignalHandlerId;

        /** 
         * @brief Handler Id for the Websocket message Signal Handler, 0 when not set
         */
        gulong m_messageSignalHandlerId;

        /** 
         * @brief .
         */
        GstWebRTCSessionDescription* m_pOffer;

        /**
         * @brief Client's JSON Parsor for parsing all messages.
         */
        JsonParser*  m_pJsonParser;
    };

    static void on_soup_websocket_closed_cb(SoupWebsocketConnection * pConnection, 
        gpointer pSignalingTransceiver);

    static void on_soup_websocket_message_cb(SoupWebsocketConnection* pConnection, 
        SoupWebsocketDataType dataType, GBytes* message, gpointer pSignalingTransceiver);

    static void on_remote_desc_set_cb(GstPromise * promise, gpointer pSignalingTransceiver);


    class SoupServerMgr
    {
    public: 

        /** 
         * @brief Returns a pointer to this singleton Manager
         * @return instance pointer to Services
         */
        static SoupServerMgr* GetMgr();

        /**
         * @brief Ctor for this singleton SoupServerMgr class
         */
        SoupServerMgr();

        /**
         * @brief Dtor for this singleton SoupServerMgr class
         */
        ~SoupServerMgr();


        /**
         * @brief Adds a new Winsocket path to handle.
         * @param[in] path new path to handle
         * @return true on successful add, false otherwise
         */
        bool AddPath(const char* path);

        /**
         * @brief Starts the Soup Server Manager listening on the specified.
         * @param[in] portNumber HTTP port number to start listening on
         * @return true on successful start, false otherwise
         */
        bool StartListening(uint portNumber);

        /**
         * @brief Stops the Soup Server Manager from listening on its current port.
         * @return true on successful stop, false otherwise
         */
        bool StopListening();

        /**
         * @brief Get the current listening state of the Soup Server Manager.
         * @param [out] portNumber HTTP port number to start listening on
         * @return true if currently listening, false otherwise.
         */
        bool GetListeningState(uint* portNumber);

        /**
         * @brief Adds a Signaling Transceiver to this server
         * @param [in] pSignalingTransceiver unique Signaling Transceiver to add
         * @return true on successful add, false otherwise
         */
        bool AddSignalingTransceiver(SignalingTransceiver* signalingTransceiver);

        /**
         * @brief Removes a Signaling Transceiver from this server
         * @param [in] pSignalingTransceiver unique Signaling Transceiver to remove
         * @return true on successful remove, false otherwise
         */
        bool RemoveSignalingTransceiver(SignalingTransceiver* signalingTransceiver);

        /**
         * @brief Checks whether a Signaling Transceiver has been previously 
         * added to this Soup Server Manager
         * @param [in] pSignalingTransceiver unique Signaling Transceiver to check for
         * @return true if found, false otherwise
         */
        bool HasSignalingTransceiver(SignalingTransceiver* signalingTransceiver);

        /**
         * @brief Returns a Signaling Transceiver with a specific connection
         * @param [in] pConnection specific connection to check for
         * @return pointer to a Signaling Transceiver, NULL ohterwise. 
         */
        const SignalingTransceiver* GetSignalingTransceiver(SoupWebsocketConnection* pConnection);

        /**
         * @brief Handles a new Websocket Connection
         * @param[in] pConnection unique connection to open 
         */
        void HandleOpen(SoupWebsocketConnection* pConnection, const char* path);

        /**
         * @brief adds a callback to be notified on incomming connection event
         * @param[in] listener pointer to the client's function to call on connection event
         * @param[in] clientData opaque pointer to client data passed into the listener function.
         * @return true on successful add, false otherwise
         */
        bool AddClientListener(dsl_websocket_server_client_listener_cb listener, 
            void* clientData);

        /**
         * @brief removes a previously added callback
         * @param[in] listener pointer to the client's function to remove
         * @return true on successful remove, false otherwise
         */
        bool RemoveClientListener(dsl_websocket_server_client_listener_cb listener);

    private:

        /**
         * @brief instance pointer for this singleton class
         */
        static SoupServerMgr* m_pInstance;

        /**
         * @brief Soup Server instance used by this singlton.
         */
        SoupServer* m_pSoupServer;
    
        /**
         * @brief mutex to protect mutual access to server data
         */
        GMutex m_serverMutex;

        /**
         * @brief true if the Soup Server Manager is listening on one or more 
         * ports, false otherwise.
         */
        uint m_isListening;

        /**
         * @brief Port number to listen on for incomming connections.
         */
        uint m_portNumber;

        /**
         * @brief container of all Signaling Transceivers mapped by their unique name
         */
        std::vector<SignalingTransceiver*> m_signalingTransceivers;

        /**
         * @brief map of all currently registered client listeners
         * callback functions mapped with the user provided data
         */
        std::map<dsl_websocket_server_client_listener_cb, void*> m_clientListeners;

    };

    static void websocket_handler_cb(G_GNUC_UNUSED SoupServer* pServer, 
        SoupWebsocketConnection* pConnection, const char *path,
        G_GNUC_UNUSED SoupClientContext* clientContext, gpointer pSoupServerMgr);

} // DSL
#endif // _DSL_SOUP_SERVER_H