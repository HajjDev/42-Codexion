/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialize.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cel-hajj <cel-hajj@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 13:36:30 by cel-hajj          #+#    #+#             */
/*   Updated: 2026/07/07 13:36:32 by cel-hajj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

int	initialize_coders_and_dongles(t_sim *sim)
{
	int	i;

	sim->coders = NULL;
	sim->dongles = NULL;
	sim->coders_ready = 0;
	if (!allocate_sim_arrays(sim))
		return (0);
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
	sim->coders_ready = 1;
	return (1);
}

int	clean_up_initializing(t_sim *simulator)
{
	destroy_coder_mutexes(simulator);
	free(simulator->coders);
	free_up_dongles(simulator);
	pthread_mutex_destroy(&simulator->print_mutex);
	pthread_mutex_destroy(&simulator->stop_mutex);
	free(simulator);
	return (0);
}
