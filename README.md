# Pthreads-work

#### IMPLEMENTAÇÃO
O objetivo final do algoritmo é: dada uma matriz de números naturais aleatórios (intervalo 0 a 31999) contabilizar quantos
números primos existem e o tempo necessário para isso. No entanto, isso será feito de duas formas:
1. De modo serial, ou seja, a contagem dos primos será feita um a um, um após o outro. Esse será o seu tempo de
referência.
2. De modo paralelo. Para tanto, o trabalho de verificar cada número e se
for primo contabilizá-lo consistirá na subdivisão da matriz em
“macroblocos” (submatrizes), sem qualquer cópia, baseando-se apenas
nos índices.

Tome, como exemplo, uma matriz de 9 x 9. Cada macrobloco
é composto por 9 elementos (3 x 3). O macrobloco 1 vai da coluna 0 a 2
e da linha 0 a 2, e assim sucessivamente. Os macroblocos serão as
unidades de trabalho de cada thread (paralelismo de dados).
Atenção: Nem a matriz nem os macroblocos deverão ser
obrigatoriamente quadradas. A única exigência é que todos os
macroblocos tenham o mesmo tamanho. Além disso, você deve
encontrar alguma forma de PARAMETRIZAR essa divisão (usando a diretiva #define, por exemplo) a fim de poder
efetuar os testes para diferentes tamanhos de macroblocos. Os macroblocos terão tamanhos que podem variar de
desde um único elemento até a matriz toda (equivalente ao caso serial).

## My contact
- Email: [arthurdfr2000@gmail.com](mailto:arthurdfr2000@gmail.com)
- LinkedIn: [arthurdfr](https://linkedin.com/in/arthurdfr)
