# ArcTests
Testing compression algorhytms

Проект написан с целью тестирования различных алгоритмов сжатия/преобразования для лучшей сжимаемости файлов. Интерфейс разработан с помощью WPF и C#. Сами алгоритмы реализованы в нативной DLL на C++. Плюс промежуточная .NET сборка для связки управляемого и неуправляемого кода.

Выбираем файл, один или несколько алгоритмов, которыми нужно обработать файл и сжатый файл сохраняется в <имяфайла.compr>. Для распаковки выбираем .compr файл, выставляем нужные алгоритмы в том же порядке, создастся файл <имяфайла_orig>. Для распаковки нужно правильно указывать алгоритмы, т.к. в сжатом файле нет никакой информации о них.

------------------------------------------------------------------------------

The aim of project is to test various file compression/modification algorhytms. User interface is made with WPF and C#. Algorythyms are made with c++ as native DLL. Also there is an intermediate .NET assembly for linking managed and unmanaged code. At the moment source code has comments only in russian.

Choose file, one or more algorhytms to process the file, and compressed file will be saved as "filename.compr". For decompression choose a .compr file, set up algorhytms in the same order as when compressing the file, and uncompressed file will be saved as "filename_orig". For succesful decompression you have to set up right algorhytms, because compressed file has no information about them.


