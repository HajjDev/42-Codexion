/* *********************************************************************** */
/*                                                                         */
/*                                                     :::      ::::::::   */
/* initialize.c                                      :+:      :+:    :+:   */
/*                                                 +:+ +:+         +:+     */
/* By: cel-hajj <cel-hajj@student.s19.be>        +#+  +:+       +#+        */
/*                                             +#+#+#+#+#+   +#+           */
/* Created: 2026/05/21 18:14:40 by cel-hajj        #+#    #+#              */
/* Updated: 2026/05/21 18:15:33 by cel-hajj        ###   ########.fr       */
/*                                                                         */
/* *********************************************************************** */

#include "../includes/codexion.h"

static int	initialize_dongle(t_dongle *dongle, t_sim *simulator)
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

static int	initialize_coder(t_coder *coder, int id,
	t_dongle *dongles, t_sim *simulator)
{
	coder->sim = simulator;
	coder->compiles_done = 0;
	coder->id = id;
	coder->last_compiled_time = 0;
	coder->left_dongle = &dongles[(id - 1)];
	coder->right_dongle = &dongles[id % coder->sim->nb_of_coders];
	pthread_create(&coder->thread, NULL, work, coder);
	return (1);
}

int	initialize_coders_and_dongles(t_sim *sim)
{
	int	i;

	sim->coders = NULL;
	sim->dongles = NULL;
	sim->coders = (t_coder *)malloc(sim->nb_of_coders * sizeof(t_coder));
	if (!sim->coders)
		return (0);
	sim->dongles = (t_dongle *)malloc(sim->nb_of_coders * sizeof(t_dongle));
	if (!sim->dongles)
		return (0);
	memset(sim->dongles, 0, sim->nb_of_coders * sizeof(t_dongle));
	i = 0;
	while (i < sim->nb_of_coders)
	{
		if (!initialize_dongle(&sim->dongles[i], sim))
			return (0);
		i++;
	}
	i = 0;
	while (i < sim->nb_of_coders)
	{
		initialize_coder(&sim->coders[i], i + 1, sim->dongles, sim);
		i++;
	}
	return (1);
}
