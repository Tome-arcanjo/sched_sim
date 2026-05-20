#!/bin/bash

# Script para rodar experimentos do simulador de escalonamento
# e gerar uma tabela comparativa com TME, TMB e TMT médios.

echo "=== Iniciando Experimentos de Escalonamento ==="

# Compilando o projeto
echo "Limpando e compilando executáveis..."
make clean
mkdir -p build
make all

if [ $? -ne 0 ]; then
    echo "Erro na compilação. Certifique-se de estar em um terminal com gcc e make configurados (ex: WSL ou Git Bash)."
    exit 1
fi

schedulers=("fifo" "sjf" "ljf" "prio_static" "prio_dynamic" "prio_dynamic_quantum")
NPROC=50
QUANTUM=20
RUNS=10

echo ""
echo "Rodando cada escalonador $RUNS vezes com NPROC=$NPROC e QUANTUM=$QUANTUM..."
echo "| Algoritmo | TME Médio (us) | TMB Médio (us) | TMT Médio (us) |"
echo "|-----------|----------------|----------------|----------------|"

for sched in "${schedulers[@]}"
do
    exec_name="./main_$sched"
    if [ ! -f "$exec_name" ]; then
        echo "Executável $exec_name não encontrado!"
        continue
    fi
    
    total_tme=0
    total_tmb=0
    total_tmt=0
    
    for i in $(seq $RUNS)
    do
        # Roda o executável e captura as saídas do TME, TMB e TMT
        output=$($exec_name -n $NPROC -q $QUANTUM 2>/dev/null)
        
        tme=$(echo "$output" | grep "TME:" | awk '{print $2}')
        tmb=$(echo "$output" | grep "TMB:" | awk '{print $2}')
        tmt=$(echo "$output" | grep "TMT:" | awk '{print $2}')
        
        # Fallback para 0 caso alguma variável esteja vazia (evita erros no awk)
        [ -z "$tme" ] && tme=0
        [ -z "$tmb" ] && tmb=0
        [ -z "$tmt" ] && tmt=0
        
        # Acumula os valores (usando awk para somas decimais)
        total_tme=$(awk "BEGIN {print $total_tme + $tme}")
        total_tmb=$(awk "BEGIN {print $total_tmb + $tmb}")
        total_tmt=$(awk "BEGIN {print $total_tmt + $tmt}")
    done
    
    # Calcula as médias
    avg_tme=$(awk "BEGIN {print $total_tme / $RUNS}")
    avg_tmb=$(awk "BEGIN {print $total_tmb / $RUNS}")
    avg_tmt=$(awk "BEGIN {print $total_tmt / $RUNS}")
    
    printf "| %-20s | %-14.2f | %-14.2f | %-14.2f |\n" "$sched" "$avg_tme" "$avg_tmb" "$avg_tmt"

    # Atualiza o arquivo markdown automaticamente
    if [ -f "relatorio_sbc_escalonadores.md" ]; then
        case "$sched" in
            "fifo") label="FIFO" ;;
            "sjf") label="SJF" ;;
            "ljf") label="LJF" ;;
            "prio_static") label="Static" ;;
            "prio_dynamic") label="Dynamic" ;;
            "prio_dynamic_quantum") label="Quantum" ;;
        esac
        
        # Formata com duas casas decimais
        formatted_tme=$(printf "%.2f" "$avg_tme")
        formatted_tmb=$(printf "%.2f" "$avg_tmb")
        formatted_tmt=$(printf "%.2f" "$avg_tmt")

        sed -i "s/\[Inserir TME do $label\]/$formatted_tme/g" relatorio_sbc_escalonadores.md
        sed -i "s/\[Inserir TMB do $label\]/$formatted_tmb/g" relatorio_sbc_escalonadores.md
        sed -i "s/\[Inserir TMT do $label\]/$formatted_tmt/g" relatorio_sbc_escalonadores.md
    fi
done

echo ""
echo "O arquivo 'relatorio_sbc_escalonadores.md' foi atualizado automaticamente com as métricas!"
echo "=== Experimentos Concluídos ==="
