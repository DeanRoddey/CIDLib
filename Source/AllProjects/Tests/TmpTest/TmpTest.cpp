#include <windows.h>

int main(int argc, char** argv)
{
    try
    {
       ::Beep(880, 400);
    }

    catch(...)
    {
    }
    return 0;
}


