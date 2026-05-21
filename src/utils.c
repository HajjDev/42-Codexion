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

int	clean_up_initializing(t_sim *simulator)
{
	return (0);
}
