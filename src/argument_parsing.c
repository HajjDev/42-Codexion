/* *********************************************************************** */
/*                                                                         */
/*                                                     :::      ::::::::   */
/* argument_parsing.c                                :+:      :+:    :+:   */
/*                                                 +:+ +:+         +:+     */
/* By: cel-hajj <cel-hajj@student.s19.be>        +#+  +:+       +#+        */
/*                                             +#+#+#+#+#+   +#+           */
/* Created: 2026/04/27 01:06:41 by cel-hajj        #+#    #+#              */
/* Updated: 2026/04/27 01:10:49 by cel-hajj        ###   ########.fr       */
/*                                                                         */
/* *********************************************************************** */

#include "../includes/codexion.h"

static int	ft_isvalid_int(char *str)
{
	int		i;
	char	*max;

	i = 0;
	max = "2147483647";
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	if (i == 0 || i > 10)
		return (0);
	if (i == 10 && strcmp(str, max) > 0)
		return (0);
	return (1);
}

static int	verify_args(char **argv)
{
	int	i;

	i = 1;
	while (argv[i])
	{
		if (i < 8)
		{
			if (!ft_isvalid_int(argv[i]))
				return (0);
			if (i != 7 && atoi(argv[i]) == 0)
				return (0);
		}
		else
		{
			if (strcmp("fifo", argv[i]) != 0 && strcmp("edf", argv[i]) != 0)
				return (0);
		}
		i++;
	}
	return (1);
}

int	parse_and_extract(char **argv, t_sim *sim)
{
	int	valid_arguments;

	valid_arguments = verify_args(argv);
	if (valid_arguments)
	{
		sim->number_of_coders = atoi(argv[1]);
		sim->time_to_burnout = atoi(argv[2]);
		sim->time_to_compile = atoi(argv[3]);
		sim->time_to_debug = atoi(argv[4]);
		sim->time_to_refactor = atoi(argv[5]);
		sim->number_of_compiles_required = atoi(argv[6]);
		sim->dongle_cooldown = atoi(argv[7]);
		sim->scheduler = argv[8];
		return (1);
	}
	return (0);
}
