#include "sched.h"
/**
 *Dijo que nos basasemos en el RR y SFJ
 **/
/* Global prp quantum parameter */
int prp_quantum=3;

/* Structure to store RR thread-specific fields */
struct prp_data {
	int remaining_ticks_slice;
};

static task_t* pick_next_task_prp(runqueue_t* rq)
{
	task_t* t=head_slist(&rq->tasks);

	/* Current is not on the rq -> let's remove it */
	if (t) 
		remove_slist(&rq->tasks,t);

	return t;
}

static int compare_tasks_cpu_burst(void *t1,void *t2)
{ 
	task_t* tsk1=(task_t*)t1;
	task_t* tsk2=(task_t*)t2;

	return tsk1->prio - tsk2->prio;
}

static void enqueue_task_prp(task_t* t,runqueue_t* rq, int preempted)
{

	struct prp_data* cs_data=(struct prp_data*) t->tcs_data;

	if (t->on_rq || is_idle_task(t))
		return;

	if (t->flags & TF_INSERT_FRONT) {
		//Clear flag
		t->flags&=~TF_INSERT_FRONT;
		sorted_insert_slist_front(&rq->tasks, t, 1, compare_tasks_cpu_burst);  //Push task
	} else
		sorted_insert_slist(&rq->tasks, t, 1, compare_tasks_cpu_burst);  //Push task

	cs_data->remaining_ticks_slice=prp_quantum; // Reset slice
    
    /* If the task was not using the current CPU, check whether a preemption is in order or not */
	if (!preempted) {
		task_t* current=rq->cur_task;

		/* Trigger a preemption if this task has a shorter CPU burst than current */
		if (preemptive_scheduler && cs_data->remaining_ticks_slice<=0 && t->prio < current->prio) {
			rq->need_resched=TRUE;
			current->flags|=TF_INSERT_FRONT; /* To avoid unfair situations in the event
                                                another task with the same length wakes up as well*/
		}
	}

}

static task_t* steal_task_prp(runqueue_t* rq)
{
	task_t* t=tail_slist(&rq->tasks);

	if (t) 
		remove_slist(&rq->tasks,t);
	
	return t;
}


static int task_new_prp(task_t* t)
{
	struct prp_data* cs_data=malloc(sizeof(struct prp_data));

	if (!cs_data)
		return 1;  /* Cannot reserve memory */

	/* initialize the quantum */
	cs_data->remaining_ticks_slice=prp_quantum;
	t->tcs_data=cs_data;
	return 0;
}

static void task_free_prp(task_t* t)
{
	if (t->tcs_data) {
		free(t->tcs_data);
		t->tcs_data=NULL;
	}
}

static void task_tick_prp(runqueue_t* rq)
{
	task_t* current=rq->cur_task;
	struct prp_data* cs_data=(struct prp_data*) current->tcs_data;

	if (is_idle_task(current))
		return;

	cs_data->remaining_ticks_slice--; /* Charge tick */

	if (cs_data->remaining_ticks_slice<=0)
		rq->need_resched=TRUE; //Force a resched !!
}


sched_class_t prp_sched= {
	.task_new=task_new_prp,
    .task_free=task_free_prp,
    .task_tick=task_tick_prp,
	.pick_next_task=pick_next_task_prp,
	.enqueue_task=enqueue_task_prp,
	.steal_task=steal_task_prp
};

