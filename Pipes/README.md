## Pipes
---
Программа pipes: pipes имена программ
./pipes pr1 pr2  ... prN

Запустить параллельно программы pr1 ... prN (fork,execlp), связать их
каналами (pipe, dup2). Т.е. получить аналог в shell:
pr1 | pr2 | ... | prN
