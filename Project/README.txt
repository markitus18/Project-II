
To launch de game execute "The void's comeback - Pylon Rush 1.0.exe" located inside the "Game" folder.


If you've played any earlier version of the game, please delete the local data folder "Void's comeback - Pylon Rush".
It is located at: "  C:\Users\[UserName]\AppData\Roaming\UPC\Void's comeback - Pylon Rush  "
You can also locate it by executing the command "%appdata" and finding the UPC folder.







--- GAME EXPLANATION -------------------------------------------------

In this gamemode, the player controls a Protoss base that’s sent to a planet to erradicate
the Zerg plague. The game will mostly play like any other starcraft game.

Defeat the enemy main base, shown on the map, while protecting your own base and most importantly, the
"Zerg Sample", the building near the player's starting location. Loosing it means loosing the game.

Upon destroying the enemy main base, Kerrigan will appear and attack the player. Defeat her before she
gets to the sample or all hope of cleansing the planet will be lost!




Victory: All Zerg bases are eliminated and Kerrigan dies.
Defeat: Zerg Sample is destroyed.

Zerg Bases:
The main base will always have Ultralisks defending it. Destroy it to unleash Kerrigan's rage and get a
step closer to victory, but make sure your defenses are ready to take the hit, every Zerg alive will charge
towards you!

There will be up to 3 other bases around the map, randomly spawn. Each one will have a type of enemy that
will behave differently:

-Zergling base. Weak units but with high damage output in groups. Will attack the player.
-Mutalisk base. Medium units, with ranged attacks and the ability to fly. Units will roam around the map.
-Hydralisk base. High damage medium ranged units. Units will aid other Zerg bases.
-Infested Terran base. When detecting an enemy, they will run towards it and explode nearby, dealing a huge
damage in an area. They have low life and range of vision, but deadly damage. Units will disperse
around the map.




--BOSS PHASE---------------------------------------

Once the boss spawns all the Zergs that are still alive will inmediatly attack the player, that's the first
impact the player defenses will have to survive. Kerrigan will slowly but steadily march towards the player's
base attacking with different patterned explosions that will deal an area of effect damage. Every explosion
will be first announced by a circular stencil on the ground, giving the player a chance to avoid it.

The boss has a huge carapace and it won't be easy to damage. This is represented by her shield, displayed by the
blue bar on her life bar. Upon depletion, Kerrigan will stand still for a period of time and the player will be
able to damage her directly, reducing her health (the red bar). Altough, whenshe recomposes, she'll
aniquilate any nearby units and regain the whole shield again.








--- DEFAULT KEYS -------------------------------------------------


-- In-game:

Escape: open the menu.
Arrow keys: move the cammera around.

TAB: move the camera to the last point of interest.
SPACE: move the camera to the current selection; or the Zerg sample if there are no units selected.

Numeric keys from 1 to 6: Move the camera to a previously saved position.
Shift + Nº 1 to 6: Save the current camera position to the number pressed.


-Shortcuts:
With a probe selected:
	-N: Warp in a Nexus --- (deposit of resources, can spawn probes).
	-P: Warp in a Pylon --- (creates buildable terrain and increases the maximum psi, the population limit).
	-A: Warp in an Assimilator --- (can be built on a vespene gas geyser and allows probes to extract it).
	-G: Warp in a Gateway --- (creates military units).
	-C: Warp in a Cybernetics core --- (unlocks the Dragoon, the Observer, the Photon Cannon and the Robotics Facility).
	-H: Warp in a Photon Cannon --- (defensive building, shoots at enemies).
	-R: Warp in the Robotics Facility --- (unlocks the Scout, the Dark Templar and the Templar Archives).
	-T: Warp in the Templar Archives --- (unlocks the Reaver and the High Templar).

With a Nexus selected:
	-P: Warp in a Probe --- (basic worker. Can gather resources and build structures).

With the Gateway selected:
	-Z: Warp in a Zealot --- (basic mele infantry).
	-D: Warp in a Dragoon --- (basic long range infantry).
	-O: Warp in a Observer --- (flying exploration unit. Great vision range, but can't attack).
	-S: Warp in a Scout --- (flying spaceship. High damage against other flying units).
	-T: Warp in a Dark Templar --- (advanced mele infantry. Low life, but really high damage).
	-R: Warp in a Reaver --- (siege machinery. Really slow, but with high damage and AoE attacks).
	-H: Warp in a High Templar --- (psionic mage. Low life, but deals moderat damage in a huge AoE).




-- Debug Keys:
F1: Open the console.

F2: General entities debug.
	While this mode is active
	- Press any unit creation sortcut to spawn it at the mouse position.
	- Use number keys from 6 to 0 to spawn Zergs at the mouse position.
	- I: Get 1000 mineral.
	- O: Get 1000 gas.
	- P: Get 100 psi.

F3: Pathfinding debug.

F4: UI debug.

F5: Disable fog of war.

F6: Explosions debug.
	While this mode is active
	- Z, X, C, V to create diferent boss explosions at mouse position.
	- B to create a deadly explosion (for allies and enemies).

F7: Spawn two Game Masters, an ally and an enemy.

F8: Kill all zergs.




--- LINKS -------------------------------------------------

GitHub:
https://github.com/markitus18/Project-II

Page:
http://markitus18.github.io/Project-II/

Facebook:
https://www.facebook.com/pylonrush

Twitter:
https://twitter.com/Pylon_Rush

--- CHANGELOG -------------------------------------------------


0.1 ----------------------------------------------------------

Basic walkable map.
Isometric small map, with slimes moving over it.
Editable Walkable map.


0.2 ----------------------------------------------------------

Changed to Ortogonal.
Created basic units, spawning and walking.
Porting the code to std libraries.
Closing game design.


0.3 ----------------------------------------------------------

Units interactions: attacking and dying.
Diferent types of units, with basic animations implemented.
Ended port to std.
Starting UI.


0.4 ----------------------------------------------------------

Created Buildings and Resources.
Probes now can collect resources.
Basic IA.
Added a single control panel to build.


0.5 ----------------------------------------------------------

Added new animations and improved exisitng ones:
-Idle, attack, death
Resources now decay while they get lower on resources.
Probes display the resource they're carrting.
Improved minimap: it now displays units and buildings and it's controller.
Added flying units!
Added enemy IA: four independant Zerg bases that will attack the player and aid each other.
Added Mutalisks, Hydralisks and Ultralisks.
UI improvements: orders panel now works properly, and is 100% operative.
Probes now are smarter, they should gather resources more efficently and not get stuck all in a single mineral.
Player can't spawn buildings anywhere anymore. Probes are required to build now.
Added victory conditions: Destroy all Zergs.
Added defeat conditions: Zerg Sample is Destroyed.
Added a few sounds & music.
Added long range attacks.
Added main menu.
You can now actually quit the game without using the console command. Finally!
UI resizeable, so it can fit into any screen resolution.
The game is now in fullscreen!
General bugfixing and performance improvement.



0.6 ----------------------------------------------------------

Added Kerrigan, simple attack patterns.
Added all missing units and buildings.
Improved pathfinding & unit collision, FPS should be more stable.
General BugFix & stabilization.
Added buildings spawn animation.



0.7 ----------------------------------------------------------

General BugFix, fixing crashes.
Re-balancing some units and IA.
Randomizing Zerg Base Generation.
Added more boss attacks: against buildings & more attack patterns.
Added UI while multiple units are selected.



0.8 ----------------------------------------------------------

General BugFix, added new sprites for feedback.
Added sounds for all units & buildings upon selection, attack, death, acknowledgement.
Added queue for buildings (logic & UI display).



0.9a ----------------------------------------------------------

General BugFix.



0.9b ----------------------------------------------------------

Still fixing bugs and balancing the game.
It's completely playable and all features should be in, just polish remaining!



0.9c ----------------------------------------------------------

Polishing sounds & interaction.
General bugfix.
Added Kerrigan Health Bar.
Added camera shortucts.



0.9 ----------------------------------------------------------

Game in Alpha state!
Added units & buldings shortcuts.
Cleaning debug features and removing them from the game while debug mode is deactivated.
Polished Kerrigan attack sprites.



0.9.1 --------------------------------------------------------

General improvement!
Graphical enchancment.
Added feedback on menus.
Save & Load games!
Optimized and smoother FPS.
Added three different difficulty levels.
Redesigned dependances tree and rebalanced a lot of units.
New Kerrigan attack sprites.



1.0 ----------------------------------------------------------

Feedback polish.
BugFix.
Re-balancing IA behaviour.
Changed Kerrigan attributes depending on the difficulty level.
Added "Inactive probe" button.






