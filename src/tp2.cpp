/**
* \file tp2.cpp
* \brief Fichier Main, initialise la fenêtre et lance la scène
* \date 22/12/2016
* \author Anna Benneton
* \author Anna-Katharina Bergmann
*/
//------------------------------
#include "scene.hpp"
//------------------------------


Scene scene ;   //!< La scène principale

/**
* \brief Boucle d'affichage principale
* \return 1 si tout c'est bien passé
*/
int draw()
{
    return scene.draw() ;
}


/**
* \brief Fonction Principale
* \param[in] argc : le nombre d'arguments
* \param[in] argv : les arguments
* \return 0 si tout c'est bien passé
*/
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