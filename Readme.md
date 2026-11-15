timetracker

Un petit démon en C pour Linux/X11 qui mesure le temps passé sur chaque application, et un client qui affiche le résultat sous forme de jauges dans le terminal.

brave            ██████████████████████████████████████████████████   37m 08s  33%
wezterm-gui      ██████████████████████████████████░░░░░░░░░░░░░░░░   26m 32s  23%
systemsettings   █████████████████████░░░░░░░░░░░░░░░░░░░░░░░░░░░░░   16m 33s  14%
firefox-bin      ████████████████████░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░   15m 58s  14%
plasmashell      ████████░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░    6m 34s   5%
dolphin          █░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░    1m 19s   1%

Le projet a deux objectifs : être utile au quotidien, et servir de support d'apprentissage de la programmation système bas niveau en C (syscalls, /proc, Xlib, signaux, fichiers, mémoire dynamique, daemon systemd) avec une architecture modulaire et testée.

Sommaire
Fonctionnement
Prérequis
Compilation et tests
Installation comme service
Affichage à l'ouverture d'un terminal
Utilisation au quotidien
Configuration
Architecture du code
Format du fichier de données
Principes de conception
Dépannage
Limites connues
Pistes d'évolution
Fonctionnement

Deux programmes indépendants communiquent par un fichier :

┌───────────────────────────────┐          ┌─────────────────────────┐
│ tracker  (démon)              │          │ tracker-ui  (client)    │
│                               │          │                         │
│ chaque seconde :              │          │ à la demande :          │
│  1. X11 : fenêtre active ?    │          │  1. lit stats.dat       │
│  2. PID de cette fenêtre      │  écrit   │  2. trie par durée      │
│  3. /proc/<pid>/comm → nom    │ ───────► │  3. dessine les jauges  │
│  4. +1 seconde pour ce nom    │ stats.dat│     ANSI/UTF-8, adaptées│
│                               │ ◄─────── │     à la largeur du     │
│ toutes les 60 s et à l'arrêt :│  lit     │     terminal            │
│  sauvegarde atomique          │          │                         │
└───────────────────────────────┘          └─────────────────────────┘
   lancé par systemd --user                  appelé depuis ~/.bashrc
Le démon (tracker) interroge le serveur X toutes les secondes pour savoir quelle fenêtre a le focus. Il lit la propriété _NET_ACTIVE_WINDOW de la fenêtre racine, puis _NET_WM_PID de cette fenêtre, et retrouve le nom du processus dans /proc/<pid>/comm.
Il accumule les secondes par nom d'application et sauvegarde la table toutes les 60 secondes, ainsi qu'à l'arrêt (SIGTERM / SIGINT). Les secondes s'additionnent d'un lancement à l'autre.
Le client (tracker-ui) ne dépend pas d'X11 : il lit simplement le fichier de données.
Prérequis
Élément	Détail
Système	Linux (développé sous Ubuntu)
Session graphique	X11 (Xorg). Wayland n'est pas supporté, voir Limites connues
Compilateur	gcc (C11), make
Bibliothèque	libx11-dev (démon uniquement)
Tests	valgrind
Service	systemd (mode --user)
bash
sudo apt install build-essential libx11-dev valgrind gdb

Pour vérifier que tu es bien sous X11 :

bash
echo $XDG_SESSION_TYPE      # doit afficher "x11"

Sous Ubuntu, choisis « Ubuntu sur Xorg » via la roue crantée de l'écran de connexion si besoin.

Compilation et tests
bash
make            # compile build/tracker (démon) et build/tracker-ui (client)
make test       # compile les tests et les lance sous valgrind
make run        # compile puis lance le démon au premier plan
make clean      # supprime le dossier build/

Les tests sont dans tests/ : un exécutable par module, avec des assert. make test les exécute sous valgrind --leak-check=full : un test échoue aussi s'il y a une fuite mémoire ou une erreur d'accès.

stats : OK
storage : OK
paths : OK
render : OK
Tous les tests passent.

Flags de compilation : -Wall -Wextra -Wpedantic -g -std=c11 -D_POSIX_C_SOURCE=200809L. L'objectif est zéro warning.

Installation comme service

Le démon se lance automatiquement à l'ouverture de ta session graphique grâce à un service systemd utilisateur (aucun sudo nécessaire).

1. Créer le fichier de service

~/.config/systemd/user/timetracker.service (une copie est versionnée dans packaging/) :

ini
[Unit]
Description=Time tracker (temps passé par application)
After=graphical-session.target
PartOf=graphical-session.target

[Service]
ExecStart=/chemin/absolu/vers/timetracker/build/tracker
Restart=on-failure
RestartSec=5

[Install]
WantedBy=graphical-session.target

Adapte ExecStart : le chemin doit être absolu.

2. Vérifier que systemd voit l'écran

Le démon a besoin de DISPLAY (pour XOpenDisplay) :

bash
systemctl --user show-environment | grep -E 'DISPLAY|XAUTHORITY'

Si ces variables manquent :

bash
systemctl --user import-environment DISPLAY XAUTHORITY
3. Activer et démarrer
bash
systemctl --user daemon-reload
systemctl --user enable --now timetracker.service
systemctl --user status timetracker.service     # "active (running)" attendu
Ce que garantit la configuration
WantedBy=graphical-session.target : démarrage automatique à chaque ouverture de session.
Restart=on-failure : si le démon plante, est tué (SIGKILL) ou si X n'est pas encore prêt, systemd le relance après 5 s. Une sortie normale (code 0, par exemple systemctl --user stop) ne le relance pas.
Affichage à l'ouverture d'un terminal

Ajoute à ~/.bashrc :

bash
# timetracker : résumé à l'ouverture d'un terminal
if [ -t 1 ] && [ -x /chemin/absolu/vers/timetracker/build/tracker-ui ]; then
    /chemin/absolu/vers/timetracker/build/tracker-ui
fi
[ -t 1 ] : n'affiche que si la sortie est un vrai terminal (rien dans un scp, un script ou une commande à distance).
[ -x ... ] : ne casse pas ton shell si le binaire est absent.

Seuls les nouveaux terminaux lisent le fichier à nouveau. Si ton terminal est lancé comme shell de connexion et que rien ne s'affiche, vérifie que ~/.bash_profile ou ~/.profile charge bien ~/.bashrc.

Utilisation au quotidien
bash
./build/tracker-ui                                    # afficher les stats

systemctl --user status timetracker.service           # état du démon
systemctl --user stop timetracker.service             # arrêt propre (sauvegarde)
systemctl --user start timetracker.service
systemctl --user restart timetracker.service          # à faire après chaque recompilation

journalctl --user-unit timetracker.service -n 20 --no-pager   # journal
cat ~/.local/share/timetracker/stats.dat              # données brutes

Après chaque make, redémarre le service : un service actif continue d'exécuter l'ancien binaire chargé en mémoire.

Ne lance pas ./build/tracker à la main pendant que le service tourne : les deux processus écriraient dans le même fichier.

Le client affiche l'état du dernier enregistrement : le démon écrit toutes les 60 s, les chiffres peuvent donc avoir jusqu'à une minute de retard.

Configuration

Il n'y a pas de fichier de configuration : les réglages sont des constantes en tête de fichier. Après modification, recompile (et redémarre le service pour le démon).

Constante	Fichier	Rôle	Défaut
SAVE_INTERVAL	src/daemon_main.c	Secondes entre deux sauvegardes périodiques	60u
MIN_PERCENT	src/render.c	Les applications sous ce pourcentage ne sont pas affichées (les données restent enregistrées)	1
MAX_BAR	src/render.c	Largeur maximale d'une jauge, en colonnes	50
MIN_BAR	src/render.c	Largeur minimale d'une jauge	10
MIN_WIDTH	src/render.c	Largeur de terminal minimale prise en compte	40

Emplacement des données : $XDG_DATA_HOME/timetracker/stats.dat si XDG_DATA_HOME est défini, sinon ~/.local/share/timetracker/stats.dat. Les dossiers sont créés avec le mode 0700.

Architecture du code
timetracker/
├── Makefile
├── README.md
├── packaging/
│   └── timetracker.service        # copie du service systemd
├── src/
│   ├── daemon_main.c              # exécutable 1 : le démon (assemblage + signaux)
│   ├── ui_main.c                  # exécutable 2 : le client terminal
│   ├── x11_probe.[ch]             # SEUL module qui connaît X11
│   ├── proc_info.[ch]             # PID → nom du processus (/proc)
│   ├── stats.[ch]                 # table nom → secondes (type opaque)
│   ├── storage.[ch]               # sauvegarde / chargement du fichier
│   ├── paths.[ch]                 # chemin du fichier de données (XDG)
│   ├── tracker.[ch]               # logique : sonde + stats + persistance
│   ├── ticker.[ch]                # boucle « une fois par seconde »
│   └── render.[ch]                # jauges et durées (client)
└── tests/
    ├── test_stats.c
    ├── test_storage.c
    ├── test_paths.c
    └── test_render.c
Graphe de dépendances
daemon_main ──► tracker ──► x11_probe        (Xlib)
     │             ├──────► proc_info        (/proc)
     │             ├──────► stats
     │             └──────► storage ──► stats
     ├──────────► ticker
     └──────────► paths

ui_main ──► paths
        ├─► storage ──► stats
        └─► render  ──► stats

Les dépendances vont toujours dans un seul sens, sans cycle. tracker-ui n'est pas lié à libX11.

Les modules
Module	Responsabilité	Points notables
x11_probe	Retourne le PID de la fenêtre active (_NET_ACTIVE_WINDOW, puis _NET_WM_PID)	Type opaque, create/destroy. Retourne 1 (et non une erreur) quand il n'y a pas de fenêtre active ou pas de PID
proc_info	Lit /proc/<pid>/comm	Le buffer est fourni par l'appelant, aucune allocation. Nom limité à 15 caractères par le noyau
stats	Table nom → secondes	Tableau dynamique (realloc ×2), noms copiés et possédés par la table, type opaque
storage	Sauvegarde / chargement	Écriture atomique (.tmp puis rename), fclose vérifié, lignes mal formées ignorées
paths	Calcule et crée le dossier de données	Respecte XDG_DATA_HOME, mkdir niveau par niveau
tracker	Orchestre sonde, statistiques et persistance	Aucun printf : un module de logique n'affiche rien
ticker	Appelle un callback chaque seconde	ticker_run (N fois) et ticker_run_until (jusqu'à un drapeau de signal), pattern callback + userdata
render	Trie, calcule les jauges, formate les durées	Pur : écrit dans un FILE *, donc testable avec open_memstream
daemon_main	Installe les handlers SIGINT/SIGTERM, charge, boucle, sauvegarde	Le handler ne fait que lever un drapeau volatile sig_atomic_t
ui_main	Lit le fichier, détecte la largeur (ioctl(TIOCGWINSZ)), appelle render	80 colonnes par défaut si la sortie n'est pas un terminal
Cycle de vie du démon
paths_stats_file → install_handlers → tracker_create → tracker_load
        → ticker_run_until(&g_stop, on_tick, &app)
              └─ chaque seconde : tracker_tick
                 chaque 60 ticks : tracker_save
        (SIGTERM / SIGINT : g_stop = 1, la boucle sort en < 1 s)
        → tracker_save → tracker_destroy
Format du fichier de données

Texte brut, un enregistrement par ligne : <secondes><TAB><nom>.

2228    brave
1592    wezterm-gui
993     systemsettings
Lisible avec cat, modifiable à la main.
Les lignes mal formées sont ignorées au chargement.
Les secondes s'ajoutent à celles déjà en mémoire : recharger un fichier ne remplace rien.
L'écriture est atomique : le fichier est d'abord écrit dans stats.dat.tmp, puis renommé. Si le processus est tué en pleine écriture, l'ancien fichier reste intact.
Principes de conception

Le projet suit quelques règles, appliquées module par module :

Un module = un .h (le contrat) + un .c (l'implémentation). Tout ce qui n'est pas public est static.
Types opaques (stats_t, x11_probe_t, tracker_t) : le header ne montre pas la structure, on passe par des fonctions.
Propriété de la mémoire explicite : tout xxx_create() a son xxx_destroy() (qui accepte NULL), et la documentation dit qui libère quoi.
Pas de variable globale (hormis le drapeau de signal, obligatoire) : l'état passe en paramètre, via void *userdata dans les callbacks.
Erreurs par valeur de retour (0 / -1, errno positionné), jamais de exit() dans un module. Les cas normaux (pas de fenêtre active) ne sont pas des erreurs.
Libération sur tous les chemins d'erreur, dans l'ordre inverse de l'acquisition.
Isolation de X11 dans un seul module : porter le projet vers Wayland ne toucherait que x11_probe.
Chaque module a son test, exécuté sous valgrind.

Concepts C couverts, par module : pointeurs de fonction et userdata (ticker), parsing de /proc et errno (proc_info), malloc/realloc/free (stats), E/S fichiers et écriture atomique (storage), Xlib et mémoire côté serveur (x11_probe), signaux et sigaction (daemon_main), qsort et UTF-8 (render), ioctl (ui_main), unités systemd (packaging/).

Dépannage
Symptôme	Cause probable	Solution
make ne produit rien dans build/tracker	La cible par défaut n'est pas all	Utiliser le Makefile fourni (.DEFAULT_GOAL := all)
*** missing separator	Des espaces au lieu d'une tabulation devant les commandes du Makefile	Remplacer par de vraies tabulations
No rule to make target 'build/ui_main.o'	src/ui_main.c absent	Vérifier ls src/
structure sigaction inconnue	Macro POSIX manquante	-D_POSIX_C_SOURCE=200809L dans CFLAGS, avant tout #include
undefined reference to 'XOpenDisplay'	-lX11 manquant à l'édition de liens	Vérifier LDLIBS et son usage dans la règle du démon
Le démon affiche creation tracker et s'arrête	Pas de serveur X joignable (DISPLAY absent, session Wayland)	echo $DISPLAY, echo $XDG_SESSION_TYPE, systemctl --user import-environment DISPLAY XAUTHORITY
Les chiffres du client ne bougent pas	Le service est arrêté, ou l'écriture périodique n'a pas encore eu lieu	systemctl --user status timetracker.service, patienter 60 s
Le comportement ne change pas après un make	Le service exécute l'ancien binaire	systemctl --user restart timetracker.service
Le service échoue après un make clean	Le binaire build/tracker n'existe plus	Relancer make, puis restart
journalctl --user -u ... : « No journal files »	Le journal utilisateur n'est pas lisible	journalctl --user-unit timetracker.service
Rien ne s'affiche à l'ouverture d'un terminal	~/.bashrc non lu ou binaire absent	bash -i pour tester, ls -l build/tracker-ui, charger .bashrc depuis .profile
Limites connues
X11 uniquement. Wayland interdit volontairement à un programme de demander quelle fenêtre a le focus. Sous GNOME/Wayland, il faudrait une extension du shell ou D-Bus, et seul x11_probe serait à réécrire.
Noms tronqués à 15 caractères : /proc/<pid>/comm est limité par le noyau (gnome-terminal-server devient gnome-terminal-).
Applications sans _NET_WM_PID : certaines applications (sandbox Snap/Flatpak notamment) n'exposent pas leur PID et ne sont pas comptées.
Pas de détection d'inactivité : le temps passé devant un écran verrouillé ou loin de l'ordinateur est compté tant qu'une fenêtre a le focus.
Fenêtre fermée entre deux requêtes : X renvoie BadWindow, et le gestionnaire d'erreurs par défaut de Xlib termine le programme. C'est rare, et Restart=on-failure relance le démon, mais l'erreur n'est pas encore interceptée (XSetErrorHandler).
Pas de verrou : deux démons simultanés écriraient dans le même fichier.
Retard d'affichage jusqu'à 60 s (écriture périodique), et jusqu'à 60 s de données perdues sur un SIGKILL ou une coupure de courant.
Noms contenant une tabulation ou un saut de ligne casseraient le format du fichier. Les noms viennent de /proc/<pid>/comm, donc c'est improbable.
Lignes de plus de 255 caractères dans le fichier : tronquées par fgets (impossible avec des noms de 15 caractères).
Un tick = une seconde : l'horloge est un sleep(1), sans correction de dérive.
Pistes d'évolution
[ ] make install : copier les binaires dans ~/.local/bin et ne plus dépendre du dossier de développement
[ ] Verrou (flock) pour garantir un seul démon
[ ] Socket Unix (socket, bind, listen, accept) : chiffres en direct au lieu d'un fichier
[ ] Détection d'inactivité (extension XScreenSaver)
[ ] XSetErrorHandler pour survivre à BadWindow
[ ] Statistiques par jour (un fichier par date) et remise à zéro
[ ] Horloge monotone (clock_gettime) à la place de sleep(1)
[ ] Mesure du temps réel écoulé entre ticks
[ ] Support Wayland (nouveau module à la place de x11_probe)

                                                Licence
Diego DELVIG, diegodvg@tutamail.com
