/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cel-hajj <cel-hajj@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 13:36:07 by cel-hajj          #+#    #+#             */
/*   Updated: 2026/07/07 13:36:10 by cel-hajj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

void	take_dongle(t_coder *coder)
{
	if (coder->id % 2 == 0)
	{
		wait_for_dongle(coder, coder->left_dongle);
		wait_for_dongle(coder, coder->right_dongle);
	}
	else
	{
		wait_for_dongle(coder, coder->right_dongle);
		wait_for_dongle(coder, coder->left_dongle);
	}
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
	coder->compiles_done++;
	coder->last_compiled_time = ms;
	usleep(coder->sim->time_to_compile * 1000);
}

void	put_dongle(t_coder *coder)
{
	struct timeval	tv;
	long			ms;

	pthread_mutex_lock(&coder->left_dongle->mutex);
	coder->left_dongle->in_use = 0;
	gettimeofday(&tv, NULL);
	ms = ((tv.tv_sec * 1000) + tv.tv_usec / 1000) - coder->sim->sim_start;
	coder->left_dongle->last_used_time = ms;
	pthread_cond_broadcast(&coder->left_dongle->cond);
	pthread_mutex_unlock(&(coder->left_dongle->mutex));
	pthread_mutex_lock(&coder->right_dongle->mutex);
	coder->right_dongle->in_use = 0;
	gettimeofday(&tv, NULL);
	ms = ((tv.tv_sec * 1000) + tv.tv_usec / 1000) - coder->sim->sim_start;
	coder->right_dongle->last_used_time = ms;
	pthread_cond_broadcast(&coder->right_dongle->cond);
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
	usleep(coder->sim->time_to_debug * 1000);
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
	usleep(coder->sim->time_to_refactor * 1000);
}
