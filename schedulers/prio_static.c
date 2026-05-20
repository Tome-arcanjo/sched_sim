#include <stdio.h>
#include <stdlib.h>

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
    struct proc * selected = NULL;

    // =====================================
    // PROCESSO QUE ESTAVA EXECUTANDO (SAÍDA)
    // =====================================
    if(current != NULL)
    {
        // Processo terminou
        if(current->remaining_time <= 0)
        {
            current->state = FINISHED;

            printf("Processo %d finalizado\n",
                   current->pid);

            count_finished_in(current);

            enqueue(finished, current);
        }
        else
        {
            current->state = READY;

            count_ready_in(current);

            // Ao saírem da execução, verifica-se em qual fila o processo deve ficar
            if(current->process_time_total > ((MAX_TIME * 30) / 100))
            {
                current->queue = 0; // Primeira fila (ready)
            }
            else
            {
                current->queue = 1; // Segunda fila (ready2)
            }

            // Sempre volta para a mesma fila definida em 'current->queue'
            if(current->queue == 0)
            {
                enqueue(ready, current);

                printf("Processo %d voltou para READY 1\n",
                       current->pid);
            }
            else
            {
                enqueue(ready2, current);

                printf("Processo %d voltou para READY 2\n",
                       current->pid);
            }
        }
    }

    
    // ESCOLHE FILA 
    
    int chance = rand() % 100;

    if(chance < 70)
    {
        if(!isempty(ready))
            selected = dequeue(ready);
        else if(!isempty(ready2))
            selected = dequeue(ready2);
    }
    else
    {
        if(!isempty(ready2))
            selected = dequeue(ready2);
        else if(!isempty(ready))
            selected = dequeue(ready);
    }

    // =====================================
    // ATUALIZA ESTADO DO PROCESSO SELECIONADO
    // =====================================
    if(selected != NULL)
    {
        selected->state = RUNNING;

        count_ready_out(selected);

        // O 'selected->queue' vai refletir corretamente a fila estática dele
        printf("Processo %d escalonado da fila %d\n",
               selected->pid,
               selected->queue);
    }

    return selected;
}