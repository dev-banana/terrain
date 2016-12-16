#include "scene.hpp"

Scene scene ;


int draw()
{
    return scene.draw() ;
}


int main( int argc, char **argv )
{
    Window window= create_window(1024, 640);
    if(window == NULL) return 1;
    Context context= create_context(window);
    if(context == NULL) return 1;


    if( scene.init() == - 1 )
    {
        printf("[error] init failed.\n");
        return 1;
    }
    run(window, draw);
    
    scene.quit();

    release_context(context);
    release_window(window);
    return 0;
}