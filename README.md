# INF8702_Projet
Manuel utilisateur :

Les .bat sont pour essayer API trace.
Sinon, juste lancer Projet.exe devrait suffire.

Actions possibles :
-Drag de la souris pour se déplacer autour du point (0,0)
-Appuyer sur "l" pour déclencher l'activation de l'affichage des lumičres debug 
	(sphere vertes ŕ l'endroit des lumičres)
-Appuyer sur les touches numériques (1 ŕ 4) pour déclencher l'activation des lumičres "point light".
	Initialement, uniquement la lumičre 4 est allumée, en plus d'une lumičre directionnelle.
-Appuyer sur "b" pour activer le bloom
-Appuyer sur "g" pour activer les god rays (rayons crépusculaires)
-Appuyer sur "h" pour activer l'affichage de la passe de luminosité 
	le bloom ou les god rays doivent ętre activer pour obtenir un effet visible

*****Pour seulement voir les sources du projet dans eclipse cdt, 
*****importer ce dernier devrait suffire


*************POUR INSTALLER LE PROJET******************
***ATTENTION, plusieurs étapes nécessaires!

Télécharger et installer le port de GCC pour windows, le compilateur mingw.
	S'assurer d'avoir bien assigner les variables PATH tel que requis dans le howto.

Télécharger glsdk 0.5.2 ŕ l'adresse suivante : 
	http://sourceforge.net/projects/glsdk/files/

Télécharger premake4 :
	http://industriousone.com/premake

Downloader la SFML (version 2.1) pour le bon compilateur (MinGW, par exemple)
. 32 bit pour notre projet.
	
Extraire l'archive glsdk-X.X.7z
2 dans le dossier parent du dossier du projet Eclipse 
	"premake COMPILATEUR" 3 
	Dans notre cas, "gmake" pour mingw.

Compiler glsdk.... 
(Sous linux ou avec MinGW, "make" suffit...avec VS, lancer la solution et compiler)


Extraire la SFML au męme niveau que glsdk.
	->L'arborescence "/glsdk_X_X_X/", "/SFML-2.1/", "/Projet/", "/Projet/src", etc.

Ouvrir Eclipse, "Import..."->General->Existing Project Into Workspace->

	S'assurer que les liens vers SFML et glsdk sont bien présent!!
	Sinon, refaire les liens vers les dossiers de ces librairies.

***** Si sous visual studio (jamais réussi ŕ faire foncitonner, ca devrait pourtant fonctionner....) *****

Changer le toolchain, (Right click project-> Property-> C/C++ Build-> Tool Chain Settings)


Ajuster les includes et les libs (Right click project-> Property-> C/C++ Build-> Settings)
