/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cel-hajj <cel-hajj@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 13:33:06 by cel-hajj          #+#    #+#             */
/*   Updated: 2026/07/08 01:16:11 by cel-hajj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

/* ********************* */
/* *** Packages Used *** */
/* ********************* */

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <limits.h>
# include <pthread.h>
# include <sys/time.h>

/* *********************** */
/* *** Structures Used *** */
/* *********************** */

typedef struct s_sim	t_sim;
typedef struct s_coder	t_coder;
typedef struct s_dongle	t_dongle;
typedef struct s_entry	t_entry;
typedef struct s_queue	t_queue;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				in_use;
	long			last_used_time;
	t_queue			*queue;
}	t_dongle;

typedef struct s_sim
{
	int				nb_of_coders;
	int				coders_ready;
	long			sim_start;
	int				sim_stop;
	long			time_to_compile;
	long			time_to_debug;
	long			time_to_refactor;
	long			number_of_compiles_required;
	long			dongle_cooldown;
	long			time_to_burnout;
	char			*scheduler;
	t_coder			*coders;
	t_dongle		*dongles;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	stop_mutex;
}	t_sim;

typedef struct s_coder
{
	int				id;
	volatile int	compiles_done;
	volatile long	last_compiled_time;
	pthread_mutex_t	data_mutex;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	t_sim			*sim;
	pthread_t		thread;
}	t_coder;

typedef struct s_entry
{
	t_coder	*coder;
	long	priority;
}	t_entry;

typedef struct s_queue
{
	t_entry	*entries;
	int		size;
	int		capacity;
}	t_queue;

/* ****************************************** */
/* *** Parsing and Verification Functions *** */
/* ****************************************** */
int		parse_and_extract(char **argv, t_sim *sim);

/* ********************* */
/* *** Coder Actions *** */
/* ********************* */

void	take_dongle(t_coder *coder);
void	wait_for_dongle(t_coder *coder, t_dongle *dongle);
void	compile(t_coder *coder);
void	put_dongle(t_coder *coder);
void	debug(t_coder *coder);
void	refactor(t_coder *coder);

/* *********************** */
/* *** Utils Functions *** */
/* *********************** */

void	ft_swap(t_queue *queue, int i1, int i2);
int		ft_min(t_entry entry1, t_entry entry2, int i);
int		sim_is_stopped(t_sim *sim);
void	broadcast_all_dongles(t_sim *sim);

/* *********************** */
/* *** Queue Functions *** */
/* *********************** */

t_queue	*queue_init(int capacity);
void	queue_push(t_queue *queue, t_entry *entry);
t_entry	queue_pop(t_queue *queue);
void	queue_free(t_queue *queue);

/* *************************** */
/* *** Execution Functions *** */
/* *************************** */
int		initialize_coders_and_dongles(t_sim *sim);
int		allocate_sim_arrays(t_sim *sim);
int		initialize_dongle(t_dongle *dongle, t_sim *simulator);
int		initialize_coder(t_coder *coder, int id,
			t_dongle *dongles, t_sim *simulator);
void	*work(void *arg);
void	*monitor(void *arg);

/* ********************** */
/* *** Memory Cleanup *** */
/* ********************** */
int		clean_up_initializing(t_sim *simulator);
void	free_up_dongles(t_sim *sim);
void	destroy_coder_mutexes(t_sim *sim);

#endif