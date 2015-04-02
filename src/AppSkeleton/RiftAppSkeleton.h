// RiftAppSkeleton.h

#pragma once

#ifdef __APPLE__
#include "opengl/gl.h"
#endif

#ifdef _WIN32
#  define WINDOWS_LEAN_AND_MEAN
#  define NOMINMAX
#  include <windows.h>
#endif

#include <Kernel/OVR_Types.h> // Pull in OVR_OS_* defines 
#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>

#ifdef USE_ANTTWEAKBAR
#  include <AntTweakBar.h>
#endif

#include "FBO.h"
#ifdef USE_SIXENSE
#include "HydraScene.h"
#endif
#include "OVRScene.h"
#include "RaymarchShaderScene.h"
#include "ShaderGalleryScene.h"
#include "FloorScene.h"
#include "DashboardScene.h"

#include "FlyingMouse.h"
#include "VirtualTrackball.h"

///@brief Encapsulates as much of the VR viewer state as possible,
/// pushing all viewer-independent stuff to Scene.
class RiftAppSkeleton
{
public:
    RiftAppSkeleton();
    virtual ~RiftAppSkeleton();

    void initGL();
    void initHMD();
    void initVR();
    void exitVR();
    void RecenterPose();
    void ResetChassisTransformations();
    void SetChassisPosition(glm::vec3 p) { m_chassisPos = p; }
    int ConfigureRendering();
    int ConfigureSDKRendering();
    int ConfigureClientRendering();
    void DismissHealthAndSafetyWarning() const;
    bool CheckForTapOnHmd();

    void DoSceneRenderPrePasses() const;
    void display_raw() const;
    void display_buffered(bool setViewport=true) const;
    void display_stereo_undistorted() const;
    void display_sdk() const;
    void display_client() const;

    // Direct mode and SDK rendering hooks
    void AttachToWindow(void* pWindow) { ovrHmd_AttachToWindow(m_Hmd, pWindow, NULL, NULL); }
#if defined(OVR_OS_WIN32)
    void setWindow(HWND w) { m_Cfg.OGL.Window = w; }
#elif defined(OVR_OS_LINUX)
    void setWindow(_XDisplay* Disp) { m_Cfg.OGL.Disp = Disp; }
#endif

    void timestep(double absTime, double dt);
    void resize(int w, int h);

    void DiscoverShaders(bool recurse=true);
    void SetTextureLibraryPointer();
    void LoadTextureLibrary();
    void ToggleShaderWorld();

    float GetFBOScale() const { return m_fboScale; }
    void SetFBOScale(float s);
#ifdef USE_ANTTWEAKBAR
    float* GetFBOScalePointer() { return &m_fboScale; }
#endif

    GLuint getRenderBufferTex() const { return m_renderBuffer.tex; }
    float GetFboScale() const { return m_fboScale; }
    ovrSizei getHmdResolution() const;
    ovrVector2i getHmdWindowPos() const;
    bool UsingDebugHmd() const { return m_usingDebugHmd; }
    bool UsingDirectMode() const { return m_directHmdMode; }

protected:
    void _initPresentFbo();
    void _initPresentDistMesh(ShaderWithVariables& shader, int eyeIdx);
    void _resetGLState() const;
    void _drawSceneMono() const;
    void _DrawScenes(
        const float* pMview,
        const float* pPersp,
        const ovrRecti& rvp,
        const float* pMvLocal) const;
    void _StoreHmdPose(const ovrPosef& eyePose) const;
    void _ToggleShaderWorld();

    virtual glm::mat4 makeWorldToEyeMatrix() const;
    glm::mat4 makeWorldToChassisMatrix() const;

    ovrHmd m_Hmd;
    ovrFovPort m_EyeFov[2];
    ovrGLConfig m_Cfg;
    ovrEyeRenderDesc m_EyeRenderDesc[2];
    ovrGLTexture m_EyeTexture[2];
    bool m_usingDebugHmd;
    bool m_directHmdMode;

    // For client rendering
    ovrRecti m_RenderViewports[2];
    ovrDistortionMesh m_DistMeshes[2];
    mutable ovrPosef m_eyePoseCached;

    // For eye ray tracking - set during draw function
    mutable glm::vec3 m_hmdRo;
    mutable glm::vec3 m_hmdRd;
    mutable glm::vec3 m_hmdRoLocal;
    mutable glm::vec3 m_hmdRdLocal;

public:
    // This public section is for exposing state variables to AntTweakBar
    RaymarchShaderScene m_raymarchScene;
    ShaderGalleryScene m_galleryScene;
    OVRScene m_ovrScene;
    DashboardScene m_dashScene;
    FloorScene m_floorScene;
#ifdef USE_SIXENSE
    HydraScene m_hydraScene;
#endif

    glm::vec3 m_chassisPos;
    glm::vec3 m_chassisPosCached;
    float m_chassisPitch;
    float m_chassisRoll;

protected:
    std::vector<IScene*> m_scenes;
    FBO m_renderBuffer;
    float m_fboScale;
    ShaderWithVariables m_presentFbo;
    ShaderWithVariables m_presentDistMeshL;
    ShaderWithVariables m_presentDistMeshR;

    float m_chassisYaw;
    float m_chassisYawCached;

    VirtualTrackball m_hyif;
    std::map<std::string, textureChannel> m_texLibrary;
    glm::ivec2 m_windowSize;

    Timer m_transitionTimer;
    int m_transitionState;

public:
    float m_headSize;
    FlyingMouse m_fm;
    glm::vec3 m_keyboardMove;
    glm::vec3 m_joystickMove;
    glm::vec3 m_mouseMove;
    float m_keyboardYaw;
    float m_joystickYaw;
    float m_mouseDeltaYaw;
    float m_keyboardDeltaPitch;
    float m_keyboardDeltaRoll;

    float m_cinemaScopeFactor;
    float m_fboMinScale;
#ifdef USE_ANTTWEAKBAR
    TwBar* m_pTweakbar;
    TwBar* m_pShaderTweakbar;
#endif

private: // Disallow copy ctor and assignment operator
    RiftAppSkeleton(const RiftAppSkeleton&);
    RiftAppSkeleton& operator=(const RiftAppSkeleton&);
};
