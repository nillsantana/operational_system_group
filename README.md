#Projeto prático 01
Projeto de cópia exata de um arquivo/diretório de origem para arquivo/diretório de destino

### Conteúdo do diretório:
- customfunctions.h: 
  - arquivo header com todas as notações de funções utilizados pelos dois programas.
- customfunctions.c: 
  - arquivo fonte contendo todas as implementações das funções definidas no arquivo acima.
- filecopy.c: 
  - código fonte do programa de cópia de arquivo.
- treecopy.c: 
  - código fonte do programa de cópia de diretório/subdiretórios.

Observação importante: para o correto funcionamento de um dos programas, os aqurivos customfunctions.c/.h devem estar no mesmo diretório.

### Compilação e execução
Para compilar o programa de cópia de arquivo, utilize o código abaixo em um terminal:
```
gcc filecopy.c -o filecopy.o
```

Para executar o programa de cópia de arquivo, execute o comando abaixo:
```
./filecopy.o <arquivo_origem> <arquivo_destino>
```

Para compilar o programa de cópia de diretórios, utilize o código abaixo em um terminal:
```
gcc treecopy.c -o treecopy.o
```

Para executar o programa de cópia de diretórios, execute o comando abaixo:
```
./treecopy.o <diretorio_origem> <diretorio_destino>
```

##Licença:
Não se aplica