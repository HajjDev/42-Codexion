/* *********************************************************************** */
/*                                                                         */
/*                                                     :::      ::::::::   */
/* queue.c                                           :+:      :+:    :+:   */
/*                                                 +:+ +:+         +:+     */
/* By: cel-hajj <cel-hajj@student.s19.be>        +#+  +:+       +#+        */
/*                                             +#+#+#+#+#+   +#+           */
/* Created: 2026/05/12 08:39:28 by cel-hajj        #+#    #+#              */
/* Updated: 2026/05/17 21:48:26 by cel-hajj        ###   ########.fr       */
/*                                                                         */
/* *********************************************************************** */

#include "../includes/codexion.h"

t_queue	*queue_init(int capacity)
{
	t_queue	*queue;

	queue = (t_queue *)malloc(sizeof(t_queue));
	if (!queue)
		return (NULL);
	queue->capacity = capacity;
	queue->size = 0;
	queue->entries = (t_entry *)malloc(sizeof(t_entry) * (capacity + 1));
	if (!(queue->entries))
	{
		free(queue);
		return (NULL);
	}
	return (queue);
}

void	queue_push(t_queue *queue, t_entry *entry)
{
	int		i;

	queue->entries[queue->size] = *entry;
	i = queue->size;
	while (i > 0
		&& queue->entries[i].priority < queue->entries[(i - 1) / 2].priority)
	{
		ft_swap(queue, i, (i - 1) / 2);
		i = (i - 1) / 2;
	}
	queue->size++;
}

t_entry	queue_pop(t_queue *queue)
{
	int		i;
	t_entry	popped;
	int		smallest;

	popped = queue->entries[0];
	ft_swap(queue, 0, queue->size - 1);
	queue->size--;
	i = 0;
	while ((2 * i + 1) < queue->size)
	{
		if ((2 * i + 2) >= queue->size)
			smallest = (2 * i + 1);
		else
			smallest = ft_min(queue->entries[(i * 2) + 1],
					queue->entries[(i * 2) + 2], i);
		if (queue->entries[smallest].priority < queue->entries[i].priority)
		{
			ft_swap(queue, smallest, i);
			i = smallest;
		}
		else
			break ;
	}
	return (popped);
}

void	queue_free(t_queue *queue)
{
	free(queue->entries);
	free(queue);
}
