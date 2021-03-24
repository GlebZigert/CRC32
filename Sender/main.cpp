#include "mainwindow.h"

#include <QApplication>
#include <QMutex>
#include <QSystemSemaphore>
#include <QMessageBox>

#if (defined (_WIN32) || defined (_WIN64))
#include <Windows.h>
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

        HANDLE hMutex_Loader =  CreateMutexA(NULL, false, "Loader for RLM C");
         if (GetLastError() == ERROR_ALREADY_EXISTS)
            {
               CloseHandle(hMutex_Loader);
               QMessageBox::critical(0,"Ошибка","Утилита уже запущена");

               return FALSE;
            }





    MainWindow w;
    w.show();
    return a.exec();
}
