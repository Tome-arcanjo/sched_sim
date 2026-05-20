# `sched_sim` - Simulador de Escalonamento de Processos

Simulador educacional de escalonamento de CPU baseado em **POSIX Threads** e desenvolvido em linguagem C para a disciplina **DCT2101 - Sistemas Operacionais** do Bacharelado em Sistemas de Informação (BSI) no Centro de Ensino Superior do Seridó (CERES), Universidade Federal do Rio Grande do Norte (UFRN).

O projeto simula a execução concorrente de múltiplos processos sob diferentes políticas de escalonamento, contabilizando estatísticas de tempo como tempo de espera, tempo de bloqueio e tempo de retorno (turnaround).

---

## 🚀 Algoritmos de Escalonamento Implementados

O simulador suporta seis algoritmos de escalonamento distintos:

1. **FIFO (First-In, First-Out):** Escalonamento não-preemptivo simples baseado na ordem de chegada na fila de prontos.
2. **SJF (Shortest Job First):** Política não-preemptiva que seleciona o processo com menor tempo restante estimado de CPU (`remaining_time`).
3. **LJF (Longest Job First):** Política não-preemptiva oposta ao SJF, priorizando os processos com maior tempo restante.
4. **PRIO STATIC (Prioridade Estática):** Escalonamento com duas filas e seleção probabilística de 70% para Fila 1 e 30% para Fila 2. O processo é alocado estaticamente em uma das filas de acordo com o tempo de vida total consumido.
5. **PRIO DYNAMIC (Prioridade Dinâmica por Comportamento):** Utiliza duas filas com distribuição probabilística 70/30. Processos CPU-bound que sofrem preempção (estouro de quantum) mantêm a alta prioridade (Fila 1), enquanto processos I/O-bound que entram em E/S são penalizados (rebaixados para a Fila 2).
6. **PRIO DYNAMIC QUANTUM (Prioridade Dinâmica por Fração de Quantum):** Utiliza duas filas com distribuição probabilística 70/30. Processos que utilizam menos de 50% de seu quantum antes de liberarem a CPU (interativos) permanecem na Fila 1 (alta prioridade), enquanto processos que usam 50% ou mais de seu quantum são rebaixados para a Fila 2.

---

## 🛠️ Compilação

Para compilar todos os escalonadores de uma vez:

```bash
make all
```

Para compilar um escalonador específico de forma individual:

```bash
make fifo
make sjf
make ljf
make prio_static
make prio_dynamic
make prio_dynamic_quantum
```

Para remover os arquivos de build temporários e limpar o diretório:

```bash
make clean
```

---

## 💻 Execução

Após a compilação, são gerados executáveis individuais para cada algoritmo no diretório raiz do projeto (ex: `main_fifo`, `main_sjf`, etc.).

Execute o simulador desejado passando os parâmetros necessários:

```bash
./main_fifo -n 10 -q 20
```

### Parâmetros Suportados:
* **`-n NPROC`**: Número de processos a serem criados (padrão: `10`, valor mínimo: `1`).
* **`-q QUANTUM`**: Valor do quantum de tempo em microssegundos (padrão: `20`, valor mínimo: `2`).
* **`-s SEED`**: Semente para fixar a geração de números pseudo-aleatórios (padrão: gerado a partir de `time(NULL) + PID`).
* **`-v`**: Ativa o modo *verbose* (debug), exibindo detalhes da fila de prontos e troca de contexto no terminal.
* **`-h`**: Exibe o menu de ajuda com a lista de parâmetros.

---

## 📁 Estrutura de Diretórios do Projeto

* `build/`: Diretório temporário gerado para armazenar os arquivos de objeto (`.o`). (Ignorado pelo git).
* `include/`: Cabeçalhos (`.h`) do simulador, contendo definições de estruturas (`proc.h`, `queue.h`), definições das rotinas centrais e dos escalonadores.
* `schedulers/`: Lógica central dos algoritmos de escalonamento. Cada arquivo `.c` implementa a função `scheduler()`.
* `src/`: Implementação das funções utilitárias e da infraestrutura de threads (`proc_init.c`, `queue.c`, `stats.c`, `thread.c`, etc.).
* `utils/`: Scripts utilitários de simulação e automação (ex: `run.sh`).
* `main.c`: Ponto de entrada do simulador.
* `Makefile`: Instruções para compilação.
