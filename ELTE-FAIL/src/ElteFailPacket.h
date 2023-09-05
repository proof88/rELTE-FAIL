#pragma once

/*
    ###################################################################################
    ElteFailPacket.h
    Network packets defined for ELTE-FAIL.
    Made by PR00F88
    EMAIL : PR0o0o0o0o0o0o0o0o0o0oF88@gmail.com
    ###################################################################################
*/

#include "../../../PGE/PGE/Network/PgePacket.h"

#include <array>

namespace elte_fail
{

    enum class ElteFailMsgId : pge_network::TPgeMsgAppMsgId  /* underlying type should be same as type of MsgAppArea::msgId */
    {
        USER_SETUP = 0,
        USER_CMD_MOVE,
        USER_UPDATE,
        LastMsgId
    };

    struct ElteFailMsgId2ZStringPair
    {
        ElteFailMsgId msgId;
        const char* const zstring;
    };

    // this way of defining std::array makes sure code cannot compile if we forget to align the array after changing ElteFailMsgId
    constexpr std::array<ElteFailMsgId2ZStringPair, static_cast<size_t>(ElteFailMsgId::LastMsgId)> MapMsgAppId2String
    { {
         {ElteFailMsgId::USER_SETUP,    "MsgUserSetupFromServer"},
         {ElteFailMsgId::USER_CMD_MOVE, "MsgUserCmdFromClient"},
         {ElteFailMsgId::USER_UPDATE,   "MsgUserUpdateFromServer"}
    } };

    // server -> self (inject) and clients
    struct MsgUserSetup
    {
        static const ElteFailMsgId id = ElteFailMsgId::USER_SETUP;
        static const uint8_t nUserNameMaxLength = 64;
        static const uint8_t nTrollfaceTexMaxLength = 64;

        static bool initPkt(
            pge_network::PgePacket& pkt,
            const pge_network::PgeNetworkConnectionHandle& connHandleServerSide,
            bool bCurrentClient,
            const std::string& sUserName,
            const std::string& sTrollFaceTex,
            const std::string& sIpAddress)
        {
            // although preparePktMsgAppFill() does runtime check, we should fail already at compile-time if msg is too big!
            static_assert(sizeof(MsgUserSetup) <= pge_network::MsgAppArea::nMessagesAreaLength, "msg size");

            pge_network::PgePacket::initPktMsgApp(pkt, connHandleServerSide);

            uint8_t* const pMsgAppData = pge_network::PgePacket::preparePktMsgAppFill(
                pkt, static_cast<pge_network::TPgeMsgAppMsgId>(id), sizeof(MsgUserSetup));
            if (!pMsgAppData)
            {
                return false;
            }

            elte_fail::MsgUserSetup& msgUserSetup = reinterpret_cast<elte_fail::MsgUserSetup&>(*pMsgAppData);
            msgUserSetup.m_bCurrentClient = bCurrentClient;
            strncpy_s(msgUserSetup.m_szUserName, nUserNameMaxLength, sUserName.c_str(), sUserName.length());
            strncpy_s(msgUserSetup.m_szTrollfaceTex, nTrollfaceTexMaxLength, sTrollFaceTex.c_str(), sTrollFaceTex.length());
            strncpy_s(msgUserSetup.m_szIpAddress, sizeof(msgUserSetup.m_szIpAddress), sIpAddress.c_str(), sIpAddress.length());

            return true;
        }

        bool m_bCurrentClient;
        char m_szUserName[nUserNameMaxLength];
        char m_szTrollfaceTex[nTrollfaceTexMaxLength];
        char m_szIpAddress[pge_network::MsgUserConnectedServerSelf::nIpAddressMaxLength];
    };

    enum class VerticalDirection : std::uint8_t
    {
        NONE = 0,
        UP,
        DOWN
    };

    enum class HorizontalDirection : std::uint8_t
    {
        NONE = 0,
        LEFT,
        RIGHT
    };

    // clients -> server
    // MsgUserCmdMove messages are sent from clients to server, so server will do sg and then update all the clients with MsgUserUpdate
    struct MsgUserCmdMove
    {
        static const ElteFailMsgId id = ElteFailMsgId::USER_CMD_MOVE;

        static bool initPkt(
            pge_network::PgePacket& pkt,
            const HorizontalDirection& dirHorizontal,
            const VerticalDirection& dirVertical)
        {
            // although preparePktMsgAppFill() does runtime check, we should fail already at compile-time if msg is too big!
            static_assert(sizeof(MsgUserCmdMove) <= pge_network::MsgAppArea::nMessagesAreaLength, "msg size");

            pge_network::PgePacket::initPktMsgApp(pkt, 0 /* m_connHandleServerSide is ignored in this message */);

            uint8_t* const pMsgAppData = pge_network::PgePacket::preparePktMsgAppFill(
                pkt, static_cast<pge_network::TPgeMsgAppMsgId>(id), sizeof(MsgUserCmdMove));
            if (!pMsgAppData)
            {
                return false;
            }

            elte_fail::MsgUserCmdMove& msgUserCmdMove = reinterpret_cast<elte_fail::MsgUserCmdMove&>(*pMsgAppData);
            msgUserCmdMove.m_dirHorizontal = dirHorizontal;
            msgUserCmdMove.m_dirVertical = dirVertical;

            return true;
        }

        HorizontalDirection m_dirHorizontal;
        VerticalDirection m_dirVertical;
    };

    // server -> self (inject) and clients
    struct MsgUserUpdate
    {
        static const ElteFailMsgId id = ElteFailMsgId::USER_UPDATE;

        static bool initPkt(
            pge_network::PgePacket& pkt,
            const pge_network::PgeNetworkConnectionHandle& connHandleServerSide,
            const TPureFloat x,
            const TPureFloat y, 
            const TPureFloat z)
        {
            // although preparePktMsgAppFill() does runtime check, we should fail already at compile-time if msg is too big!
            static_assert(sizeof(MsgUserUpdate) <= pge_network::MsgAppArea::nMessagesAreaLength, "msg size");
            
            pge_network::PgePacket::initPktMsgApp(pkt, connHandleServerSide);

            uint8_t* const pMsgAppData = pge_network::PgePacket::preparePktMsgAppFill(
                pkt, static_cast<pge_network::TPgeMsgAppMsgId>(id), sizeof(MsgUserUpdate));
            if (!pMsgAppData)
            {
                return false;
            }

            elte_fail::MsgUserUpdate& msgUserCmdUpdate = reinterpret_cast<elte_fail::MsgUserUpdate&>(*pMsgAppData);
            msgUserCmdUpdate.m_pos.x = x;
            msgUserCmdUpdate.m_pos.y = y;
            msgUserCmdUpdate.m_pos.z = z;

            return true;
        }

        TXYZ m_pos;  // Z-coord is actually unused because it never gets changed during the whole gameplay ...
    };

} // namespace elte_fail