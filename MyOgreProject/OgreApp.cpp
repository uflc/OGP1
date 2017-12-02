#include "OgreApp.h"
#include <OgreException.h>
#include <OgreConfigFile.h>

#define MAIN_CAM_NAME "MainCam"//

MyOgreApp::MyOgreApp()
	: mRoot(0), vel(0, 0, 0), mShutDown(false),
	mResourcesCfg(Ogre::StringUtil::BLANK),
	mPluginsCfg(Ogre::StringUtil::BLANK)
{
}

MyOgreApp::~MyOgreApp()
{
	//
	// Remove ourself as a Window listener
	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);

	delete mRoot;
}

void MyOgreApp::windowResized(Ogre::RenderWindow* rw)
{
	int left, top;
	unsigned int width, height, depth;

	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState& ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

void MyOgreApp::windowClosed(Ogre::RenderWindow* rw)
{
	if (rw == mWindow) {
		if (mInputMgr) {
			mInputMgr->destroyInputObject(mMouse);
			mInputMgr->destroyInputObject(mKeyboard);
			OIS::InputManager::destroyInputSystem(mInputMgr);
			mInputMgr = 0;
		}
	}
}

bool MyOgreApp::frameRenderingQueued(const Ogre::FrameEvent& fe)
{
	if (mWindow->isClosed() || mShutDown)	return false;

	mKeyboard->capture();
	mMouse->capture();

	Ogre::Real deltatime = fe.timeSinceLastFrame;

	CEGUI::System::getSingleton().injectTimePulse(deltatime);

	//////////////////////////////////////////////////

	//테스트 기본적 이동 조작.
	Ogre::SceneNode* nod;
	nod = mSceneMgr->getSceneNode("EntNode0");
	nod->translate(vel*deltatime, Ogre::Node::TS_LOCAL);

	Ogre::Vector3 dest = nod->getPosition();

	nod = mSceneMgr->getSceneNode("EntNode1");
	Ogre::Vector3 src = nod->getPosition();

	Ogre::Vector3 delta = dest - src;
	delta.y = 0;

	Ogre::Vector3 dir = Ogre::Vector3(delta.z, 0, -delta.x);
	if (delta.squaredLength() > 13000)
		dir += delta;
	else
		dir -= delta;

	nod->translate(dir.normalisedCopy() * 55 * deltatime, Ogre::Node::TS_LOCAL);

	return true;
}

void MyOgreApp::createCamera()
{
	Ogre::Camera* cam = mSceneMgr->createCamera(MAIN_CAM_NAME);
	cam->setPosition(Ogre::Vector3(200, 500, 200));
	cam->lookAt(Ogre::Vector3(0, 50, 0));
	cam->setNearClipDistance(5);

	Ogre::Viewport* vp = mWindow->addViewport(cam);
	//vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
	cam->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}

void MyOgreApp::createFrameListener(void)
{
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");

	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	mInputMgr = OIS::InputManager::createInputSystem(pl);

	// 버퍼 인풋 키보드, 마우스
	mKeyboard = static_cast<OIS::Keyboard*>(mInputMgr->createInputObject(OIS::OISKeyboard, true));
	mMouse = static_cast<OIS::Mouse*>(mInputMgr->createInputObject(OIS::OISMouse, true));
	// 리스너 등록
	mKeyboard->setEventCallback(this);
	mMouse->setEventCallback(this);

	// Set initial mouse clipping size
	windowResized(mWindow);

	// Register as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

	// Register as a Frame listener
	mRoot->addFrameListener(this);
}

bool MyOgreApp::keyPressed(const OIS::KeyEvent & ke)
{
	//
	CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
	context.injectKeyDown((CEGUI::Key::Scan)ke.key);
	context.injectChar((CEGUI::Key::Scan)ke.text);

	//
	switch (ke.key)
	{
	case OIS::KC_W:
		vel.z -= 100.0;
		break;
	case OIS::KC_S:
		vel.z += 100.0;
		break;
	case OIS::KC_D:
		vel.x += 100.0;
		break;
	case OIS::KC_A:
		vel.x -= 100.0;
		break;
	default:
		break;
	}

	return true;
}

bool MyOgreApp::keyReleased(const OIS::KeyEvent & ke)
{
	//
	CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp((CEGUI::Key::Scan)ke.key);

	//
	switch (ke.key)
	{
	case OIS::KC_W:
		vel.z += 100.0;
		break;
	case OIS::KC_S:
		vel.z -= 100.0;
		break;
	case OIS::KC_D:
		vel.x -= 100.0;
		break;
	case OIS::KC_A:
		vel.x += 100.0;
		break;
	default:
		break;
	}

	return true;
}

bool MyOgreApp::mouseMoved(const OIS::MouseEvent & arg)
{
	CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
	context.injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
	// Scroll wheel.
	if (arg.state.Z.rel)
		context.injectMouseWheelChange(arg.state.Z.rel / 120.0f); //magic number 120

	return true;
}

bool MyOgreApp::mousePressed(const OIS::MouseEvent & arg, OIS::MouseButtonID id)
{
	CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
	context.injectMouseButtonDown((CEGUI::MouseButton)id);

	return true;
}

bool MyOgreApp::mouseReleased(const OIS::MouseEvent & arg, OIS::MouseButtonID id)
{
	CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
	context.injectMouseButtonUp((CEGUI::MouseButton)id);

	return true;
}

bool MyOgreApp::quit(const CEGUI::EventArgs & e)
{
	mShutDown = true;
	return true;
}

void MyOgreApp::setupResources()
{
	// Load resource paths from config file
	Ogre::ConfigFile cf;
	cf.load(mResourcesCfg);

	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
			// OS X does not set the working directory relative to the app.
			// In order to make things portable on OS X we need to provide
			// the loading with it's own bundle path location.
			if (!Ogre::StringUtil::startsWith(archName, "/", false)) // only adjust relative directories
				archName = Ogre::String(Ogre::macBundlePath() + "/" + archName);
#endif
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName);
		}
	}
}

void MyOgreApp::setupCEGUI()
{
	using namespace CEGUI;

	mRenderer = &OgreRenderer::bootstrapSystem();

	GUIContext &context = System::getSingleton().getDefaultGUIContext();
	WindowManager &wmgr = WindowManager::getSingleton();

	//루트 Window
	Window *sheet = wmgr.createWindow("DefaultWindow", "root");
	context.setRootWindow(sheet);

	ImageManager::setImagesetDefaultResourceGroup("Imagesets");
	Font::setDefaultResourceGroup("Fonts");
	Scheme::setDefaultResourceGroup("Schemes");
	WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	WindowManager::setDefaultResourceGroup("Layouts");


	//TaharezLook 스킴으로 기본 설정. 
	SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
	context.getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");
	//MouseCursor::getSingleton().setImage(System::getSingleton().getDefaultMouseCursor());
	//context.setDefaultTooltipType("TaharezLook/Tooltip");


	//종료 (버튼) Window
	Window *quit = wmgr.createWindow("TaharezLook/Button", "CEGUIDemo/QuitButton");
	quit->setText("Quit");
	quit->setSize(USize(UDim(0.15, 0), UDim(0.05, 0)));
	//quit->setPosition(\UVector2(UDim(0.25f, 0.0f), UDim(0.25f, 0.0f))); //(0,0)
	sheet->addChild(quit);
	quit->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&MyOgreApp::quit, this));


	//작은 뷰포트 RTT
	Ogre::TexturePtr tex = mRoot->getTextureManager()->createManual(
		"RTT",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D,
		512,
		512,
		0,
		Ogre::PF_R8G8B8,
		Ogre::TU_RENDERTARGET);
	Ogre::RenderTexture *rtex = tex->getBuffer()->getRenderTarget();

	//작은 뷰포트
	Ogre::Camera *cam = mSceneMgr->createCamera("RTTCam");
	cam->setPosition(100, 300, -200);
	cam->lookAt(0, 0, 0);
	Ogre::Viewport *v = rtex->addViewport(cam);
	v->setOverlaysEnabled(false);
	v->setClearEveryFrame(true);
	v->setBackgroundColour(Ogre::ColourValue::Black);

	//BasicImage
	Texture &guiTex = mRenderer->createTexture("RTT", tex);
	const Rectf rect(Vector2f(0.0f, 0.0f), guiTex.getOriginalDataSize());
	BasicImage* image = (BasicImage*)(&ImageManager::getSingleton().create("BasicImage", "RTTImage"));
	image->setTexture(&guiTex);
	image->setArea(rect);
	image->setAutoScaled(ASM_Both);

	//작은 뷰포트 Window
	Window *si = wmgr.createWindow("TaharezLook/StaticImage", "RTTWindow");
	si->setSize(USize(UDim(0.3f, 0), UDim(0.2f, 0)));
	si->setPosition(UVector2(UDim(0.1f, 0), UDim(0.7f, 0)));
	si->setProperty("Image", "RTTImage");
	sheet->addChild(si);
}

void MyOgreApp::createScene()
{
	//씬 매니저
	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);

	//그림자
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	//depth shadow mapping 
	//mSceneMgr->setShadowTextureSettings(1024, 2);
	//mSceneMgr->setShadowTextureCasterMaterial("Ogre/DepthShadowmap/Caster/Float");//
	//mSceneMgr->setShadowTextureReceiverMaterial("Ogre/DepthShadowmap/Receiver/Float/PCF");
	//mSceneMgr->setShadowTextureSelfShadow(true);
	//mSceneMgr->setShadowTexturePixelFormat(Ogre::PF_FLOAT32_R);
	//mSceneMgr->setShadowCasterRenderBackFaces(false);
	//mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
	//mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");

	createCamera();
	createLight();

	//플레인
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, -10);
	Ogre::MeshManager::getSingleton().createPlane("plane",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
		1500, 1500, 255, 255, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
	Ogre::Entity* ent = mSceneMgr->createEntity("LightPlaneEntity", "plane");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
	ent->setMaterialName("Examples/BeachStones");
	//ent->setCastShadows(false);

	//탄젠트 벡터 생성용 변수
	unsigned short src, index;

	//사이보그
	Ogre::Entity* cyborgEnt = mSceneMgr->createEntity("CyberSoldier.mesh");
	cyborgEnt->getMesh()->suggestTangentVectorBuildParams(Ogre::VES_TANGENT, src, index);
	cyborgEnt->getMesh()->buildTangentVectors();
	if (!cyborgEnt->getMesh()->isEdgeListBuilt())	cyborgEnt->getMesh()->buildEdgeList();
	//node0
	Ogre::SceneNode* node0 = mSceneMgr->getRootSceneNode()->createChildSceneNode("EntNode0");
	node0->attachObject(cyborgEnt);
	node0->setScale(80, 80, 80);
	node0->setPosition(50, 0, 0);


	//드론
	Ogre::Entity* droneEnt = mSceneMgr->createEntity("PA_Drone.mesh");
	droneEnt->getMesh()->suggestTangentVectorBuildParams(Ogre::VES_TANGENT, src, index);
	droneEnt->getMesh()->buildTangentVectors(Ogre::VES_TANGENT, src, index);
	if (!droneEnt->getMesh()->isEdgeListBuilt())	droneEnt->getMesh()->buildEdgeList();
	//node1
	Ogre::SceneNode* node1 = mSceneMgr->getRootSceneNode()->createChildSceneNode("EntNode1");
	node1->attachObject(droneEnt);
	node1->setScale(80, 80, 80);
	node1->setPosition(0, 165, 0);
}

void MyOgreApp::createLight()
{
	mSceneMgr->setAmbientLight(Ogre::ColourValue(.25, .25, .25));

	Ogre::Light* light = mSceneMgr->createLight("MainLight");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDirection(0.5, -1, -1);
}

bool MyOgreApp::go()
{
#ifdef _DEBUG
	mResourcesCfg = "resources_d.cfg";
	mPluginsCfg = "plugins_d.cfg";
#else
	mResourcesCfg = "resources.cfg";
	mPluginsCfg = "plugins.cfg";
#endif

	mRoot = new Ogre::Root(mPluginsCfg);

	setupResources();

	//check Ogre.cfg
	if (!(mRoot->restoreConfig() || mRoot->showConfigDialog())) return false;

	mWindow = mRoot->initialise(true, "My OgreApp");


	//--------------------------------------------------------------------------


	// Set LOD level
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	// Initialize all of the resources found by OGRE.
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	createFrameListener();

	//test
	createScene();

	//로그가 남지 않는 문제(주로 dll관련)가 있어도 에러 메시지를 띄워서 확인할 수 있게 함. 
	try {
		setupCEGUI();
	}
	catch (CEGUI::Exception e) {
		MessageBox(NULL, e.getMessage().c_str(), "CEGUI exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}


	// Start rendering loop
	mRoot->startRendering();

	return true;
}



#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
	int main(int argc, char *argv[])
#endif
	{
		// Create application object
		MyOgreApp app;

		try {
			app.go();
		}
		catch (Ogre::Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
			std::cerr << "An exception has occurred: " <<
				e.getFullDescription().c_str() << std::endl;
#endif
		}

		return 0;
	}

#ifdef __cplusplus
}
#endif

//---------------------------------------------------------------------------