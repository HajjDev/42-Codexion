/* *********************************************************************** */
/*                                                                         */
/*                                                     :::      ::::::::   */
/* functions.c                                       :+:      :+:    :+:   */
/*                                                 +:+ +:+         +:+     */
/* By: cel-hajj <cel-hajj@student.s19.be>        +#+  +:+       +#+        */
/*                                             +#+#+#+#+#+   +#+           */
/* Created: 2026/05/12 07:30:28 by cel-hajj        #+#    #+#              */
/* Updated: 2026/05/12 07:30:47 by cel-hajj        ###   ########.fr       */
/*                                                                         */
/* *********************************************************************** */

#include "../includes/codexion.h"

void	take_dongle(t_coder *coder)
{
	struct timeval	tv;
	long			ms;

	if (coder->id % 2 == 0)
	{
		pthread_mutex_lock(&(coder->left_dongle->mutex));
		pthread_mutex_lock(&(coder->right_dongle->mutex));
	}
	else
	{
		pthread_mutex_lock(&(coder->right_dongle->mutex));
		pthread_mutex_lock(&(coder->left_dongle->mutex));
	}
	gettimeofday(&tv, NULL);
	ms = ((tv.tv_sec * 1000) + tv.tv_usec / 1000) - coder->sim->sim_start;
	pthread_mutex_lock(&(coder->sim->print_mutex));
	printf("%ld %d has taken a dongle\n", ms, coder->id);
	pthread_mutex_unlock(&(coder->sim->print_mutex));

	coder->left_dongle->in_use = 1;
	coder->right_dongle->in_use = 1;
}

void	compile(t_coder *coder)
{
	struct timeval	tv;
	long			ms;

	gettimeofday(&tv, NULL);
	ms = ((tv.tv_sec * 1000) + tv.tv_usec / 1000) - coder->sim->sim_start;

	pthread_mutex_lock(&(coder->sim->print_mutex));
	printf("%ld %d is compiling\n", ms, coder->id);
	pthread_mutex_unlock(&(coder->sim->print_mutex));
	coder->last_compiled_time = ms;
}

void	put_dongle(t_coder *coder)
{
	coder->left_dongle->in_use = 0;
	coder->right_dongle->in_use = 0;
	pthread_mutex_unlock(&(coder->left_dongle->mutex));
	pthread_mutex_unlock(&(coder->right_dongle->mutex));
}

void	debug(t_coder *coder)
{
	struct timeval	tv;
	long			ms;

	gettimeofday(&tv, NULL);
	ms = ((tv.tv_sec * 1000) + tv.tv_usec / 1000) - coder->sim->sim_start;

	pthread_mutex_lock(&(coder->sim->print_mutex));
	printf("%ld %d is debugging\n", ms, coder->id);
	pthread_mutex_unlock(&(coder->sim->print_mutex));
}

void	refactor(t_coder *coder)
{
	struct timeval	tv;
	long			ms;

	gettimeofday(&tv, NULL);
	ms = ((tv.tv_sec * 1000) + tv.tv_usec / 1000) - coder->sim->sim_start;

	pthread_mutex_lock(&(coder->sim->print_mutex));
	printf("%ld %d is refactoring\n", ms, coder->id);
	pthread_mutex_unlock(&(coder->sim->print_mutex));
}
