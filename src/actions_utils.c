/* *********************************************************************** */
/*                                                                         */
/*                                                     :::      ::::::::   */
/* actions_utils.c                                   :+:      :+:    :+:   */
/*                                                 +:+ +:+         +:+     */
/* By: cel-hajj <cel-hajj@student.s19.be>        +#+  +:+       +#+        */
/*                                             +#+#+#+#+#+   +#+           */
/* Created: 2026/05/21 17:32:15 by cel-hajj        #+#    #+#              */
/* Updated: 2026/05/21 18:15:42 by cel-hajj        ###   ########.fr       */
/*                                                                         */
/* *********************************************************************** */

#include "../includes/codexion.h"

void	take_odd_dongles(t_coder *coder)
{
	struct timeval	tv;
	long			ms;

	gettimeofday(&tv, NULL);
	ms = ((tv.tv_sec * 1000) + tv.tv_usec / 1000) - coder->sim->sim_start;
	pthread_mutex_lock(&(coder->right_dongle->mutex));
	pthread_mutex_lock(&(coder->sim->print_mutex));
	printf("%ld %d has taken a dongle\n", ms, coder->id);
	pthread_mutex_unlock(&(coder->sim->print_mutex));
	gettimeofday(&tv, NULL);
	ms = ((tv.tv_sec * 1000) + tv.tv_usec / 1000) - coder->sim->sim_start;
	pthread_mutex_lock(&(coder->left_dongle->mutex));
	pthread_mutex_lock(&(coder->sim->print_mutex));
	printf("%ld %d has taken a dongle\n", ms, coder->id);
	pthread_mutex_unlock(&(coder->sim->print_mutex));
}

void	take_even_dongles(t_coder *coder)
{
	struct timeval	tv;
	long			ms;

	gettimeofday(&tv, NULL);
	ms = ((tv.tv_sec * 1000) + tv.tv_usec / 1000) - coder->sim->sim_start;
	pthread_mutex_lock(&(coder->left_dongle->mutex));
	pthread_mutex_lock(&(coder->sim->print_mutex));
	printf("%ld %d has taken a dongle\n", ms, coder->id);
	pthread_mutex_unlock(&(coder->sim->print_mutex));
	gettimeofday(&tv, NULL);
	ms = ((tv.tv_sec * 1000) + tv.tv_usec / 1000) - coder->sim->sim_start;
	pthread_mutex_lock(&(coder->right_dongle->mutex));
	pthread_mutex_lock(&(coder->sim->print_mutex));
	printf("%ld %d has taken a dongle\n", ms, coder->id);
	pthread_mutex_unlock(&(coder->sim->print_mutex));
}
