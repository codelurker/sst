#include "sst.h"

static int height;

static char *classes[] = {"M","N","O"};

static int consumeTime(void) 
{
/* I think most of this avoidance was caused by overlay scheme.
   Let's see what happens if all events can occur here */

//  double asave;
    ididit = 1;
#if 0
    /* Don't wory about this */
    if (future[FTBEAM] <= game.state.date+Time && game.state.remcom != 0 && condit != IHDOCKED) {
	/* We are about to be tractor beamed -- operation fails */
	return 1;
    }
#endif
//	asave = future[FSNOVA];
//	future[FSNOVA] = 1e30; /* defer supernovas */
    events();	/* Used to avoid if future[FSCMOVE] within time */
//	future[FSNOVA] = asave;
    /*fails if game over, quadrant super-novas or we've moved to new quadrant*/
    if (alldone || game.state.galaxy[quadx][quady] == SUPERNOVA_PLACE || justin != 0) return 1;
    return 0;
}

void preport(void) 
{
    int iknow = 0, i;
    skip(1);
    chew();
    prout("Spock-  \"Planet report follows, Captain.\"");
    skip(1);
    for (i = 0; i < inplan; i++) {
	if (game.state.plnets[i].known != unknown
#ifdef DEBUG
	    || ( idebug && game.state.plnets[i].x !=0)
#endif
	    ) {
	    iknow = 1;
#ifdef DEBUG
	    if (idebug && game.state.plnets[i].known==unknown) proutn("(Unknown) ");
#endif
	    proutn(cramlc(quadrant, game.state.plnets[i].x, game.state.plnets[i].y));
	    proutn("   class ");
	    proutn(classes[game.state.plnets[i].pclass]);
	    proutn("   ");
	    if (game.state.plnets[i].crystals == 0) proutn("no ");
	    prout("dilithium crystals present.");
	    if (game.state.plnets[i].known==shuttle_down) 
		prout("    Shuttle Craft Galileo on surface.");
	}
    }
    if (iknow==0) prout("No information available.");
}

void orbit(void) 
{
    skip(1);
    chew();
    if (inorbit!=0) {
	prout("Already in standard orbit.");
	return;
    }
    if (game.damage[DWARPEN] != 0 && game.damage[DIMPULS] != 0) {
	prout("Both warp and impulse engines damaged.");
	return;
    }
    if (plnetx == 0 || abs(sectx-plnetx) > 1 || abs(secty-plnety) > 1) {
	crmshp();
	prout(" not adjacient to planet.\n");
	return;
    }
    Time = 0.02+0.03*Rand();
    prout("Helmsman Sulu-  \"Entering standard orbit, Sir.\"");
    newcnd();
    if (consumeTime()) return;
    height = (1400.+7200.*Rand());
    prout("Sulu-  \"Entered orbit at altitude %.2f kilometers.\"", height);
    inorbit = 1;
    ididit=1;
}

void sensor(int force) 
{
    skip(1);
    chew();
    if (game.damage[DSRSENS] != 0.0) {
	prout("Short range sensors damaged.");
	return;
    }
    if (!plnetx && ((game.state.plnets[iplnet].known == unknown || force))) {
	prout("Spock- \"No planet in this quadrant, Captain.\"");
	return;
    }
    if ((plnetx != 0)&&(game.state.plnets[iplnet].known == unknown)) {
	prout("Spock-  \"Sensor scan for %s-", cramlc(quadrant, quadx, quady));
	skip(1);
	prout("         Planet at %s is of class %s.", 
	      cramlc(sector, plnetx, plnety),
	      classes[game.state.plnets[iplnet].pclass]);
	if (game.state.plnets[iplnet].known==shuttle_down) 
	    prout("         Sensors show Galileo still on surface.");
	proutn("         Readings indicate");
	if (game.state.plnets[iplnet].crystals == 0) proutn(" no");
	prout(" dilithium crystals present.\"");
	if (game.state.plnets[iplnet].known == unknown) game.state.plnets[iplnet].known = known;
    }
}

void beam(void) 
{
    chew();
    skip(1);
    if (game.damage[DTRANSP] != 0) {
	prout("Transporter damaged.");
	if (game.damage[DSHUTTL]==0 && (game.state.plnets[iplnet].known==shuttle_down || iscraft == 1)) {
	    skip(1);
	    proutn("Spock-  \"May I suggest the shuttle craft, Sir?\" ");
	    if (ja() != 0) shuttle();
	}
	return;
    }
    if (inorbit==0) {
	crmshp();
	prout(" not in standard orbit.");
	return;
    }
    if (shldup!=0) {
	prout("Impossible to transport through shields.");
	return;
    }
    if (game.state.plnets[iplnet].known==unknown) {
	prout("Spock-  \"Captain, we have no information on this planet");
	prout("  and Starfleet Regulations clearly state that in this situation");
	prout("  you may not go down.\"");
	return;
    }
    if (landed==1) {
	/* Coming from planet */
	if (game.state.plnets[iplnet].known==shuttle_down) {
	    proutn("Spock-  \"Wouldn't you rather take the Galileo?\" ");
	    if (ja() != 0) {
		chew();
		return;
	    }
	    prout("Your crew hides the Galileo to prevent capture by aliens.");
	}
	prout("Landing party assembled, ready to beam up.");
	skip(1);
	prout("Kirk whips out communicator...");
	prouts("BEEP  BEEP  BEEP");
	skip(2);
	prout("\"Kirk to enterprise-  Lock on coordinates...energize.\"");
    }
    else {
	/* Going to planet */
	if (game.state.plnets[iplnet].crystals==0) {
	    prout("Spock-  \"Captain, I fail to see the logic in");
	    prout("  exploring a planet with no dilithium crystals.");
	    proutn("  Are you sure this is wise?\" ");
	    if (ja()==0) {
		chew();
		return;
	    }
	}
	prout("Scotty-  \"Transporter room ready, Sir.\"");
	skip(1);
	prout("Kirk, and landing party prepare to beam down to planet surface.");
	skip(1);
	prout("Kirk-  \"Energize.\"");
    }
    ididit=1;
    skip(1);
    prouts("WWHOOOIIIIIRRRRREEEE.E.E.  .  .  .  .   .    .");
    skip(2);
    if (Rand() > 0.98) {
	prouts("BOOOIIIOOOIIOOOOIIIOIING . . .");
	skip(2);
	prout("Scotty-  \"Oh my God!  I've lost them.\"");
	finish(FLOST);
	return;
    }
    prouts(".    .   .  .  .  .  .E.E.EEEERRRRRIIIIIOOOHWW");
    skip(2);
    prout("Transport complete.");
    landed = -landed;
    if (landed==1 && game.state.plnets[iplnet].known==shuttle_down) {
	prout("The shuttle craft Galileo is here!");
    }
    if (landed!=1 && imine==1) {
	icrystl = 1;
	cryprob = 0.05;
    }
    imine = 0;
    return;
}

void mine(void) 
{
    skip(1);
    chew();
    if (landed!= 1) {
	prout("Mining party not on planet.");
	return;
    }
    if (game.state.plnets[iplnet].crystals == 0) {
	prout("No dilithium crystals on this planet.");
	return;
    }
    if (imine == 1) {
	prout("You've already mined enough crystals for this trip.");
	return;
    }
    if (icrystl == 1 && cryprob == 0.05) {
	proutn("With all those fresh crystals aboard the ");
	crmshp();
	skip(1);
	prout("there's no reason to mine more at this time.");
	return;
    }
    Time = (0.1+0.2*Rand())*game.state.plnets[iplnet].pclass;
    if (consumeTime()) return;
    prout("Mining operation complete.");
    imine = 1;
    ididit=1;
}

void usecrystals(void) 
{
    ididit=0;
    skip(1);
    chew();
    if (icrystl!=1) {
	prout("No dilithium crystals available.");
	return;
    }
    if (energy >= 1000) {
	prout("Spock-  \"Captain, Starfleet Regulations prohibit such an operation");
	prout("  except when condition Yellow exists.");
	return;
    }
    prout("Spock- \"Captain, I must warn you that loading");
    prout("  raw dilithium crystals into the ship's power");
    prout("  system may risk a severe explosion.");
    proutn("  Are you sure this is wise?\" ");
    if (ja()==0) {
	chew();
	return;
    }
    skip(1);
    prout("Engineering Officer Scott-  \"(GULP) Aye Sir.");
    prout("  Mr. Spock and I will try it.\"");
    skip(1);
    prout("Spock-  \"Crystals in place, Sir.");
    prout("  Ready to activate circuit.\"");
    skip(1);
    prouts("Scotty-  \"Keep your fingers crossed, Sir!\"");
    skip(1);
    if (Rand() <= cryprob) {
	prouts("  \"Activating now! - - No good!  It's***");
	skip(2);
	prouts("***RED ALERT!  RED A*L********************************");
	skip(1);
	stars();
	prouts("******************   KA-BOOM!!!!   *******************");
	skip(1);
	kaboom();
	return;
    }
    energy += 5000.0*(1.0 + 0.9*Rand());
    prouts("  \"Activating now! - - ");
    prout("The instruments");
    prout("   are going crazy, but I think it's");
    prout("   going to work!!  Congratulations, Sir!\"");
    cryprob *= 2.0;
    ididit=1;
}

void shuttle(void) 
{
    chew();
    skip(1);
    if(game.damage[DSHUTTL] != 0.0) {
	if (game.damage[DSHUTTL] == -1.0) {
	    if (inorbit && game.state.plnets[iplnet].known == shuttle_down)
		prout("Ye Faerie Queene has no shuttle craft bay to dock it at.");
	    else
		prout("Ye Faerie Queene had no shuttle craft.");
	}
	else if (game.damage[DSHUTTL] > 0)
	    prout("The Galileo is damaged.");
	else prout("Shuttle craft is now serving Big Macs.");
	return;
    }
    if (inorbit==0) {
	crmshp();
	prout(" not in standard orbit.");
	return;
    }
    if ((game.state.plnets[iplnet].known != shuttle_down) && iscraft != 1) {
	prout("Shuttle craft not currently available.");
	return;
    }
    if (landed==-1 && game.state.plnets[iplnet].known==shuttle_down) {
	prout("You will have to beam down to retrieve the shuttle craft.");
	return;
    }
    if (shldup!=0 || condit == IHDOCKED) {
	prout("Shuttle craft cannot pass through shields.");
	return;
    }
    if (game.state.plnets[iplnet].known==unknown) {
	prout("Spock-  \"Captain, we have no information on this planet");
	prout("  and Starfleet Regulations clearly state that in this situation");
	prout("  you may not fly down.\"");
	return;
    }
    Time = 3.0e-5*height;
    if (Time >= 0.8*game.state.remtime) {
	prout("First Officer Spock-  \"Captain, I compute that such");
	proutn("  a maneuver would require approximately 2d%% of our",
	       (int)(100*Time/game.state.remtime));
	prout("remaining time.");
	proutn("Are you sure this is wise?\" ");
	if (ja()==0) {
	    Time = 0.0;
	    return;
	}
    }
    if (landed == 1) {
	/* Kirk on planet */
	if (iscraft==1) {
	    /* Galileo on ship! */
	    if (game.damage[DTRANSP]==0) {
		proutn("Spock-  \"Would you rather use the transporter?\" ");
		if (ja() != 0) {
		    beam();
		    return;
		}
		proutn("Shuttle crew");
	    }
	    else
		proutn("Rescue party");
	    prout(" boards Galileo and swoops toward planet surface.");
	    iscraft = 0;
	    skip(1);
	    if (consumeTime()) return;
	    game.state.plnets[iplnet].known=shuttle_down;
	    prout("Trip complete.");
	    return;
	}
	else {
	    /* Ready to go back to ship */
	    prout("You and your mining party board the");
	    prout("shuttle craft for the trip back to the Enterprise.");
	    skip(1);
	    prout("The short hop begins . . .");
	    game.state.plnets[iplnet].known=known;
	    icraft = 1;
	    skip(1);
	    landed = -1;
	    if (consumeTime()) return;
	    iscraft = 1;
	    icraft = 0;
	    if (imine!=0) {
		icrystl = 1;
		cryprob = 0.05;
	    }
	    imine = 0;
	    prout("Trip complete.");
	    return;
	}
    }
    else {
	/* Kirk on ship */
	/* and so is Galileo */
	prout("Mining party assembles in the hangar deck,");
	prout("ready to board the shuttle craft \"Galileo\".");
	skip(1);
	prouts("The hangar doors open; the trip begins.");
	skip(1);
	icraft = 1;
	iscraft = 0;
	if (consumeTime()) return;
	game.state.plnets[iplnet].known = shuttle_down;
	landed = 1;
	icraft = 0;
	prout("Trip complete");
	return;
    }
}

void deathray(void) 
{
    double r = Rand();
	
    ididit = 0;
    skip(1);
    chew();
    if (ship != IHE) {
	prout("Ye Faerie Queene has no death ray.");
	return;
    }
    if (nenhere==0) {
	prout("Sulu-  \"But Sir, there are no enemies in this quadrant.\"");
	return;
    }
    if (game.damage[DDRAY] > 0.0) {
	prout("Death Ray is damaged.");
	return;
    }
    prout("Spock-  \"Captain, the 'Experimental Death Ray'");
    prout("  is highly unpredictible.  Considering the alternatives,");
    proutn("  are you sure this is wise?\" ");
    if (ja()==0) return;
    prout("Spock-  \"Acknowledged.\"");
    skip(1);
    ididit=1;
    prouts("WHOOEE ... WHOOEE ... WHOOEE ... WHOOEE");
    skip(1);
    prout("Crew scrambles in emergency preparation.");
    prout("Spock and Scotty ready the death ray and");
    prout("prepare to channel all ship's power to the device.");
    skip(1);
    prout("Spock-  \"Preparations complete, sir.\"");
    prout("Kirk-  \"Engage!\"");
    skip(1);
    prouts("WHIRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR");
    skip(1);
    if (r > .30) {
	prouts("Sulu- \"Captain!  It's working!\"");
	skip(2);
	while (nenhere > 0)
	    deadkl(game.kx[1],game.ky[1],game.quad[game.kx[1]][game.ky[1]],game.kx[1],game.ky[1]);
	prout("Ensign Chekov-  \"Congratulations, Captain!\"");
	if (game.state.remkl == 0) finish(FWON);
	prout("Spock-  \"Captain, I believe the `Experimental Death Ray'");
	if (Rand() <= 0.05) {
	    prout("   is still operational.\"");
	}
	else {
	    prout("   has been rendered nonfunctional.\"");
	    game.damage[DDRAY] = 39.95;
	}
	return;
    }
    r = Rand();	// Pick failure method 
    if (r <= .30) {
	prouts("Sulu- \"Captain!  It's working!\"");
	skip(1);
	prouts("***RED ALERT!  RED ALERT!");
	skip(1);
	prout("***MATTER-ANTIMATTER IMPLOSION IMMINENT!");
	skip(1);
	prouts("***RED ALERT!  RED A*L********************************");
	skip(1);
	stars();
	prouts("******************   KA-BOOM!!!!   *******************");
	skip(1);
	kaboom();
	return;
    }
    if (r <= .55) {
	prouts("Sulu- \"Captain!  Yagabandaghangrapl, brachriigringlanbla!\"");
	skip(1);
	prout("Lt. Uhura-  \"Graaeek!  Graaeek!\"");
	skip(1);
	prout("Spock-  \"Fascinating!  . . . All humans aboard");
	prout("  have apparently been transformed into strange mutations.");
	prout("  Vulcans do not seem to be affected.");
	skip(1);
	prout("Kirk-  \"Raauch!  Raauch!\"");
	finish(FDRAY);
	return;
    }
    if (r <= 0.75) {
	int i,j;
	prouts("Sulu- \"Captain!  It's   --WHAT?!?!\"");
	skip(2);
	proutn("Spock-  \"I believe the word is");
	prouts(" *ASTONISHING*");
	prout(" Mr. Sulu.");
	for (i=1; i<=QUADSIZE; i++)
	    for (j=1; j<=QUADSIZE; j++)
		if (game.quad[i][j] == IHDOT) game.quad[i][j] = IHQUEST;
	prout("  Captain, our quadrant is now infested with");
	prouts(" - - - - - -  *THINGS*.");
	skip(1);
	prout("  I have no logical explanation.\"");
	return;
    }
    prouts("Sulu- \"Captain!  The Death Ray is creating tribbles!\"");
    skip(1);
    prout("Scotty-  \"There are so many tribbles down here");
    prout("  in Engineering, we can't move for 'em, Captain.\"");
    finish(FTRIBBLE);
    return;
}
