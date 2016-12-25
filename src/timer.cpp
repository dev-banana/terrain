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
* \brief Initialisation des différents timer utilisés
*/
void Timer::init()
{
	is_journee = JOURNEE ;

	glGenQueries(1, &gpu) ;
	speed = DAY_SPEED ;
	ellapse = 0 ;
	
	day = 0 ;
	hour = 0 ;
	minutes = 0 ;
	secondes = 0 ;
	timeOfDay = 0 ;
}

/**
* \brief libère les objets de la classe
*/
void Timer::release()
{
	glDeleteQueries(1, &gpu);
}