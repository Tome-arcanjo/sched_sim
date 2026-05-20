#include <stdio.h>

#include "queue.h"
#include "proc.h"
#include "stats.h"
#include "utils.h"

extern struct queue * ready;
extern struct queue * ready2;
extern struct queue * blocked;
extern struct queue * finished;

extern int MAX_TIME;

struct proc * scheduler(struct proc * current)
{
    struct proc * p;
    struct proc * selected = NULL;

    /*
     * LJF NÃO-PREEMPTIVO
     *
     * Se existe processo executando
     * e ele ainda não terminou,
     * continua executando.
     */
    if (current != NULL)
    {
        if (current->remaining_time > 0)
        {
            return current;
        }

        /*
         * Processo terminou
         */
        current->state = FINISHED;

        enqueue(finished, current);

        printf("[LJF] PID %d FINALIZADO\n",
               current->pid);

        current = NULL;
    }

    /*
     * READY vazia
     */
    if (isempty(ready))
    {
        return NULL;
    }

    /*
     * Procura MAIOR remaining_time
     */
    p = ready->head;

    while (p != NULL)
    {
        if (selected == NULL ||
            p->remaining_time > selected->remaining_time)
        {
            selected = p;
        }

        p = p->next;
    }

    /*
     * Remove da READY
     */
    selected = dequeue_bypid(ready, selected->pid);

    /*
     * Atualiza estado
     */
    selected->state = RUNNING;

    printf("[LJF] PID %d selecionado | remaining_time=%d\n",
           selected->pid,
           selected->remaining_time);

    return selected;
}