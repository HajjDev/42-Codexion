/* *********************************************************************** */
/*                                                                         */
/*                                                     :::      ::::::::   */
/* codexion.c                                        :+:      :+:    :+:   */
/*                                                 +:+ +:+         +:+     */
/* By: cel-hajj <cel-hajj@student.s19.be>        +#+  +:+       +#+        */
/*                                             +#+#+#+#+#+   +#+           */
/* Created: 2026/04/14 07:56:17 by cel-hajj        #+#    #+#              */
/* Updated: 2026/05/21 18:14:49 by cel-hajj        ###   ########.fr       */
/*                                                                         */
/* *********************************************************************** */

#include "../includes/codexion.h"

int	initialize(t_sim *sim)
{
	struct timeval	tv;
	int				i;

	gettimeofday(&tv, NULL);
	sim->sim_start = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	pthread_mutex_init(&sim->print_mutex, NULL);
	pthread_mutex_init(&sim->stop_mutex, NULL);
	sim->sim_stop = 0;

	if (!initialize_coders_and_dongles(sim))
		return (clean_up_initializing(sim));
	i = 0;
	while (i < sim->nb_of_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	return (1);
}

int	main(int argc, char **argv)
{
	int		valid_args;
	t_sim	*sim;

	if (argc != 9)
	{
		printf("Please make sure to include all mandatory keys.");
		return (1);
	}
	sim = (t_sim *)malloc(sizeof(t_sim));
	if (!sim)
		return (1);
	valid_args = parse_and_extract(argv, sim);
	if (!valid_args)
	{
		printf("Please make sure to only pass valid arguments!");
		free(sim);
		return (1);
	}
	initialize(sim);
	return (0);
}
