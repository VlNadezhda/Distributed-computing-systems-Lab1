# Distributed-computing-systems-Lab1
Process spawning in Linux

Постановка задачи:

Создать config — файл, содержащий следующую информацию:

Processes — число процессов;          
Inputprefix — входной префикс;           
Outputprefix — выходной префикс;
maxFilesize — максимальный допустимый размер файла ( в байтах).

Создать входные файлы, содержащие случайное количество слов (число букв и их порядок случайны).

Создать количество процессов, указанное в config — файле. 
Процесс считает количество слов и формирует выходной файл со следующей информацией:
name — имя файла, для которого считали количество слов;
count — количество слов в файле;
pid — pid процесса;
time — время выполнения.
