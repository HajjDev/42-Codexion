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

t_entry	*create_entry(t_coder *coder)
{
	long			priority;
	t_entry			*entry;
	struct timeval	tv;

	entry = (t_entry *)malloc(sizeof(t_entry));
	if (!entry)
		return (NULL);
	if (!strcmp(coder->sim->scheduler, "fifo"))
	{
		gettimeofday(&tv, NULL);
		priority = ((tv.tv_sec * 1000) + tv.tv_usec / 1000);
	}
	else
		priority = coder->last_compiled_time + coder->sim->time_to_burnout;
	entry->coder = coder;
	entry->priority = priority;
	return (entry);
}

static int	dongle_not_ready(t_coder *coder, t_dongle *dongle, long ms)
{
	return (dongle->queue->entries[0].coder->id != coder->id
		|| dongle->in_use
		|| (ms - dongle->last_used_time) < coder->sim->dongle_cooldown);
}

static void	cond_wait_dongle(t_coder *coder, t_dongle *dongle, long ms)
{
	struct timespec	ts;
	long			abs_ms;

	if (!dongle->in_use
		&& dongle->queue->entries[0].coder->id == coder->id
		&& (ms - dongle->last_used_time) < coder->sim->dongle_cooldown)
	{
		abs_ms = dongle->last_used_time + coder->sim->dongle_cooldown
			+ coder->sim->sim_start;
		ts.tv_sec = abs_ms / 1000;
		ts.tv_nsec = (abs_ms % 1000) * 1000000;
		pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
	}
	else
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
}

static void	wait_for_dongle(t_coder *coder, t_dongle *dongle)
{
	struct timeval	tv;
	long			ms;
	t_entry			*entry;

	pthread_mutex_lock(&dongle->mutex);
	entry = create_entry(coder);
	if (!entry)
		return (pthread_mutex_unlock(&dongle->mutex), (void)0);
	queue_push(dongle->queue, entry);
	free(entry);
	gettimeofday(&tv, NULL);
	ms = ((tv.tv_sec * 1000) + tv.tv_usec / 1000) - coder->sim->sim_start;
	while (!sim_is_stopped(coder->sim) && dongle_not_ready(coder, dongle, ms))
	{
		cond_wait_dongle(coder, dongle, ms);
		gettimeofday(&tv, NULL);
		ms = ((tv.tv_sec * 1000) + tv.tv_usec / 1000) - coder->sim->sim_start;
	}
	if (!coder->sim->sim_stop)
	{
		dongle->in_use = 1;
		queue_pop(dongle->queue);
		pthread_mutex_unlock(&dongle->mutex);
		pthread_mutex_lock(&(coder->sim->print_mutex));
		printf("%ld %d has taken a dongle\n", ms, coder->id);
		pthread_mutex_unlock(&(coder->sim->print_mutex));
	}
	else
		pthread_mutex_unlock(&dongle->mutex);
}

void	take_odd_dongles(t_coder *coder)
{
	wait_for_dongle(coder, coder->right_dongle);
	wait_for_dongle(coder, coder->left_dongle);
}

void	take_even_dongles(t_coder *coder)
{
	wait_for_dongle(coder, coder->left_dongle);
	wait_for_dongle(coder, coder->right_dongle);
}
