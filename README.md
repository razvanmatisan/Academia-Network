# Academia Network

Studenti:

+ Razvan-Andrei Matisan - razvan.matisan@stud.acs.upb.ro
+ Radu-Stefan Minea     - radu_stefan.minea@stud.acs.upb.ro

## Descrierea proiectului

Academia Netowrk reprezinta un sistem ce agrega informatii despre articole 
stiintfice prelaute dintr-o baza de date de peste 100k astfel de elemente.

Programul raspunde la query-uri folosind cele mai recente date salvate. 

## Rulare

1. Construirea programului:

    * User-ul introduce in terminal:
        make -f Makefile_tema3
        make
        
2. Executarea programului:

    * User-ul isi trece paper-urile pe care vrea sa le adauge in sistem, dar
    si query-urile pe care vrea sa le faca asupra acestora intr-un fisier de
    tipul <input.in>. Query-uri disponibile:
        + add_paper <paper_id>
        + get_oldest_influence <paper_id>
        + get_venue_impact_factor <venue_name>
        + get_number_of_influenced_papers <paper_id> <max_distance>
        + get_number_of_papers_between_dates <earliest_date> <latest_date>
        + get_number_of_authors_with_field <institution_name> <field_name>
        + get_histogram_of_citations <author_id> <number_of_years>
    
    * Pentru a verifica daca programul furnizeaza output-ul corect, se creeaza
    un fisier de tip <reference.out>
    
    * Pentru a executa programul, user-ul introduce in terminal:
        ./tema3 <input.in> <reference.out>
        
    * Daca in terminal nu este furnizat niciun mesaj de tip eroare, inseamna ca
    a fost furnizat output-ul asteptat.

## Structura proiectului

Rezolvarea temei este impartita pe mai multe fisiere .c sau .h care sunt
compilate de un fisier Makefile. Acestea contin urmatoarele implementari:

+ LinkedList.c + .h -> lista simplu inlantuite

+ Queue.c + .h -> coada

+ Hashtables.c + .h -> toate hashtable-urile auxiliare

+ utils.c + .h -> functiile auxiliare, folosite pentru rezolvarea taskurilor

+ publications.c + .h -> contin atat definirea structurii de date PublData, cat
si rezolvarile propriu-zise ale taskurilor.


## Structuri de date folosite

Multe, muulte hashtable-uri + un graf "mascat"!

* Citations_HT
    + Key - ID-ul paper-urilor
    + Content - de cate ori a fost citat un paper anume

* Venue_HT
    + Key - venue-ul X
    + Content - ce paper-uri au fost publicate la venue-ul X

* Field_HT
    + Key - field-ul X
    + Content - paper-uri au fost publicate in field-ul X

* Authors_HT
    + Key - ID-ul autorului X
    + Content - ce paper-uri a publicat autorul X (dar si anul publicarii 
    acestora, pentru a rezolva task-ul mai usor si mai eficient)

* Influence_HT - un graf simulat:
    + Key - ID-ul paper-urului X
    + Content - "vecinii" sunt paper-urile ce au fost infleuntate de paper-ul X
        - Exemplu: [X] -> A -> B -> C -> NULL
        - Paper-ul X a influentat paper-urile A, B si C.

* Markings_HT
    + Key - ID-ul paper-urilor
    + Content - arata daca un paper este viziat (visited) pentru parcurgerile
    BFS, dar si distanta catre origine (pentru get_number_of_influenced_papers)

* "Papers_HT"
    + PublData contine, pe langa hashtable-urile auxiliare (Influenced,
    Citations etc.) un hashtable "mare":
        - Key - ID-ul paper-urilor
        - Content - structura de tip "paper" continand toate datele despre un
        paper anume (title, year, etc.)

* Coada
    + Pentru parcugeri de tip BFS
    + Se bazeaza pe liste inlantuite

* LinkedList - optimizate masiv, in felul urmator:
    + Adaugarile (la cozi, la hashtable-uri) se fac mereu la finalul listei,
    asa ca m-am folosit de pointerul taskurilor
    + Remove-urile se fac mereu de la inceputul listei (dequeue de la coada),
    asa ca pentru remove am tratat exclusiv acest caz


## Rezolvarea task-urilor

~~~~~~~~~ Task 1 ~~~~~~~~~

Cheia rezolvarii taskului 1 este o parcurgere BFS, pornind de la primul paper
identificat prin id-ul dat ca parametru.

Pentru parcurgerea BFS, ne-am folosit de o coada.

Astfel, determinarea celei mai vechi influente pentru paper-ul dat a fost
determinata folosind o variabila de tip Paper (oldest_influence) si o
functie de comparare, care compara doua paper-uri dupa cele trei criterii
mentionate in cerinta.

Este important de mentionat faptul ca, in functia de comparare, numarul de
citari a fost determinat apeland functia get_no_citations.

Mai mult, elementele adaugate in coada sunt referintele paper-ului analizat
in acel moment. Pentru a nu adauga un paper de mai multe ori, am initializat
cu 0 o variabila ok in interiorul structurii ce descrie Paper-ul, ce devine
1 in momentul in care a fost adaugat in coada.

La final, ne folosim de functia clean_refs_aux_data si de o parcurgere DFS
pentru a reinitializa cu 0 toate ok-urile.

In cazul in care oldest_influence NU este NULL, returnam titlul paper-ului.
Altfel, returnam "None"

~~~~~~~~~ Task 2 ~~~~~~~~~

Pentru rezolvarea acestei cerinte, ne-am folosit de Venue_HT
(hashtable ce are drept key = venue si value = id-ul paper-ului), inclus in
PublData.

Cautarea paper-urilor se limiteaza, astfel, in a cauta intr-o lista simplu
inlantuita toate id-urile corespunzatoare venue-ului dorit.

Ca sa identificam numarul de citari ale unui paper, ne-am folosti de functia
get_no_citations care are nevoie doar de id-ul paper-ului si de Citations_HT
pentru a returna ceea ce ne dorim.

Adunam toate citarile si numarul de paper-uri cu venue specific, facem media
si returnam rezultatul dorit.

~~~~~~~~~ Task 3 ~~~~~~~~~

Numarul de paper-uri influentate de un autor "to a certain degree" (pana la
distanta max_dist) il aflam printr-un BFS prin Influence_HT, unde:
    + Nodul este un autor ("influencer")
    + Vecinii acestuia sunt cei pe care i-a influentat ("imitator")

Parcurgem aceste liste printr-un BFS implementat cu ajutorul unei cozi.

Pentru a retine parametrul "visited" si distanta pana la origine, am folosit
structura Markings_HT. Aceasta:
    + Se initializeaza la inceputul task-ului
    + Se actualizeaza de fiecare data cand un imitator nevizitat este intalnit
        - distance_to_origin(imitator) = distance_to_origin(influencer) + 1 
    + Memoria se elibereaza la finalul task-ului

~~~~~~~~~ Task 6 ~~~~~~~~~- 

Implementarea este foarte ineficienta, pentru ca iteram prin tot Papers_HT
(unde se afla TOATE paper-urile introduse) pentru a le gasi pe cele 
publicate intre cele doua date.

Mai eficient - puteam folosi un arbore binar de cautare (BST), precum un
Treap sau un Red-Black Tree.

~~~~~~~~~ Task 7 ~~~~~~~~~

Pentru rezolvarea acestei cerinte, ne-am folosit de Field_HT
(hashtable ce are drept key = field si value = id-ul paper-ului), inclus in
PublData.

Cautarea paper-urilor se limiteaza, astfel, in a cauta intr-o lista simplu
inlantuita toate id-urile corespunzatoare venue-ului dorit.

Pentru modularizare, am creat functia ids_with_field care adauga intr-un 
vector toate id-urile paper-urilor despre un field anume si returneaza 
numarul de elemente introduse in acesta.

Mai departe, am parcurs acest vector si am numarat pentru fiecare paper in
parte (returnat folosind functia find_paper_with_id) numarul de autori care
fac parte de la institutia data ca parametru al functiei.

Am avut grija sa nu numaram de doua ori un autor, retinand numele tuturor
autorilor luati in considerare intr-un vector de stringuri.

~~~~~~~~~ Task 8 ~~~~~~~~~

Folosim un Hashtable de autori (Authors_HT) pentru a stii exact ce paper-uri
a publicat autorul respectiv, dar si in ce an.

Histograma se aloca initial ca un vector cu un singur element.
    + Realocarea se face in functie de anul minim al paper-urilor publicate de
    de autorul dat
    + Dupa realocare, initializam cu 0 slot-urile nou adaugate prin memset
    + Numarul de citari, dat de functia get_no_citations()

===============================================================================
## Limitari

+ Multe warning-uri de compilare

+ #include-uri in plus

+ PublData cu field-uri inutile (care se folosesc doar in cadrul ht-urilor
mici, nu si in Papers_HT)
