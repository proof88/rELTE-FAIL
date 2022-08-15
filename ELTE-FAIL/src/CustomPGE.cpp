/*
    ###################################################################################
    CustomPGE.cpp
    Customized PGE.
    Made by PR00F88
    EMAIL : PR0o0o0o0o0o0o0o0o0o0oF88@gmail.com
    ###################################################################################
*/

#include "CustomPGE.h"

#include <cassert>
#include <sstream>

#include "../../../PGE/PGE/PRRE/include/external/PRREuiManager.h"
#include "../../../PGE/PGE/PRRE/include/external/Display/PRREWindow.h"
#include "../../../PGE/PGE/PRRE/include/external/PRRECamera.h"
#include "../../../CConsole/CConsole/src/CConsole.h"



// ############################### PUBLIC ################################


/**
    Creates and gets the only instance.
*/
CustomPGE* CustomPGE::createAndGetCustomPGEinstance()
{
    static CustomPGE pgeInstance(ELTEFAIL_NAME);
    return &pgeInstance;
} // createAndGetCustomPGEinstance()


// ############################## PROTECTED ##############################


/**
    This is the only usable ctor, this is used by the static createAndGet().
*/
CustomPGE::CustomPGE(const char* gameTitle) :
    PGE(gameTitle)
{

} // CustomPGE(...)


CustomPGE::~CustomPGE()
{

} // ~CustomPGE()


CConsole& CustomPGE::getConsole() const
{
    return CConsole::getConsoleInstance(getLoggerModuleName());
} // getConsole()


const char* CustomPGE::getLoggerModuleName()
{
    return "CustomPGE";
} // getLoggerModuleName()


/**
    Must-have minimal stuff before loading anything.
    Game engine calls this before even finishing its own initialization.
*/
void CustomPGE::onGameInitializing()
{
    // Earliest we can enable our own logging
    getConsole().Initialize("ELTE-FAIL log", true);
    getConsole().SetFGColor( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, "999999" );
    getConsole().SetIntsColor( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, "FFFF00" );
    getConsole().SetStringsColor( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY, "FFFFFF" );
    getConsole().SetFloatsColor( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, "FFFF00" );
    getConsole().SetBoolsColor( FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY, "00FFFF" );

    // CustomPGE (game) logs
    getConsole().SetLoggingState(getLoggerModuleName(), true);
    getConsole().SetLoggingState("PGESysNET", true);

    // Turn everything on for development only
    getConsole().SetLoggingState("4LLM0DUL3S", true);
}


/** 
    Loading game content here.
*/
void CustomPGE::onGameInitialized()
{
    getConsole().OLnOI("CustomPGE::onGameInitialized()");

    // Dont want to see logs of loading of resources cause I'm debugging network now
    getConsole().SetLoggingState("4LLM0DUL3S", false);

    getPRRE().getCamera().SetNearPlane(0.1f);
    getPRRE().getCamera().SetFarPlane(100.0f);

    PRRETexture* const tex1 = getPRRE().getTextureManager().createFromFile("gamedata\\proba128x128x24.bmp");

    /*  */
    box1 = getPRRE().getObject3DManager().createBox(1, 1, 1);
    box1->getPosVec().SetZ(2.0f);
    box1->getPosVec().SetX(1.5f);
    box1->SetOccluder(true);
    box1->SetOcclusionTested(false);
    
    // since a while this vertex coloring is not working
    box1->getMaterial().getColors()[0].red    = 1.0f;
    box1->getMaterial().getColors()[0].green  = 0.0f;
    box1->getMaterial().getColors()[0].blue   = 0.0f;
    box1->getMaterial().getColors()[0].alpha  = 0.0f;
    box1->getMaterial().getColors()[9].red    = 1.0f;
    box1->getMaterial().getColors()[9].green  = 0.0f;
    box1->getMaterial().getColors()[9].blue   = 0.0f;
    box1->getMaterial().getColors()[9].alpha  = 0.0f;
    
    box1->getMaterial().setTexture(tex1);
    box1->setVertexTransferMode(PRRE_VT_DYN_IND_SVA_GEN);  
    
    /*    */
    box2 = getPRRE().getObject3DManager().createFromFile("gamedata\\models\\cube.obj");
    box2->setVertexTransferMode(PRRE_VT_DYN_DIR_1_BY_1);
    box2->getPosVec().SetZ(3); 
    
    /*       
    PRREObject3D* const plane1 = getPRRE().getObject3DManager().createPlane(2, 2);
    plane1->getPosVec().SetX(0);
    plane1->getPosVec().SetZ(2);
    plane1->getMaterial().setTexture(tex1);
    plane1->setVertexTransferMode(PRRE_VT_DYN_IND_SVA_GEN);
    */

    PRREObject3D* const snail = getPRRE().getObject3DManager().createFromFile("gamedata\\models\\snail_proofps\\snail.obj");
    snail->SetScaling(0.02f);
    snail->getPosVec().SetZ(2);  
    
    /* */   
    PRREObject3D* snail_lm = getPRRE().getObject3DManager().createFromFile("gamedata\\models\\snail_proofps\\snail_lm.obj");
    snail_lm->SetScaling(0.02f);
    snail_lm->Hide();
     
    // dealing with lightmaps ...
    
    if ( snail->getCount() == snail_lm->getCount() ) 
    {
        for (TPRREint i = 0; i < snail->getCount(); i++)
        {
            PRREObject3D* const snailSub = (PRREObject3D*) snail->getAttachedAt(i);
            // assuming that snail_lm has the same subobjects and vertex count as snail
            PRREObject3D* const snailLMSub = (PRREObject3D*) snail_lm->getAttachedAt(i);
            if ( snailSub && snailLMSub )
            {
                // copying lightmap data into snail material's 2nd layer
                snailSub->getMaterial(false).copyFromMaterial(snailLMSub->getMaterial(false), 1, 0);
                snailSub->getMaterial(false).setBlendFuncs(PRRE_SRC_ALPHA, PRRE_ONE_MINUS_SRC_ALPHA, 1);
            }
        }
    }
    else
    {
        getConsole().EOLn("snail->getCount() != snail_lm->getCount(): %d != %d", snail->getCount(), snail_lm->getCount());
    }    
    
    snail->setVertexTransferMode(PRRE_VT_DYN_IND_SVA_GEN);
    snail->SetDoubleSided(true);


    // at this point, we should be safe to delete snail_lm since object's dtor calls material's dtor which doesn't free up the textures
    // however, a mechanism is needed to be implemented to correctly handle this situation.
    // WA1: CopyFromMaterial() should hardcopy the textures also; deleting material should delete its textures too;
    // WA2: (better) textures should maintain refcount. Material deletion would decrement refcount and would effectively delete textures when refcount reaches 0.
    delete snail_lm;
    snail_lm = NULL; 

    /*
    PRREObject3D* snail_clone = getPRRE().getObject3DManager().createCloned(*snail);
    snail_clone->getPosVec().SetX(-1);
    snail->SetOcclusionTested(false);
    */

    /*         
    getPRRE().getTextureManager().setDefaultIsoFilteringMode(PRRE_ISO_LINEAR_MIPMAP_LINEAR, PRRE_ISO_LINEAR);

    PRREObject3D* const arena = getPRRE().getObject3DManager().createFromFile("gamedata\\models\\arena\\arena.obj");
    arena->SetScaling(0.005f);
    arena->getPosVec().SetZ(2.f); 
    arena->getPosVec().SetY(-1.5f);

    PRREObject3D* arena_lm = getPRRE().getObject3DManager().createFromFile("gamedata\\models\\arena\\arena_lm.obj");
    arena_lm->SetScaling(0.02f);
    arena_lm->Hide();

    // dealing with lightmaps ...
    if ( arena->getCount() == arena_lm->getCount() ) 
    {
        for (TPRREuint i = 0; i < arena->getCount(); i++)
        {
            PRREObject3D* const arenaSub = (PRREObject3D*) arena->getAttachedAt(i);
            // assuming that arena_lm has the same subobjects and vertex count as arena
            PRREObject3D* const arenaLMSub = (PRREObject3D*) arena_lm->getAttachedAt(i);
            if ( arenaSub && arenaLMSub )
            {
                // copying lightmap data into snail material's 2nd layer
                arenaSub->getMaterial(false).copyFromMaterial(arenaLMSub->getMaterial(false), 1, 0);
                arenaSub->getMaterial(false).setBlendFuncs(PRRE_SRC_ALPHA, PRRE_ONE_MINUS_SRC_ALPHA, 1);
            }
        }
    }
    else
    {
        getConsole().EOLn("arena->getCount() != arenaLMSub->getCount(): %d != %d", arena->getCount(), arena_lm->getCount());
    }

    arena->setVertexTransferMode(PRRE_VT_DYN_DIR_SVA_GEN);

    delete arena_lm;
    arena_lm = NULL;
    */
    
    getPRRE().getUImanager().addText("almafaALMAFA012345������_+", 10, 10);

    getConsole().OO();
    getConsole().OLn("");

    getPRRE().WriteList();

    if (!isServer())
    {
        if (!ConnectClient())
        {
            PGE::showErrorDialog("Client has FAILED to establish connection to the server!");
            assert(false);
        }
    }

} // onGameInitialized()


/** 
    Game logic here.
    DO NOT make any unnecessary operations here, as this function must complete below 16 msecs to keep 60 fps!
    Avoid dynamic memory allocations as well! Make sure you use a preallocated memory pool!
*/
void CustomPGE::onGameRunning()
{
    const PRREWindow& window = getPRRE().getWindow();
    const PGEInputHandler& input = PGEInputHandler::createAndGet();

    static bool bCameraLocked = false;

    /*const int oldmx = input.getMouse().getCursorPosX();
    const int oldmy = input.getMouse().getCursorPosY();*/

    input.getMouse().SetCursorPos(window.getX() + window.getWidth()/2,
                                  window.getY() + window.getHeight()/2);

    /*
    int changeX = oldmx - input.getMouse().getCursorPosX();
    int changeY = oldmy - input.getMouse().getCursorPosY();
    pGFX->getCamera().getTargetVec().SetY( pGFX->getCamera().getTargetVec().getY() - changeY / 10.0f );
    pGFX->getCamera().getTargetVec().SetX( pGFX->getCamera().getTargetVec().getX() - changeX / 10.0f );*/

    if ( box1 != NULL )
    {
       box1->getAngleVec().SetY( box1->getAngleVec().getY() + 0.2f );
    }

    if ( input.getKeyboard().isKeyPressed(VK_UP) )
    {
        getPRRE().getCamera().Move(0.01f);
        getConsole().OLn("cam Z: %f", getPRRE().getCamera().getPosVec().getZ());
    }
    if ( input.getKeyboard().isKeyPressed(VK_DOWN) )
    {
        getPRRE().getCamera().Move(-0.01f);
        getConsole().OLn("cam Z: %f", getPRRE().getCamera().getPosVec().getZ());
    }
    if ( input.getKeyboard().isKeyPressed(VK_LEFT) )
    {
        getPRRE().getCamera().Strafe(-0.01f);
    }
    if ( input.getKeyboard().isKeyPressed(VK_RIGHT) )
    {
        getPRRE().getCamera().Strafe(0.01f);
    }
    if ( input.getKeyboard().isKeyPressed(VK_SPACE) )
    {
        getPRRE().getCamera().Elevate(0.01f);
    }
    if ( input.getKeyboard().isKeyPressed(VK_CONTROL) )
    {
        getPRRE().getCamera().Elevate(-0.01f);
    }

    if ( input.getKeyboard().isKeyPressed((unsigned char)VkKeyScan('1')) )
    {
        if ( box1 != NULL )
        {
            box1->SetRenderingAllowed( !box1->isRenderingAllowed() );
            Sleep(200); /* to make sure key is released, avoid bouncing */
        }
    }

    if ( input.getKeyboard().isKeyPressed((unsigned char)VkKeyScan('2')) )
    {
        PRREObject3D* snailobj = (PRREObject3D*) getPRRE().getObject3DManager().getByFilename("gamedata\\models\\snail_proofps\\snail.obj");
        if ( snailobj != NULL )
        {
            snailobj->SetRenderingAllowed( !snailobj->isRenderingAllowed() );
            Sleep(200);
        }
    }

    //if ( input.getKeyboard().isKeyPressed((unsigned char)VkKeyScan('3')) )
    //{
    //    PRREObject3D* arenaobj = (PRREObject3D*) getPRRE().getObject3DManager().getByFilename("gamedata\\models\\arena\\arena.obj");
    //    if ( arenaobj != NULL )
    //    {
    //        arenaobj->SetRenderingAllowed( !arenaobj->isRenderingAllowed() );
    //        Sleep(200);
    //    }
    //}

    //if ( input.getKeyboard().isKeyPressed((unsigned char)VkKeyScan('w')) )
    //{
    //    box1->getPosVec().SetZ( box1->getPosVec().getZ() + 0.01f );
    //}
    //if ( input.getKeyboard().isKeyPressed((unsigned char)VkKeyScan('s')) )
    //{
    //    box1->getPosVec().SetZ( box1->getPosVec().getZ() - 0.01f );
    //}
    //if ( input.getKeyboard().isKeyPressed((unsigned char)VkKeyScan('a')) )
    //{
    //    box1->getPosVec().SetX( box1->getPosVec().getX() - 0.01f );
    //}
    //if ( input.getKeyboard().isKeyPressed((unsigned char)VkKeyScan('d')) )
    //{
    //    box1->getPosVec().SetX( box1->getPosVec().getX() + 0.01f );
    //}
    //if ( input.getKeyboard().isKeyPressed((unsigned char)VkKeyScan('i')) )
    //{
    //    box1->getPosVec().SetY( box1->getPosVec().getY() + 0.01f );
    //}
    //if ( input.getKeyboard().isKeyPressed((unsigned char)VkKeyScan('k')) )
    //{
    //    box1->getPosVec().SetY( box1->getPosVec().getY() - 0.01f );
    //}

    HorizontalDirection horDir = HorizontalDirection::NONE;
    VerticalDirection verDir = VerticalDirection::NONE;
    if (input.getKeyboard().isKeyPressed((unsigned char)VkKeyScan('w')))
    {
        verDir = VerticalDirection::UP;
    }
    if (input.getKeyboard().isKeyPressed((unsigned char)VkKeyScan('s')))
    {
        verDir = VerticalDirection::DOWN;
    }
    if (input.getKeyboard().isKeyPressed((unsigned char)VkKeyScan('a')))
    {
        horDir = HorizontalDirection::LEFT;
    }
    if (input.getKeyboard().isKeyPressed((unsigned char)VkKeyScan('d')))
    {
        horDir = HorizontalDirection::RIGHT;
    }

    if ((horDir != HorizontalDirection::NONE) || (verDir != VerticalDirection::NONE))
    {
        PgePacket pktCmdMove;
        memset(&pktCmdMove, 0, sizeof(pktCmdMove));
        pktCmdMove.pktId = PgePktUserCmdMove::id;
        pktCmdMove.msg.userCmdMove.horDirection = horDir;
        pktCmdMove.msg.userCmdMove.verDirection = verDir;

        if (isServer())
        {
            // TODO
        }
        else
        {
            SendPacketToServer(pktCmdMove);
        }
    }

    // L for camera Lock
    if ( input.getKeyboard().isKeyPressed((unsigned char)VkKeyScan('l')) )
    {
        bCameraLocked = !bCameraLocked;
        Sleep(200);
    }

    if ( bCameraLocked )
    {
        getPRRE().getCamera().getTargetVec().Set( box1->getPosVec().getX(), box1->getPosVec().getY(), box1->getPosVec().getZ() );
    }



    std::stringstream str;
    //str << "MX1: " << changeX << "   MY1: " << changeY;
    //str << "key: " << input.getKeyboard().isKeyPressed(VK_RETURN);

    //window.SetCaption(str.str());
} // onGameRunning()


/**
    Called when a new network packet is received.
*/
void CustomPGE::onPacketReceived(const PgePacket& pkt)
{
    switch (pkt.pktId)
    {
    case PgePktUserConnected::id:
        HandleUserConnected(pkt.msg.userConnected);
        break;
    case PgePktUserCmdMove::id:
        HandleUserCmdMove(pkt.msg.userCmdMove);
        break;
    case PgePktUserUpdate::id:
        HandleUserUpdate(pkt.msg.userUpdate);
        break;
    default:
        getConsole().EOLn("CustomPGE::%s(): unknown pktId %d", __func__, pkt.pktId);
    }
}


/** 
    Freeing up game content here.
    Free up everything that has been allocated in onGameInitialized() and onGameRunning().
*/
void CustomPGE::onGameDestroying()
{
    getPlayers().clear();

    delete box1;
    box1 = NULL;
    getPRRE().getObject3DManager().DeleteAll();

    getConsole().Deinitialize();
} // onGameRunning()


// ############################### PRIVATE ###############################


void CustomPGE::HandleUserConnected(const PgePktUserConnected& pkt)
{
    if (isServer())
    {
        getConsole().OLn("CustomPGE::%s(): new user %s connected and I'm server", __func__, pkt.sUserName);
    }
    else
    {
        if (pkt.bCurrentClient)
        {
            getConsole().OLn("CustomPGE::%s(): this is me, my name is %s", __func__, pkt.sUserName);
            // TODO: set user name for myself
        }
        else
        {
            getConsole().OLn("CustomPGE::%s(): new user %s connected and I'm client", __func__, pkt.sUserName);
        }
    }

    if (getPlayers().end() != getPlayers().find(pkt.sUserName))
    {
        getConsole().EOLn("CustomPGE::%s(): user %s is already present in players list!", __func__, pkt.sUserName);
        return;
    }

    PRREObject3D* const plane = getPRRE().getObject3DManager().createPlane(1, 1);
    plane->getPosVec().SetX(0);
    plane->getPosVec().SetZ(2);
    //plane->getMaterial().setTexture(tex1);
    plane->setVertexModifyingHabit(PRRE_VMOD_STATIC);
    plane->setVertexReferencingMode(PRRE_VREF_INDEXED);

    getPlayers()[pkt.sUserName].pObject3D = plane;
}

void CustomPGE::HandleUserCmdMove(const PgePktUserCmdMove& pkt)
{
    if (!isServer())
    {
        getConsole().EOLn("CustomPGE::%s(): should not be received by any client!", __func__);
        return;
    }
    
    auto it = getPlayers().find(pkt.sUserName);
    if (getPlayers().end() == it)
    {
        getConsole().EOLn("CustomPGE::%s(): failed to find user: %d!", __func__, pkt.sUserName);
        return;
    }

    PRREObject3D* obj = it->second.pObject3D;
    if (!obj)
    {
        getConsole().EOLn("CustomPGE::%s(): user %d doesn't have associated Object3D!", __func__, pkt.sUserName);
        return;
    }

    if ((pkt.horDirection == HorizontalDirection::NONE) && (pkt.verDirection == VerticalDirection::NONE))
    {
        getConsole().EOLn("CustomPGE::%s(): user %d sent invalid cmdMove!", __func__, pkt.sUserName);
        return;
    }

    //getConsole().OLn("CustomPGE::%s(): user %s sent valid cmdMove", __func__, pkt.sUserName);
    switch (pkt.horDirection)
    {
    case HorizontalDirection::LEFT:
        obj->getPosVec().SetX( obj->getPosVec().getX() - 0.01f );
        break;
    case HorizontalDirection::RIGHT:
        obj->getPosVec().SetX(obj->getPosVec().getX() + 0.01f);
        break;
    default: /* no-op */
        break;
    }

    switch (pkt.verDirection)
    {
    case VerticalDirection::DOWN:
        obj->getPosVec().SetY(obj->getPosVec().getY() - 0.01f);
        break;
    case VerticalDirection::UP:
        obj->getPosVec().SetY(obj->getPosVec().getY() + 0.01f);
        break;
    default: /* no-op */
        break;
    }

    PgePacket pktUserUpdate;
    memset(&pktUserUpdate, 0, sizeof(pktUserUpdate));
    pktUserUpdate.pktId = PgePktUserUpdate::id;
    strncpy_s(pktUserUpdate.msg.userUpdate.sUserName, 64, pkt.sUserName, 64);
    pktUserUpdate.msg.userUpdate.pos.x = obj->getPosVec().getX();
    pktUserUpdate.msg.userUpdate.pos.y = obj->getPosVec().getY();
    SendPacketToAllClients(pktUserUpdate);
}

void CustomPGE::HandleUserUpdate(const PgePktUserUpdate& pkt)
{
    if (isServer())
    {
        getConsole().EOLn("CustomPGE::%s(): should not be received by server!", __func__);
        return;
    }

    auto it = getPlayers().find(pkt.sUserName);
    if (getPlayers().end() == it)
    {
        getConsole().EOLn("CustomPGE::%s(): failed to find user: %d!", __func__, pkt.sUserName);
        return;
    }

    PRREObject3D* obj = it->second.pObject3D;
    if (!obj)
    {
        getConsole().EOLn("CustomPGE::%s(): user %d doesn't have associated Object3D!", __func__, pkt.sUserName);
        return;
    }

    obj->getPosVec().SetX(pkt.pos.x);
    obj->getPosVec().SetY(pkt.pos.y);
}


