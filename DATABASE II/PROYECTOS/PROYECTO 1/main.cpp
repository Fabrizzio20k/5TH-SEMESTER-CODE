#include "include/gui.h"


int main()
{
    cout << "Hello, World!" << endl;
    wxApp::SetInstance(new MyApp());
    wxEntryStart(0, nullptr);
    wxTheApp->OnInit();
    wxTheApp->OnRun();
    wxEntryCleanup();
    return 0;
}