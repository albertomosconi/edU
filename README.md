# edU, o ed con Undo multipli

Progetto di algoritmi e strutture dati

## Indice

- [Descrizione](#Descrizione)
- [Esempio di input](#Esempio-di-input)
- [Implementazione](#Implementazione)

## Descrizione

Il progetto consiste nell’implementare un semplice editor di testi. L’editor considera un documento come una sequenza di righe, di dimensione arbitraria, numerate a partire da uno. La rappresentazione del documento comprende anche un indice intero, che rappresenta il numero di riga corrente, indicato da qui in poi come _indirizzo corrente_.

L’interfaccia dell’editor è costituita da comandi testuali, terminati da un carattere di “a capo”. I comandi possono essere seguiti da una porzione di testo, costituita da una o più righe, terminata da un carattere **.** (punto) che compare come unico carattere sulla riga successiva. I comandi sono costituiti da un’unica lettera, preceduta opzionalmente da uno o due numeri interi.

L’interfaccia dell’editor è liberamente ispirata a quella del tradizionale editor ed.

In alcuni comandi, gli interi che compaiono in essi rappresentano degli _specificatori di indirizzo_. Più precisamente, uno specificatore di indirizzo è un numero n, espresso in decimale, che indica l’indirizzo della n-esima riga; la prima riga del testo ha indirizzo 1.

I comandi supportati sono i seguenti, con la convenzione che **ind1**, **ind2** indicano due specificatori di indirizzo tali che **ind1 ≤ ind2** e le parentesi tonde sono introdotte per semplicità di lettura di questo testo, ma non incluse nel comando:

- **(ind1,ind2)c**  
  Cambia il testo presente alle righe comprese tra **ind1** ed **ind2** (estremi inclusi). Il testo che segue il comando deve essere fatto di un numero di righe pari a **ind2-ind1+1**.**ind1** deve essere o un indirizzo effettivamente presente nel testo, oppure il primo indirizzo dopo l’ultima riga presente nel testo (o 1 se il testo è ancora vuoto).
- **(ind1,ind2)d**  
  Cancella le righe comprese tra **ind1** ed **ind2** (estremi inclusi), spostando verso l’alto le righe successive a quella di indirizzo **ind2** (se ce ne sono). La cancellazione di una riga che non esiste nel testo non ha alcun effetto.
- **(ind1,ind2)p**  
  Stampa le righe comprese tra **ind1** ed **ind2** (o tra **ind1** e la fine del testo se **ind2** non corrisponde ad alcun indirizzo nel testo). Per ogni riga non esistente compresa nella regione tra **ind1** e **ind2** (inclusi) l’editor stampa una riga contenente il solo carattere punto.
- **(numero)u**  
  Effettua l'annullamento (**undo**) di un numero di comandi (**c** o **d**) pari a quello specificato tra parentesi tonde (laddove **numero** è un intero strettamente maggiore di zero). Una sequenza di comandi di **undo** consecutivi annulla un numero di passi pari alla somma dei passi specificati in ognuno di essi. Se il numero di comandi di cui effettuare l'annullamento è superiore a quello dei comandi eseguiti, vengono annullati tutti i passi. L'esecuzione di un comando di modifica del testo (**c**, **d**) dopo un undo cancella gli effetti dei comandi annullati definitivamente. Si noti che nel numero dei comandi da annullare vengono contati anche i comandi che non hanno effetto (per esempio la cancellazione di un blocco di righe che non esistono).
- **(numero)r**  
  Annulla l’effetto di **undo** per un numero di comandi pari a numero a partire dalla versione corrente (funzione di **redo**). Si noti che **numero** deve essere un intero strettamente maggiore di zero. Si ha quindi che una sequenza di comandi del tipo

  ```
    10u
    5r
  ```

  è a tutti gli effetti equivalente al solo comando **5u**. Similmente, la sequenza

  ```
    12u
    3r
    2u
    7r
  ```

  equivale al comando **4u**. Nel caso in cui il numero di comandi di cui effettuare il **redo** è superiore a quelli annullati correntemente, viene effettuato il numero massimo di **redo** possibili.

- **q**  
  Termina l’esecuzione dell’editor

Una riga di testo fornita in ingresso all’editor può contenere al massimo 1024 caratteri.

Si assuma che all’editor vengano dati solo comandi corretti (non è quindi necessario verificare la loro correttezza). Per esempio, non viene mai dato un comando **c** in cui è **ind1** > **ind2**, o **ind1** < 1, o **ind1** non è né l’indirizzo di una riga presente nel testo, né l’indirizzo della prima riga dopo il testo. Similmente per gli altri comandi. Si faccia attenzione però al fatto che, in alcuni casi, il comando è ammesso, ma semplicemente non ha effetto; per esempio, è possibile dare un comando **d** in cui **ind1** non è l’indirizzo di una riga del testo, ma in questo caso il comando non ha alcun effetto sul testo.

## Esempio di input

Esempio di sequenza di comandi:

```
1,2c
prima riga
seconda riga
.
2,3c
nuova seconda riga
terza riga
.
1,3p
1,1c
nuova prima riga
.
1,2p
2,2d
4,5p
1,3p
4,5d
1,4p
3u
1,6p
1r
1,3p
q
```

La precedente sequenza di comandi dà origine al seguente output da parte dell’editor

```
prima riga
nuova seconda riga
terza riga
nuova prima riga
nuova seconda riga
.
.
nuova prima riga
terza riga
.
nuova prima riga
terza riga
.
.
prima riga
nuova seconda riga
terza riga
.
.
.
nuova prima riga
nuova seconda riga
terza riga
```

## Implementazione

La struttura principale che contiene le righe di testo è un albero rosso/nero e, tramite questo, si garantisce l'assenzia di stringhe doppie, ed una lettura e scrittura veloce dei contenuti.

La storia dei comandi eseguiti dall'utente è tenuta in una lista concatenata bidirezionale, per cui l'inserimento di un nuovo comando avviene in tempo costante.
Ogni nodo di questa lista è una struct contenente, oltre al carattere che identifica il comando e i due numeri i1 e i2, una lista di puntatori alle stringhe interessate in memoria.
