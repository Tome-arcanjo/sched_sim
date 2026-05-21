import re
import numpy as np
import csv

dados = {}

print("Processando o arquivo de dados brutos...")

# Abre o arquivo gerado pelo terminal
with open("dados_brutos.txt", "r") as f:
    linha_tag = None
    for linha in f:
        if "TAG:" in linha:
            linha_tag = linha.strip()
        elif "TME:" in linha and linha_tag:
            # Captura os parâmetros e o valor do TME correspondente
            match_tag = re.search(r"TAG:\s+(\w+)\s+\|\s+Q:\s+(\d+)\s+\|\s+N:\s+(\d+)", linha_tag)
            match_tme = re.search(r"TME:\s+([\d.]+)", linha)
            
            if match_tag and match_tme:
                alg = match_tag.group(1).upper()
                q = int(match_tag.group(2))
                n = int(match_tag.group(3))
                tme = float(match_tme.group(1))
                
                chave = (alg, q, n)
                if chave not in dados:
                    dados[chave] = []
                dados[chave].append(tme)

# Cria e estrutura o CSV final da atividade
with open("resultados_consolidados.csv", mode="w", newline="") as file:
    writer = csv.writer(file)
    writer.writerow(["Algoritmo", "Quantum", "NPROC", "TME_Media", "TME_Desvio_Padrao"])
    
    for (alg, q, n), lista_tme in dados.items():
        if len(lista_tme) >= 30:
            # Garante a análise sobre as 30 rodadas padrão
            amostras = lista_tme[:30]
            media = np.mean(amostras)
            desvio = np.std(amostras, ddof=1) # ddof=1 define o desvio padrão amostral exigido
            
            writer.writerow([alg, q, n, round(media, 4), round(desvio, 4)])

print("Concluído! O arquivo 'resultados_consolidados.csv' foi gerado.")