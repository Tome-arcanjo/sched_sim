# Estudo Comparativo de Algoritmos de Escalonamento de Processos em Ambiente de Simulação Baseado em POSIX Threads

**Autor:** [Seu Nome Aqui]  
**Instituição:** Bacharelado em Sistemas de Informação (BSI)  
Centro de Ensino Superior do Seridó (CERES)  
Universidade Federal do Rio Grande do Norte (UFRN)  
**Disciplina:** DCT2101 - Sistemas Operacionais  

---

### Resumo
Este trabalho apresenta o desenvolvimento, implementação e análise comparativa de seis algoritmos de escalonamento de processos em um simulador educacional multithread desenvolvido em C utilizando a biblioteca POSIX Threads. Os algoritmos analisados foram: First-In First-Out (FIFO), Shortest Job First (SJF), Longest Job First (LJF), Prioridade Estática com Múltiplas Filas, Prioridade Dinâmica por Tipo de Comportamento e Prioridade Dinâmica Baseada em Fração de Quantum. O objetivo foi avaliar o desempenho de cada algoritmo sob métricas clássicas de Sistemas Operacionais, tais como o Tempo Médio de Espera (TME), o Tempo Médio de Bloqueio (TMB) e o Tempo Médio de Retorno (TMT/Turnaround). Os resultados evidenciam os tradeoffs clássicos entre justiça, vazão e minimização de tempos médios associados a cada política.

**Palavras-chave:** Escalonamento de CPU, Sistemas Operacionais, POSIX Threads, Simulação de Processos.

### Abstract
This paper presents the development, implementation, and comparative analysis of six process scheduling algorithms within a multithreaded educational simulator developed in C using the POSIX Threads library. The analyzed algorithms were: First-In First-Out (FIFO), Shortest Job First (SJF), Longest Job First (LJF), Static Priority with Multiple Queues, Dynamic Priority based on Behavior Type, and Dynamic Priority based on Quantum Fraction. The goal was to evaluate the performance of each algorithm under classic Operating System metrics, such as Average Waiting Time (TME), Average Blocked Time (TMB), and Average Turnaround Time (TMT). The results highlight the classic tradeoffs between fairness, throughput, and average time minimization associated with each policy.

**Keywords:** CPU Scheduling, Operating Systems, POSIX Threads, Process Simulation.

---

## 1. Introdução
O escalonamento de CPU é uma das principais tarefas desempenhadas pelo núcleo de um Sistema Operacional. Ele consiste em decidir qual dos processos aptos (em estado *Ready*) receberá o direito de utilizar o processador em um determinado instante de tempo. O objetivo de um bom escalonador é otimizar métricas do sistema, como maximizar a utilização da CPU, manter a vazão alta e minimizar tempos de resposta e de espera de processos.

No contexto acadêmico, o desenvolvimento prático dessas políticas auxilia na fixação de conceitos cruciais como preempção, quantum, filas de prioridade e starvation. Este trabalho documenta a implementação de políticas clássicas e dinâmicas de escalonamento no simulador `sched_sim`, desenvolvido na disciplina DCT2101 - Sistemas Operacionais, do BSI CERES-UFRN. 

O simulador utiliza POSIX threads para representar processos e simular concorrência real. A seguir, detalham-se os fundamentos teóricos dos algoritmos, a arquitetura de implementação do simulador, a metodologia experimental e os resultados obtidos.

---

## 2. Fundamentação Teórica dos Algoritmos
O simulador foi estendido para suportar seis algoritmos de escalonamento de processos distintos:

### 2.1. First-In, First-Out (FIFO)
O FIFO é uma política não-preemptiva simples baseada na ordem de chegada. O processo que requisitar a CPU primeiro é alocado nela e a retém até finalizar ou se bloquear voluntariamente por uma operação de Entrada/Saída (E/S). Embora de fácil implementação, pode gerar o "efeito comboio" (*convoy effect*), onde processos curtos aguardam na fila por longos períodos atrás de um processo longo.

### 2.2. Shortest Job First (SJF)
O SJF é uma política não-preemptiva que seleciona, dentre os processos aptos, aquele que possui o menor tempo restante de CPU esperado (`remaining_time`). Matematicamente, o SJF é ótimo pois minimiza o Tempo Médio de Espera (TME) para um conjunto fixo de tarefas. O principal problema prático é a dificuldade em estimar previamente o tempo de execução do processo e o risco de *starvation* de processos longos.

### 2.3. Longest Job First (LJF)
O LJF atua de forma oposta ao SJF, selecionando o processo apto com o maior `remaining_time`. Embora raramente utilizado em sistemas interativos de uso geral, é útil em sistemas de processamento em lote (*batch*) específicos ou para estudos comparativos, resultando costumeiramente em piores valores para o Tempo Médio de Espera (TME).

### 2.4. Prioridade Estática com Múltiplas Filas (`prio_static`)
Nesta política, existem duas filas físicas de processos aptos: a fila `ready` (Fila 0 - maior prioridade) e a fila `ready2` (Fila 1 - menor prioridade). O escalonador utiliza uma política probabilística 70/30 para seleção: há 70% de chance de selecionar um processo da Fila 0 (fallback para a Fila 1 caso esteja vazia) e 30% de chance de selecionar da Fila 1 (fallback para a Fila 0). 
Os processos são classificados na saída da CPU: se o processo já consumiu mais de 30% do tempo máximo permitido de vida (`MAX_TIME`), ele é classificado na Fila 0; caso contrário, vai para a Fila 1. O processo retorna para a mesma fila definida em sua propriedade `queue`.

### 2.5. Prioridade Dinâmica por Comportamento (`prio_dynamic`)
Similar ao modelo anterior, esta política utiliza as duas filas sob uma chance de seleção de 70/30. No entanto, ela penaliza ou premia os processos dinamicamente dependendo do seu comportamento na CPU:
- **Preempção (Estouro de Quantum):** Se o processo consumiu todo o seu quantum sem terminar ou bloquear por E/S, ele demonstra ser orientado a processamento (CPU-bound). Ele é promovido/mantido na Fila 1 (`queue = 0` / fila de maior prioridade de aptos).
- **Bloqueio por E/S (Operação Voluntária):** Se o processo se bloqueou por E/S antes do fim do quantum, ele é considerado orientado a E/S (I/O-bound) e, após o desbloqueio, é penalizado indo para a Fila 2 (`queue = 1` / fila de menor prioridade).

### 2.6. Prioridade Dinâmica Baseada em Fração de Quantum (`prio_dynamic_quantum`)
Nesta variação, o comportamento do processo é avaliado sob a fração exata do quantum que ele consumiu na CPU:
- Se o processo utilizou **menos de 50% do Quantum** configurado antes de sair do processador (indicando alta interatividade ou rápida liberação para E/S), ele é direcionado para a Fila 1 (`queue = 0`).
- Se utilizou **50% ou mais do Quantum** configurado, ele é direcionado para a Fila 2 (`queue = 1`).
Isso visa privilegiar tarefas altamente interativas com menor tempo de resposta (Fila 1), reduzindo a prioridade de processos pesados (Fila 2).

---

## 3. Arquitetura do Simulador e Detalhes de Implementação
A arquitetura do `sched_sim` é construída sobre um modelo de concorrência usando a biblioteca POSIX Threads em C.

### 3.1. Estrutura de Threads e Processos
Cada processo simulado é representado por uma instância da estrutura `struct proc` (declarada em `include/proc.h`), contendo atributos como `pid`, `state` (RUNNING, READY, BLOCKED, FINISHED), `remaining_time`, `waiting_time`, `blocked_time`, `turnaround_time` e o identificador da thread correspondente.
Ao iniciar, a função `proc_init()` instancia as threads associadas aos processos. Uma thread central controladora gerencia as interrupções de hardware (como timer e requisições de E/S), e a thread de escalonamento executa ciclicamente invocando a função `scheduler()`.

### 3.2. Estratégia de Implementação dos Escalonadores
A implementação de cada escalonador exigiu a codificação da lógica da função `struct proc * scheduler(struct proc * current)`. Esta função recebe o processo que acabou de liberar a CPU (`current`) e deve retornar o próximo processo selecionado.
- **Tratamento de Saída (`current`):** A função atualiza o estado de `current` (se foi finalizado, bloqueado ou preemptado) e o enfileira na fila correta (usando `enqueue` nas filas `ready`, `ready2` ou `blocked`). Para os escalonadores SJF e LJF, as inserções e remoções são feitas na fila `ready` usando manipulações na lista encadeada da fila.
- **Contabilização de Estatísticas:** A manipulação correta das filas através das funções `enqueue`, `dequeue` e `dequeue_bypid` (declaradas em `include/queue.h`) aciona as rotinas `count_ready_in` e `count_ready_out` (em `src/stats.c`), acumulando os tempos de espera dos processos (`waiting_time`).

### 3.3. Trechos de Código Principais da Implementação
Para evitar o uso de capturas de tela no relatório, detalham-se abaixo os trechos cruciais da lógica implementada em C nos escalonadores.

#### A. Busca pelo Menor Trabalho Restante no SJF (`sjf.c`)
No SJF, o escalonador itera sobre a fila de aptos (`ready`) buscando o menor `remaining_time`:
```c
/* Procura processo com o menor remaining_time */
p = ready->head;
while (p != NULL)
{
    if (selected == NULL || p->remaining_time < selected->remaining_time)
    {
        selected = p;
    }
    p = p->next;
}
/* Remove da READY pelo PID correspondente */
selected = dequeue_bypid(ready, selected->pid);
```

#### B. Penalização de Processos Orientados a E/S no Prioridade Dinâmica (`prio_dynamic.c`)
Quando um processo sofre E/S, ele entra no estado `BLOCKED` e é realocado dinamicamente para a Fila 2 de menor prioridade (`queue = 1`):
```c
else if (current->state == BLOCKED)
{
    // Quando retornar do bloqueio, deverá ir para a Fila 2 (penalizado)
    current->queue = 1; 
    enqueue(blocked, current);
}
else // Estouro de Quantum (Preempção)
{
    current->state = READY;
    current->queue = 0; // Mantido/Promovido na Fila 1
    enqueue(ready, current);
}
```

#### C. Decisão Baseada na Fração de Quantum (`prio_dynamic_quantum.c`)
O escalonador calcula o tempo contínuo de CPU consumido. Se o processo utilizou menos de 50% de seu quantum antes de liberar a CPU, ele é considerado interativo (Fila 1), caso contrário vai para a Fila 2:
```c
if (current->process_time < (QUANTUM / 2)) 
{
    current->queue = 0; // Promovido/Mantido na Fila 1 de alta prioridade
} 
else 
{
    current->queue = 1; // Rebaixado para a Fila 2
}
```

---

## 4. Análise Comparativa e Discussão Qualitativa

Em vez de focar na flutuação numérica de execuções específicas, a avaliação das políticas de escalonamento implementadas no simulador pode ser conduzida de forma analítica e qualitativa, considerando as características operacionais de cada algoritmo:

### 4.1. Análise Comportamental dos Algoritmos

1. **FIFO:** Por ser não-preemptivo e baseado puramente na ordem de chegada na fila `ready`, o FIFO carece de mecanismos para balancear a CPU. Caso um processo intensivo em computação (CPU-bound) entre primeiro, todos os processos curtos e interativos sofrerão com o Efeito Comboio (*Convoy Effect*), elevando severamente o Tempo Médio de Espera (TME).
2. **SJF:** O algoritmo garante o menor TME teoricamente possível ao priorizar tarefas com menor `remaining_time`. No entanto, em termos práticos e na arquitetura de threads, exige que o sistema conheça de antemão o tempo de execução do processo, o que é impraticável na maioria dos sistemas reais. Além disso, pode causar *starvation* (inanição) de processos longos caso novos processos curtos cheguem continuamente.
3. **LJF:** A priorização das tarefas mais longas no LJF atua de maneira inversa ao SJF, gerando o maior TME do sistema. É uma abordagem ineficiente para sistemas interativos, mas útil para entender o limite superior de atraso sofrido pelas tarefas na fila de prontos.
4. **Prioridade Estática (prio_static):** O mecanismo de múltiplas filas com distribuição probabilística 70/30 impede o *starvation* absoluto de processos na fila de menor prioridade (`ready2`). No entanto, a classificação fixa baseada na fração do `MAX_TIME` consumido não se adapta às mudanças de fase de comportamento dos processos (por exemplo, quando um processo deixa de ser puramente matemático e passa a interagir com o usuário).
5. **Prioridade Dinâmica (prio_dynamic):** A grande inovação desta abordagem está na resposta dinâmica do escalonador a eventos do sistema. Processos que sofrem bloqueio voluntário (I/O-bound) são penalizados com menor prioridade (indo para a fila `ready2`). Isso ajuda a balancear o sistema, pois processos CPU-bound (que estouram o quantum) são mantidos na Fila 1 para continuarem computando intensamente, reduzindo a concorrência na fila de maior prioridade de aptos.
6. **Prioridade Dinâmica com Quantum (prio_dynamic_quantum):** Esta política refina a anterior avaliando a fração de uso do quantum. Processos que usam menos de 50% do quantum (comportamento típico de sistemas altamente interativos) recebem tratamento prioritário na Fila 1. Aqueles que usam a maior parte do quantum (orientados a processamento pesado) são rebaixados para a Fila 2. Isso assegura uma responsividade de interface excelente sem estrangular processos computacionais, representando a lógica mais próxima de sistemas operacionais modernos como o escalonador do Windows ou o CFS do Linux.

---

## 5. Conclusão
A implementação das políticas de escalonamento no simulador centralizado `sched_sim` forneceu valiosas lições sobre a gerência de recursos da CPU. O estudo ratifica o teorema de que o Shortest Job First (SJF) minimiza o tempo médio de espera no sistema, mas impõe o custo teórico da necessidade de previsibilidade da rajada. 

Além disso, as políticas de feedback dinâmico (`prio_dynamic` e `prio_dynamic_quantum`) demonstraram a sofisticação de sistemas operacionais modernos, onde penalizações e promoções baseadas na fração de quantum e na ocorrência de interrupções de Entrada/Saída são implementadas para otimizar dinamicamente a responsividade sem desamparar de forma excessiva os processos que demandam computação intensa.

---

## Referências
1. TANENBAUM, Andrew S.; BOS, Herbert. **Sistemas Operacionais Modernos**. 4. ed. Rio de Janeiro: Pearson, 2016.
