#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "remote.hpp"

#define OVERRIDEPOSTPROCESSINGDISABLE_SIGNATURE "\x80\x3D\x00\x00\x00\x00\x00\x0F\x85\x00\x00\x00\x00\x85\xC9"
#define OVERRIDEPOSTPROCESSINGDISABLE_MASK "xx????xxx????xx"

using namespace std;

remote::Handle csgo;
remote::MapModuleMemoryRegion client;

int main(int argc, char* argv[]) {
    if (getuid() != 0) {
        cout << "You should run this as root." << endl;
        return 0;
    }

    cout << "Waiting for csgo.";
    while (true) {
        if (remote::FindProcessByName("csgo_linux64", &csgo)) {
            break;
        }
        cout << ".";
        usleep(1000000);
    }

    cout << endl << "CSGO Process Located [" << csgo.GetPath() << "][" << csgo.GetPid() << "]" << endl << endl;

    client.start = 0;

    while (client.start == 0) {
        if (!csgo.IsRunning()) {
            cout << "Exited game before client could be located, terminating" << endl;
            return 0;
        }

        csgo.ParseMaps();

        for (auto region : csgo.regions) {
            if (region.filename.compare("client_client.so") == 0 && region.executable) {
                cout << "client_client.so: [" << std::hex << region.start << "][" << std::hex << region.end << "][" <<
                region.pathname << "]" << endl;
                client = region;
                csgo.addressOfClientModule = region.start;
                break;
            }
        }

        usleep(500);
    }


    unsigned long foundOverridePostProcessingDisableInstr = (long) client.find(csgo, OVERRIDEPOSTPROCESSINGDISABLE_SIGNATURE, OVERRIDEPOSTPROCESSINGDISABLE_MASK);
    cout << ">>> raw s_bOverridePostProcessingDisable instr. pointer: 0x" << std::hex << foundOverridePostProcessingDisableInstr << endl << endl;

    unsigned long addressOfOverridePostProcessingDisablePointer = csgo.GetAbsoluteAddress((void*)(foundOverridePostProcessingDisableInstr), 2, 7);
    cout << ">>> Address of s_bOverridePostProcessingDisable: 0x" << std::hex << addressOfOverridePostProcessingDisablePointer << endl << endl;

    bool disable = true;

    while (csgo.IsRunning()) {
      	csgo.Write((void*) (addressOfOverridePostProcessingDisablePointer), &disable, sizeof(bool));
        usleep(1000000);
    }


    cout << "Game ended." << endl;


    return 0;
}
