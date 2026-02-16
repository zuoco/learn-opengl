#include "main.h"
#include "application.h"

int main(int argc, char *argv[])
{
    auto mainWindow = MainWindow::create();
    
    mainWindow->on_close_requested([&mainWindow]() {
        Application::getInstance().quit();
    });
    
    mainWindow->run();
    
    return 0;
}



