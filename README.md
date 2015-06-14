# LinuxFileSystemBenchmark
A benchmark to test different linux writing speeds

## Tarefa
Programa de teste de desempenho (benchmark) para arquivos

De maneira simplificada, um sistema de arquivos é uma organização provida pelo sistema operacional para o armazenamento de conjuntos de informações de forma não volátil. Para tanto, um sistema de arquivo trata de como utilizar os blocos de armazenamento providos pelos discos para o armazenamento de programas e dados, organizados na forma de uma hierarquia de diretórios e arquivos.

Diferentes atributos e informações de controle podem estar associados aos arquivos e diferentes mecanismos podem ser utilizados para identificar os blocos do disco que contêm seus dados.

Considerando o uso de chamadas de sistema padronizadas para o acesso aos arquivos pelas aplicações, cabe ao SO traduzir essas chamadas em comandos apropriados para os controladores de disco. Também cabe ao SO compatibilizar os tamanhos das requisições feitas com os tamanhos dos blocos de armazenamento que podem ser lidos ou escritos no discos, limitados a esse tipo de transferência.

Assim, a eficiência de um sistema de arquivos depende de diversos fatores, que vão da distribuição física dos blocos de dados de um arquivo em disco, passando pelas estruturas de controle usadas para identificação dos blocos de um arquivo, e chegando aos mecanismos de transferência e armazenamento intermediário providos pelo SO. 

O padrão de acesso aos dados também é relevante no acesso a um arquivo. Acessos sequenciais podem ser beneficiados por mecanismos de busca antecipada (prefetching), enquanto acessos aleatórios podem favorecer-se de mecanismos de cache. 

Para avaliar a eficiência de sistemas de arquivo, diferentes estudos e programas de teste de desempenho (benchmarks) têm sido criados ao longo do tempo, como pode ser visto em [1]. Uma discussão sobre diferentes aspectos que podem ser avaliados por um benchmark de sistema de arquivos pode ser vista em [2].

Problema

Criar um programa benchmark simplificado para sistema de arquivos, que analise o desempenho de diferentes operações:

leitura sequencial com read
leitura reversa com read (do fim para o início do arquivo) - lseek + read
leitura fragmentada (read N, skip k)
leitura aleatória com lseek + read
escrita sequencial com write
escrita aleatória com lseek + write
Para avaliar o efeito do tamanho dos blocos dos arquivos em disco, também devem ser testados diferentes tamanhos de requisição, variando de 1 a 64k bytes (2^0 a 2^16). Para tentar evitar que o uso de caches do sistema de arquivos mascare os resultados, os arquivos utilizados nos testes devem ter ao menos o dobro do tamanho da memória. Assim, número_de requisições X tamanho_da_requisição >= 2 * tamanho_da_RAM.

Entrega

Para avaliar o trabalho desenvolvido, é preciso entregar o código fonte do programa comentado e um gráfico com resultados obtidos.

Para a execução dos testes, é preciso executar ao menos 10 vezes cada avaliação, plotando-se a média com indicação do desvio, para os diferentes tamanhos de requisição.

Também é relevante incluir uma descrição do computador (CPU, memória, disco, SO e sistema de arquivos) usados no teste.
