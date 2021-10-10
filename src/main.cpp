#include <iostream>
#include <Renderer.h>


int main() {

    Renderer r;

    try
    {
        r.Run();
    } catch(std::exception(&e))
    {
        std::cout << e.what() << '\n';
    }

    return 0;
}
