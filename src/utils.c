/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cel-hajj <cel-hajj@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 13:36:39 by cel-hajj          #+#    #+#             */
/*   Updated: 2026/07/07 13:36:40 by cel-hajj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

void	broadcast_all_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_of_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].mutex);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].mutex);
		i++;
	}
}
