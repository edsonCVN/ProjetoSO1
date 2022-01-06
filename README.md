 # ProjectoSO1
29/12/2021

    verificar funcionamento correto das diretorias

    funções a verficar thread-safety:
        -inode_create
        -find_in_dir
        -inode_get
        -add_dir_entry
        -inode_delete
        -add_to_open_file_table
        -remove_from_open_file_table
        -get_open_file_entry
        -data_block_alloc
        -data_block_get
        -touch_all_memory (->insert_delay)
        (FIQUEI NA LINHA 177 do state.c)

    ver condições de erro mal feitas no read

    PÔR DELETE DE DIRETORIAS A FUNCIONAR! (inode_delete) (perguntar ao stor se é código lixo)


obs: podiamos fazer trincos mais facies (i.e. mais globais) e certificarmo-nos que funcionavam. depois, começacamos a ser mais minuciosos e iamos alterando e testando de forma incremental


tfs_open Trucate
    1) não está preparado para truncar ficheiro que utilze somente 4 blocos direto (à 5ºa referencia na iteração ele vai injetar -1 (pq esta ref. não esstá a ser utiizada) no data_block_free() e levanta um erro desnecessário)
    2) não está preparado para aceder ao bloco indireto.


perguntar ao stor na 5a: 
    -como aceder/utilizar os valores de retorno do pthread_join;
    -pedir opinião sobre a granulidade dos nossos trincos;
    -copy_to_external: como aplicar trincos de forma a só dar lock de escrita quando a segunda thread tiver o mesmo dest_path e de forma a que uma segunda thread não consiga alterar o source_path;
    -necessidade/importância de pôr o tfs_destroy thread-safe
    

ATUALIZAR AS ASSINATURAS DO .h

deveriamos criar funcções para inicializar as estruturas de parametros.. (para evitar que o esqeucimento faça com que código lixo se propague)



resumo do horário de dúvidas:
    cumprir api no enunciado dentro do operations.c +  criar funções intermédias que utilizem as estruturas (DONE!)

    usar trincos para as estruturas partilhadas do state.c

    testar tfs open com + de 5 threads (DONE!)

    criar função que dá um novo trinco (mutex) a cada dest_path diferente
        a utlizar no início do "do"  da função copy_to_external

    tfs_open: usar trinco na verificação e operação (criar igual verificação para cada operação para aumentar paralelismo)

