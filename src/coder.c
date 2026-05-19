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
#include <pthread.h>
#include <sys/select.h>

static int	sim_is_stopped(t_sim *sim)
{
	int	stopped_status;

	pthread_mutex_lock(&(sim->stop_mutex));
	stopped_status = sim->sim_stop;
	pthread_mutex_unlock(&(sim->stop_mutex));
	return (stopped_status);
}

void	*work(void *arg)
{
	t_coder			*coder;

	coder = (t_coder *)arg;
	while (!sim_is_stopped(coder->sim))
	{
		take_dongle(coder);
		compile(coder);
		put_dongle(coder);
		debug(coder);
		refactor(coder);
	}
	return (NULL);
}
