#include "SohMenuBar.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "ImGui/imgui.h"
#include "regex"
#include "public/bridge/consolevariablebridge.h"
#include <libultraship/libultraship.h>
#include "UIWidgets.hpp"
#include "include/z64audio.h"
#include "OTRGlobals.h"
#include "z64.h"
#include "Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/presets.h"
#include "soh/Enhancements/mods.h"
#include "Enhancements/cosmetics/authenticGfxPatches.h"
#ifdef ENABLE_REMOTE_CONTROL
#include "Enhancements/crowd-control/CrowdControl.h"
#include "Enhancements/game-interactor/GameInteractor_Sail.h"
#endif


#include "Enhancements/audio/AudioEditor.h"
#include "Enhancements/controls/InputViewer.h"
#include "Enhancements/cosmetics/CosmeticsEditor.h"
#include "Enhancements/debugger/actorViewer.h"
#include "Enhancements/debugger/colViewer.h"
#include "Enhancements/debugger/debugSaveEditor.h"
#include "Enhancements/debugger/dlViewer.h"
#include "Enhancements/debugger/valueViewer.h"
#include "Enhancements/gameplaystatswindow.h"
#include "Enhancements/debugger/MessageViewer.h"
#include "Enhancements/randomizer/randomizer_check_tracker.h"
#include "Enhancements/randomizer/randomizer_entrance_tracker.h"
#include "Enhancements/randomizer/randomizer_item_tracker.h"
#include "Enhancements/randomizer/randomizer_settings_window.h"
#include "Enhancements/resolution-editor/ResolutionEditor.h"

extern bool ToggleAltAssetsAtEndOfFrame;
extern bool isBetaQuestEnabled;

extern "C" PlayState* gPlayState;

enum SeqPlayers {
    /* 0 */ SEQ_BGM_MAIN,
    /* 1 */ SEQ_FANFARE,
    /* 2 */ SEQ_SFX,
    /* 3 */ SEQ_BGM_SUB,
    /* 4 */ SEQ_MAX
};

std::string GetWindowButtonText(const char* text, bool menuOpen) {
    char buttonText[100] = "";
    if (menuOpen) {
        strcat(buttonText, ICON_FA_CHEVRON_RIGHT " ");
    }
    strcat(buttonText, text);
    if (!menuOpen) { strcat(buttonText, "  "); }
    return buttonText;
}

static const char* imguiScaleOptions[4] = { "Small", "Normal", "Large", "X-Large" };

    static const char* filters[3] = {
#ifdef __WIIU__
            "",
#else
            "Three-Point",
#endif
            "Linear", "None"
    };

    static const char* chestStyleMatchesContentsOptions[4] = { "Disabled", "Both", "Texture Only", "Size Only" };
    static const char* bunnyHoodOptions[3] = { "Disabled", "Faster Run & Longer Jump", "Faster Run" };
    static const char* mirroredWorldModes[9] = {
        "Disabled",           "Always",        "Random",          "Random (Seeded)",          "Dungeons",
        "Dungeons (Vanilla)", "Dungeons (MQ)", "Dungeons Random", "Dungeons Random (Seeded)",
    };
    static const char* enemyRandomizerModes[3] = { "Disabled", "Random", "Random (Seeded)" };
    static const char* allPowers[9] = {
                        "Vanilla (1x)",
                        "Double (2x)",
                        "Quadruple (4x)",
                        "Octuple (8x)",
                        "Foolish (16x)",
                        "Ridiculous (32x)",
                        "Merciless (64x)",
                        "Pure Torture (128x)",
                        "OHKO (256x)" };
    static const char* subPowers[8] = { allPowers[0], allPowers[1], allPowers[2], allPowers[3], allPowers[4], allPowers[5], allPowers[6], allPowers[7] };
    static const char* subSubPowers[7] = { allPowers[0], allPowers[1], allPowers[2], allPowers[3], allPowers[4], allPowers[5], allPowers[6] };
    static const char* zFightingOptions[3] = { "Disabled", "Consistent Vanish", "No Vanish" };
    static const char* autosaveLabels[6] = { "Off", "New Location + Major Item", "New Location + Any Item", "New Location", "Major Item", "Any Item" };
    static const char* DebugSaveFileModes[3] = { "Off", "Vanilla", "Maxed" };
    static const char* FastFileSelect[5] = { "File N.1", "File N.2", "File N.3", "Zelda Map Select (require OoT Debug Mode)", "File select" };
    static const char* DekuStickCheat[3] = { "Normal", "Unbreakable", "Unbreakable + Always on Fire" };
    static const char* bonkDamageValues[8] = {
        "No Damage",
        "0.25 Heart",
        "0.5 Heart",
        "1 Heart",
        "2 Hearts",
        "4 Hearts",
        "8 Hearts",
        "OHKO"
    };
    static const char* timeTravelOptions[3] = { "Disabled", "Ocarina of Time", "Any Ocarina" };
    static const char* swordToggleModes[3] = { "Disabled", "Child Toggle", "Both Ages (May lead to unintended behaviour)"};

extern "C" SaveContext gSaveContext;

namespace SohGui {

void DrawMenuBarIcon() {
    static bool gameIconLoaded = false;
    if (!gameIconLoaded) {
        LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Game_Icon", "textures/icons/gIcon.png");
        gameIconLoaded = true;
    }

    if (LUS::Context::GetInstance()->GetWindow()->GetGui()->HasTextureByName("Game_Icon")) {
#ifdef __SWITCH__
        ImVec2 iconSize = ImVec2(20.0f, 20.0f);
        float posScale = 1.0f;
#elif defined(__WIIU__)
        ImVec2 iconSize = ImVec2(16.0f * 2, 16.0f * 2);
        float posScale = 2.0f;
#else
        ImVec2 iconSize = ImVec2(16.0f, 16.0f);
        float posScale = 1.0f;
#endif
        ImGui::SetCursorPos(ImVec2(5, 2.5f) * posScale);
        ImGui::Image(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("Game_Icon"), iconSize);
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(25, 0) * posScale);
    }
}

void DrawShipMenu() {
    if (ImGui::BeginMenu("Ship")) {
        if (ImGui::MenuItem("Hide Menu Bar",
#if !defined(__SWITCH__) && !defined(__WIIU__)
         "F1"
#else
         "[-]"
#endif
        )) {
            LUS::Context::GetInstance()->GetWindow()->GetGui()->GetMenuBar()->ToggleVisibility();
        }
        UIWidgets::Spacer(0);
#if !defined(__SWITCH__) && !defined(__WIIU__)
        if (ImGui::MenuItem("Toggle Fullscreen", "F11")) {
            LUS::Context::GetInstance()->GetWindow()->ToggleFullscreen();
        }
        UIWidgets::Spacer(0);
#endif
        if (ImGui::MenuItem("Reset",
#ifdef __APPLE__
                            "Command-R"
#elif !defined(__SWITCH__) && !defined(__WIIU__)
                            "Ctrl+R"
#else
                            ""
#endif
                            )) {
            std::reinterpret_pointer_cast<LUS::ConsoleWindow>(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->Dispatch("reset");
        }
#if !defined(__SWITCH__) && !defined(__WIIU__)
        UIWidgets::Spacer(0);
        if (ImGui::MenuItem("Open App Files Folder")) {
            std::string filesPath = LUS::Context::GetInstance()->GetAppDirectoryPath();
            SDL_OpenURL(std::string("file:///" + std::filesystem::absolute(filesPath).string()).c_str());
        }
        UIWidgets::Spacer(0);

        if (ImGui::MenuItem("Quit")) {
            LUS::Context::GetInstance()->GetWindow()->Close();
        }
#endif
        ImGui::EndMenu();
    }
}

extern std::shared_ptr<LUS::GuiWindow> mInputEditorWindow;
extern std::shared_ptr<InputViewer> mInputViewer;
extern std::shared_ptr<InputViewerSettingsWindow> mInputViewerSettings;
extern std::shared_ptr<AdvancedResolutionSettings::AdvancedResolutionSettingsWindow> mAdvancedResolutionSettingsWindow;

void DrawSettingsMenu() {
    if (ImGui::BeginMenu("Settings"))
    {
        if (ImGui::BeginMenu("Audio")) {
            UIWidgets::PaddedEnhancementSliderFloat("Master Volume: %.1f %%", "##Master_Vol", "gGameMasterVolume", 0.0f, 1.0f, "", 1.0f, true, true, false, true);
            if (UIWidgets::PaddedEnhancementSliderFloat("Main Music Volume: %.1f %%", "##Main_Music_Vol", "gMainMusicVolume", 0.0f, 1.0f, "", 1.0f, true, true, false, true)) {
                Audio_SetGameVolume(SEQ_BGM_MAIN, CVarGetFloat("gMainMusicVolume", 1.0f));
            }
            if (UIWidgets::PaddedEnhancementSliderFloat("Sub Music Volume: %.1f %%", "##Sub_Music_Vol", "gSubMusicVolume", 0.0f, 1.0f, "", 1.0f, true, true, false, true)) {
                Audio_SetGameVolume(SEQ_BGM_SUB, CVarGetFloat("gSubMusicVolume", 1.0f));
            }
            if (UIWidgets::PaddedEnhancementSliderFloat("Sound Effects Volume: %.1f %%", "##Sound_Effect_Vol", "gSFXMusicVolume", 0.0f, 1.0f, "", 1.0f, true, true, false, true)) {
                Audio_SetGameVolume(SEQ_SFX, CVarGetFloat("gSFXMusicVolume", 1.0f));
            }
            if (UIWidgets::PaddedEnhancementSliderFloat("Fanfare Volume: %.1f %%", "##Fanfare_Vol", "gFanfareVolume", 0.0f, 1.0f, "", 1.0f, true, true, false, true)) {
                Audio_SetGameVolume(SEQ_FANFARE, CVarGetFloat("gFanfareVolume", 1.0f));
            }

            static std::unordered_map<LUS::AudioBackend, const char*> audioBackendNames = {
                { LUS::AudioBackend::WASAPI, "Windows Audio Session API" },
                { LUS::AudioBackend::SDL, "SDL" }
            };

            ImGui::Text("Audio API (Needs reload)");
            auto currentAudioBackend = LUS::Context::GetInstance()->GetAudio()->GetAudioBackend();

            if (LUS::Context::GetInstance()->GetAudio()->GetAvailableAudioBackends()->size() <= 1) {
                UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
            }
            if (ImGui::BeginCombo("##AApi", audioBackendNames[currentAudioBackend])) {
                for (uint8_t i = 0; i < LUS::Context::GetInstance()->GetAudio()->GetAvailableAudioBackends()->size(); i++) {
                    auto backend = LUS::Context::GetInstance()->GetAudio()->GetAvailableAudioBackends()->data()[i];
                    if (ImGui::Selectable(audioBackendNames[backend], backend == currentAudioBackend)) {
                        LUS::Context::GetInstance()->GetAudio()->SetAudioBackend(backend);
                    }
                }
                ImGui::EndCombo();
            }
            if (LUS::Context::GetInstance()->GetAudio()->GetAvailableAudioBackends()->size() <= 1) {
                UIWidgets::ReEnableComponent("");
            }

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Controller")) {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 (12.0f, 6.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
            if (mInputEditorWindow) {
                if (ImGui::Button(GetWindowButtonText("Controller Mapping", CVarGetInteger("gControllerConfigurationEnabled", 0)).c_str(), ImVec2 (-1.0f, 0.0f))) {
                    mInputEditorWindow->ToggleVisibility();
                }
            }
            UIWidgets::PaddedSeparator();
            ImGui::PopStyleColor(1);
            ImGui::PopStyleVar(3);
        #ifndef __SWITCH__
            UIWidgets::EnhancementCheckbox("Menubar Controller Navigation", "gControlNav");
            UIWidgets::Tooltip("Allows controller navigation of the SOH menu bar (Settings, Enhancements,...)\nCAUTION: This will disable game inputs while the menubar is visible.\n\nD-pad to move between items, A to select, and X to grab focus on the menu bar");
            UIWidgets::PaddedSeparator();
        #endif
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 (12.0f, 6.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
            if (mInputViewer) {
                if (ImGui::Button(GetWindowButtonText("Input Viewer", CVarGetInteger("gOpenWindows.InputViewer", 0)).c_str(), ImVec2 (-1.0f, 0.0f))) {
                    mInputViewer->ToggleVisibility();
                }
            }
            if (mInputViewerSettings) {
                if (ImGui::Button(GetWindowButtonText("Input Viewer Settings", CVarGetInteger("gOpenWindows.InputViewerSettings", 0)).c_str(), ImVec2 (-1.0f, 0.0f))) {
                    mInputViewerSettings->ToggleVisibility();
                }
            }
            ImGui::PopStyleColor(1);
            ImGui::PopStyleVar(3);

            UIWidgets::PaddedEnhancementSliderInt("Simulated Input Lag: %d frames", "##SimulatedInputLag", "gSimulatedInputLag", 0, 6, "", 0, true, true, false);
            UIWidgets::Tooltip("Buffers your inputs to be executed a specified amount of frames later");

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Graphics")) {
        #ifndef __APPLE__
            const bool disabled_resolutionSlider = CVarGetInteger("gAdvancedResolution.VerticalResolutionToggle", 0) &&
                                                   CVarGetInteger("gAdvancedResolution.Enabled", 0);
            if (UIWidgets::EnhancementSliderFloat("Internal Resolution: %.1f %%", "##IMul", "gInternalResolution", 0.5f,
                                                  2.0f, "", 1.0f, true, true, disabled_resolutionSlider)) {
                LUS::Context::GetInstance()->GetWindow()->SetResolutionMultiplier(CVarGetFloat("gInternalResolution", 1));
            }
            UIWidgets::Tooltip("Resolution scale. Multiplies output resolution by this value, on each axis relative to window size.\n"
                               "Lower values may improve performance.\n"
                               "Values above 100% can be used for super-sampling, as an intensive but highly effective form of anti-aliasing.\n\n"
                               "Default: 100%");
            
            if (mAdvancedResolutionSettingsWindow) {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
                UIWidgets::Spacer(0);
                if (ImGui::Button(GetWindowButtonText("Advanced Resolution", CVarGetInteger("gAdvancedResolutionEditorEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                    mAdvancedResolutionSettingsWindow->ToggleVisibility();
                }
                ImGui::PopStyleColor(1);
                ImGui::PopStyleVar(3);
            }
        #else
            // macOS: Internal resolution is currently disabled in libultraship.
            ImGui::BeginGroup();
            ImGui::Text("Internal Resolution: 100.0%%");
            UIWidgets::Spacer(0);
            ImGui::Text(" " ICON_FA_INFO_CIRCLE " Not available on this system.");
            UIWidgets::Spacer(0);
            ImGui::EndGroup();
        #endif

        #ifndef __WIIU__
            if (UIWidgets::PaddedEnhancementSliderInt(
                    (CVarGetInteger("gMSAAValue", 1) == 1) ? "Anti-aliasing (MSAA): Off" : "Anti-aliasing (MSAA): %d",
                    "##IMSAA", "gMSAAValue", 1, 8, "", 1, true, true, false)) {
                LUS::Context::GetInstance()->GetWindow()->SetMsaaLevel(CVarGetInteger("gMSAAValue", 1));
            }
            UIWidgets::Tooltip("Activates MSAA (multi-sample anti-aliasing) from 2x up to 8x, to smooth the edges of rendered geometry.\n"
                               "Higher sample count will result in smoother edges on models, but may reduce performance.\n\n"
                               "Recommended: 2x or 4x");
        #endif

            UIWidgets::PaddedSeparator(true, true, 3.0f, 3.0f);
            { // FPS Slider
                const int minFps = 20;
                static int maxFps;
                if (LUS::Context::GetInstance()->GetWindow()->GetWindowBackend() == LUS::WindowBackend::DX11) {
                    maxFps = 360;
                } else {
                    maxFps = LUS::Context::GetInstance()->GetWindow()->GetCurrentRefreshRate();
                }
                int currentFps = fmax(fmin(OTRGlobals::Instance->GetInterpolationFPS(), maxFps), minFps);
            #ifdef __WIIU__
                UIWidgets::Spacer(0);
                // only support divisors of 60 on the Wii U
                if (currentFps > 60) {
                    currentFps = 60;
                } else {
                    currentFps = 60 / (60 / currentFps);
                }

                int fpsSlider = 1;
                if (currentFps == 20) {
                    ImGui::Text("FPS: Original (20)");
                } else {
                    ImGui::Text("FPS: %d", currentFps);
                    if (currentFps == 30) {
                        fpsSlider = 2;
                    } else { // currentFps == 60
                        fpsSlider = 3;
                    }
                }
                if (CVarGetInteger("gMatchRefreshRate", 0)) {
                    UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
                }

                if (ImGui::Button(" - ##WiiUFPS")) {
                    fpsSlider--;
                }
                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);

                UIWidgets::Spacer(0);

                ImGui::PushItemWidth(std::min((ImGui::GetContentRegionAvail().x - 60.0f), 260.0f));
                ImGui::SliderInt("##WiiUFPSSlider", &fpsSlider, 1, 3, "", ImGuiSliderFlags_AlwaysClamp);
                ImGui::PopItemWidth();

                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
                if (ImGui::Button(" + ##WiiUFPS")) {
                    fpsSlider++;
                }

                if (CVarGetInteger("gMatchRefreshRate", 0)) {
                    UIWidgets::ReEnableComponent("");
                }
                if (fpsSlider > 3) {
                    fpsSlider = 3;
                } else if (fpsSlider < 1) {
                    fpsSlider = 1;
                }

                if (fpsSlider == 1) {
                    currentFps = 20;
                } else if (fpsSlider == 2) {
                    currentFps = 30;
                } else if (fpsSlider == 3) {
                    currentFps = 60;
                }
                CVarSetInteger("gInterpolationFPS", currentFps);
                LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            #else
                bool matchingRefreshRate =
                    CVarGetInteger("gMatchRefreshRate", 0) && LUS::Context::GetInstance()->GetWindow()->GetWindowBackend() != LUS::WindowBackend::DX11;
                UIWidgets::PaddedEnhancementSliderInt(
                    (currentFps == 20) ? "Frame Rate: Original (20 fps)" : "Frame Rate: %d fps",
                    "##FPSInterpolation", "gInterpolationFPS", minFps, maxFps, "", 20, true, true, false, matchingRefreshRate);
            #endif
                if (LUS::Context::GetInstance()->GetWindow()->GetWindowBackend() == LUS::WindowBackend::DX11) {
                    UIWidgets::Tooltip(
                        "Uses Matrix Interpolation to create extra frames, resulting in smoother graphics.\n"
                        "This is purely visual and does not impact game logic, execution of glitches etc.\n"
                        "Higher frame rate settings may impact CPU performance."
                        "\n\n " ICON_FA_INFO_CIRCLE 
                        " There is no need to set this above your monitor's refresh rate. Doing so will waste resources and may give a worse result.");
                } else {
                    UIWidgets::Tooltip(
                        "Uses Matrix Interpolation to create extra frames, resulting in smoother graphics.\n"
                        "This is purely visual and does not impact game logic, execution of glitches etc.\n"
                        "Higher frame rate settings may impact CPU performance.");
                }
            } // END FPS Slider

            if (LUS::Context::GetInstance()->GetWindow()->GetWindowBackend() == LUS::WindowBackend::DX11) {
                UIWidgets::Spacer(0);
                if (ImGui::Button("Match Frame Rate to Refresh Rate")) {
                    int hz = LUS::Context::GetInstance()->GetWindow()->GetCurrentRefreshRate();
                    if (hz >= 20 && hz <= 360) {
                        CVarSetInteger("gInterpolationFPS", hz);
                        LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                    }
                }
            } else {
                UIWidgets::PaddedEnhancementCheckbox("Match Frame Rate to Refresh Rate", "gMatchRefreshRate", true, false);
            }
            UIWidgets::Tooltip("Matches interpolation value to the game window's current refresh rate.");

            if (LUS::Context::GetInstance()->GetWindow()->GetWindowBackend() == LUS::WindowBackend::DX11) {
                UIWidgets::PaddedEnhancementSliderInt(CVarGetInteger("gExtraLatencyThreshold", 80) == 0 ? "Jitter fix: Off" : "Jitter fix: >= %d FPS",
                    "##ExtraLatencyThreshold", "gExtraLatencyThreshold", 0, 360, "", 80, true, true, false);
                UIWidgets::Tooltip(
                    "(For DirectX backend only)\n\n"
                    "When Interpolation FPS (Frame Rate) setting is at least this threshold, add one frame of delay (e.g. 16.6 ms for 60 FPS) in order to avoid jitter."
                    "This setting allows the CPU to work on one frame while GPU works on the previous frame.\n"
                    "This setting should be used when your computer is too slow to do CPU + GPU work in time.");
            }

            UIWidgets::PaddedSeparator(true, true, 3.0f, 3.0f);

            ImGui::Text("ImGui Menu Scale");
            ImGui::SameLine();
            ImGui::TextColored({ 0.85f, 0.35f, 0.0f, 1.0f }, "(Experimental)");
            if (UIWidgets::EnhancementCombobox("gImGuiScale", imguiScaleOptions, 1)) {
                OTRGlobals::Instance->ScaleImGui();
            }
            UIWidgets::Tooltip("Changes the scaling of the ImGui menu elements.");

            UIWidgets::PaddedSeparator(true, true, 3.0f, 3.0f);
            
            static std::unordered_map<LUS::WindowBackend, const char*> windowBackendNames = {
                { LUS::WindowBackend::DX11, "DirectX" },
                { LUS::WindowBackend::SDL_OPENGL, "OpenGL"},
                { LUS::WindowBackend::SDL_METAL, "Metal" },
                { LUS::WindowBackend::GX2, "GX2"}
            };

            ImGui::Text("Renderer API (Needs reload)");
            LUS::WindowBackend runningWindowBackend = LUS::Context::GetInstance()->GetWindow()->GetWindowBackend();
            LUS::WindowBackend configWindowBackend;
            int configWindowBackendId = LUS::Context::GetInstance()->GetConfig()->GetInt("Window.Backend.Id", -1);
            if (configWindowBackendId != -1 && configWindowBackendId < static_cast<int>(LUS::WindowBackend::BACKEND_COUNT)) {
                configWindowBackend = static_cast<LUS::WindowBackend>(configWindowBackendId);
            } else {
                configWindowBackend = runningWindowBackend;
            }

            if (LUS::Context::GetInstance()->GetWindow()->GetAvailableWindowBackends()->size() <= 1) {
                UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
            }
            if (ImGui::BeginCombo("##RApi", windowBackendNames[configWindowBackend])) {
                for (size_t i = 0; i < LUS::Context::GetInstance()->GetWindow()->GetAvailableWindowBackends()->size(); i++) {
                    auto backend = LUS::Context::GetInstance()->GetWindow()->GetAvailableWindowBackends()->data()[i];
                    if (ImGui::Selectable(windowBackendNames[backend], backend == configWindowBackend)) {
                        LUS::Context::GetInstance()->GetConfig()->SetInt("Window.Backend.Id", static_cast<int>(backend));
                        LUS::Context::GetInstance()->GetConfig()->SetString("Window.Backend.Name",
                                                                            windowBackendNames[backend]);
                        LUS::Context::GetInstance()->GetConfig()->Save();
                    }
                }
                ImGui::EndCombo();
            }
            if (LUS::Context::GetInstance()->GetWindow()->GetAvailableWindowBackends()->size() <= 1) {
                UIWidgets::ReEnableComponent("");
            }

            if (LUS::Context::GetInstance()->GetWindow()->CanDisableVerticalSync()) {
                UIWidgets::PaddedEnhancementCheckbox("Enable Vsync", "gVsyncEnabled", true, false);
                UIWidgets::Tooltip("Activate vertical sync, to prevent screen tearing.");
            }

            if (LUS::Context::GetInstance()->GetWindow()->SupportsWindowedFullscreen()) {
                UIWidgets::PaddedEnhancementCheckbox("Windowed fullscreen", "gSdlWindowedFullscreen", true, false);
            }

            if (LUS::Context::GetInstance()->GetWindow()->GetGui()->SupportsViewports()) {
                UIWidgets::PaddedEnhancementCheckbox("Allow multi-windows (Needs reload)", "gEnableMultiViewports", true, false, false, "", UIWidgets::CheckboxGraphics::Cross, true);
                UIWidgets::Tooltip("Allows windows to be able to be dragged off of the main game window. Requires a reload to take effect.");
            }

            // If more filters are added to LUS, make sure to add them to the filters list here
            ImGui::Text("Texture Filtering (Needs reload)");
            UIWidgets::EnhancementCombobox("gTextureFilter", filters, FILTER_THREE_POINT);
            UIWidgets::Tooltip("Texture filtering, aka texture smoothing. Requires a reload to take effect.\n\n"
                               "Three-Point: Replicates real N64 texture filtering.\n"
                               "Bilinear: If Three-Point causes poor performance, try this.\n"
                               "Nearest: Disables texture smoothing. (Not recommended)");

            UIWidgets::PaddedSeparator(true, true, 3.0f, 3.0f);

            // Draw LUS settings menu (such as Overlays Text Font)
            LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGameOverlay()->DrawSettings();

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Languages")) {
            UIWidgets::PaddedEnhancementCheckbox("Translate Title Screen", "gTitleScreenTranslation");
            if (UIWidgets::EnhancementRadioButton("English", "gLanguages", LANGUAGE_ENG)) {
                GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSetGameLanguage>();
            }
            if (UIWidgets::EnhancementRadioButton("German", "gLanguages", LANGUAGE_GER)) {
                GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSetGameLanguage>();
            }
            if (UIWidgets::EnhancementRadioButton("French", "gLanguages", LANGUAGE_FRA)) {
                GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSetGameLanguage>();
            }
            ImGui::EndMenu();
        }
        
        UIWidgets::Spacer(0);
        
        if (ImGui::BeginMenu("Accessibility")) {
        #if defined(_WIN32) || defined(__APPLE__)
            UIWidgets::PaddedEnhancementCheckbox("Text to Speech", "gA11yTTS");
            UIWidgets::Tooltip("Enables text to speech for in game dialog");
        #endif
            UIWidgets::PaddedEnhancementCheckbox("Disable Idle Camera Re-Centering", "gA11yDisableIdleCam");
            UIWidgets::Tooltip("Disables the automatic re-centering of the camera when idle.");
            
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
}

extern std::shared_ptr<AudioEditor> mAudioEditorWindow;
extern std::shared_ptr<CosmeticsEditorWindow> mCosmeticsEditorWindow;
extern std::shared_ptr<GameplayStatsWindow> mGameplayStatsWindow;

void DrawEnhancementsMenu() {
    if (ImGui::BeginMenu("Enhancements"))
    {
        ImGui::BeginDisabled(CVarGetInteger("gDisableChangingSettings", 0));

        DrawPresetSelector(PRESET_TYPE_ENHANCEMENTS);

        UIWidgets::PaddedSeparator();

        if (ImGui::BeginMenu("Gameplay"))
        {
            if (ImGui::BeginMenu("Time Savers"))
            {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8.0f);
                ImGui::BeginTable("##timeSaversMenu", 2, ImGuiTableFlags_SizingFixedFit);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableNextColumn();
                UIWidgets::Spacer(0);
                ImGui::Text("Speed-ups:");
                UIWidgets::PaddedSeparator();

                UIWidgets::PaddedEnhancementSliderInt("Text Speed: %dx", "##TEXTSPEED", "gTextSpeed", 1, 5, "", 1, true, false, true);
                UIWidgets::PaddedEnhancementCheckbox("Skip Text", "gSkipText", false, true);
                UIWidgets::Tooltip("Holding down B skips text");
                UIWidgets::PaddedEnhancementSliderInt("King Zora Speed: %dx", "##MWEEPSPEED", "gMweepSpeed", 1, 5, "", 1, true, false, true);
                UIWidgets::PaddedEnhancementSliderInt("Vine/Ladder Climb speed +%d", "##CLIMBSPEED", "gClimbSpeed", 0, 12, "", 0, true, false, true);
                UIWidgets::PaddedEnhancementSliderInt("Block pushing speed +%d", "##BLOCKSPEED", "gFasterBlockPush", 0, 5, "", 0, true, false, true);
                UIWidgets::PaddedEnhancementSliderInt("Crawl speed %dx", "##CRAWLSPEED", "gCrawlSpeed", 1, 5, "", 1, true, false, true);
                UIWidgets::PaddedEnhancementCheckbox("Faster Heavy Block Lift", "gFasterHeavyBlockLift", false, false);
                UIWidgets::Tooltip("Speeds up lifting silver rocks and obelisks");
                UIWidgets::PaddedEnhancementCheckbox("Skip Pickup Messages", "gFastDrops", true, false);
                UIWidgets::Tooltip("Skip pickup messages for new consumable items and bottle swipes");
                UIWidgets::PaddedEnhancementCheckbox("Fast Ocarina Playback", "gFastOcarinaPlayback", true, false);
                UIWidgets::Tooltip("Skip the part where the Ocarina playback is called when you play a song");
                bool forceSkipScarecrow = IS_RANDO && OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SKIP_SCARECROWS_SONG);
                static const char* forceSkipScarecrowText = "This setting is forcefully enabled because a savefile\nwith \"Skip Scarecrow Song\" is loaded";
                UIWidgets::PaddedEnhancementCheckbox("Skip Scarecrow Song", "gSkipScarecrow", true, false,
                                                        forceSkipScarecrow, forceSkipScarecrowText, UIWidgets::CheckboxGraphics::Checkmark);
                UIWidgets::Tooltip("Pierre appears when Ocarina is pulled out. Requires learning scarecrow song.");
                UIWidgets::PaddedEnhancementCheckbox("Skip Magic Arrow Equip Animation", "gSkipArrowAnimation", true, false);
                UIWidgets::PaddedEnhancementCheckbox("Skip save confirmation", "gSkipSaveConfirmation", true, false);
                UIWidgets::Tooltip("Skip the \"Game saved.\" confirmation screen");
                UIWidgets::PaddedEnhancementCheckbox("Faster Farore's Wind", "gFastFarores", true, false);
                UIWidgets::Tooltip("Greatly decreases cast time of Farore's Wind magic spell.");
                UIWidgets::PaddedEnhancementCheckbox("Skip water take breath animation", "gSkipSwimDeepEndAnim", true, false);
                UIWidgets::Tooltip("Skips Link's taking breath animation after coming up from water. This setting does not interfere with getting items from underwater.");

                ImGui::TableNextColumn();
                UIWidgets::Spacer(0);
                ImGui::Text("Changes:");
                UIWidgets::PaddedSeparator();
                
                UIWidgets::PaddedEnhancementSliderInt("Biggoron Forge Time: %d days", "##FORGETIME", "gForgeTime", 0, 3, "", 3, true, false, true);
                UIWidgets::Tooltip("Allows you to change the number of days it takes for Biggoron to forge the Biggoron Sword");
                UIWidgets::PaddedEnhancementCheckbox("Remember Save Location", "gRememberSaveLocation", false, false);
                UIWidgets::Tooltip("When loading a save, places Link at the last entrance he went through.\n"
                        "This doesn't work if the save was made in a grotto.");
                UIWidgets::PaddedEnhancementCheckbox("No Forced Navi", "gNoForcedNavi", true, false);
                UIWidgets::Tooltip("Prevent forced Navi conversations");
                UIWidgets::PaddedEnhancementCheckbox("Navi Timer Resets", "gEnhancements.ResetNaviTimer", true, false);
                UIWidgets::Tooltip("Resets the Navi timer on scene change. If you have already talked to her, she will try and talk to you again, instead of needing a save warp or death. ");
                UIWidgets::PaddedEnhancementCheckbox("No Skulltula Freeze", "gSkulltulaFreeze", true, false);
                UIWidgets::Tooltip("Stops the game from freezing the player when picking up Gold Skulltulas");
                UIWidgets::PaddedEnhancementCheckbox("Nighttime GS Always Spawn", "gNightGSAlwaysSpawn", true, false);
                UIWidgets::Tooltip("Nighttime Skulltulas will spawn during both day and night.");
                UIWidgets::PaddedEnhancementCheckbox("Dampe Appears All Night", "gDampeAllNight", true, false);
                UIWidgets::Tooltip("Makes Dampe appear anytime during the night, not just his usual working hours.");
                UIWidgets::PaddedEnhancementCheckbox("Fast Chests", "gFastChests", true, false);
                UIWidgets::Tooltip("Kick open every chest");
                UIWidgets::PaddedText("Chest size & texture matches contents", true, false);
                if (UIWidgets::EnhancementCombobox("gChestSizeAndTextureMatchesContents", chestStyleMatchesContentsOptions, CSMC_DISABLED)) {
                    if (CVarGetInteger("gChestSizeAndTextureMatchesContents", CSMC_DISABLED) == CSMC_DISABLED) {
                        CVarSetInteger("gChestSizeDependsStoneOfAgony", 0);
                    }
                }
                UIWidgets::Tooltip(
                    "Chest sizes and textures are changed to help identify the item inside.\n"
                    " - Major items: Large gold chests\n"
                    " - Lesser items: Large brown chests\n"
                    " - Junk items: Small brown chests\n"
                    " - Small keys: Small silver chest\n"
                    " - Boss keys: Vanilla size and texture\n"
                    " - Skulltula Tokens: Small skulltula chest\n"
                    "\n"
                    "NOTE: Textures will not apply if you are using a mod pack with a custom chest model."
                );
                if (CVarGetInteger("gChestSizeAndTextureMatchesContents", CSMC_DISABLED) != CSMC_DISABLED) {
                    UIWidgets::PaddedEnhancementCheckbox("Chests of Agony", "gChestSizeDependsStoneOfAgony", true, false);
                    UIWidgets::Tooltip("Only change the size/texture of chests if you have the Stone of Agony.");
                }
                UIWidgets::PaddedEnhancementCheckbox("Ask to Equip New Items", "gAskToEquip", true, false);
                UIWidgets::Tooltip("Adds a prompt to equip newly-obtained swords, shields and tunics");
                UIWidgets::PaddedEnhancementCheckbox("Better Owl", "gBetterOwl", true, false);
                UIWidgets::Tooltip("The default response to Kaepora Gaebora is always that you understood what he said");
                UIWidgets::PaddedEnhancementCheckbox("Exit Market at Night", "gMarketSneak", true, false);
                UIWidgets::Tooltip("Allows exiting Hyrule Castle Market Town to Hyrule Field at night by speaking to the guard next to the gate.");
                UIWidgets::PaddedEnhancementCheckbox("Shops and Games Always Open", "gEnhancements.OpenAllHours", true, false);
                UIWidgets::Tooltip("Shops and minigames are open both day and night. Requires scene reload to take effect.");
                UIWidgets::PaddedEnhancementCheckbox("Link as default file name", "gLinkDefaultName", true, false);
                UIWidgets::Tooltip("Allows you to have \"Link\" as a premade file name");
				UIWidgets::PaddedEnhancementCheckbox("Quit Fishing At Door", "gQuitFishingAtDoor", true, false);
                UIWidgets::Tooltip("Fisherman asks if you want to quit at the door when you still have the rod");
                UIWidgets::PaddedText("Time Travel with the Song of Time", true, false);
                UIWidgets::EnhancementCombobox("gTimeTravel", timeTravelOptions, 0);
                UIWidgets::Tooltip("Allows Link to freely change age by playing the Song of Time.\n"
                    "Time Blocks can still be used properly.\n\n"
                    "Requirements:\n"
                    "- Obtained the Ocarina of Time (depends on selection)\n"
                    "- Obtained the Song of Time\n"
                    "- Obtained the Master Sword\n"
                    "- Not within range of Time Block\n"
                    "- Not within range of Ocarina playing spots");
                UIWidgets::PaddedEnhancementCheckbox("Pause Warp", "gPauseWarp", true, false);
                UIWidgets::Tooltip("Selection of warp song in pause menu initiates warp. Disables song playback.");
                
                ImGui::EndTable();
                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            if (ImGui::BeginMenu("Items"))
            {
                UIWidgets::PaddedEnhancementCheckbox("Instant Putaway", "gInstantPutaway", true, false);
                UIWidgets::Tooltip("Allow Link to put items away without having to wait around");
                UIWidgets::PaddedEnhancementCheckbox("Instant Boomerang Recall", "gFastBoomerang", true, false);
                UIWidgets::Tooltip("Instantly return the boomerang to Link by pressing its item button while it's in the air");
                UIWidgets::PaddedEnhancementCheckbox("Prevent Dropped Ocarina Inputs", "gDpadNoDropOcarinaInput", true, false);
                UIWidgets::Tooltip("Prevent dropping inputs when playing the ocarina quickly");
                UIWidgets::PaddedText("Bunny Hood Effect", true, false);
                UIWidgets::EnhancementCombobox("gMMBunnyHood", bunnyHoodOptions, BUNNY_HOOD_VANILLA);
                UIWidgets::Tooltip(
                    "Wearing the Bunny Hood grants a speed increase like in Majora's Mask. The longer jump option is not accounted for in randomizer logic.\n\n"
                    "Also disables NPC's reactions to wearing the Bunny Hood."
                );
                UIWidgets::PaddedEnhancementCheckbox("Bunny Hood Equippable as Adult", "gAdultBunnyHood", true, false, (CVarGetInteger("gMMBunnyHood", BUNNY_HOOD_VANILLA) == BUNNY_HOOD_VANILLA), "Only available with increased bunny hood speed", UIWidgets::CheckboxGraphics::Cross, false);
                UIWidgets::Tooltip("Allows the bunny hood to be equipped normally from the pause menu as adult.");
                UIWidgets::PaddedEnhancementCheckbox("Mask Select in Inventory", "gMaskSelect", true, false);
                UIWidgets::Tooltip("After completing the mask trading sub-quest, press A and any direction on the mask slot to change masks");
                UIWidgets::PaddedEnhancementCheckbox("Nuts explode bombs", "gNutsExplodeBombs", true, false);
                UIWidgets::Tooltip("Makes nuts explode bombs, similar to how they interact with bombchus. This does not affect bombflowers.");
                UIWidgets::PaddedEnhancementCheckbox("Equip Multiple Arrows at Once", "gSeparateArrows", true, false);
                UIWidgets::Tooltip("Allow the bow and magic arrows to be equipped at the same time on different slots. (Note this will disable the behaviour of the 'Equip Dupe' glitch)");
                UIWidgets::PaddedEnhancementCheckbox("Bow as Child/Slingshot as Adult", "gBowSlingShotAmmoFix", true, false);
                UIWidgets::Tooltip("Allows child to use bow with arrows.\nAllows adult to use slingshot with seeds.\n\nRequires glitches or 'Timeless Equipment' cheat to equip.");
                UIWidgets::PaddedEnhancementCheckbox("Better Farore's Wind", "gBetterFW", true, false);
                UIWidgets::Tooltip("Helps FW persist between ages, gives child and adult separate FW points, and can be used in more places.");
                UIWidgets::PaddedEnhancementCheckbox("Remove Explosive Limit", "gRemoveExplosiveLimit", true, false);
                UIWidgets::Tooltip("Removes the cap of 3 active explosives being deployed at once.");
                UIWidgets::PaddedEnhancementCheckbox("Static Explosion Radius", "gStaticExplosionRadius", true, false);
                UIWidgets::Tooltip("Explosions are now a static size, like in Majora's Mask and OoT3D. Makes bombchu hovering much easier.");
                UIWidgets::PaddedEnhancementCheckbox("Prevent Bombchus Forcing First-Person", "gDisableFirstPersonChus", true, false);
                UIWidgets::Tooltip("Prevent bombchus from forcing the camera into first-person mode when released.");
                UIWidgets::PaddedEnhancementCheckbox("Aiming reticle for the bow/slingshot", "gBowReticle", true, false);
                UIWidgets::Tooltip("Aiming with a bow or slingshot will display a reticle as with the hookshot when the projectile is ready to fire.");
                if (UIWidgets::PaddedEnhancementCheckbox("Allow strength equipment to be toggled", "gToggleStrength", true, false)) {
                    if (!CVarGetInteger("gToggleStrength", 0)) {
                        CVarSetInteger("gStrengthDisabled", 0);
                    }
                }
                UIWidgets::Tooltip("Allows strength to be toggled on and off by pressing A on the strength upgrade in the equipment subscreen of the pause menu (This allows performing some glitches that require the player to not have strength).");
                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            if (ImGui::BeginMenu("Difficulty Options"))
            {
                if (ImGui::BeginMenu("Potion Values"))
                {
                    UIWidgets::EnhancementCheckbox("Change Red Potion Effect", "gRedPotionEffect");
                    UIWidgets::Tooltip("Enable the following changes to the amount of health restored by Red Potions");
                    bool disabledRedPotion = !CVarGetInteger("gRedPotionEffect", 0);
                    static const char* disabledTooltipRedPotion = "This option is disabled because \"Change Red Potion Effect\" is turned off";
                    UIWidgets::EnhancementSliderInt("Red Potion Health: %d", "##REDPOTIONHEALTH", "gRedPotionHealth", 1, 100, "", 0, true, disabledRedPotion, disabledTooltipRedPotion);
                    UIWidgets::Tooltip("Changes the amount of health restored by Red Potions");
                    UIWidgets::EnhancementCheckbox("Red Potion Percent Restore", "gRedPercentRestore", disabledRedPotion, disabledTooltipRedPotion);
                    UIWidgets::Tooltip("Toggles from Red Potions restoring a fixed amount of health to a percent of the player's current max health");

                    ImGui::Separator();

                    UIWidgets::EnhancementCheckbox("Change Green Potion Effect", "gGreenPotionEffect");
                    UIWidgets::Tooltip("Enable the following changes to the amount of mana restored by Green Potions");
                    bool disabledGreenPotion = !CVarGetInteger("gGreenPotionEffect", 0);
                    static const char* disabledTooltipGreenPotion = "This option is disabled because \"Change Green Potion Effect\" is turned off";
                    UIWidgets::EnhancementSliderInt("Green Potion Mana: %d", "##GREENPOTIONMANA", "gGreenPotionMana", 1, 100, "", 0, true, disabledGreenPotion, disabledTooltipGreenPotion);
                    UIWidgets::Tooltip("Changes the amount of mana restored by Green Potions, base max mana is 48, max upgraded mana is 96");
                    UIWidgets::EnhancementCheckbox("Green Potion Percent Restore", "gGreenPercentRestore", disabledGreenPotion, disabledTooltipGreenPotion);
                    UIWidgets::Tooltip("Toggles from Green Potions restoring a fixed amount of mana to a percent of the player's current max mana");

                    ImGui::Separator();

                    UIWidgets::EnhancementCheckbox("Change Blue Potion Effects", "gBluePotionEffects");
                    UIWidgets::Tooltip("Enable the following changes to the amount of health and mana restored by Blue Potions");
                    bool disabledBluePotion = !CVarGetInteger("gBluePotionEffects", 0);
                    static const char* disabledTooltipBluePotion = "This option is disabled because \"Change Blue Potion Effects\" is turned off";
                    UIWidgets::EnhancementSliderInt("Blue Potion Health: %d", "##BLUEPOTIONHEALTH", "gBluePotionHealth", 1, 100, "", 0, true, disabledBluePotion, disabledTooltipBluePotion);
                    UIWidgets::Tooltip("Changes the amount of health restored by Blue Potions");
                    UIWidgets::EnhancementCheckbox("Blue Potion Health Percent Restore", "gBlueHealthPercentRestore", disabledBluePotion, disabledTooltipBluePotion);
                    UIWidgets::Tooltip("Toggles from Blue Potions restoring a fixed amount of health to a percent of the player's current max health");

                    ImGui::Separator();

                    UIWidgets::EnhancementSliderInt("Blue Potion Mana: %d", "##BLUEPOTIONMANA", "gBluePotionMana", 1, 100, "", 0, true, disabledBluePotion, disabledTooltipBluePotion);
                    UIWidgets::Tooltip("Changes the amount of mana restored by Blue Potions, base max mana is 48, max upgraded mana is 96");
                    UIWidgets::EnhancementCheckbox("Blue Potion Mana Percent Restore", "gBlueManaPercentRestore", disabledBluePotion, disabledTooltipBluePotion);
                    UIWidgets::Tooltip("Toggles from Blue Potions restoring a fixed amount of mana to a percent of the player's current max mana");

                    ImGui::Separator();

                    UIWidgets::EnhancementCheckbox("Change Milk Effect", "gMilkEffect");
                    UIWidgets::Tooltip("Enable the following changes to the amount of health restored by Milk");
                    bool disabledMilk = !CVarGetInteger("gMilkEffect", 0);
                    static const char* disabledTooltipMilk = "This option is disabled because \"Change Milk Effect\" is turned off";
                    UIWidgets::EnhancementSliderInt("Milk Health: %d", "##MILKHEALTH", "gMilkHealth", 1, 100, "", 0, true, disabledMilk, disabledTooltipMilk);
                    UIWidgets::Tooltip("Changes the amount of health restored by Milk");
                    UIWidgets::EnhancementCheckbox("Milk Percent Restore", "gMilkPercentRestore", disabledMilk, disabledTooltipMilk);
                    UIWidgets::Tooltip("Toggles from Milk restoring a fixed amount of health to a percent of the player's current max health");

                    ImGui::Separator();

                    UIWidgets::EnhancementCheckbox("Separate Half Milk Effect", "gSeparateHalfMilkEffect", disabledMilk, disabledTooltipMilk);
                    UIWidgets::Tooltip("Enable the following changes to the amount of health restored by Half Milk\nIf this is disabled, Half Milk will behave the same as Full Milk.");
                    bool disabledHalfMilk = disabledMilk || !CVarGetInteger("gSeparateHalfMilkEffect", 0);
                    static const char* disabledTooltipHalfMilk = "This option is disabled because \"Separate Half Milk Effect\" is turned off";
                    UIWidgets::EnhancementSliderInt("Half Milk Health: %d", "##HALFMILKHEALTH", "gHalfMilkHealth", 1, 100, "", 0, true, disabledHalfMilk, disabledTooltipHalfMilk);
                    UIWidgets::Tooltip("Changes the amount of health restored by Half Milk");
                    UIWidgets::EnhancementCheckbox("Half Milk Percent Restore", "gHalfMilkPercentRestore", disabledHalfMilk, disabledTooltipHalfMilk);
                    UIWidgets::Tooltip("Toggles from Half Milk restoring a fixed amount of health to a percent of the player's current max health");

                    ImGui::Separator();

                    UIWidgets::EnhancementCheckbox("Change Fairy Effect", "gFairyEffect");
                    UIWidgets::Tooltip("Enable the following changes to the amount of health restored by Fairies");
                    bool disabledFairy = !CVarGetInteger("gFairyEffect", 0);
                    static const char* disabledTooltipFairy = "This option is disabled because \"Change Fairy Effect\" is turned off";
                    UIWidgets::EnhancementSliderInt("Fairy: %d", "##FAIRYHEALTH", "gFairyHealth", 1, 100, "", 0, true, disabledFairy, disabledTooltipFairy);
                    UIWidgets::Tooltip("Changes the amount of health restored by Fairies");
                    UIWidgets::EnhancementCheckbox("Fairy Percent Restore", "gFairyPercentRestore", disabledFairy, disabledTooltipFairy);
                    UIWidgets::Tooltip("Toggles from Fairies restoring a fixed amount of health to a percent of the player's current max health");

                    ImGui::Separator();

                    UIWidgets::EnhancementCheckbox("Change Fairy Revive Effect", "gFairyReviveEffect");
                    UIWidgets::Tooltip("Enable the following changes to the amount of health restored by Fairy Revivals");
                    bool disabledFairyRevive = !CVarGetInteger("gFairyReviveEffect", 0);
                    static const char* disabledTooltipFairyRevive = "This option is disabled because \"Change Fairy Revive Effect\" is turned off";
                    UIWidgets::EnhancementSliderInt("Fairy Revival: %d", "##FAIRYREVIVEHEALTH", "gFairyReviveHealth", 1, 100, "", 0, true, disabledFairyRevive, disabledTooltipFairyRevive);
                    UIWidgets::Tooltip("Changes the amount of health restored by Fairy Revivals");
                    UIWidgets::EnhancementCheckbox("Fairy Revive Percent Restore", "gFairyRevivePercentRestore", disabledFairyRevive, disabledTooltipFairyRevive);
                    UIWidgets::Tooltip("Toggles from Fairy Revivals restoring a fixed amount of health to a percent of the player's current max health");

                    ImGui::EndMenu();
                }

                UIWidgets::Spacer(0);

                if (ImGui::BeginMenu("Shooting Gallery")) {
                    UIWidgets::EnhancementCheckbox("Customize Behavior", "gCustomizeShootingGallery");
                    UIWidgets::Tooltip("Turn on/off changes to the shooting gallery behavior");
                    bool disabled = !CVarGetInteger("gCustomizeShootingGallery", 0);
                    static const char* disabledTooltip = "This option is disabled because \"Customize Behavior\" is turned off";
                    UIWidgets::PaddedEnhancementCheckbox("Instant Win", "gInstantShootingGalleryWin", true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("Skips the shooting gallery minigame");
                    UIWidgets::PaddedEnhancementCheckbox("No Rupee Randomization", "gConstantAdultGallery", true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("Forces the rupee order to not be randomized as adult, making it the same as chlid");
                    UIWidgets::PaddedEnhancementSliderInt("Child Starting Ammunition: %d", "##cShootingGalleryAmmunition", "gChildShootingGalleryAmmunition", 10, 30, "", 15, true, true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("The ammunition at the start of the shooting gallery minigame as a child");
                    UIWidgets::PaddedEnhancementSliderInt("Adult Starting Ammunition: %d", "##aShootingGalleryAmmunition", "gAdultShootingGalleryAmmunition", 10, 30, "", 15, true, true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("The ammunition at the start of the shooting gallery minigame as an adult");
                    ImGui::EndMenu();
                }

                UIWidgets::Spacer(0);

                if (ImGui::BeginMenu("Bombchu Bowling")) {
                    UIWidgets::EnhancementCheckbox("Customize Behavior", "gCustomizeBombchuBowling");
                    UIWidgets::Tooltip("Turn on/off changes to the bombchu bowling behavior");
                    bool disabled = CVarGetInteger("gCustomizeBombchuBowling", 0) == 0;
                    static const char* disabledTooltip = "This option is disabled because \"Customize Behavior\" is turned off";
                    UIWidgets::PaddedEnhancementCheckbox("Remove Small Cucco", "gBombchuBowlingNoSmallCucco", true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("Prevents the small cucco from appearing in the bombchu bowling minigame");
                    UIWidgets::PaddedEnhancementCheckbox("Remove Big Cucco", "gBombchuBowlingNoBigCucco", true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("Prevents the big cucco from appearing in the bombchu bowling minigame");
                    UIWidgets::PaddedEnhancementSliderInt("Bombchu Count: %d", "##cBombchuBowlingAmmunition", "gBombchuBowlingAmmunition", 3, 20, "", 10, true, true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("The number of bombchus available at the start of the bombchu bowling minigame");
                    ImGui::EndMenu();
                }

                UIWidgets::Spacer(0);

                if (ImGui::BeginMenu("Fishing")) {
                    UIWidgets::EnhancementCheckbox("Customize Behavior", "gCustomizeFishing");
                    UIWidgets::Tooltip("Turn on/off changes to the fishing behavior");
                    bool disabled = !CVarGetInteger("gCustomizeFishing", 0);
                    static const char* disabledTooltip = "This option is disabled because \"Customize Behavior\" is turned off";
                    UIWidgets::PaddedEnhancementCheckbox("Instant Fishing", "gInstantFishing", true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("All fish will be caught instantly");
                    UIWidgets::PaddedEnhancementCheckbox("Guarantee Bite", "gGuaranteeFishingBite", true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("When a line is stable, guarantee bite. Otherwise use default logic");
                    UIWidgets::PaddedEnhancementCheckbox("Fish Never Escape", "gFishNeverEscape", true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("Once a hook has been set, fish will never let go while being reeled in.");
                    UIWidgets::PaddedEnhancementSliderInt("Child Minimum Weight: %d", "##cMinimumWeight", "gChildMinimumWeightFish", 3, 10, "", 10, true, true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("The minimum weight for the unique fishing reward as a child");
                    UIWidgets::PaddedEnhancementSliderInt("Adult Minimum Weight: %d", "##aMinimumWeight", "gAdultMinimumWeightFish", 6, 13, "", 13, true, true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("The minimum weight for the unique fishing reward as an adult");
                    ImGui::EndMenu();
                }
                UIWidgets::Spacer(0);

                if (ImGui::BeginMenu("Lost Woods Ocarina Game")) {
                    UIWidgets::EnhancementCheckbox("Customize Behavior", "gCustomizeOcarinaGame");
                    UIWidgets::Tooltip("Turn on/off changes to the lost woods ocarina game behavior");
                    bool disabled = !CVarGetInteger("gCustomizeOcarinaGame", 0);
                    static const char* disabledTooltip = "This option is disabled because \"Customize Behavior\" is turned off";
                    UIWidgets::PaddedEnhancementCheckbox("Instant Win", "gInstantOcarinaGameWin", true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("Skips the lost woods ocarina game");
                    UIWidgets::PaddedEnhancementSliderInt("Note Play Speed: %dx", "##OcarinaGameNoteSpeed", "gOcarinaGameNoteSpeed", 1, 5, "", 1, true, true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("Adjust the speed that the skull kids play notes");
                    UIWidgets::PaddedEnhancementCheckbox("Unlimited Playback Time", "gOcarinaUnlimitedFailTime", true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("Removes the timer to play back the song");
                    UIWidgets::PaddedEnhancementSliderInt("Number of Starting Notes: %d", "##OcarinaGameStartingNotes", "gOcarinaGameStartingNotes", 1, 8, "", 3, true, true, false,
                                                          disabled, disabledTooltip);
                    UIWidgets::Tooltip("Adjust the number of notes the skull kids play to start the first round");
                    int roundMin = CVarGetInteger("gOcarinaGameStartingNotes", 3);
                    UIWidgets::PaddedEnhancementSliderInt("Round One Notes: %d", "##OcarinaGameRoundOne",
                                                          "gOcarinaGameRoundOneNotes", roundMin, 8, "", 5, true, true,
                                                          false,
                                                          disabled, disabledTooltip);
                    UIWidgets::Tooltip("Adjust the number of notes you need to play to end the first round");
                    UIWidgets::PaddedEnhancementSliderInt("Round Two Notes: %d", "##OcarinaGameRoundTwoNotes",
                                                          "gOcarinaGameRoundTwoNotes", roundMin, 8, "", 6, true, true,
                                                          false,
                                                          disabled, disabledTooltip);
                    UIWidgets::Tooltip("Adjust the number of notes you need to play to end the second round");
                    UIWidgets::PaddedEnhancementSliderInt("Round Three Notes: %d", "##OcarinaGameRoundThreeNotes",
                                                          "gOcarinaGameRoundThreeNotes", roundMin, 8, "", 8, true, true,
                                                          false,
                                                          disabled, disabledTooltip);
                    UIWidgets::Tooltip("Adjust the number of notes you need to play to end the third round");
                    ImGui::EndMenu();
                }

                UIWidgets::Spacer(0);

                UIWidgets::PaddedEnhancementCheckbox("Delete File On Death", "gDeleteFileOnDeath", true, false);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                UIWidgets::Tooltip("Dying will delete your file\n\n     " ICON_FA_EXCLAMATION_TRIANGLE " WARNING " ICON_FA_EXCLAMATION_TRIANGLE "\nTHIS IS NOT REVERSABLE\nUSE AT YOUR OWN RISK!");
                ImGui::PopStyleColor();
                if (UIWidgets::PaddedEnhancementCheckbox("Permanent heart loss", "gPermanentHeartLoss", true, false)) {
                    UpdatePermanentHeartLossState();
                }
                UIWidgets::Tooltip("When you lose 4 quarters of a heart you will permanently lose that heart container.\n\nDisabling this after the fact will restore your heart containers.");
                ImGui::Text("Damage Multiplier");
                UIWidgets::EnhancementCombobox("gDamageMul", allPowers, 0);
                UIWidgets::Tooltip(
                    "Modifies all sources of damage not affected by other sliders\n"
                    "2x: Can survive all common attacks from the start of the game\n"
                    "4x: Dies in 1 hit to any substantial attack from the start of the game\n"
                    "8x: Can only survive trivial damage from the start of the game\n"
                    "16x: Can survive all common attacks with max health without double defense\n"
                    "32x: Can survive all common attacks with max health and double defense\n"
                    "64x: Can survive trivial damage with max health without double defense\n"
                    "128x: Can survive trivial damage with max health and double defense\n"
                    "256x: Cannot survive damage"
                );
                UIWidgets::PaddedText("Fall Damage Multiplier", true, false);
                UIWidgets::EnhancementCombobox("gFallDamageMul", subPowers, 0);
                UIWidgets::Tooltip(
                    "Modifies all fall damage\n"
                    "2x: Can survive all fall damage from the start of the game\n"
                    "4x: Can only survive short fall damage from the start of the game\n"
                    "8x: Cannot survive any fall damage from the start of the game\n"
                    "16x: Can survive all fall damage with max health without double defense\n"
                    "32x: Can survive all fall damage with max health and double defense\n"
                    "64x: Can survive short fall damage with double defense\n"
                    "128x: Cannot survive fall damage"
                );
                UIWidgets::PaddedText("Void Damage Multiplier", true, false);
                UIWidgets::EnhancementCombobox("gVoidDamageMul", subSubPowers, 0);
                UIWidgets::Tooltip(
                    "Modifies damage taken after falling into a void\n"
                    "2x: Can survive void damage from the start of the game\n"
                    "4x: Cannot survive void damage from the start of the game\n"
                    "8x: Can survive void damage twice with max health without double defense\n"
                    "16x: Can survive void damage with max health without double defense\n"
                    "32x: Can survive void damage with max health and double defense\n"
                    "64x: Cannot survive void damage"
                );
                UIWidgets::PaddedText("Bonk Damage Multiplier", true, false);
                UIWidgets::EnhancementCombobox("gBonkDamageMul", bonkDamageValues, BONK_DAMAGE_NONE);
                UIWidgets::Tooltip("Modifies damage taken after bonking.");
                UIWidgets::PaddedEnhancementCheckbox("Spawn with full health", "gFullHealthSpawn", true, false);
                UIWidgets::Tooltip("Respawn with full health instead of 3 Hearts");
                UIWidgets::PaddedEnhancementCheckbox("No Random Drops", "gNoRandomDrops", true, false);
                UIWidgets::Tooltip("Disables random drops, except from the Goron Pot, Dampe, and bosses");
                bool forceEnableBombchuDrops = IS_RANDO &&
                    OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_ENABLE_BOMBCHU_DROPS) == 1;
                static const char* forceEnableBombchuDropsText =
                    "This setting is forcefully enabled because a savefile\nwith \"Enable Bombchu Drops\" is loaded.";
                UIWidgets::PaddedEnhancementCheckbox("Enable Bombchu Drops", "gBombchuDrops", true, false,
                                                        forceEnableBombchuDrops, forceEnableBombchuDropsText, UIWidgets::CheckboxGraphics::Checkmark);
                UIWidgets::Tooltip("Bombchus will sometimes drop in place of bombs");
                UIWidgets::PaddedEnhancementCheckbox("Trees Drop Sticks", "gTreeStickDrops", true, false);
                UIWidgets::Tooltip("Bonking into trees will have a chance to drop up to 3 sticks. Must already have obtained sticks.");
                UIWidgets::PaddedEnhancementCheckbox("No Heart Drops", "gNoHeartDrops", true, false);
                UIWidgets::Tooltip("Disables heart drops, but not heart placements, like from a Deku Scrub running off\nThis simulates Hero Mode from other games in the series");
                if (UIWidgets::PaddedEnhancementCheckbox("Hyper Bosses", "gHyperBosses", true, false)) {
                    UpdateHyperBossesState();
                }
                UIWidgets::Tooltip("All major bosses move and act twice as fast.");
                if (UIWidgets::PaddedEnhancementCheckbox("Hyper Enemies", "gHyperEnemies", true, false)) {
                    UpdateHyperEnemiesState();
                }
                UIWidgets::Tooltip("All regular enemies and mini-bosses move and act twice as fast.");
                UIWidgets::PaddedEnhancementCheckbox("Always Win Goron Pot", "gGoronPot", true, false);
                UIWidgets::Tooltip("Always get the heart piece/purple rupee from the spinning Goron pot");
                UIWidgets::PaddedEnhancementCheckbox("Always Win Dampe Digging Game", "gDampeWin", true, false, SaveManager::Instance->IsRandoFile(),
                                                        "This setting is always enabled in randomizer files", UIWidgets::CheckboxGraphics::Checkmark);
                UIWidgets::Tooltip("Always win the heart piece/purple rupee on the first dig in Dampe's grave digging game, just like in rando\nIn a rando file, this is unconditionally enabled");
                UIWidgets::PaddedEnhancementCheckbox("All Dogs are Richard", "gAllDogsRichard", true, false);
                UIWidgets::Tooltip("All dogs can be traded in and will count as Richard.");
                UIWidgets::PaddedEnhancementSliderInt("Cuccos Stay Put Multiplier: %dx", "##CuccoStayDurationMultiplier", "gCuccoStayDurationMultiplier", 1, 5, "", 1, true, true, false);
                UIWidgets::Tooltip("Cuccos will stay in place longer after putting them down, by a multiple of the value of the slider.");
                UIWidgets::PaddedEnhancementSliderInt("Leever Spawn Rate: %d seconds", "##LeeverSpawnRate", "gEnhancements.LeeverSpawnRate", 0, 10, "", 0, true, true, false);
                UIWidgets::Tooltip("The time between leever groups spawning.");

                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            if (ImGui::BeginMenu("Reduced Clutter"))
            {
                UIWidgets::EnhancementCheckbox("Mute Low HP Alarm", "gLowHpAlarm");
                UIWidgets::Tooltip("Disable the low HP beeping sound");
                UIWidgets::PaddedEnhancementCheckbox("Minimal UI", "gMinimalUI", true, false);
                UIWidgets::Tooltip("Hides most of the UI when not needed\nNote: Doesn't activate until after loading a new scene");
                UIWidgets::PaddedEnhancementCheckbox("Disable Navi Call Audio", "gDisableNaviCallAudio", true, false);
                UIWidgets::Tooltip("Disables the voice audio when Navi calls you");
                UIWidgets::PaddedEnhancementCheckbox("Disable Hot/Underwater Warning Text", "gDisableTunicWarningText", true, false);
                UIWidgets::Tooltip("Disables warning text when you don't have on the Goron/Zora Tunic in Hot/Underwater conditions.");
                UIWidgets::PaddedEnhancementCheckbox("Remember Minimap State Between Areas", "gEnhancements.RememberMapToggleState");
                UIWidgets::Tooltip("Preserves the minimap visibility state when going between areas rather than defaulting it to \"on\" when going through loading zones.");

                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            UIWidgets::EnhancementCheckbox("Visual Stone of Agony", "gVisualAgony");
            UIWidgets::Tooltip("Displays an icon and plays a sound when Stone of Agony should be activated, for those without rumble");
            UIWidgets::PaddedEnhancementCheckbox("Assignable Tunics and Boots", "gAssignableTunicsAndBoots", true, false);
            UIWidgets::Tooltip("Allows equipping the tunic and boots to c-buttons");
            UIWidgets::PaddedEnhancementCheckbox("Equipment Toggle", "gEquipmentCanBeRemoved", true, false);
            UIWidgets::Tooltip("Allows equipment to be removed by toggling it off on\nthe equipment subscreen.");
            if (CVarGetInteger("gEquipmentCanBeRemoved", 0)) {
                UIWidgets::PaddedText("Sword Toggle Options", true, false);
                UIWidgets::EnhancementCombobox("gEnhancements.SwordToggle", swordToggleModes, SWORD_TOGGLE_NONE);
                UIWidgets::Tooltip(
                    "Introduces Options for unequipping Link's sword\n\n"
                    "None: Only Biggoron's Sword/Giant's Knife can be toggled. Doing so will equip the Master Sword.\n\n"
                    "Child Toggle: This will allow for completely unequipping any sword as child link.\n\n"
                    "Both Ages: Any sword can be unequipped as either age. This may lead to swordless glitches as Adult.\n"
                );
            }

            UIWidgets::PaddedEnhancementCheckbox("Link's Cow in Both Time Periods", "gCowOfTime", true, false);
            UIWidgets::Tooltip("Allows the Lon Lon Ranch obstacle course reward to be shared across time periods");
            UIWidgets::PaddedEnhancementCheckbox("Enable visible guard vision", "gGuardVision", true, false);
            UIWidgets::PaddedEnhancementCheckbox("Enable passage of time on file select", "gTimeFlowFileSelect", true, false);
            UIWidgets::PaddedEnhancementCheckbox("Item counts in messages", "gInjectItemCounts", true, false);
            UIWidgets::Tooltip("Injects item counts in pickup messages, like golden skulltula tokens and heart pieces");
            UIWidgets::PaddedEnhancementCheckbox("Pull grave during the day", "gDayGravePull", true, false);
            UIWidgets::Tooltip("Allows graves to be pulled when child during the day");
            UIWidgets::PaddedEnhancementCheckbox("Dogs follow you everywhere", "gDogFollowsEverywhere", true, false);
            UIWidgets::Tooltip("Allows dogs to follow you anywhere you go, even if you leave the market");
            UIWidgets::PaddedEnhancementCheckbox("Don't require input for Credits sequence", "gNoInputForCredits", true, false);
            UIWidgets::Tooltip("Removes the input requirement on textboxes after defeating Ganon, allowing Credits sequence to continue to progress");

            // Blue Fire Arrows
            bool forceEnableBlueFireArrows = IS_RANDO &&
                OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_BLUE_FIRE_ARROWS);
            static const char* forceEnableBlueFireArrowsText =
                "This setting is forcefully enabled because a savefile\nwith \"Blue Fire Arrows\" is loaded.";
            UIWidgets::PaddedEnhancementCheckbox("Blue Fire Arrows", "gBlueFireArrows", true, false, 
                forceEnableBlueFireArrows, forceEnableBlueFireArrowsText, UIWidgets::CheckboxGraphics::Checkmark);
            UIWidgets::Tooltip("Allows Ice Arrows to melt red ice.\nMay require a room reload if toggled during gameplay.");

            // Sunlight Arrows
            bool forceEnableSunLightArrows = IS_RANDO &&
                OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SUNLIGHT_ARROWS);
            static const char* forceEnableSunLightArrowsText =
                "This setting is forcefully enabled because a savefile\nwith \"Sunlight Arrows\" is loaded.";
            UIWidgets::PaddedEnhancementCheckbox("Sunlight Arrows", "gSunlightArrows", true, false, 
                forceEnableSunLightArrows, forceEnableSunLightArrowsText, UIWidgets::CheckboxGraphics::Checkmark);
            UIWidgets::Tooltip("Allows Light Arrows to activate sun switches.\nMay require a room reload if toggled during gameplay.");

            UIWidgets::PaddedEnhancementCheckbox("Disable Crit wiggle", "gDisableCritWiggle", true, false);
            UIWidgets::Tooltip("Disable random camera wiggle at low health");
            UIWidgets::PaddedEnhancementCheckbox("Enemy Health Bars", "gEnemyHealthBar", true, false);
            UIWidgets::Tooltip("Renders a health bar for enemies when Z-Targeted");

            UIWidgets::PaddedEnhancementCheckbox("Targetable Hookshot Reticle", "gHookshotableReticle", true, false);
            UIWidgets::Tooltip("Use a different color when aiming at hookshotable collision");

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Graphics"))
        {
            if (ImGui::BeginMenu("Mods")) {
                if (UIWidgets::PaddedEnhancementCheckbox("Use Alternate Assets", "gAltAssets", false, false)) {
                    // The checkbox will flip the alt asset CVar, but we instead want it to change at the end of the game frame
                    // We toggle it back while setting the flag to update the CVar later
                    CVarSetInteger("gAltAssets", !CVarGetInteger("gAltAssets", 0));
                    ToggleAltAssetsAtEndOfFrame = true;
                }
                UIWidgets::Tooltip("Toggle between standard assets and alternate assets. Usually mods will indicate if this setting has to be used or not.");
                UIWidgets::PaddedEnhancementCheckbox("Disable Bomb Billboarding", "gDisableBombBillboarding", true, false);
                UIWidgets::Tooltip("Disables bombs always rotating to face the camera. To be used in conjunction with mods that want to replace bombs with 3D objects.");
                UIWidgets::PaddedEnhancementCheckbox("Disable Grotto Fixed Rotation", "gDisableGrottoRotation", true, false);
                UIWidgets::Tooltip("Disables grottos rotating with the camera. To be used in conjunction with mods that want to replace grottos with 3D objects.");
                UIWidgets::PaddedEnhancementCheckbox("Invisible Bunny Hood", "gHideBunnyHood", true, false);
                UIWidgets::Tooltip("Turns Bunny Hood invisible while still maintaining its effects.");
                UIWidgets::PaddedEnhancementCheckbox("Disable HUD Heart animations", "gNoHUDHeartAnimation", true, false);
                UIWidgets::Tooltip("Disables the beating animation of the hearts on the HUD.");

                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            if (ImGui::BeginMenu("Animated Link in Pause Menu")) {
                ImGui::Text("Rotation");
                UIWidgets::EnhancementRadioButton("Disabled", "gPauseLiveLinkRotation", 0);
                UIWidgets::EnhancementRadioButton("Rotate Link with D-pad", "gPauseLiveLinkRotation", 1);
                UIWidgets::Tooltip("Allow you to rotate Link on the Equipment menu with the D-pad\nUse D-pad Up or D-pad Down to reset Link's rotation");
                UIWidgets::EnhancementRadioButton("Rotate Link with C-buttons", "gPauseLiveLinkRotation", 2);
                UIWidgets::Tooltip("Allow you to rotate Link on the Equipment menu with the C-buttons\nUse C-Up or C-Down to reset Link's rotation");
                UIWidgets::EnhancementRadioButton("Rotate Link with Right Stick", "gPauseLiveLinkRotation", 3);
                UIWidgets::Tooltip("Allow you to rotate Link on the Equipment menu with the Right Stick\nYou can zoom in by pointing up and reset Link's rotation by pointing down");
                if (CVarGetInteger("gPauseLiveLinkRotation", 0) != 0) {
                    UIWidgets::EnhancementSliderInt("Rotation Speed: %d", "##MinRotationSpeed", "gPauseLiveLinkRotationSpeed", 1, 20, "", 1);
                }
                UIWidgets::PaddedSeparator();
                ImGui::Text("Static loop");
                UIWidgets::EnhancementRadioButton("Disabled", "gPauseLiveLink", 0);
                UIWidgets::EnhancementRadioButton("Idle (standing)", "gPauseLiveLink", 1);
                UIWidgets::EnhancementRadioButton("Idle (look around)", "gPauseLiveLink", 2);
                UIWidgets::EnhancementRadioButton("Idle (belt)", "gPauseLiveLink", 3);
                UIWidgets::EnhancementRadioButton("Idle (shield)", "gPauseLiveLink", 4);
                UIWidgets::EnhancementRadioButton("Idle (test sword)", "gPauseLiveLink", 5);
                UIWidgets::EnhancementRadioButton("Idle (yawn)", "gPauseLiveLink", 6);
                UIWidgets::EnhancementRadioButton("Battle Stance", "gPauseLiveLink", 7);
                UIWidgets::EnhancementRadioButton("Walking (no shield)", "gPauseLiveLink", 8);
                UIWidgets::EnhancementRadioButton("Walking (holding shield)", "gPauseLiveLink", 9);
                UIWidgets::EnhancementRadioButton("Running (no shield)", "gPauseLiveLink", 10);
                UIWidgets::EnhancementRadioButton("Running (holding shield)", "gPauseLiveLink", 11);
                UIWidgets::EnhancementRadioButton("Hand on hip", "gPauseLiveLink", 12);
                UIWidgets::EnhancementRadioButton("Spin attack charge", "gPauseLiveLink", 13);
                UIWidgets::EnhancementRadioButton("Look at hand", "gPauseLiveLink", 14);
                UIWidgets::PaddedSeparator();
                ImGui::Text("Randomize");
                UIWidgets::EnhancementRadioButton("Random", "gPauseLiveLink", 15);
                UIWidgets::Tooltip("Randomize the animation played each time you open the menu");
                UIWidgets::EnhancementRadioButton("Random cycle", "gPauseLiveLink", 16);
                UIWidgets::Tooltip("Randomize the animation played on the menu after a certain time");
                UIWidgets::EnhancementRadioButton("Random cycle (Idle)", "gPauseLiveLink", 17);
                UIWidgets::Tooltip("Randomize the animation played on the menu after a certain time (Idle animations only)");
                if (CVarGetInteger("gPauseLiveLink", 0) >= 16) {
                    UIWidgets::EnhancementSliderInt("Frame to wait: %d", "##MinFrameCount", "gMinFrameCount", 1, 1000, "", 0);
                }

                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            UIWidgets::PaddedEnhancementCheckbox("Disable LOD", "gDisableLOD", true, false);
            UIWidgets::Tooltip("Turns off the Level of Detail setting, making models use their higher-poly variants at any distance");
            if (UIWidgets::PaddedEnhancementCheckbox("Disable Draw Distance", "gDisableDrawDistance", true, false)) {
                if (CVarGetInteger("gDisableDrawDistance", 0) == 0) {
                    CVarSetInteger("gDisableKokiriDrawDistance", 0);
                }
            }
            UIWidgets::Tooltip("Turns off the objects draw distance, making objects being visible from a longer range");
            if (CVarGetInteger("gDisableDrawDistance", 0) == 1) {
                UIWidgets::PaddedEnhancementCheckbox("Kokiri Draw Distance", "gDisableKokiriDrawDistance", true, false);
                UIWidgets::Tooltip("The Kokiri are mystical beings that fade into view when approached\nEnabling this will remove their draw distance");
            }
            if (UIWidgets::PaddedEnhancementCheckbox("Show Age-Dependent Equipment", "gEnhancements.EquimentAlwaysVisible", true,
                                                     false)) {
                UpdatePatchHand();
            }
            UIWidgets::Tooltip("Makes all equipment visible, regardless of Age.");
            if (CVarGetInteger("gEnhancements.EquimentAlwaysVisible", 0) == 1) {
				UIWidgets::PaddedEnhancementCheckbox("Scale Adult Equipment as Child", "gEnhancements.ScaleAdultEquimentAsChild", true, false);
				UIWidgets::Tooltip("Scales all of the Adult Equipment, as well and moving some a bit, to fit on Child Link Better. May not work properly with some mods.");
			}
            UIWidgets::PaddedEnhancementCheckbox("N64 Mode", "gLowResMode", true, false);
            UIWidgets::Tooltip("Sets aspect ratio to 4:3 and lowers resolution to 240p, the N64's native resolution");
            UIWidgets::PaddedEnhancementCheckbox("Glitch line-up tick", "gDrawLineupTick", true, false);
            UIWidgets::Tooltip("Displays a tick in the top center of the screen to help with glitch line-ups in SoH, as traditional UI based line-ups do not work outside of 4:3");
            UIWidgets::PaddedEnhancementCheckbox("Enable 3D Dropped items/projectiles", "gNewDrops", true, false);
            UIWidgets::Tooltip("Change most 2D items and projectiles on the overworld to their 3D versions");
            UIWidgets::PaddedEnhancementCheckbox("Disable Black Bar Letterboxes", "gDisableBlackBars", true, false);
            UIWidgets::Tooltip("Disables Black Bar Letterboxes during cutscenes and Z-targeting\nNote: there may be minor visual glitches that were covered up by the black bars\nPlease disable this setting before reporting a bug");
            UIWidgets::PaddedEnhancementCheckbox("Dynamic Wallet Icon", "gDynamicWalletIcon", true, false);
            UIWidgets::Tooltip("Changes the rupee in the wallet icon to match the wallet size you currently have");
            UIWidgets::PaddedEnhancementCheckbox("Always show dungeon entrances", "gAlwaysShowDungeonMinimapIcon", true, false);
            UIWidgets::Tooltip("Always shows dungeon entrance icons on the minimap");
            UIWidgets::PaddedEnhancementCheckbox("Show Gauntlets in First Person", "gFPSGauntlets", true, false);
            UIWidgets::Tooltip("Renders Gauntlets when using the Bow and Hookshot like in OOT3D");
            if (UIWidgets::PaddedEnhancementCheckbox("Color Temple of Time's Medallions", "gToTMedallionsColors", true, false)) {
                PatchToTMedallions();
            }
            UIWidgets::Tooltip("When medallions are collected, the medallion imprints around the Master Sword pedestal in the Temple of Time will become colored");
            UIWidgets::PaddedEnhancementCheckbox("Show locked door chains on both sides of locked doors", "gShowDoorLocksOnBothSides", true, false);
            UIWidgets::PaddedText("Fix Vanishing Paths", true, false);
            if (UIWidgets::EnhancementCombobox("gSceneSpecificDirtPathFix", zFightingOptions, ZFIGHT_FIX_DISABLED) && gPlayState != NULL) {
                UpdateDirtPathFixState(gPlayState->sceneNum);
            }
            UIWidgets::Tooltip("Disabled: Paths vanish more the higher the resolution (Z-fighting is based on resolution)\n"
                                "Consistent: Certain paths vanish the same way in all resolutions\n"
                                "No Vanish: Paths do not vanish, Link seems to sink in to some paths\n"
                                "This might affect other decal effects\n");
            UIWidgets::PaddedEnhancementSliderInt("Text Spacing: %d", "##TEXTSPACING", "gTextSpacing", 4, 6, "", 6, true, true, true);
            UIWidgets::Tooltip("Space between text characters (useful for HD font textures)");
            UIWidgets::PaddedEnhancementCheckbox("More info in file select", "gFileSelectMoreInfo", true, false);
            UIWidgets::Tooltip("Shows what items you have collected in the file select screen, like in N64 randomizer");
            UIWidgets::PaddedEnhancementCheckbox("Better ammo rendering in pause menu", "gEnhancements.BetterAmmoRendering", true, false);
            UIWidgets::Tooltip("Ammo counts in the pause menu will work correctly regardless of the position of items in the inventory");
            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Fixes"))
        {
            UIWidgets::EnhancementCheckbox("Fix L&R Pause menu", "gUniformLR");
            UIWidgets::Tooltip("Makes the L and R buttons in the pause menu the same color");
            UIWidgets::PaddedEnhancementCheckbox("Fix L&Z Page switch in Pause menu", "gNGCKaleidoSwitcher", true, false);
            UIWidgets::Tooltip("Makes L and R switch pages like on the GameCube\nZ opens the Debug Menu instead");
            UIWidgets::PaddedEnhancementCheckbox("Fix Dungeon entrances", "gFixDungeonMinimapIcon", true, false);
            UIWidgets::Tooltip("Removes the dungeon entrance icon on the top-left corner of the screen when no dungeon is present on the current map");
            UIWidgets::PaddedEnhancementCheckbox("Fix Two Handed idle animations", "gTwoHandedIdle", true, false);
            UIWidgets::Tooltip("Re-enables the two-handed idle animation, a seemingly finished animation that was disabled on accident in the original game");
            UIWidgets::PaddedEnhancementCheckbox("Fix the Gravedigging Tour Glitch", "gGravediggingTourFix", true, false);
            UIWidgets::Tooltip("Fixes a bug where the Gravedigging Tour Heart Piece disappears if the area reloads");
            UIWidgets::PaddedEnhancementCheckbox("Fix Deku Nut upgrade", "gDekuNutUpgradeFix", true, false);
            UIWidgets::Tooltip("Prevents the Forest Stage Deku Nut upgrade from becoming unobtainable after receiving the Poacher's Saw");
            UIWidgets::PaddedEnhancementCheckbox("Fix Navi text HUD position", "gNaviTextFix", true, false);
            UIWidgets::Tooltip("Correctly centers the Navi text prompt on the HUD's C-Up button");
            UIWidgets::PaddedEnhancementCheckbox("Fix Anubis fireballs", "gAnubisFix", true, false);
            UIWidgets::Tooltip("Make Anubis fireballs do fire damage when reflected back at them with the Mirror Shield");
            if (UIWidgets::PaddedEnhancementCheckbox("Fix Megaton Hammer crouch stab", "gCrouchStabHammerFix", true, false)) {
                if (!CVarGetInteger("gCrouchStabHammerFix", 0)) {
                    CVarClear("gCrouchStabFix");
                }
            }
            UIWidgets::Tooltip("Make the Megaton Hammer's crouch stab able to destroy rocks without first swinging it normally");
            if (CVarGetInteger("gCrouchStabHammerFix", 0)) {
                UIWidgets::PaddedEnhancementCheckbox("Remove power crouch stab", "gCrouchStabFix", true, false);
                UIWidgets::Tooltip("Make crouch stabbing always do the same damage as a regular slash");
            }
            UIWidgets::PaddedEnhancementCheckbox("Fix credits timing", "gCreditsFix", true, false);
            UIWidgets::Tooltip("Extend certain credits scenes so the music lines up properly with the visuals");
            UIWidgets::PaddedEnhancementCheckbox("Fix Gerudo Warrior's clothing colors", "gGerudoWarriorClothingFix", true, false);
            UIWidgets::Tooltip("Prevent the Gerudo Warrior's clothes changing color when changing Link's tunic or using bombs in front of her");
            UIWidgets::PaddedEnhancementCheckbox("Fix Camera Drift", "gFixCameraDrift", true, false);
            UIWidgets::Tooltip("Fixes camera slightly drifting to the left when standing still due to a math error");
            UIWidgets::PaddedEnhancementCheckbox("Fix Camera Swing", "gFixCameraSwing", true, false);
            UIWidgets::Tooltip("Fixes camera getting stuck on collision when standing still, also fixes slight shift back in camera when stop moving");
            UIWidgets::PaddedEnhancementCheckbox("Fix Hanging Ledge Swing Rate", "gFixHangingLedgeSwingRate", true, false);
            UIWidgets::Tooltip("Fixes camera swing rate when player falls off a ledge and camera swings around");
            UIWidgets::PaddedEnhancementCheckbox("Fix Missing Jingle after 5 Silver Rupees", "gSilverRupeeJingleExtend", true, false);
            UIWidgets::Tooltip(
                "Adds 5 higher pitches for the Silver Rupee Jingle for the rooms with more than 5 Silver Rupees. "
                "Currently only relevant in Master Quest.");
            if (UIWidgets::PaddedEnhancementCheckbox("Fix out of bounds textures", "gFixTexturesOOB", true, false)) {
                ApplyAuthenticGfxPatches();
            }
            UIWidgets::Tooltip("Fixes authentic out of bounds texture reads, instead loading textures with the correct size");
            UIWidgets::PaddedEnhancementCheckbox("Fix Poacher's Saw Softlock", "gFixSawSoftlock", true, false, CVarGetInteger("gSkipText", 0),
                "This is disabled because it is forced on when Skip Text is enabled.", UIWidgets::CheckboxGraphics::Checkmark);
            UIWidgets::Tooltip("Prevents the Poacher's Saw softlock from mashing through the text, or with Skip Text enabled.");
            UIWidgets::PaddedEnhancementCheckbox("Fix enemies not spawning near water", "gEnemySpawnsOverWaterboxes", true, false);
            UIWidgets::Tooltip("Causes respawning enemies, like stalchildren, to appear on land near bodies of water. "
                                "Fixes an incorrect calculation that acted like water underneath ground was above it.");
            UIWidgets::PaddedEnhancementCheckbox("Fix Bush Item Drops", "gBushDropFix", true, false);
            UIWidgets::Tooltip("Fixes the bushes to drop items correctly rather than spawning undefined items.");
            UIWidgets::PaddedEnhancementCheckbox("Fix falling from vine edges", "gFixVineFall", true, false); 
            UIWidgets::Tooltip("Prevents immediately falling off climbable surfaces if climbing on the edges."); 
            UIWidgets::PaddedEnhancementCheckbox("Fix Link's eyes open while sleeping", "gFixEyesOpenWhileSleeping", true, false);
            UIWidgets::Tooltip("Fixes Link's eyes being open in the opening cutscene when he is supposed to be sleeping.");
            UIWidgets::PaddedEnhancementCheckbox("Fix Darunia dancing too fast", "gEnhancements.FixDaruniaDanceSpeed",
                                                 true, false, false, "", UIWidgets::CheckboxGraphics::Cross, true);
            UIWidgets::Tooltip("Fixes Darunia's dancing speed so he dances to the beat of Saria's Song, like in vanilla.");
            UIWidgets::PaddedEnhancementCheckbox("Fix raised Floor Switches", "gEnhancements.FixFloorSwitches", true, false);
            UIWidgets::Tooltip("Fixes the two raised floor switches, the one in Forest Temple Basement and the one at the top of Fire Temple. \n"
                "This will lower them, making activating them easier");
            UIWidgets::PaddedEnhancementCheckbox("Fix Zora hint dialogue", "gFixZoraHintDialogue", true, false);
            UIWidgets::Tooltip("Fixes one Zora's dialogue giving a hint about bringing Ruto's Letter to King Zora to properly occur before moving King Zora rather than after");

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Restoration"))
        {
            UIWidgets::EnhancementCheckbox("Authentic Logo Screen", "gAuthenticLogo");
            UIWidgets::Tooltip("Hide the game version and build details and display the authentic model and texture on the boot logo start screen");
            UIWidgets::PaddedEnhancementCheckbox("Red Ganon blood", "gRedGanonBlood", true, false);
            UIWidgets::Tooltip("Restore the original red blood from NTSC 1.0/1.1. Disable for green blood");
            UIWidgets::PaddedEnhancementCheckbox("Fish while hovering", "gHoverFishing", true, false);
            UIWidgets::Tooltip("Restore a bug from NTSC 1.0 that allows casting the Fishing Rod while using the Hover Boots");
            UIWidgets::PaddedEnhancementCheckbox("N64 Weird Frames", "gN64WeirdFrames", true, false);
            UIWidgets::Tooltip("Restores N64 Weird Frames allowing weirdshots to behave the same as N64");
            UIWidgets::PaddedEnhancementCheckbox("Bombchus out of bounds", "gBombchusOOB", true, false);
            UIWidgets::Tooltip("Allows bombchus to explode out of bounds\nSimilar to GameCube and Wii VC");
            UIWidgets::PaddedEnhancementCheckbox("Quick Putaway", "gQuickPutaway", true, false);
            UIWidgets::Tooltip("Restore a bug from NTSC 1.0 that allows putting away an item without an animation and performing Putaway Ocarina Items");
            UIWidgets::PaddedEnhancementCheckbox("Restore old Gold Skulltula cutscene", "gGsCutscene", true, false);
            UIWidgets::Tooltip("Restore pre-release behavior where defeating a Gold Skulltula will play a cutscene showing it die.");
            UIWidgets::PaddedEnhancementCheckbox("Quick Bongo Kill", "gQuickBongoKill", true, false);
            UIWidgets::Tooltip("Restore a bug from NTSC 1.0 that allows bypassing Bongo Bongo's intro cutscene to quickly kill him");
            UIWidgets::PaddedEnhancementCheckbox("Original RBA Values", "gRestoreRBAValues", true, false);
            UIWidgets::Tooltip("Restores the original outcomes when performing Reverse Bottle Adventure.");

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Extra Modes")) {
            UIWidgets::PaddedText("Mirrored World", true, false);
            if (UIWidgets::EnhancementCombobox("gMirroredWorldMode", mirroredWorldModes, MIRRORED_WORLD_OFF) && gPlayState != NULL) {
                UpdateMirrorModeState(gPlayState->sceneNum);
            }
            UIWidgets::Tooltip(
                "Mirrors the world horizontally\n\n"
                "- Always: Always mirror the world\n"
                "- Random: Randomly decide to mirror the world on each scene change\n"
                "- Random (Seeded): Scenes are mirrored based on the current randomizer seed/file\n"
                "- Dungeons: Mirror the world in Dungeons\n"
                "- Dungeons (Vanilla): Mirror the world in vanilla Dungeons\n"
                "- Dungeons (MQ): Mirror the world in MQ Dungeons\n"
                "- Dungeons Random: Randomly decide to mirror the world in Dungeons\n"
                "- Dungeons Random (Seeded): Dungeons are mirrored based on the current randomizer seed/file\n"
            );

            UIWidgets::PaddedText("Enemy Randomizer", true, false);
            UIWidgets::EnhancementCombobox("gRandomizedEnemies", enemyRandomizerModes, ENEMY_RANDOMIZER_OFF);
            UIWidgets::Tooltip(
                "Replaces fixed enemies throughout the game with a random enemy. Bosses, mini-bosses and a few specific regular enemies are excluded.\n"
                "Enemies that need more than Deku Nuts + either Deku Sticks or a sword to kill are excluded from spawning in \"clear enemy\" rooms.\n\n"
                "- Random: Enemies are randomized every time you load a room\n"
                "- Random (Seeded): Enemies are randomized based on the current randomizer seed/file\n"
            );

            UIWidgets::PaddedEnhancementCheckbox("Randomized Enemy Sizes", "gRandomizedEnemySizes", true, false);
            UIWidgets::Tooltip("Enemies and Bosses spawn with random sizes.");

            if (CVarGetInteger("gRandomizedEnemySizes", 0)) {
                UIWidgets::EnhancementCheckbox("Scale Health with Size", "gEnemySizeScalesHealth");
                UIWidgets::Tooltip("Scales normal enemies health with their randomized size. *This will NOT affect bosses*");
            }

            UIWidgets::PaddedEnhancementCheckbox("Ivan the Fairy (Coop Mode)", "gIvanCoopModeEnabled", true, false);
            UIWidgets::Tooltip("Enables Ivan the Fairy upon the next map change. Player 2 can control Ivan and "
                                "press the C-Buttons to use items and mess with Player 1!");

            UIWidgets::PaddedEnhancementCheckbox("Rupee Dash Mode", "gRupeeDash", true, false);
            UIWidgets::Tooltip("Rupees reduced over time, Link suffers damage when the count hits 0.");

            if (CVarGetInteger("gRupeeDash", 0)) {
                UIWidgets::PaddedEnhancementSliderInt(
                    "Rupee Dash Interval: %d", "##DashInterval", "gDashInterval", 1, 10, "", 5, true, true, false,
                    !CVarGetInteger("gRupeeDash", 0),
                    "This option is disabled because \"Rupee Dash Mode\" is turned off");
                UIWidgets::Tooltip("Interval between Rupee reduction in Rupee Dash Mode");
            }

            UIWidgets::PaddedEnhancementCheckbox("Shadow Tag Mode", "gShadowTag", true, false);
            UIWidgets::Tooltip("A wallmaster follows Link everywhere, don't get caught!");

            UIWidgets::PaddedEnhancementCheckbox("Additional Traps", "gAddTraps.enabled", true, false);
            UIWidgets::Tooltip("Enables additional Trap variants.");

            if (CVarGetInteger("gAddTraps.enabled", 0)) {
                UIWidgets::PaddedSeparator();
                if (ImGui::BeginMenu("Trap Options")) {
                    ImGui::Text("Tier 1 Traps:");
                    UIWidgets::Spacer(0);
                    UIWidgets::PaddedEnhancementCheckbox("Freeze Traps", "gAddTraps.Ice", true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Burn Traps", "gAddTraps.Burn", true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Shock Traps", "gAddTraps.Shock", true, false);

                    UIWidgets::PaddedSeparator();
                    ImGui::Text("Tier 2 Traps:");
                    UIWidgets::Spacer(0);
                    UIWidgets::PaddedEnhancementCheckbox("Knockback Traps", "gAddTraps.Knock", true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Speed Traps", "gAddTraps.Speed", true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Bomb Traps", "gAddTraps.Bomb", true, false);

                    UIWidgets::PaddedSeparator();
                    ImGui::Text("Tier 3 Traps:");
                    UIWidgets::Spacer(0);
                    UIWidgets::PaddedEnhancementCheckbox("Void Traps", "gAddTraps.Void", true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Ammo Traps", "gAddTraps.Ammo", true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Death Traps", "gAddTraps.Kill", true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Teleport Traps", "gAddTraps.Tele", true, false);

                    ImGui::EndMenu();
                }
            }

            UIWidgets::Spacer(0);
            if (UIWidgets::PaddedEnhancementCheckbox("Hurt Container Mode", "gHurtContainer", true, false)) {
                UpdateHurtContainerModeState(CVarGetInteger("gHurtContainer", 0));
            }
            UIWidgets::Tooltip("Changes Heart Piece and Heart Container functionality.\n\n"
                "- Each Heart Container or full Heart Piece reduces Links hearts by 1.\n"
                "- Can be enabled retroactively after a File has already started.");

            ImGui::EndMenu();
        }

        UIWidgets::PaddedSeparator(false, true);

        // Autosave enum value of 1 is the default in presets and the old checkbox "on" state for backwards compatibility
        UIWidgets::PaddedText("Autosave", false, true);
        UIWidgets::EnhancementCombobox("gAutosave", autosaveLabels, AUTOSAVE_OFF);
        UIWidgets::Tooltip("Automatically save the game when changing locations and/or obtaining items\n"
            "Major items exclude rupees and health/magic/ammo refills (but include bombchus unless bombchu drops are enabled)");

        UIWidgets::PaddedSeparator(true, true, 2.0f, 2.0f);

        ImGui::EndDisabled();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));

        if (mCosmeticsEditorWindow) {
            if (ImGui::Button(GetWindowButtonText("Cosmetics Editor", CVarGetInteger("gCosmeticsEditorEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mCosmeticsEditorWindow->ToggleVisibility();
            }
        }

        if (mAudioEditorWindow) {
            if (ImGui::Button(GetWindowButtonText("Audio Editor", CVarGetInteger("gAudioEditor.WindowOpen", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mAudioEditorWindow->ToggleVisibility();
            }
        }

        if (mGameplayStatsWindow) {
            if (ImGui::Button(GetWindowButtonText("Gameplay Stats", CVarGetInteger("gGameplayStatsEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mGameplayStatsWindow->ToggleVisibility();
            }
        }
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(1);

        #ifdef __SWITCH__
        UIWidgets::Spacer(0);
        ImGui::Text("Switch performance mode");
        if (UIWidgets::EnhancementCombobox("gSwitchPerfMode", SWITCH_CPU_PROFILES, (int)LUS::SwitchProfiles::STOCK)) {
            SPDLOG_INFO("Profile:: %s", SWITCH_CPU_PROFILES[CVarGetInteger("gSwitchPerfMode", (int)LUS::SwitchProfiles::STOCK)]);
            LUS::Switch::ApplyOverclock();
        }
        #endif

        ImGui::EndMenu();
    }
}

void DrawCheatsMenu() {
    if (ImGui::BeginMenu("Cheats"))
    {
        ImGui::BeginDisabled(CVarGetInteger("gDisableChangingSettings", 0));
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8.0f);
        ImGui::BeginTable("##cheatsMenu", 2, ImGuiTableFlags_SizingFixedFit);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableNextColumn();
        UIWidgets::Spacer(2.0f);
        ImGui::Text("Inventory:");
        UIWidgets::PaddedSeparator();

        UIWidgets::PaddedEnhancementCheckbox("Super Tunic", "gSuperTunic", true, false);
        UIWidgets::Tooltip("Makes every tunic have the effects of every other tunic");
        UIWidgets::PaddedEnhancementCheckbox("Easy ISG", "gEzISG", true, false);
        UIWidgets::Tooltip("Passive Infinite Sword Glitch\nIt makes your sword's swing effect and hitbox stay active indefinitely");
        UIWidgets::PaddedEnhancementCheckbox("Easy QPA", "gEzQPA", true, false);
        UIWidgets::Tooltip("Gives you the glitched damage value of the quick put away glitch.");
        UIWidgets::PaddedEnhancementCheckbox("Timeless Equipment", "gTimelessEquipment", true, false);
        UIWidgets::Tooltip("Allows any item to be equipped, regardless of age\nAlso allows Child to use Adult strength upgrades");
        UIWidgets::PaddedEnhancementCheckbox("Unrestricted Items", "gNoRestrictItems", true, false);
        UIWidgets::Tooltip("Allows you to use any item at any location");
        UIWidgets::PaddedEnhancementCheckbox("Fireproof Deku Shield", "gFireproofDekuShield", true, false);
        UIWidgets::Tooltip("Prevents the Deku Shield from burning on contact with fire");
        UIWidgets::PaddedEnhancementCheckbox("Shield with Two-Handed Weapons", "gShieldTwoHanded", true, false);
        UIWidgets::Tooltip("This allows you to put up your shield with any two-handed weapon in hand except for Deku Sticks");
        UIWidgets::Spacer(2.0f);
        ImGui::Text("Deku Sticks:");
        UIWidgets::EnhancementCombobox("gDekuStickCheat", DekuStickCheat, DEKU_STICK_NORMAL);
        UIWidgets::Spacer(2.0f);
        UIWidgets::EnhancementSliderFloat("Bomb Timer Multiplier: %.2fx", "##gBombTimerMultiplier", "gBombTimerMultiplier", 0.1f, 5.0f, "", 1.0f, false);
        UIWidgets::PaddedEnhancementCheckbox("Hookshot Everything", "gHookshotEverything", true, false);
        UIWidgets::Tooltip("Makes every surface in the game hookshot-able");
        UIWidgets::Spacer(0);
        UIWidgets::EnhancementSliderFloat("Hookshot Reach Multiplier: %.2fx", "##gCheatHookshotReachMultiplier", "gCheatHookshotReachMultiplier", 1.0f, 5.0f, "", 1.0f, false);
        UIWidgets::Spacer(2.0f);
        if (ImGui::Button("Change Age")) {
            CVarSetInteger("gSwitchAge", 1);
        }
        UIWidgets::Tooltip("Switches Link's age and reloads the area.");  
        UIWidgets::Spacer(2.0f);
        if (ImGui::Button("Clear Cutscene Pointer")) {
            GameInteractor::RawAction::ClearCutscenePointer();
        }
        UIWidgets::Tooltip("Clears the cutscene pointer to a value safe for wrong warps.");   

        ImGui::TableNextColumn();
        UIWidgets::Spacer(2.0f);

        if (ImGui::BeginMenu("Infinite...")) {
            UIWidgets::EnhancementCheckbox("Money", "gInfiniteMoney");
            UIWidgets::PaddedEnhancementCheckbox("Health", "gInfiniteHealth", true, false);
            UIWidgets::PaddedEnhancementCheckbox("Ammo", "gInfiniteAmmo", true, false);
            UIWidgets::PaddedEnhancementCheckbox("Magic", "gInfiniteMagic", true, false);
            UIWidgets::PaddedEnhancementCheckbox("Nayru's Love", "gInfiniteNayru", true, false);
            UIWidgets::PaddedEnhancementCheckbox("Epona Boost", "gInfiniteEpona", true, false);

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Save States")) {
            ImGui::TextColored({ 0.85f, 0.85f, 0.0f, 1.0f }, "          " ICON_FA_EXCLAMATION_TRIANGLE);
            ImGui::SameLine();
            ImGui::TextColored({ 0.85f, 0.35f, 0.0f, 1.0f }, " WARNING!!!! ");
            ImGui::SameLine();
            ImGui::TextColored({ 0.85f, 0.85f, 0.0f, 1.0f }, ICON_FA_EXCLAMATION_TRIANGLE);
            UIWidgets::PaddedText("These are NOT like emulator states.", true, false);
            UIWidgets::PaddedText("They do not save your game progress, and", true, false);
            UIWidgets::PaddedText("they WILL break across transitions and", true, false);
            UIWidgets::PaddedText("load zones (like doors). Support for", true, false);
            UIWidgets::PaddedText("related issues will not be provided.", true, false);
            if (UIWidgets::PaddedEnhancementCheckbox("I promise I have read the warning", "gSaveStatePromise", true,
                                                     false)) {
                CVarSetInteger("gSaveStatesEnabled", 0);
                LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            }
            if (CVarGetInteger("gSaveStatePromise", 0) == 1) {
                UIWidgets::PaddedEnhancementCheckbox("I understand, enable save states", "gSaveStatesEnabled", true,
                                                     false);
                UIWidgets::Tooltip("F5 to save, F6 to change slots, F7 to load");
            }

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(2.0f);
        ImGui::Text("Behavior:");
        UIWidgets::PaddedSeparator();

        UIWidgets::PaddedEnhancementCheckbox("No Clip", "gNoClip", true, false);
        UIWidgets::Tooltip("Allows you to walk through walls");
        UIWidgets::PaddedEnhancementCheckbox("Climb Everything", "gClimbEverything", true, false);
        UIWidgets::Tooltip("Makes every surface in the game climbable");
        UIWidgets::PaddedEnhancementCheckbox("Moon Jump on L", "gMoonJumpOnL", true, false);
        UIWidgets::Tooltip("Holding L makes you float into the air");
        UIWidgets::PaddedEnhancementCheckbox("Easy Frame Advancing", "gCheatEasyPauseBufferEnabled", true, false);
        UIWidgets::Tooltip("Continue holding START button when unpausing to only advance a single frame and then re-pause");
        const bool bEasyFrameAdvanceEnabled = CVarGetInteger("gCheatEasyPauseBufferEnabled", 0);
        UIWidgets::PaddedEnhancementCheckbox("Easy Input Buffering", "gCheatEasyInputBufferingEnabled", true, false, bEasyFrameAdvanceEnabled, "Forced enabled when Easy Frame Advancing is enabled", UIWidgets::CheckboxGraphics::Checkmark);
        UIWidgets::Tooltip("Inputs that are held down while the Subscreen is closing will be pressed when the game is resumed");
        UIWidgets::PaddedEnhancementCheckbox("Drops Don't Despawn", "gDropsDontDie", true, false);
        UIWidgets::Tooltip("Drops from enemies, grass, etc. don't disappear after a set amount of time");
        UIWidgets::PaddedEnhancementCheckbox("Fish Don't despawn", "gNoFishDespawn", true, false);
        UIWidgets::Tooltip("Prevents fish from automatically despawning after a while when dropped");
        UIWidgets::PaddedEnhancementCheckbox("Bugs Don't despawn", "gNoBugsDespawn", true, false);
        UIWidgets::Tooltip("Prevents bugs from automatically despawning after a while when dropped");
        UIWidgets::PaddedEnhancementCheckbox("Freeze Time", "gFreezeTime", true, false);
        UIWidgets::Tooltip("Freezes the time of day");
        UIWidgets::PaddedEnhancementCheckbox("Time Sync", "gTimeSync", true, false);
        UIWidgets::Tooltip("This syncs the ingame time with the real world time");
        UIWidgets::PaddedEnhancementCheckbox("No ReDead/Gibdo Freeze", "gNoRedeadFreeze", true, false);
        UIWidgets::Tooltip("Prevents ReDeads and Gibdos from being able to freeze you with their scream");
        UIWidgets::PaddedEnhancementCheckbox("Keese/Guay don't target you", "gNoKeeseGuayTarget", true, false);
        UIWidgets::Tooltip("Keese and Guay no longer target you and simply ignore you as if you were wearing the skull mask");
        {
            static int32_t betaQuestEnabled = CVarGetInteger("gEnableBetaQuest", 0);
            static int32_t lastBetaQuestEnabled = betaQuestEnabled;
            static int32_t betaQuestWorld = CVarGetInteger("gBetaQuestWorld", 0xFFEF);
            static int32_t lastBetaQuestWorld = betaQuestWorld;

            if (!isBetaQuestEnabled) {
                UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
            }

            UIWidgets::PaddedEnhancementCheckbox("Enable Beta Quest", "gEnableBetaQuest", true, false);
            UIWidgets::Tooltip("Turns on OoT Beta Quest. *WARNING* This will reset your game.");
            betaQuestEnabled = CVarGetInteger("gEnableBetaQuest", 0);
            if (betaQuestEnabled) {
                if (betaQuestEnabled != lastBetaQuestEnabled) {
                    betaQuestWorld = 0;
                }

                ImGui::Text("Beta Quest World: %d", betaQuestWorld);

                if (ImGui::Button(" - ##BetaQuest")) {
                    betaQuestWorld--;
                }
                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);

                ImGui::SliderInt("##BetaQuest", &betaQuestWorld, 0, 8, "", ImGuiSliderFlags_AlwaysClamp);
                UIWidgets::Tooltip("Set the Beta Quest world to explore. *WARNING* Changing this will reset your game.\nCtrl+Click to type in a value.");

                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
                if (ImGui::Button(" + ##BetaQuest")) {
                    betaQuestWorld++;
                }

                if (betaQuestWorld > 8) {
                    betaQuestWorld = 8;
                }
                else if (betaQuestWorld < 0) {
                    betaQuestWorld = 0;
                }
            }
            else {
                lastBetaQuestWorld = betaQuestWorld = 0xFFEF;
                CVarClear("gBetaQuestWorld");
            }
            if (betaQuestEnabled != lastBetaQuestEnabled || betaQuestWorld != lastBetaQuestWorld)
            {
                // Reset the game if the beta quest state or world changed because beta quest happens on redirecting the title screen cutscene.
                lastBetaQuestEnabled = betaQuestEnabled;
                lastBetaQuestWorld = betaQuestWorld;
                CVarSetInteger("gEnableBetaQuest", betaQuestEnabled);
                CVarSetInteger("gBetaQuestWorld", betaQuestWorld);

                std::reinterpret_pointer_cast<LUS::ConsoleWindow>(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->Dispatch("reset");
                LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            }

            if (!isBetaQuestEnabled) {
                UIWidgets::ReEnableComponent("");
            }
        }

        ImGui::EndTable();
        ImGui::EndDisabled();
        ImGui::EndMenu();
    }
}

extern std::shared_ptr<LUS::GuiWindow> mStatsWindow;
extern std::shared_ptr<LUS::GuiWindow> mConsoleWindow;
extern std::shared_ptr<SaveEditorWindow> mSaveEditorWindow;
extern std::shared_ptr<ColViewerWindow> mColViewerWindow;
extern std::shared_ptr<ActorViewerWindow> mActorViewerWindow;
extern std::shared_ptr<DLViewerWindow> mDLViewerWindow;
extern std::shared_ptr<ValueViewerWindow> mValueViewerWindow;
extern std::shared_ptr<MessageViewer> mMessageViewerWindow;

void DrawDeveloperToolsMenu() {
    if (ImGui::BeginMenu("Developer Tools")) {
        ImGui::BeginDisabled(CVarGetInteger("gDisableChangingSettings", 0));

        UIWidgets::EnhancementCheckbox("OoT Debug Mode", "gDebugEnabled");
        UIWidgets::Tooltip("Enables Debug Mode, allowing you to select maps with L + R + Z, noclip with L + D-pad Right, and open the debug menu with L on the pause screen");
        if (CVarGetInteger("gDebugEnabled", 0)) {
            UIWidgets::EnhancementCheckbox("OoT Registry Editor", "gRegEditEnabled");
            UIWidgets::Tooltip("Enables the registry editor");
            ImGui::Text("Debug Save File Mode:");
            UIWidgets::EnhancementCombobox("gDebugSaveFileMode", DebugSaveFileModes, 1);
            UIWidgets::Tooltip(
                "Changes the behaviour of debug file select creation (creating a save file on slot 1 with debug mode on)\n"
                "- Off: The debug save file will be a normal savefile\n"
                "- Vanilla: The debug save file will be the debug save file from the original game\n"
                "- Maxed: The debug save file will be a save file with all of the items & upgrades"
            );
        }
        UIWidgets::PaddedEnhancementCheckbox("OoT Skulltula Debug", "gSkulltulaDebugEnabled", true, false);
        UIWidgets::Tooltip("Enables Skulltula Debug, when moving the cursor in the menu above various map icons (boss key, compass, map screen locations, etc) will set the GS bits in that area.\nUSE WITH CAUTION AS IT DOES NOT UPDATE THE GS COUNT.");
        UIWidgets::PaddedEnhancementCheckbox("Fast File Select", "gSkipLogoTitle", true, false);
        UIWidgets::Tooltip("Load the game to the selected menu or file\n\"Zelda Map Select\" require debug mode else you will fallback to File choose menu\nUsing a file number that don't have save will create a save file only if you toggle on \"Create a new save if none ?\" else it will bring you to the File choose menu");
        if (CVarGetInteger("gSkipLogoTitle", 0)) {
            ImGui::Text("Loading:");
            UIWidgets::EnhancementCombobox("gSaveFileID", FastFileSelect, 0);
        };
        UIWidgets::PaddedEnhancementCheckbox("Better Debug Warp Screen", "gBetterDebugWarpScreen", true, false);
        UIWidgets::Tooltip("Optimized debug warp screen, with the added ability to chose entrances and time of day");
        UIWidgets::PaddedEnhancementCheckbox("Debug Warp Screen Translation", "gDebugWarpScreenTranslation", true, false, false, "", UIWidgets::CheckboxGraphics::Cross, true);
        UIWidgets::Tooltip("Translate the Debug Warp Screen based on the game language");
        if (gPlayState != NULL) {
            UIWidgets::PaddedSeparator();
            ImGui::Checkbox("Frame Advance##frameAdvance", (bool*)&gPlayState->frameAdvCtx.enabled);
            if (gPlayState->frameAdvCtx.enabled) {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0,0));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
                if (ImGui::Button("Advance 1", ImVec2(ImGui::GetContentRegionAvail().x / 2.0f, 0.0f))) {
                    CVarSetInteger("gFrameAdvance", 1);
                }
                ImGui::SameLine();
                ImGui::Button("Advance (Hold)");
                if (ImGui::IsItemActive()) {
                    CVarSetInteger("gFrameAdvance", 1);
                }
                ImGui::PopStyleVar(3);
                ImGui::PopStyleColor(1);
            }
        }
        UIWidgets::PaddedSeparator();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0,0));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
        if (mStatsWindow) {
            if (ImGui::Button(GetWindowButtonText("Stats", CVarGetInteger("gStatsEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mStatsWindow->ToggleVisibility();
            }
            UIWidgets::Tooltip("Shows the stats window, with your FPS and frametimes, and the OS you're playing on");
        }
        UIWidgets::Spacer(0);
        if (mConsoleWindow) {
            if (ImGui::Button(GetWindowButtonText("Console", CVarGetInteger("gConsoleEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mConsoleWindow->ToggleVisibility();
            }
            UIWidgets::Tooltip("Enables the console window, allowing you to input commands, type help for some examples");
        }
        UIWidgets::Spacer(0);
        if (mSaveEditorWindow) {
            if (ImGui::Button(GetWindowButtonText("Save Editor", CVarGetInteger("gSaveEditorEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mSaveEditorWindow->ToggleVisibility();
            }
        }
        UIWidgets::Spacer(0);
        if (mColViewerWindow) {
            if (ImGui::Button(GetWindowButtonText("Collision Viewer", CVarGetInteger("gCollisionViewerEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mColViewerWindow->ToggleVisibility();
            }
        }
        UIWidgets::Spacer(0);
        if (mActorViewerWindow) {
            if (ImGui::Button(GetWindowButtonText("Actor Viewer", CVarGetInteger("gActorViewerEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mActorViewerWindow->ToggleVisibility();
            }
        }
        UIWidgets::Spacer(0);
        if (mDLViewerWindow) {
            if (ImGui::Button(GetWindowButtonText("Display List Viewer", CVarGetInteger("gDLViewerEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mDLViewerWindow->ToggleVisibility();
            }
        }
        UIWidgets::Spacer(0);
        if (mValueViewerWindow) {
            if (ImGui::Button(GetWindowButtonText("Value Viewer", CVarGetInteger("gValueViewer.WindowOpen", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mValueViewerWindow->ToggleVisibility();
            }
        }
        UIWidgets::Spacer(0);
        if (mMessageViewerWindow) {
            if (ImGui::Button(GetWindowButtonText("Message Viewer", CVarGetInteger("gMessageViewerEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mMessageViewerWindow->ToggleVisibility();
            }
        }

        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(1);

        ImGui::EndDisabled();

        ImGui::EndMenu();
    }
}

bool isStringEmpty(std::string str) {
    // Remove spaces at the beginning of the string
    std::string::size_type start = str.find_first_not_of(' ');
    // Remove spaces at the end of the string
    std::string::size_type end = str.find_last_not_of(' ');

    // Check if the string is empty after stripping spaces
    if (start == std::string::npos || end == std::string::npos)
        return true; // The string is empty
    else
        return false; // The string is not empty
}

#ifdef ENABLE_REMOTE_CONTROL
void DrawRemoteControlMenu() {
    if (ImGui::BeginMenu("Network")) {
        static std::string ip = CVarGetString("gRemote.IP", "127.0.0.1");
        static uint16_t port = CVarGetInteger("gRemote.Port", 43384);
        bool isFormValid = !isStringEmpty(CVarGetString("gRemote.IP", "127.0.0.1")) && port > 1024 && port < 65535;

        const char* remoteOptions[2] = { "Sail", "Crowd Control"};

        ImGui::BeginDisabled(GameInteractor::Instance->isRemoteInteractorEnabled);
        ImGui::Text("Remote Interaction Scheme");
        if (UIWidgets::EnhancementCombobox("gRemote.Scheme", remoteOptions, GI_SCHEME_SAIL)) {
            switch (CVarGetInteger("gRemote.Scheme", GI_SCHEME_SAIL)) {
                case GI_SCHEME_SAIL:
                case GI_SCHEME_CROWD_CONTROL:
                    CVarSetString("gRemote.IP", "127.0.0.1");
                    CVarSetInteger("gRemote.Port", 43384);
                    ip = "127.0.0.1";
                    port = 43384;
                    break;
            }
            LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
        }
        switch (CVarGetInteger("gRemote.Scheme", GI_SCHEME_SAIL)) {
            case GI_SCHEME_SAIL:
                UIWidgets::InsertHelpHoverText(
                    "Sail is a networking protocol designed to facilitate remote "
                    "control of the Ship of Harkinian client. It is intended to "
                    "be utilized alongside a Sail server, for which we provide a "
                    "few straightforward implementations on our GitHub. The current "
                    "implementations available allow integration with Twitch chat "
                    "and SAMMI Bot, feel free to contribute your own!\n"
                    "\n"
                    "Click the question mark to copy the link to the Sail Github "
                    "page to your clipboard."
                );
                if (ImGui::IsItemClicked()) {
                    ImGui::SetClipboardText("https://github.com/HarbourMasters/sail");
                }
                break;
            case GI_SCHEME_CROWD_CONTROL:
                UIWidgets::InsertHelpHoverText(
                    "Crowd Control is a platform that allows viewers to interact "
                    "with a streamer's game in real time.\n"
                    "\n"
                    "Click the question mark to copy the link to the Crowd Control "
                    "website to your clipboard."
                );
                if (ImGui::IsItemClicked()) {
                    ImGui::SetClipboardText("https://crowdcontrol.live");
                }
                break;
        }

        ImGui::Text("Remote IP & Port");
        if (ImGui::InputText("##gRemote.IP", (char*)ip.c_str(), ip.capacity() + 1)) {
            CVarSetString("gRemote.IP", ip.c_str());
            LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetFontSize() * 5);
        if (ImGui::InputScalar("##gRemote.Port", ImGuiDataType_U16, &port)) {
            CVarSetInteger("gRemote.Port", port);
            LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
        }

        ImGui::PopItemWidth();
        ImGui::EndDisabled();

        ImGui::Spacing();

        ImGui::BeginDisabled(!isFormValid);
        const char* buttonLabel = GameInteractor::Instance->isRemoteInteractorEnabled ? "Disable" : "Enable";
        if (ImGui::Button(buttonLabel, ImVec2(-1.0f, 0.0f))) {
            if (GameInteractor::Instance->isRemoteInteractorEnabled) {
                CVarSetInteger("gRemote.Enabled", 0);
                LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                switch (CVarGetInteger("gRemote.Scheme", GI_SCHEME_SAIL)) {
                    case GI_SCHEME_SAIL:
                        GameInteractorSail::Instance->Disable();
                        break;
                    case GI_SCHEME_CROWD_CONTROL:
                        CrowdControl::Instance->Disable();
                        break;
                }
            } else {
                CVarSetInteger("gRemote.Enabled", 1);
                LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                switch (CVarGetInteger("gRemote.Scheme", GI_SCHEME_SAIL)) {
                    case GI_SCHEME_SAIL:
                        GameInteractorSail::Instance->Enable();
                        break;
                    case GI_SCHEME_CROWD_CONTROL:
                        CrowdControl::Instance->Enable();
                        break;
                }
            }
        }
        ImGui::EndDisabled();

        if (GameInteractor::Instance->isRemoteInteractorEnabled) {
            ImGui::Spacing();
            if (GameInteractor::Instance->isRemoteInteractorConnected) {
                ImGui::Text("Connected");
            } else {
                ImGui::Text("Connecting...");
            }
        }

        ImGui::Dummy(ImVec2(0.0f, 0.0f));
        ImGui::EndMenu();
    }
}
#endif

extern std::shared_ptr<RandomizerSettingsWindow> mRandomizerSettingsWindow;
extern std::shared_ptr<ItemTrackerWindow> mItemTrackerWindow;
extern std::shared_ptr<ItemTrackerSettingsWindow> mItemTrackerSettingsWindow;
extern std::shared_ptr<EntranceTrackerWindow> mEntranceTrackerWindow;
extern std::shared_ptr<CheckTracker::CheckTrackerWindow> mCheckTrackerWindow;
extern std::shared_ptr<CheckTracker::CheckTrackerSettingsWindow> mCheckTrackerSettingsWindow;

void DrawRandomizerMenu() {
    if (ImGui::BeginMenu("Randomizer")) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
    #ifdef __WIIU__
        static ImVec2 buttonSize(200.0f * 2.0f, 0.0f);
    #else
        static ImVec2 buttonSize(200.0f, 0.0f);
    #endif
        if (mRandomizerSettingsWindow) {
            if (ImGui::Button(GetWindowButtonText("Randomizer Settings", CVarGetInteger("gRandomizerSettingsEnabled", 0)).c_str(), buttonSize)) {
                mRandomizerSettingsWindow->ToggleVisibility();
            }
        }

        UIWidgets::Spacer(0);
        if (mItemTrackerWindow) {
            if (ImGui::Button(GetWindowButtonText("Item Tracker", CVarGetInteger("gItemTrackerEnabled", 0)).c_str(), buttonSize)) {
                mItemTrackerWindow->ToggleVisibility();
            }
        }

        UIWidgets::Spacer(0);
        if (mItemTrackerSettingsWindow) {
            if (ImGui::Button(GetWindowButtonText("Item Tracker Settings", CVarGetInteger("gItemTrackerSettingsEnabled", 0)).c_str(), buttonSize)) {
                mItemTrackerSettingsWindow->ToggleVisibility();
            }
        }
        UIWidgets::Spacer(0);
        if (mEntranceTrackerWindow) {
            if (ImGui::Button(GetWindowButtonText("Entrance Tracker", CVarGetInteger("gEntranceTrackerEnabled", 0)).c_str(), buttonSize)) {
                mEntranceTrackerWindow->ToggleVisibility();
            }
        }
        UIWidgets::Spacer(0);
        if (mCheckTrackerWindow) {
            if (ImGui::Button(GetWindowButtonText("Check Tracker", CVarGetInteger("gCheckTrackerEnabled", 0)).c_str(), buttonSize)) {
                mCheckTrackerWindow->ToggleVisibility();
            }
        }
        UIWidgets::Spacer(0);
        if (mCheckTrackerSettingsWindow) {
            if (ImGui::Button(GetWindowButtonText("Check Tracker Settings", CVarGetInteger("gCheckTrackerSettingsEnabled", 0)).c_str(), buttonSize)) {
                mCheckTrackerSettingsWindow->ToggleVisibility();
            }
        }
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(1);

        UIWidgets::PaddedSeparator();

        if (ImGui::BeginMenu("Rando Enhancements"))
        {
            UIWidgets::EnhancementCheckbox("Rando-Relevant Navi Hints", "gRandoRelevantNavi", false, "", UIWidgets::CheckboxGraphics::Cross, true);
            UIWidgets::Tooltip(
                "Replace Navi's overworld quest hints with rando-related gameplay hints."
            );
            UIWidgets::PaddedEnhancementCheckbox("Random Rupee Names", "gRandomizeRupeeNames", true, false, false, "", UIWidgets::CheckboxGraphics::Cross, true);
            UIWidgets::Tooltip(
                "When obtaining rupees, randomize what the rupee is called in the textbox."
            );

            // Only disable the key colors checkbox when none of the keysanity settings are set to "Any Dungeon", "Overworld" or "Anywhere"
            bool disableKeyColors = true;

            if (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_ANY_DUNGEON ||
                OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_OVERWORLD ||
                OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_ANYWHERE ||
                OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GERUDO_KEYS) != RO_GERUDO_KEYS_VANILLA ||
                OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_BOSS_KEYSANITY) == RO_DUNGEON_ITEM_LOC_ANY_DUNGEON ||
                OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_BOSS_KEYSANITY) == RO_DUNGEON_ITEM_LOC_OVERWORLD ||
                OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_BOSS_KEYSANITY) == RO_DUNGEON_ITEM_LOC_ANYWHERE ||
                (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GANONS_BOSS_KEY) != RO_GANON_BOSS_KEY_VANILLA &&
                    OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GANONS_BOSS_KEY) != RO_GANON_BOSS_KEY_OWN_DUNGEON &&
                    OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GANONS_BOSS_KEY) != RO_GANON_BOSS_KEY_STARTWITH) || 
                !IS_RANDO) {
                disableKeyColors = false;
            }

            static const char* disableKeyColorsText = 
                "This setting is disabled because a savefile is loaded without any key\n"
                "shuffle settings set to \"Any Dungeon\", \"Overworld\" or \"Anywhere\"";

            UIWidgets::PaddedEnhancementCheckbox("Key Colors Match Dungeon", "gRandoMatchKeyColors", true, false,
                                                  disableKeyColors, disableKeyColorsText, UIWidgets::CheckboxGraphics::Cross, true);
            UIWidgets::Tooltip(
                "Matches the color of small keys and boss keys to the dungeon they belong to. "
                "This helps identify keys from afar and adds a little bit of flair.\n\nThis only "
                "applies to seeds with keys and boss keys shuffled to Any Dungeon, Overworld, or Anywhere.");
            UIWidgets::PaddedEnhancementCheckbox("Quest Item Fanfares", "gRandoQuestItemFanfares", true, false);
            UIWidgets::Tooltip(
                "Play unique fanfares when obtaining quest items "
                "(medallions/stones/songs). Note that these fanfares are longer than usual."
            );
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
}

void SohMenuBar::DrawElement() {
    if (ImGui::BeginMenuBar()) {
        DrawMenuBarIcon();

        static ImVec2 sWindowPadding(8.0f, 8.0f);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, sWindowPadding);

        DrawShipMenu();

        ImGui::SetCursorPosY(0.0f);

        DrawSettingsMenu();

        ImGui::SetCursorPosY(0.0f);

        DrawEnhancementsMenu();

        ImGui::SetCursorPosY(0.0f);

        DrawCheatsMenu();

        ImGui::SetCursorPosY(0.0f);

        DrawDeveloperToolsMenu();

        ImGui::SetCursorPosY(0.0f);

        #ifdef ENABLE_REMOTE_CONTROL
        DrawRemoteControlMenu();

        ImGui::SetCursorPosY(0.0f);
        #endif

        DrawRandomizerMenu();

        ImGui::PopStyleVar(1);
        ImGui::EndMenuBar();
    }
}
} // namespace SohGui
