Projeto de Sistemas Operativos 2021-22
Enunciado do 1o exercício LEIC-A/LEIC-T/LETI

Interface de programação

O TecnicoFS oferece uma interface de programação (API) inspirada na API de sistema de ficheiros POSIX. No entanto, para simplificar o projeto, a API do TecnicoFS oferece apenas um subconjunto de funções com interface simplificada. São elas1:
● int tfs_open(const char *name, int flags);
● int tfs_close(int fhandle);
● ssize_t tfs_write(int fhandle, const void *buffer, size_t len);
● ssize_t tfs_read(int fhandle, void *buffer, size_t len);
Além destas funções, existem as funções de inicialização e destruição do sistema de ficheiros, tfs_init e tfs_destroy.
