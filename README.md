ProiectAscEchipaToataLuminaPeCareNuOPutemVedea.cpp este fisierul care controleaza toata magia!

Doar trebuie modificata variabila taskid cu exemplul aferent!

Se recomanda folosirea watchRegisters, printPath pentru o experienta inedita!

Actual si encodarea si decodarea se face in acelasi timp, dar functiile sunt interactive si pot fi utilizate separat!

Pentru fiecare exemplu avem:

bin.bin -> unde va fi calculat binarul

code.txt -> unde va fi codul. Pentru fiecare exemplu trebuie adaugata o secventa de genul:

main:
    
    call strlen
    
    ret
    
pentru ca programul sa stie ce functie sa apeleze prima!

"Add Element to Singly Linked List" functioneaza si aceasta, dar a fost sarita in exemple, deoarece este lasat la latitudinea dvs sa va creati propria dvs lista.

la "Reverse a string" este nevoie sa se adauge si functia strlen, dar acest lucru a fost facut deja. de asemenea, in exemplul lor # s1 = str este gresit, ar trb sa fie # a0 = str. la noi totul este corect!

memory.in -> 

formatul = reg datatype registername value sau memory datatype memorylocation value (memorylocation este reprezentat in bytes deci daca avem un vector de int atunci elementele vor fi pozitionate la distana de 4 bytes)
exemple:
reg int a0 100, reg int a1 20, memory double 100 5, memory double 108 6, memory double 20 8, memory double 28 11

query.txt -> 
formatul = query reg datatype register sau query memory datatype position

programul va va raspunde la intrebarile din fisierul query.txt!
