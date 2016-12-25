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
#include <SDL2/SDL.h>
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

		/*==============================
	                DAYS
	    ==============================*/
		float speed ;    //!< Vitesse des journées
		bool is_journee ;    //!< activation du temps qui passe

		long long int timeBefore ;	//!< mémoriser le temps précédent
		long long int timeNext ;	//!< calculer le temps écoulé
		
		int day ;	//!< Jour de l'année
		int newDay ;	//!< Pour savoir si il faut changer de jour
		int hour ;    //!< Heure de la journée en heure
		int minutes ;    //!< Heure de la journée en minutes
		int secondes ;    //!< Heure de la journée en secondes
		long long int millisecondes ;    //!< Heure de la journée en ms



		/*==============================
	                GPU
	    ==============================*/
		int milli_gpu ; //!< performances GPU (millisecondes)
		int micro_gpu ;	//!< performances GPU (microsecondes)

		GLuint gpu ;   //!< Le temps pour les requetes GPU
		GLint64 gpu_time ;	//!< Pour lire le temps GPU

		/*==============================
	                CPU
	    ==============================*/
		int milli_cpu ;	//!< performances CPU (millisecondes)
		int micro_cpu ;	//!< performances CPU (microsecondes)

		std::chrono::high_resolution_clock::time_point cpu_start ;	//!< La temps de début du CPU
		std::chrono::high_resolution_clock::time_point cpu_stop ;	//!< La temps de fin du CPU
		long long int cpu_time ;	//!< Pour lire le temps CPU


		/*==============================
	                FPS
	    ==============================*/		
		int fps ;
	    
	    std::chrono::high_resolution_clock::time_point wait_start ;	//!< Pour lire le temps CPU
	    std::chrono::high_resolution_clock::time_point wait_stop ;
	    long long int wait_time ;



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

        void timerBegin() ;
        void timerEnd() ;
} ;

#endif //TIMER_HPP