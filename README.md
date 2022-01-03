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