#include "GameData.h"
#include "LeagueMemReader.h"
#include "Offsets.h"
#include "Overlay.h"
#include "PyStructs.h"
#include "Utils.h"
#include "imgui.h"
#include "windows.h"
#include <chrono>
#include <conio.h>
#include <iostream>
#include <list>
#include <map>
#include <thread>

using namespace std::this_thread;
using namespace std::chrono;

void MainLoop(LeagueMemReader& reader);

int main()
{
    LeagueMemReader reader = LeagueMemReader();

    try {
        printf("[+] Initializing PyModule\n");
        PyImport_AppendInittab("Vippro", &PyInit_Vippro);
        Py_Initialize();

        printf("[+] Initializing Unit Data\n");
        std::string dataPath("data");
        GameData::Load(dataPath);
        LeagueRenderer::InitRenderer();

        MainLoop(reader);

        Py_Finalize();
    } catch (std::runtime_error exception) {
        std::cout << exception.what() << std::endl;
    }

    printf("Press any key to exit...");
    return _getch();
}

void OverlayThread(bool& firstIter, bool& isLeagueWindowActive, std::shared_ptr<MemSnapShot>& memSnapshot, std::shared_ptr<MemSnapShot>& ms)
{
    Overlay overlay = Overlay();

    try {
        printf("[+] Initializing DirectX UI\n");
        overlay.Init();
    } catch (std::runtime_error exception) {
        std::cout << exception.what() << std::endl;
    }

    while (true) {
        if (overlay.IsVisible()) {
            // On some systems the ingame cursor is replaced with the default Windows cursor
            // With the WS_EX_TRANSPARENT window flag enabled the cursor is as expected but the user cannot control the overlay
            if (Input::WasKeyPressed(HKey::F8)) {
                overlay.ToggleTransparent();
            }
            if (!isLeagueWindowActive) {
                overlay.Hide();
            }
        } else if (isLeagueWindowActive) {
            overlay.Show();
        }

        try {
            if (memSnapshot->player.get() == nullptr) {
                continue;
            }
            ms = std::shared_ptr<MemSnapShot>(memSnapshot);
            if (ms->player.get() == nullptr) {
                continue;
            }

            if (overlay.IsVisible()) {
                overlay.StartFrame();
            }
            // If the game started
            if (ms->gameTime > 2.f) {
                // Tell the UI that a new game has started
                if (firstIter) {
                    overlay.GameStart(*ms);
                    firstIter = false;
                }

                // Update UI
                overlay.Update(*ms);
            }
            if (overlay.IsVisible()) {
                overlay.RenderFrame();
            }
        } catch (std::runtime_error exception) {
            printf("[!] Unexpected error occured: \n [!] %s \n", exception.what());
            break;
        }

        Sleep(5);
    }
}

void MainLoop(LeagueMemReader& reader)
{
    std::shared_ptr<MemSnapShot> memSnapshot = std::shared_ptr<MemSnapShot>(new MemSnapShot());
    std::shared_ptr<MemSnapShot> ms(memSnapshot);

    bool rehook = true;
    bool firstIter = true;
    bool isLeagueWindowActive = false;

    printf("[i] Waiting for League process...\n");
    std::thread overlayWorker([&]() { OverlayThread(firstIter, isLeagueWindowActive, memSnapshot, ms); });

    while (true) {

        isLeagueWindowActive = reader.IsLeagueWindowActive();

        try {
            // Try to find the league process and get its information necessary for reading
            if (rehook) {
                reader.HookToProcess();
                rehook = false;
                firstIter = true;
                printf("[i] Found League process. The UI will appear when the game start.\n");
            } else {

                if (!reader.IsHookedToProcess()) {
                    rehook = true;
                    printf("[i] League process is dead.\n");
                    printf("[i] Waiting for League process...\n");
                }

                // Read data from memory
                memSnapshot = std::shared_ptr<MemSnapShot>(new MemSnapShot());
                reader.MakeSnapShot(*memSnapshot);
            }
        } catch (WinApiException exception) {
            // This should trigger only when we don't find the league process.
            rehook = true;
        } catch (std::runtime_error exception) {
            printf("[!] Unexpected error occured: \n [!] %s \n", exception.what());
            break;
        }

        Sleep(5);
    }
}