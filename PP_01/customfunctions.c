/* Arquivo com a implementação de todas as funções definidas em customfunctions.h
 *
 */
#include "customfunctions.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

FileDirManagement fd_management;
long SIZE_BUFFER;
char *PROGRAM_NAME;
long COUNT_DIR, COUNT_FILES, COUNT_BYTES_SIZE;

//Função que exibe as mensagens amigáveis caso um erro ocorra ou quando o processo é realizado com sucesso.
void print_errors(enum result_types_ref typesRef) {

    switch (typesRef) {
        case INVALID_PARAMS:
            fprintf(stderr, "Parâmetros inválidos!\n"
                           "Informe os nomes dos arquivos de origem e destino.\n"
                           "Ex: ./filecopy <arquivo_origem.txt> <arquivo_destino.txt>\n"
                           "Obs.: Se o arquivo conter espaço, utilize aspas duplas para selecioná-lo.\n"
                           "Ex.: ./filecopy <\"Arquivo de origem.txt\"> <\"Arquivo de destino.txt\">\n");
            break;
        case INVALID_PARAM_DIR:
            fprintf(stderr, "Parâmetros inválidos!\n"
                   "Informe os nomes dos diretórios de origem e destino.\n"
                   "Ex: ./treecopy <DiretorioOrigem> <DiretorioDestino>\n"
                   "Obs.: Se o diretório conter espaço, utilize aspas duplas para selecioná-lo.\n"
                   "Ex.: ./treecopy <\"Diretório de origem.txt\"> <\"Diretório de destino.txt\">\n");
            break;
        case WRITE_ERROR:
            fprintf(stderr, "%s: Ocorreu um erro na escrita do arquivo: %s\n", PROGRAM_NAME, fd_management.destination);
            messageError(errno);
            break;
        case OPEN_ERROR:
            fprintf(stderr, "%s: Ocorreu um problema na leitura do arquivo %s\n", PROGRAM_NAME, fd_management.source);
            messageError(errno);
            break;
        case CREATE_DIR_ERROR:
            fprintf(stderr, "%s: Ocorreu um problema ao tentar criar o diretório %s: ", PROGRAM_NAME, fd_management.destination);
            messageError(errno);
            break;
        case OPEN_DIR_ERROR:
            fprintf(stderr, "%s: Ocorreu um problema ao tentar abrir o diretório %s: ", PROGRAM_NAME, fd_management.source);
            messageError(errno);
            break;
        case WRITE_DIR_ERROR:
            fprintf(stderr, "%s: Ocorreu um problema ao tentar criar o diretório %s: ", PROGRAM_NAME, fd_management.destination);
            messageError(errno);
            break;
        default:
            messageError(errno);
            exit(EXIT_FAILURE);
    }
}

/* Função que conta o número de bites de um dado arquivo.
 * @source: arquivo que será lido
 * @buffer: --
 * */
int countFileSize(char *source, char *buffer) {
    buffer = (char *) calloc(SIZE_BUFFER, sizeof(char));
    int file_to_read = open(source, O_RDONLY);
    int size_bytes = read(file_to_read, buffer, SIZE_BUFFER);
    close(file_to_read);
    return size_bytes;
}

/* Função que verifica a leitura de um dado arquivo. Caso ocorra um problema, o programa é finalizado
 * @file: identificador do arquivo que será aberto.
 * @source: caminho ou arquivo que será aberto.
 */
int checkFileRead(int file, char *source) {
    if ((file = open(source, O_RDONLY)) == -1) {
//        printf("Erro: %d\n", errno);
//        printf("%s: não foi possível abrir o arquivo %s\n", PROGRAM_NAME, source);
//        perror("causa");
        print_errors(OPEN_ERROR);
        exit(EXIT_FAILURE);
    }
    return file;
}

/* Função que verifica se um dado arquivo possui permissão de escrita. Caso ocorra um problema, o programa é finalizado
 * @file: identificador do arquivo que será aberto.
 * @source: caminho ou arquivo que será aberto.
 */
int checkFileWrite(int file, char *source) {
    if ((file = open(source, O_CREAT | O_WRONLY | O_TRUNC, 0700)) == 1) {
        print_errors(WRITE_ERROR);
        exit(EXIT_FAILURE);
    }
    return file;
}

/* Função que realiza a cópia do conteúdo de um arquivo de origem para um arquivo de destino.
 * @source_file: arquivo que terá o conteúdo copiado (origem).
 * @destination_file: arquivo de destino.
 */
void writeFile(int source_file, int destination_file, char *buffer) {
    int n;
    while ((n = read(source_file, buffer, SIZE_BUFFER)) > 0)
        /* Caso o número de bytes retornado pela função write for diferente de n (quantidade de bytes),
         * o programa é encerrado e uma mesagem com o erro é exibida para o usuário (função print_errors)
         */
        if (write(destination_file, buffer, n) != n) {
            print_errors(WRITE_ERROR);
            exit(EXIT_FAILURE);
        }
    if (n == -1) {
        print_errors(OPEN_ERROR);
        exit(EXIT_FAILURE);
    }
}

/* Função que verifica os arquivos existentes de um dado diretório de origem para copiá-los em um diretório de destino
 * @source: Diretório de origem
 * @destination: Diretório de destino
 */
void checkDir(char *source, char *destination) {
    struct dirent *entry;
    int file_source, file_destination; // Variáveis que receberão os arquivos que serão utilizados

    // Guarda o tamanho do nome dos diretórios
    unsigned long len_source = strlen(source);
    unsigned long len_destination = strlen(destination);

    /* Cria as variáveis temporárias */
    char *source_tmp = calloc(len_source + 1, sizeof(source));
    char *destination_tmp = calloc(len_destination + 1, sizeof(destination));

    /* Faz a cópia dos nomes dos diretórios de origem e destino para as variáveis temporárias */
    strcpy(source_tmp, source);
    strcpy(destination_tmp, destination);

    DIR *dir_source = opendir(source);
    DIR *dir_destination = opendir(destination);

    /* Verifica se o diretório de origem é nulo, caso verdade, apresenta um erro */
    if (dir_source == NULL) {
        closedir(dir_source);
        print_errors(OPEN_DIR_ERROR);
        exit(EXIT_FAILURE);
    }
    /* Verifica se o diretório de destino existe, caso verdade, apresenta um erro */
    if (dir_destination != NULL) {
        print_errors(WRITE_DIR_ERROR);
        exit(EXIT_FAILURE);
    } else {
        int r = mkdir(destination, 0777); // Cria um diretório com as todas as permissões
        if (r == -1) { // Se houver algum problema na criação do diretório, apresenta um erro.
            print_errors(CREATE_DIR_ERROR);
            exit(EXIT_FAILURE);
        }
    }

    // Enquanto possuir conteúdo dentro do diretório, o laço é verdadeiro
    while ((entry = readdir(dir_source)) != NULL) {
        char *name = entry->d_name; // Recebe o nome do diretório/arquivo atual
        int dType = entry->d_type; // Recebe o tipo de entrada (diretório, arquivo ou qualquer outra coisa)
        char *buffer = (char *) calloc(SIZE_BUFFER, sizeof(char));
        if (!(strcmp(name, ".")) || !strcmp(name, "..")) // Desconsidera o diretório atual e o anterior
            continue;
        if (dType == DT_DIR) { //é diretório
            COUNT_DIR++;
            //Monta os caminhos dos novos diretórios
            source_tmp[len_source] = '/';
            destination_tmp[len_destination] = '/';

            strcpy(source_tmp + len_source + 1, name);
            strcpy(destination_tmp + len_destination + 1, name);

            // Após definir os caminhos na etapa anterior, entra de forma recursiva para copiar os conteúdos.
            checkDir(source_tmp, destination_tmp);

            // Volta para o diretório anterior, removendo as informações do diretório atual.
            source_tmp[len_source] = '\0';
            destination_tmp[len_destination] = '\0';

        } else if (dType == DT_REG) { //é um arquivo regular
            COUNT_FILES++;

            //Monta os caminhos dos arquivos
            source_tmp[len_source] = '/';
            destination_tmp[len_destination] = '/';

            strcpy(source_tmp + len_source + 1, name);
            strcpy(destination_tmp + len_destination + 1, name);

            //Verifica se é possível realizar a leitura do arquivo de origem e a escrita no arquivo de destino
            file_source = checkFileRead(file_source, source_tmp);
            file_destination = checkFileWrite(file_destination, destination_tmp);
            /* caso a etapa anterior não apresente problemas, escreve o conteúdo do arquivo de origem no arquivo de destino */
            writeFile(file_source, file_destination, buffer);
            COUNT_BYTES_SIZE += countFileSize(source_tmp, buffer);
            printf("copiando %s -> %s\n", source_tmp, destination_tmp);
        } else { // Caso o tipo de entrada não seja esperada (arquivo ou diretório), finaliza a execução do programa.
            printf("Não é nem diretório nem arquivo: %s/%s", source, name);
            exit(EXIT_FAILURE);
        }
    }
    closedir(dir_source);
    closedir(dir_destination);
}

/*
A partir do código de erro, retorna uma mensagem amigável

*/
void messageError (int cod){
    struct error_handle arr_erros[133] = {
            {1,"Operation not permitted", "Operação não permitida","EPERM"},
            {2,"No such file or directory", "Não existe esse arquivo ou diretório","ENOENT"},
            {3,"No such process", "Esse processo não existe","ESRCH"},
            {4,"Interrupted system call", "Chamada de sistema interrompida","EINTR"},
            {5,"I/O error", "Erro de E / S","EIO"},
            {6,"No such device or address", "Não existe tal dispositivo ou endereço","ENXIO"},
            {7,"Argument list too long", "Lista de argumentos muito longa","E2BIG"},
            {8,"Exec format error", "Erro de formato de execução","ENOEXEC"},
            {9,"Bad file number", "Número de arquivo inválido","EBADF"},
            {10,"No child processes", "Nenhum processo filho","ECHILD"},
            {11,"Try again", "Tente novamente","EAGAIN"},
            {12,"Out of memory", "Sem memória","ENOMEM"},
            {13,"Permission denied", "Permissão negada","EACCES"},
            {14,"Bad address", "Endereço incorreto","EFAULT"},
            {15,"Block device required", "Dispositivo de bloqueio necessário","ENOTBLK"},
            {16,"Device or resource busy", "Dispositivo ou recurso ocupado","EBUSY"},
            {17,"File exists", "Arquivo existe","EEXIST"},
            {18,"Cross-device link", "Link entre dispositivos","EXDEV"},
            {19,"No such device", "Dispositivo inexistente","ENODEV"},
            {20,"Not a directory", "Não é um diretório","ENOTDIR"},
            {21,"Is a directory", "É um diretório","EISDIR"},
            {22,"Invalid argument", "Argumento inválido","EINVAL"},
            {23,"File table overflow", "Estouro de tabela de arquivo","ENFILE"},
            {24,"Too many open files", "Muitos arquivos abertos","EMFILE"},
            {25,"Not a typewriter", "Não é uma máquina de escrever","ENOTTY"},
            {26,"Text file busy", "Arquivo de texto ocupado","ETXTBSY"},
            {27,"File too large", "Arquivo muito grande","EFBIG"},
            {28,"No space left on device", "Sem espaço no dispositivo","ENOSPC"},
            {29,"Illegal seek", "Busca ilegal","ESPIPE"},
            {30,"Read-only file system", "Sistema de arquivos somente leitura","EROFS"},
            {31,"Too many links", "Muitos links","EMLINK"},
            {32,"Broken pipe", "Tubo quebrado","EPIPE"},
            {33,"Math argument out of domain of func", "Argumento matemático fora do domínio da função","EDOM"},
            {34,"Math result not representable", "Resultado matemático não representável","ERANGE"},
            {35,"Resource deadlock would occur", "Ocorreria impasse de recursos","EDEADLK"},
            {36,"File name too long", "Nome de arquivo muito longo","ENAMETOOLONG"},
            {37,"No record locks available", "Nenhum bloqueio de registro disponível","ENOLCK"},
            {38,"Invalid system call number", "Número da chamada de Sistema inválida","ENOSYS"},
            {39,"Directory not empty", "O diretório não está vazio","ENOTEMPTY"},
            {40,"Too many symbolic links encountered", "Vários links simbólicos encontrados","ELOOP"},
            {41,"Operation would block", "A operação deveria ser bloqueada","EWOULDBLOCK EAGAIN"},
            {42,"No message of desired type", "Nenhuma mensagem do tipo desejado","ENOMSG"},
            {43,"Identifier removed", "Identificador removido","EIDRM"},
            {44,"Channel number out of range", "Número do canal fora do alcance","ECHRNG"},
            {45,"Level 2 not synchronized", "Nível 2 não sincronizado","EL2NSYNC"},
            {46,"Level 3 halted", "Nível 3 travado","EL3HLT"},
            {47,"Level 3 reset", "Nível 3 redefinido","EL3RST"},
            {48,"Link number out of range", "Número do link fora do intervalo","ELNRNG"},
            {49,"Protocol driver not attached", "Driver do protocolo não anexado","EUNATCH"},
            {50,"No CSI structure available", "Estrutura CSI indisponível","ENOCSI"},
            {51,"Level 2 halted", "Nível 2 travado","EL2HLT"},
            {52,"Invalid exchange", "Troca inválida","EBADE"},
            {53,"Invalid request descriptor", "Descritor de solicitação inválido","EBADR"},
            {54,"Exchange full", "Troca cheia","EXFULL"},
            {55,"No anode", "Sem �nodo","ENOANO"},
            {56,"Invalid request code", "Código de requisição inválido","EBADRQC"},
            {57,"Invalid slot", "Slot inválido","EBADSLT"},
            {58,"", "","EDEADLOCK EDEADLK"},
            {59,"Bad font file format", "Formato de arquivo incompatível","EBFONT"},
            {60,"Device not a stream", "O dispositivo não é um stream","ENOSTR"},
            {61,"No data available", "Nenhum dado disponível","ENODATA"},
            {62,"Timer expired", "O tempo expirou","ETIME"},
            {63,"Out of streams resources", "Recursos fora do stream","ENOSR"},
            {64,"Machine is not on the network", "A máquina não está na rede","ENONET"},
            {65,"Package not installed", "Pacote não instalado","ENOPKG"},
            {66,"Object is remote", "Objeto remoto","EREMOTE"},
            {67,"Link has been severed", "O link foi cortado","ENOLINK"},
            {68,"Advertise error", "Erro de propaganda","EADV"},
            {69,"Srmount error", "Erro de montagem","ESRMNT"},
            {70,"Communication error on send", "Erro ao enviar comunicação","ECOMM"},
            {71,"Protocol error", "Erro de protocolo","EPROTO"},
            {72,"Multihop attempted", "Tentativa de multihop","EMULTIHOP"},
            {73,"RFS specific error", "Erro específico de RFS","EDOTDOT"},
            {74,"Not a data message", "Não é uma mensagem de dados","EBADMSG"},
            {75,"Value too large for defined data type", "O valor é muito grande para o tipo de dado definido","EOVERFLOW"},
            {76,"Name not unique on network", "O nome não é exclusivo na rede","ENOTUNIQ"},
            {77,"File descriptor in bad state", "Descritor de arquivos incompatível","EBADFD"},
            {78,"Remote address changed", "Endereço remoto alterado","EREMCHG"},
            {79,"Can not access a needed shared library", "Não foi possível acessar uma biblioteca necessária compartilhada","ELIBACC"},
            {80,"Accessing a corrupted shared library", "O acesso a uma biblioteca compartilhada está corrompido","ELIBBAD"},
            {81,".lib section in a.out corrupted", "A biblioteca da sessão a.out está corrompida","ELIBSCN"},
            {82,"Attempting to link in too many shared libraries", "Tentativa de vincular em diferentes bibliotecas compartilhadas","ELIBMAX"},
            {83,"Cannot exec a shared library directly", "Não é possível executar uma biblioteca compartilhada diretamente","ELIBEXEC"},
            {84,"Illegal byte sequence", "Sequência de bytes inválidos","EILSEQ"},
            {85,"Interrupted system call should be restarted", "A interrupção da chamada de Sistema deve ser reiniciada","ERESTART"},
            {86,"Streams pipe error", "Erro no canal de streams","ESTRPIPE"},
            {87,"Too many users", "Muitos usuários","EUSERS"},
            {88,"Socket operation on non-socket", "Operação de soquetes em não soquetes","ENOTSOCK"},
            {89,"Destination address required", "O Endereço de destino é obrigatório","EDESTADDRREQ"},
            {90,"Message too long", "Mensagem muito longa","EMSGSIZE"},
            {91,"Protocol wrong type for socket", "Tipo de protocolo inválido para o soquete","EPROTOTYPE"},
            {92,"Protocol not available", "Protocolo não disponível","ENOPROTOOPT"},
            {93,"Protocol not supported", "Protocolo não compatável","EPROTONOSUPPORT"},
            {94,"Socket type not supported", "O tipo de soquete não é compatível","ESOCKTNOSUPPORT"},
            {95,"Operation not supported on transport endpoint", "Operação não suportada pelo terminal de transporte","EOPNOTSUPP"},
            {96,"Protocol family not supported", "Família de protocolo não compatível","EPFNOSUPPORT"},
            {97,"Address family not supported by protocol", "Família de endereços não suportada pelo protocolo","EAFNOSUPPORT"},
            {98,"Address already in use", "O endereço já está em uso","EADDRINUSE"},
            {99,"Cannot assign requested address", "Não é possível identificar o endereço indicado","EADDRNOTAVAIL"},
            {100,"Network is down", "A rede está fora do ar","ENETDOWN"},
            {101,"Network is unreachable", "A rede está inacessável","ENETUNREACH"},
            {102,"Network dropped connection because of reset", "A conexão da rede caiu devido � redefinição","ENETRESET"},
            {103,"Software caused connection abort", "Um Software causou interrupção da conexão","ECONNABORTED"},
            {104,"Connection reset by peer", "Conexão redefinida por par","ECONNRESET"},
            {105,"No buffer space available", "Nenhum espaço de buffer disponível","ENOBUFS"},
            {106,"Transport endpoint is already connected", "O Endpoint de transporte já está conectado","EISCONN"},
            {107,"Transport endpoint is not connected", "O Endpoint de transporte não está conectado","ENOTCONN"},
            {108,"Cannot send after transport endpoint shutdown", "Não é possível enviar após o desligamento do endpoint de  transporte","ESHUTDOWN"},
            {109,"Too many references: cannot splice", "Muitas referências: não é possível unir","ETOOMANYREFS"},
            {110,"Connection timed out", "A conexão expirou","ETIMEDOUT"},
            {111,"Connection refused", "Conexão recusada","ECONNREFUSED"},
            {112,"Host is down", "O host está fora do ar","EHOSTDOWN"},
            {113,"No route to host", "Nenhuma rota para o host","EHOSTUNREACH"},
            {114,"Operation already in progress", "A operação já está em andamento","EALREADY"},
            {115,"Operation now in progress", "Operação em andamento agora","EINPROGRESS"},
            {116,"Stale file handle", "Identificador de arquivo obsoleto","ESTALE"},
            {117,"Structure needs cleaning", "A estrutura precisa de limpeza","EUCLEAN"},
            {118,"Not a XENIX named type file", "XENIX não é um arquivo de tipo nomeado","ENOTNAM"},
            {119,"No XENIX semaphores available", "Sem semáforos XENIX disponíveis","ENAVAIL"},
            {120,"Is a named type file", "É um arquivo de tipo nomeado","EISNAM"},
            {121,"Remote I/O error", "Erro de entrada e saída remota","EREMOTEIO"},
            {122,"Quota exceeded", "Cota excedida","EDQUOT"},
            {123,"No medium found", "Nenhuma mídia encontrada","ENOMEDIUM"},
            {124,"Wrong medium type", "Tipo de meio errado","EMEDIUMTYPE"},
            {125,"Operation Canceled", "Operação cancelada","ECANCELED"},
            {126,"Required key not available", "A Chave requirida não está disponível","ENOKEY"},
            {127,"Key has expired", "A chave expirou","EKEYEXPIRED"},
            {128,"Key has been revoked", "A chave foi revogada","EKEYREVOKED"},
            {129,"Key was rejected by service", "A chave foi rejeitada pelo serviço","EKEYREJECTED"},
            {130,"Owner died", "O proprietário morreu","EOWNERDEAD"},
            {131,"State not recoverable", "Estado não recuperável","ENOTRECOVERABLE"},
            {132,"Operation not possible due to RF-kill", "Operação não disponível devido ao RF-kill","ERFKILL"},
            {133,"Memory page has hardware error", "Erro de hardware na página de memória","EHWPOISON"},

    };
    fprintf(stderr, "Erro[%d]: %s \n", arr_erros[cod-1].code, arr_erros[cod-1].translated_message);
}

