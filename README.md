# RCFiosOverlay
SceFios2 overlayer for Ratchet &amp; Clank HD.

Allows to painlessly replace files from the PSARC.

# Usage
Install `RCFiosOverlay.suprx` in the folder of your choice, then add it to your taiHEN config's `*ALL` section.

Or add it to the wanted games' section, namely :
 * `*PCSF00482` for *The Ratchet & Clank Trilogy HD* (PAL)
 * `*PCSF00484` for *Ratchet & Clank 1 HD* (PAL)
 * `*PCSF00485` for *Ratchet & Clank 2 HD* (PAL)
 * `*PCSF00486` for *Ratchet & Clank 3 HD* (PAL)
 * `*PCSA00133` for *The Ratchet & Clank Collection* (US)


Then, place the files you want to replace at `PREFIX/real/path/in/PSARC`, with prefix being the following :
 * Ratchet & Clank 1 : `ux0:data/RC1`
 * Ratchet & Clank 2 : `ux0:data/RC2`
 * Ratchet & Clank 3 : `ux0:data/RC3`
 * Ratchet & Clank Collection / Trilogy HD : `ux0:data/RCC` 
   * **note that this only applies for the game select menu, use the per-game prefix otherwise**

For example, if you want to replace `armor0.ps3` from Ratchet & Clank 3, located at `rc3/psp2data/global/armor/armor0.ps3`, 
place the replacement file at `ux0:data/RC3/rc3/psp2data/global/armor/armor0.ps3`.

**At the moment, the US trilogy (`The Ratchet & Clank Collection`) is NOT supported !**
