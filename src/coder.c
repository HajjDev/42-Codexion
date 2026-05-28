/* *********************************************************************** */
/*                                                                         */
/*                                                     :::      ::::::::   */
/* coder.c                                           :+:      :+:    :+:   */
/*                                                 +:+ +:+         +:+     */
/* By: cel-hajj <cel-hajj@student.s19.be>        +#+  +:+       +#+        */
/*                                             +#+#+#+#+#+   +#+           */
/* Created: 2026/05/12 07:34:08 by cel-hajj        #+#    #+#              */
/* Updated: 2026/05/12 07:34:09 by cel-hajj        ###   ########.fr       */
/*                                                                         */
/* *********************************************************************** */

#include "../includes/codexion.h"

static void	*print_burnout_and_stop(t_coder coder, t_sim *sim)
{
	struct timeval	tv;
	long			ms;

	gettimeofday(&tv, NULL);
	ms = (tv.tv_sec * 1000) + (tv.tv_usec / 1000) - sim->sim_start;
	pthread_mutex_lock(&sim->print_mutex);
	printf("%ld %d burned out\n", ms, coder.id);
	pthread_mutex_unlock(&sim->print_mutex);
	pthread_mutex_lock(&sim->stop_mutex);
	sim->sim_stop = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
	broadcast_all_dongles(sim);
	return (NULL);
}

static void	*stop_simulation(t_sim *sim)
{
	pthread_mutex_lock(&sim->stop_mutex);
	sim->sim_stop = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
	broadcast_all_dongles(sim);
	return (NULL);
}

static int	check_coders(t_sim *sim)
{
	struct timeval	tv;
	int				i;
	int				valid;

	i = 0;
	valid = 1;
	while (i < sim->nb_of_coders)
	{
		gettimeofday(&tv, NULL);
		if (((tv.tv_sec * 1000) + (tv.tv_usec / 1000) - sim->sim_start)
			- sim->coders[i].last_compiled_time >= sim->time_to_burnout)
			return (print_burnout_and_stop(sim->coders[i], sim), -1);
		if (sim->coders[i].compiles_done < sim->number_of_compiles_required)
			valid = 0;
		i++;
	}
	return (valid);
}

void	*monitor(void *arg)
{
	t_sim	*sim;
	int		result;

	sim = (t_sim *)arg;
	while (!sim_is_stopped(sim))
	{
		result = check_coders(sim);
		if (result == -1)
			return (NULL);
		if (result == 1)
			return (stop_simulation(sim));
		usleep(1000);
	}
	return (NULL);
}

void	*work(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (!sim_is_stopped(coder->sim))
	{
		take_dongle(coder);
		if (sim_is_stopped(coder->sim))
		{
			put_dongle(coder);
			return (NULL);
		}
		compile(coder);
		put_dongle(coder);
		if (sim_is_stopped(coder->sim))
			return (NULL);
		debug(coder);
		if (sim_is_stopped(coder->sim))
			return (NULL);
		refactor(coder);
	}
	return (NULL);
}
