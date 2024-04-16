## Laboratório 3 da Disciplina: Programação Concorrente, no período 2024.1 

### Contexto:
Este programa concorrente consiste na multiplicação de matrizes A e B de dimensões NxM, tal que N > 0 e M > 0, seguindo a restrição de que o número de linhas da matriz A deva ser igual ao número de colunas da MatrizB.

### Execução:
Neste repositório há duas versões do programa, uma sequencial ```produtoMatrizesSequencial.c```e uma concorrente, ```main.c```, sendo a concorrente o programa principal, e a sequencial, o programa para avaliar corretude dos resultados.
Também há arquivos auxiliares disponibilizados no laboratório, como o ```geraMatrizBinario.c```e o ```timer.h```.
- Para gerar matrizes float binárias, rode ```gcc -o geraMatrizBinario geraMatrizBinario.c```, gere as matrizes de entrada do seu programa para execução do produto das matrizes. Execute ```gcc -o geraMatrizBinario N M <nomeArquivoSaida>```, onde N e M são valores escolhidos.
- Para executar o programa concorrente principal rode ```gcc -o main main.c -lpthread -Wall -DGERAR_CSV=OFF``` em seguida, rode ```./main. <nomeArquivoMatrizA> <nomeArquivoMatrizB> <nomeArquivoResultante>```, onde os dois primeiros argumentos são matrizes geradas em um arquivo binário e o terceiro é o nome do arquivo binário em que deseja armazenar o resultado.
- Ao compilar, em caso de ```-DGERAR_CSV=ON``` o programa irá calcular o tempo sequencial, tempo concorrente, aceleração, eficiência, e por fim montar um csv para auxiliar na criação dos gráficos.
- Para executar a versão sequencial separada da concorrente, basta compilar a versão sequencial com ```gcc -o produtoMatrizesSequencial produtoMatrizesSequencial.c``` e rode ```./main. <nomeArquivoMatrizA> <nomeArquivoMatrizB> <nomeArquivoResultante>```
