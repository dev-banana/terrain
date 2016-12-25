/**
* \file timer.cpp
* \brief Fichier d'implémentation de la classe Timer
* \date 22/12/2016
* \author Anna Benneton
* \author Anna-Katharina Bergmann
*/
//------------------------------
#include "timer.hpp"
//------------------------------


 

/**
* \brief Initialisation des différents timers utilisés
*/
void Timer::init()
{
	is_journee = JOURNEE ;
	speed = DAY_SPEED ;

	timeBefore = 0 ;
	timeNext = 1000*60*60*START_HOUR ;
	newDay = 0 ;
	day = 0 ;
	hour = 0 ;
	minutes = 0 ;
	secondes = 0 ;
	millisecondes = 0 ;

	glGenQueries(1, &gpu) ;
}

/**
* \brief libère les objets de la classe
*/
void Timer::release()
{
	glDeleteQueries(1, &gpu);
}


/**
* \brief enregistre le temps en début de fonction
*
*	Met à jour la date et l'heure si le mode Journée est activé
*
*/
void Timer::timerBegin()
{
	glBeginQuery( GL_TIME_ELAPSED, gpu ) ;
	cpu_start = std::chrono::high_resolution_clock::now() ;

	
	int diff = SDL_GetTicks() - timeBefore ;
    if( diff > 1 )
    {	
    	timeBefore = SDL_GetTicks() ;
    	if( is_journee )
		{
	    	timeNext += diff * speed ;
	    }
	}

    millisecondes = (int)( timeNext )%( 1000*60*60*24 ) ;
    secondes = millisecondes/1000 ;
    minutes = secondes/60 ;
    hour = minutes/60 ;

    if( hour >= 23 )
    	newDay = 1 ;
	if( hour == 0 && newDay == 1 )
	{
		day++ ;
		newDay = 0 ;
	}

}


/**
* \brief calcul le temps en fin de fonction
*/
void Timer::timerEnd()
{

	cpu_stop = std::chrono::high_resolution_clock::now() ;
	cpu_time = std::chrono::duration_cast<std::chrono::nanoseconds>( cpu_stop - cpu_start ).count() ;
	
	glEndQuery( GL_TIME_ELAPSED ) ;
	
	wait_start = std::chrono::high_resolution_clock::now() ;
	
	gpu_time = 0 ;
	glGetQueryObjecti64v( gpu, GL_QUERY_RESULT, &gpu_time ) ;
	
	wait_stop = std::chrono::high_resolution_clock::now() ;
	wait_time = std::chrono::duration_cast<std::chrono::nanoseconds>( wait_stop - wait_start ).count() ;

	
	milli_gpu = (int) ( gpu_time / 1000000 ) ;
	micro_gpu = (int) ( (gpu_time / 1000) % 1000 );

	milli_cpu = (int) ( cpu_time / 1000000 ) ;
	micro_cpu = (int) ( (cpu_time / 1000) % 1000 );

	fps = (int) 1/( (cpu_time+gpu_time+wait_time) / 1000000000.0 ) ;
}