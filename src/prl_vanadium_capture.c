/*
 * Legacy intro entry retained for source compatibility only.
 * Noxichu's capture cinematic was removed from PRL's opening at user request.
 * Noxichu remains a Pokémon and on the approved title artwork.
 */
#include "global.h"

extern void CB2_InitPRLHwlScene0(void);

void CB2_InitPRLIntroTrial(void)
{
    CB2_InitPRLHwlScene0();
}
