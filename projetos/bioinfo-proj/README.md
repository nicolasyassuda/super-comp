### Relatório de Desempenho de HPC para Análise de Sequências Genéticas de DNA

#### Objetivo
O objetivo deste experimento foi desenvolver um sistema paralelo para análise de grandes sequências de DNA, focando na contagem de bases nucleotídicas, transcrição de DNA em RNA, e tradução de mRNA em aminoácidos, utilizando técnicas de programação paralela com MPI e OpenMP. O foco é otimizar o tempo de processamento em grandes arquivos genômicos, como o arquivo FASTA do genoma humano.

#### Metodologia
O sistema foi implementado utilizando o modelo de programação paralela MPI para distribuir as sequências de DNA entre os diferentes processos. OpenMP foi utilizado para paralelizar os loops de contagem de bases e substituição de bases durante a transcrição de DNA para RNA. Três arquivos FASTA (chr1.subst.fa, chr2.subst.fa, e chr3.subst.fa) foram analisados.

#### Resultados de Desempenho

**1. Tempo de Transcrição de DNA para RNA:**
   - Para o arquivo `chr1.subst.fa`, o tempo variou de 4.2788 segundos a 4.63066 segundos entre os diferentes processos.
   - Para `chr2.subst.fa`, os tempos variaram entre 0.395988 segundos e 0.413826 segundos.
   - Para `chr3.subst.fa`, os tempos variaram entre 0.334936 segundos e 0.338475 segundos.
   A transcrição foi realizada de forma eficiente, com variações mínimas de tempo entre os processos, sugerindo um bom balanceamento de carga.

**2. Contagem de Códons de Início (AUG):**
   - Para `chr1.subst.fa`, o tempo variou de 0.172529 segundos a 0.199625 segundos.
   - Para `chr2.subst.fa`, os tempos variaram entre 0.183105 segundos e 0.195079 segundos.
   - Para `chr3.subst.fa`, os tempos variaram entre 0.151334 segundos e 0.158621 segundos.
   A contagem dos códons de início foi rápida e balanceada, com tempos de execução consistentes entre os processos.

**3. Tradução de RNA para Aminoácidos:**
   - Para `chr1.subst.fa`, o tempo variou de 4.22569 segundos a 4.42261 segundos.
   - Para `chr2.subst.fa`, os tempos variaram entre 4.17974 segundos e 4.18801 segundos.
   - Para `chr3.subst.fa`, os tempos variaram entre 3.39629 segundos e 3.44336 segundos.
   A tradução para aminoácidos apresentou desempenho consistente entre os diferentes arquivos, com tempos ligeiramente menores para os arquivos menores.

**4. Total de Proteínas Inicializadas (AUG Encontrados):**
   - Para `chr1.subst.fa`, foram encontradas 2.063.058 proteínas.
   - Para `chr2.subst.fa`, o número foi 2.241.823 proteínas.
   - Para `chr3.subst.fa`, foram encontradas 1.868.427 proteínas.

#### Análise de Desempenho
Os tempos de execução para as operações de transcrição, contagem de códons e tradução de RNA foram consistentes, com pequenas variações entre os processos. A utilização de MPI permitiu uma distribuição eficiente das tarefas entre os processos, enquanto o OpenMP otimizou os loops paralelos dentro de cada processo. Pode-se perceber que muito das variações foram baseadas no tamanho medio dos arquivos onde podemos ter uma noção com base no numero de Proteinas iniciais. Porém os resultados das transcrições não forma tão precisos visando que temos muitos pares de bases invalidos como x,y,N entre outros para um cenário perfeito de analise teriamos que testar todas as opções possiveis com as bases invalidas, precisariamos também separar os arquivos de forma que o numero de letras validas fosse de base 3 para que pudessemos fazer uma base valida sem que cortasse nenhuma proteina pela metade.

Observamos uma redução no tempo de execução à medida que os arquivos se tornaram menores (de `chr1.subst.fa` para `chr3.subst.fa`), o que pode ser atribuído à diminuição do volume de dados a serem processados. A analise em questão foi feita com apenas 3 arquivos só para pudessemos observar o caso em geral porém seria mais preciso se a gente analisasse com todos os arquivos (que estão comentados no codigo).

#### Conclusão
A implementação paralela utilizando MPI e OpenMP mostrou-se eficaz na análise de grandes sequências de DNA, com tempos de execução otimizados para as tarefas de transcrição e tradução. A utilização de técnicas paralelas permitiu lidar com grandes volumes de dados de maneira eficiente, essencial para a análise de sequências genéticas de tamanho considerável, como o genoma humano, por exemplo até uma implementação simples como está é extremamente pesada (demorada) para ser executada de forma sequencial ou até mesma parelela local, em casos mais reais como citado no Análise de Desempenho esse cenário pioraria exponencialmente. A experiência também destaca a importância de balanceamento de carga e de otimização no processo de computação paralela para garantir um desempenho ideal em sistemas distribuídos.