# mutex
Example with Windows API mutexes

## Description in English
Parent process creates buffer file and mutex which handles are inheritable. Then it creates 2 child processes passing file and mutex handles to them as comand line arguments.
These 2 processes start writing records to file using the mutex created by parent process for file blocking purposes.

## Description in Armenian
Ծնողական պրոցեսը ստեղծում է բուֆեր ֆայլ և մյուտեքս, որոնց հենդլները ժառանգելի են։ Հետո այն ստեղծում է 2 որդիական պրոցեսներ փոխանցելով ֆայլի և մյուտեքսի հենդլները նրանց որպես command line արգումենտներ։
2 որդիական պրոցեսները սկսում են գրանցումներ անել օգտագործելով ծնողական պրոցեսի կողմից ստեղծված միջուկի օբյեկտ հանդիսացող մյուտեքսը բլոկավորման համար։