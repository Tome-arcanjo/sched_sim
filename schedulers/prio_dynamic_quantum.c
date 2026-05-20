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

// Tempo máximo da execução de um processo por entrada na CPU (microsegundos)
extern int QUANTUM;

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
            printf("[RIO DYNAMIC QUANTUM] Processo %d finalizado\n", current->pid);
            
            count_finished_in(current);
            enqueue(finished, current);
        }
        else 
        {
            // 2. Aplica a regra de prioridade baseada no tempo consumido na rodada
            // Se usou menos de 50% do QUANTUM -> Fila 1 (queue = 0)
            // Se usou 50% ou mais do QUANTUM -> Fila 2 (queue = 1)
            if (current->process_time < (QUANTUM / 2)) 
            {
                current->queue = 0;
            } 
            else 
            {
                current->queue = 1;
            }

            // 3. Direciona para a fila física correta dependendo do estado atual
            if(current->state == BLOCKED)
            {
                // Se saiu para E/S, vai para a fila de bloqueados.
                // Quando ele acordar, o simulador vai ler o 'current->queue' acima e pôr na fila certa.
                enqueue(blocked, current);
            }
            else 
            {
                // Se foi preempção (estouro de quantum), volta direto para uma das filas de aptos
                current->state = READY;
                count_ready_in(current);

                if(current->queue == 0)
                {
                    printf("[RIO DYNAMIC QUANTUM] Processo %d voltou para READY 1\n", current->pid);
                    enqueue(ready, current);
                }
                else
                {
                    printf("[RIO DYNAMIC QUANTUM] Processo %d voltou para READY 2\n", current->pid);
                    enqueue(ready2, current);
                }
            }
        }
    }

    // =====================================
    // SELEÇÃO DO PRÓXIMO PROCESSO (70% vs 30%)
    // =====================================
    int chance = rand() % 100;

    if(chance < 70)
    {
        // Tenta Fila 1 (ready), fallback para Fila 2 (ready2)
        if(!isempty(ready))
            selected = dequeue(ready);
        else if(!isempty(ready2))
            selected = dequeue(ready2);
    }
    else
    {
        // Tenta Fila 2 (ready2), fallback para Fila 1 (ready)
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

        printf("[RIO DYNAMIC QUANTUM] Processo %d escalonado da fila %d\n", 
               selected->pid, 
               selected->queue);
    }

    return selected;
}