MyOgreAppCEGUI_RTTtest

OGRE3D 1.9.0와 CEGUI 0.8.7 사용.

//////////////////BUILD SETTING//////////////////////

OGRE_HOME 환경변수를 sdk 폴더 경로로 지정.
CEGUI_HOME 환경변수를 sdk 폴더 경로로 지정.

resources_d.cfg에 필요한 리소스의 경로와 그룹 지정.
(리소스, dll, cfg 등등은 여기에 올리지 않음.)

/////////////////////////////////////////////////////
/////////////////////MY NOTE/////////////////////////

베이스 매터리얼 - 
노말 ao 스페큘러 매핑 쉐이더를 사용하는 기본 메터리얼을 만들어 둠. 일반적인 모델들은 이 메터리얼을 상속받아 사용.
그림자는 현재까지는 익스포트한 .mesh 파일에서 Ogre로 엣지리스트를 빌드하여 스텐실 사용.


CEGUI - 
CMake할 때 OGRE(1.9.0)에서 Boost와 OIS관련 PATH 설정이 되지 않았기 때문에 따로 설정할 필요가 있었음.
빌드 시에 여러가지 에러가 있었음.
유니코드 문자 인코딩 관련 문제, NOMINMAX 재정의 문제 등은 OGRE SDK와 컴파일러 버전이 일치하지 않아 생긴 것으로 추측됨. 버전을 통일시켜 해결.( vc11 vs2012 express 기준)
같은 프로젝트의 exe 실행 파일이 있는 폴더에 필요한 dll이 개발 환경마다 다른 문제가 있었음.(CEGUI가 워낙 dependency가 많기 때문에 정확하지 않을 수 있음)
CEGUI::OgreRenderer::bootstrapSystem, CEGUI::%Manager 등에서 에러가 났던 것인데, 이런 데서 에러가 나도 CEGUI.log에 기록되지 않기 때문에 
try catch문으로 Exception을 띄워서 직접 확인할 필요가 있었음. exception 메시지를 보고 필요한 dll을 추가하여 해결.

/////////////////////////////////////////////////////