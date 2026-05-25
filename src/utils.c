/* *********************************************************************** */
/*                                                                         */
/*                                                     :::      ::::::::   */
/* utils.c                                           :+:      :+:    :+:   */
/*                                                 +:+ +:+         +:+     */
/* By: cel-hajj <cel-hajj@student.s19.be>        +#+  +:+       +#+        */
/*                                             +#+#+#+#+#+   +#+           */
/* Created: 2026/05/17 21:43:45 by cel-hajj        #+#    #+#              */
/* Updated: 2026/05/21 18:14:13 by cel-hajj        ###   ########.fr       */
/*                                                                         */
/* *********************************************************************** */

#include "../includes/codexion.h"

void	ft_swap(t_queue *queue, int i1, int i2)
{
	t_entry	temp;

	temp = queue->entries[i1];
	queue->entries[i1] = queue->entries[i2];
	queue->entries[i2] = temp;
}

int	ft_min(t_entry entry1, t_entry entry2, int i)
{
	if (entry1.priority < entry2.priority)
		return ((i * 2) + 1);
	return ((i * 2) + 2);
}

int	sim_is_stopped(t_sim *sim)
{
	int	stopped_status;

	pthread_mutex_lock(&(sim->stop_mutex));
	stopped_status = sim->sim_stop;
	pthread_mutex_unlock(&(sim->stop_mutex));
	return (stopped_status);
}

void	free_up_dongle(t_dongle *dongle)
{
	if (!dongle->queue)
		return ;
	queue_free(dongle->queue);
	pthread_mutex_destroy(&dongle->mutex);
	pthread_cond_destroy(&dongle->cond);
}

int	clean_up_initializing(t_sim *simulator)
{
	int	i;

	i = 0;
	free(simulator->coders);
	if (simulator->dongles)
	{
		while (i < simulator->nb_of_coders)
		{
			free_up_dongle(&simulator->dongles[i]);
			i++;
		}
		free(simulator->dongles);
	}
	pthread_mutex_destroy(&simulator->print_mutex);
	pthread_mutex_destroy(&simulator->stop_mutex);
	free(simulator);
	return (0);
}
