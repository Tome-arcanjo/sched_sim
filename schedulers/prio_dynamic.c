#include <stdio.h>
#include <stdlib.h>

#include "queue.h" // contem funções uteis para filas
#include "proc.h"  // possui as funções dos processos
#include "stats.h" // possui as funções de estatisticas 
#include "utils.h" // possui funções uteis 

// Utilizando as variáveis globais definidas no 'main'
extern struct queue * ready;    // fila de aptos (Fila 1)
extern struct queue * ready2;   // segunda fila de aptos (Fila 2)
extern struct queue * blocked;  // fila de bloqueados
extern struct queue * finished; // fila de finalizados

// variavel global que indica o tempo maximo que um processo pode executar ao todo
extern int MAX_TIME;

struct proc * scheduler(struct proc * current)
{
    struct proc * selected = NULL; 

    // =====================================
    // PROCESSO QUE ESTAVA EXECUTANDO (SAÍDA)
    // =====================================
    if(current != NULL)
    {
        // 1. Processo terminou a execução totalmente
        if(current->remaining_time <= 0)
        {
            current->state = FINISHED;

            printf("Processo %d finalizado\n", current->pid);

            count_finished_in(current);
            enqueue(finished, current);
        }
        // 2. Processo saiu por motivo de E/S (Bloqueio)
        else if(current->state == BLOCKED)
        {
            // Quando ele retornar do bloqueio, deverá ir para a Fila 2
            current->queue = 1; 

            // Insere na fila de bloqueados para esperar o término da E/S
            enqueue(blocked, current);
        }
        // 3. Processo saiu por Preempção (Estouro de Quantum)
        else 
        {
            current->state = READY;
            // Retorna obrigatoriamente para a Fila 1
            current->queue = 0; 

            printf("Processo %d voltou para READY 1\n", current->pid);

            count_ready_in(current);
            enqueue(ready, current);
        }
    }

    // =====================================
    // SELEÇÃO DO PRÓXIMO PROCESSO (70% vs 30%)
    // =====================================
    int chance = rand() % 100;

    if(chance < 70)
    {
        // Tenta Fila 1, senão vai para Fila 2
        if(!isempty(ready))
            selected = dequeue(ready);
        else if(!isempty(ready2))
            selected = dequeue(ready2);
    }
    else
    {
        // Tenta Fila 2, senão vai para Fila 1
        if(!isempty(ready2))
            selected = dequeue(ready2);
        else if(!isempty(ready))
            selected = dequeue(ready);
    }

    // =====================================
    // PREPARA O PROCESSO SELECIONADO
    // =====================================
    if(selected != NULL)
    {
        selected->state = RUNNING;

        count_ready_out(selected);

        printf("Processo %d escalonado da fila %d\n",
               selected->pid,
               selected->queue);
    }

    return selected;
}