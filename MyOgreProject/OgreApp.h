#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreWindowEventUtilities.h>
#include <OgreEntity.h>
#include <OgreSubEntity.h>
#include <OgreMeshManager.h>
#include <OgreSubMesh.h>
#include <OgreHardwarePixelBuffer.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>

class MyOgreApp
	: public Ogre::WindowEventListener
	, public Ogre::FrameListener
	, public OIS::KeyListener
	, public OIS::MouseListener
{
public:
	MyOgreApp();
	virtual ~MyOgreApp();
	bool go();

protected:
	Ogre::Root* mRoot;
	Ogre::RenderWindow* mWindow;
	Ogre::String mResourcesCfg;
	Ogre::String mPluginsCfg;

	CEGUI::OgreRenderer* mRenderer;

	OIS::InputManager* mInputMgr;
	OIS::Mouse* mMouse;
	OIS::Keyboard* mKeyboard;

	
	Ogre::SceneManager* mSceneMgr;

	bool mShutDown; //true가 되면 다음 Ogre::frameRenderdingQueued에서 멈출 것.


	Ogre::Vector3 vel; //테스트 용 속도 변수

	virtual void windowResized(Ogre::RenderWindow* rw);
	virtual void windowClosed(Ogre::RenderWindow* rw);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& fe);

	virtual bool keyPressed(const OIS::KeyEvent& ke);
	virtual bool keyReleased(const OIS::KeyEvent& ke);
	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	bool quit(const CEGUI::EventArgs &e);

	//기본 셋업.
	void setupResources();
	void setupCEGUI();
	void createFrameListener(void);

	//테스트용 씬 구성
	void createScene(); //메쉬 둘.
	void createLight(); //엠비언트, 디렉셔널 라이트 하나 씩.
	void createCamera(); //메인 카메라 하나.
};