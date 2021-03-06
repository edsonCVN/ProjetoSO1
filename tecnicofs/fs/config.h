/*                      
 *                     Projeto de Sistemas Operativos 2021-22
 *                         1º exercício LEIC-A/LEIC-T/LETI 
 *                        
 *                                Grupo 17 - LETI
 *                    Pedro Alexandre Delgado Claro ist198960
 *                   Edson Fernando Cabral da Veiga ist1100731
 * 
 */

#ifndef CONFIG_H
#define CONFIG_H

/* FS root inode number */
#define ROOT_DIR_INUM (0)

#define BLOCK_SIZE (1024)
#define DATA_BLOCKS (1024)
#define INODE_TABLE_SIZE (50)
#define MAX_OPEN_FILES (20)
#define MAX_FILE_NAME (40)
#define INODE_BLOCKS_SIZE (11)
#define INDIRECT_BLOCK_INDEX (10)
#define FILE_MAXSIZE (272384)

#define DELAY (5000)

#endif // CONFIG_H
