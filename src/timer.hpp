/**
* \file timer.hpp
* \brief Fichier de déclaration de la classe Timer
* \date 22/12/2016
* \author Anna Benneton
* \author Anna-Katharina Bergmann
*/
//------------------------------
#ifndef TIMER_HPP
#define TIMER_HPP
//------------------------------
//------------------------------
//libs
#include <chrono>
//gkit
#include "glcore.h"
//tp2
#include "config.hpp"
//------------------------------


/**
* \class Timer
* \brief classe qui permet de gérer le temps écoulé
*
*       Gestion du temps CPU/GPU et des journées écoulées
*
*/
class Timer
{
    protected:
	public:

		int ellapse ;   //!< Temps écoulé depuis le début

		/*==============================
	                GPU
	    ==============================*/
		GLuint gpu ;   //!< Le temps pour le GPU

		/*==============================
	                CPU
	    ==============================*/
		std::chrono::high_resolution_clock::time_point cpu_start ;      //!< La temps de début du CPU

		/*==============================
	                DAYS
	    ==============================*/
		float speed ;    //!< Vitesse des journées

		bool is_journee ;    //!< activation du temps qui passe
		
		int day ;	//!< Jour de l'année
		float hour ;    //!< Heure de la journée en heure
		float minutes ;    //!< Heure de la journée en minutes
		float secondes ;    //!< Heure de la journée en secondes
		int timeOfDay ;    //!< Heure de la journée en ms


        /**
        * \brief Constructeur de la classe Timer
        */
        Timer(){}
        /**
        * \brief Destructeur de la classe Timer
        */
        ~Timer(){}

        void init() ;
        void release() ;
} ;

#endif //TIMER_HPP