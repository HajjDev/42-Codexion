/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialize_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cel-hajj <cel-hajj@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 00:00:00 by cel-hajj          #+#    #+#             */
/*   Updated: 2026/07/08 00:00:00 by cel-hajj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

int	allocate_sim_arrays(t_sim *sim)
{
	sim->coders = (t_coder *)malloc(sim->nb_of_coders * sizeof(t_coder));
	if (!sim->coders)
		return (0);
	sim->dongles = (t_dongle *)malloc(sim->nb_of_coders * sizeof(t_dongle));
	if (!sim->dongles)
		return (0);
	memset(sim->dongles, 0, sim->nb_of_coders * sizeof(t_dongle));
	return (1);
}

int	initialize_dongle(t_dongle *dongle, t_sim *simulator)
{
	dongle->in_use = 0;
	dongle->queue = queue_init(simulator->nb_of_coders);
	if (!dongle->queue)
		return (0);
	pthread_cond_init(&dongle->cond, NULL);
	pthread_mutex_init(&dongle->mutex, NULL);
	dongle->last_used_time = -(simulator->dongle_cooldown);
	return (1);
}

int	initialize_coder(t_coder *coder, int id,
	t_dongle *dongles, t_sim *simulator)
{
	coder->sim = simulator;
	coder->compiles_done = 0;
	coder->id = id;
	coder->last_compiled_time = 0;
	pthread_mutex_init(&coder->data_mutex, NULL);
	coder->left_dongle = &dongles[(id - 1)];
	coder->right_dongle = &dongles[id % coder->sim->nb_of_coders];
	pthread_create(&coder->thread, NULL, work, coder);
	return (1);
}

void	free_up_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	if (!sim->dongles)
		return ;
	while (i < sim->nb_of_coders)
	{
		if (sim->dongles[i].queue)
		{
			queue_free(sim->dongles[i].queue);
			pthread_mutex_destroy(&sim->dongles[i].mutex);
			pthread_cond_destroy(&sim->dongles[i].cond);
		}
		i++;
	}
	free(sim->dongles);
}

void	destroy_coder_mutexes(t_sim *sim)
{
	int	i;

	i = 0;
	if (!sim->coders_ready)
		return ;
	while (i < sim->nb_of_coders)
	{
		pthread_mutex_destroy(&sim->coders[i].data_mutex);
		i++;
	}
}
