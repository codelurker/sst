#include "sst.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

void attakreport(int curt) 
{
    if (!curt) {
	if (game.future[FCDBAS] < 1e30) {
	    prout("Starbase in %s is currently under Commander attack.",
		  cramlc(quadrant, batx, baty));
	    prout("It can hold out until Stardate %d.", 
		  (int)game.future[FCDBAS]);
	}
	if (isatb == 1) {
	    prout("Starbase in %s is under Super-commander attack.",
		  cramlc(quadrant, game.state.isx, game.state.isy));
	    prout("It can hold out until Stardate %d.", 
		  (int)game.future[FSCDBAS]);
	}
    } else {
        if (game.future[FCDBAS] < 1e30)
	    proutn("Base in %i - %i attacked by C. Alive until %.1f", batx, baty, game.future[FCDBAS]);
        if (isatb == 1)
	    proutn("Base in %i - %i attacked by S. Alive until %.1f", game.state.isx, game.state.isy, game.future[FSCDBAS]);
    }
    clreol();
}
	

void report(void) 
{
    char *s1,*s2,*s3;

    chew();
    s1 = (thawed?"thawed ":"");
    switch (length) {
    case 1: s2="short"; break;
    case 2: s2="medium"; break;
    case 4: s2="long"; break;
    default: s2="unknown length"; break;
    }
    switch (skill) {
    case SKILL_NOVICE: s3="novice"; break;
    case SKILL_FAIR: s3="fair"; break;
    case SKILL_GOOD: s3="good"; break;
    case SKILL_EXPERT: s3="expert"; break;
    case SKILL_EMERITUS: s3="emeritus"; break;
    default: s3="skilled"; break;
    }
    prout("");
    prout("You %s playing a %s%s %s game.",
	  alldone? "were": "are now", s1, s2, s3);
    if (skill>SKILL_GOOD && thawed && !alldone) prout("No plaque is allowed.");
    if (tourn) prout("This is tournament game %d.", tourn);
    prout("Your secret password is \"%s\"",game.passwd);
    proutn("%d of %d Klingons have been killed",
	   game.state.killk+game.state.killc+game.state.nsckill, inkling);
    if (game.state.killc) prout(", including %d Commander%s.", game.state.killc, game.state.killc==1?"":"s");
    else if (game.state.killk+game.state.nsckill > 0) prout(", but no Commanders.");
    else prout(".");
    if (skill > SKILL_FAIR) prout("The Super Commander has %sbeen destroyed.",
				  game.state.nscrem?"not ":"");
    if (game.state.rembase != inbase) {
	proutn("There ");
	if (inbase-game.state.rembase==1) proutn("has been 1 base");
	else {
	    proutn("have been %d bases", inbase-game.state.rembase);
	}
	prout(" destroyed, %d remaining.", game.state.rembase);
    }
    else prout("There are %d bases.", inbase);
    if (game.damage[DRADIO] == 0.0 || condit == IHDOCKED || iseenit) {
	/* Don't report this if not seen and
	   either the radio is dead or not at base! */
	attakreport(0);
	iseenit = 1;
    }
    if (casual) prout("%d casualt%s suffered so far.",
		      casual, casual==1? "y" : "ies");
    if (nhelp) prout("There were %d call%s for help.",
		     nhelp, nhelp==1 ? "" : "s");
    if (ship == IHE) {
	proutn("You have ");
	if (nprobes) proutn("%d", nprobes);
	else proutn("no");
	proutn(" deep space probe");
	if (nprobes!=1) proutn("s");
	prout(".");
    }
    if ((game.damage[DRADIO] == 0.0 || condit == IHDOCKED)&&
	game.future[FDSPROB] != 1e30) {
	if (isarmed) 
	    proutn("An armed deep space probe is in");
	else
	    proutn("A deep space probe is in");
	proutn(cramlc(quadrant, probecx, probecy));
	prout(".");
    }
    if (icrystl) {
	if (cryprob <= .05)
	    prout("Dilithium crystals aboard ship... not yet used.");
	else {
	    int i=0;
	    double ai = 0.05;
	    while (cryprob > ai) {
		ai *= 2.0;
		i++;
	    }
	    prout("Dilithium crystals have been used %d time%s.",
		  i, i==1? "" : "s");
	}
    }
    skip(1);
}
	
void lrscan(void) 
{
    int x, y;
    chew();
    if (game.damage[DLRSENS] != 0.0) {
	/* Now allow base's sensors if docked */
	if (condit != IHDOCKED) {
	    prout("LONG-RANGE SENSORS DAMAGED.");
	    return;
	}
	proutn("Starbase's long-range scan");
    }
    else {
	prout("Long-range scan");
    }
    for (x = quadx-1; x <= quadx+1; x++) {
	proutn(" ");
	for (y = quady-1; y <= quady+1; y++) {
	    if (x == 0 || x > GALSIZE || y == 0 || y > GALSIZE)
		proutn("  -1");
	    else {
		if (game.state.galaxy[x][y]<SUPERNOVA_PLACE) proutn(" %3d", game.state.galaxy[x][y]);
		else proutn("***");
		game.starch[x][y] = game.damage[DRADIO] > 0 ? game.state.galaxy[x][y]+SUPERNOVA_PLACE : 1;
	    }
	}
	prout(" ");
    }
}

void dreprt(void) 
{
    int jdam = FALSE, i;
    chew();

    for (i = 1; i <= NDEVICES; i++) {
	if (game.damage[i] > 0.0) {
	    if (!jdam) {
		prout("DEVICE            -REPAIR TIMES-");
		prout("                IN FLIGHT   DOCKED");
		jdam = TRUE;
	    }
	    prout("  %16s %8.2f  %8.2f", 
		  device[i],
		  game.damage[i]+0.05,
		  docfac*game.damage[i]+0.005);
	}
    }
    if (!jdam) prout("All devices functional.");
}

void chart(int nn) 
{
    int i,j;
    char *cp;
    chew();
    if (stdamtim != 1e30 && stdamtim != game.state.date && condit == IHDOCKED) {
	proutn("Spock-  \"I revised the Star Chart from the starbase's records.\"\n\r");
    }
    if (nn == 0) proutn("       STAR CHART FOR THE KNOWN GALAXY\n\r");
    if (stdamtim != 1e30) {
	if (condit == IHDOCKED) {
	    /* We are docked, so restore chart from base information */
	    stdamtim = game.state.date;
	    for (i=1; i <= GALSIZE ; i++)
		for (j=1; j <= GALSIZE; j++)
		    if (game.starch[i][j] == 1) game.starch[i][j] = game.state.galaxy[i][j]+SUPERNOVA_PLACE;
	}
	else {
	    proutn("(Last surveillance update %d stardates ago.",
		   (int)(game.state.date-stdamtim));
	}
    }

    prout("      1    2    3    4    5    6    7    8");
    for (i = 1; i <= GALSIZE; i++) {
	proutn("%d |", i);
	for (j = 1; j <= GALSIZE; j++) {
	    char buf[4];
	    proutn("  ");
	    if (game.starch[i][j] == CHART_UNKNOWN)
		strcpy(buf, ".1.");
	    else if (game.starch[i][j] == 0)
		strcpy(buf, "...");
	    else if (game.state.galaxy[i][j]>=SUPERNOVA_PLACE)
		strcpy(buf, "***");
	    else
		sprintf(buf, "%03d", game.state.galaxy[i][j]);
	    for (cp = buf; cp < buf + sizeof(buf); cp++)
		if (*cp == '0')
		    *cp = '.';
	    proutn(buf);
	}
	proutn("  |");
	if (i<GALSIZE) proutn("\n\r");
    }
    prout("");	/* flush output */
}

static void sectscan(int goodScan, int i, int j) 
{
    if (goodScan || (abs(i-sectx)<= 1 && abs(j-secty) <= 1)){
	if ((game.quad[i][j]==IHMATER0)||(game.quad[i][j]==IHMATER1)||(game.quad[i][j]==IHMATER2)||(game.quad[i][j]==IHE)||(game.quad[i][j]==IHF)){
	    switch (condit) {
	    case IHRED: textcolor(RED); break;
	    case IHGREEN: textcolor(GREEN); break;
	    case IHYELLOW: textcolor(YELLOW); break;
	    case IHDOCKED: textcolor(CYAN); break;
	    case IHDEAD: textcolor(BROWN);
	    }
	    if (game.quad[i][j] != ship) 
		highvideo();
	}
	if (game.quad[i][j] & DAMAGED) 
	    highvideo();
	proutn("%c ",game.quad[i][j] & ~DAMAGED);
	textcolor(DEFAULT);
    }
    else
	proutn("- ");
}

static void status(int req) 
{
    char *cp = NULL;
    int t, dam = 0;
    switch (req) {
    case 1:
	proutn("Stardate      %.1f, Time Left %.2f", game.state.date, game.state.remtime);
	break;
    case 2:
	if (condit != IHDOCKED) newcnd();
	switch (condit) {
	case IHRED: cp = "RED"; break;
	case IHGREEN: cp = "GREEN"; break;
	case IHYELLOW: cp = "YELLOW"; break;
	case IHDOCKED: cp = "DOCKED"; break;
	case IHDEAD: cp="DEAD"; break;
	}
	for (t=0;t<=NDEVICES;t++)
	    if (game.damage[t]>0) dam++;
	proutn("Condition     %s, %i DAMAGES", cp, dam);
	break;
    case 3:
	proutn("Position      %d - %d , %d - %d",
	       quadx, quady, sectx, secty);
	break;
    case 4:
	proutn("Life Support  ");
	if (game.damage[DLIFSUP] != 0.0) {
	    if (condit == IHDOCKED)
		proutn("DAMAGED, Base provides");
	    else
		proutn("DAMAGED, reserves=%4.2f", lsupres);
	}
	else
	    proutn("ACTIVE");
	break;
    case 5:
	proutn("Warp Factor   %.1f", warpfac);
	break;
    case 6:
	proutn("Energy        %.2f", energy);
	if (icrystl)	/* ESR */
	    proutn(" (have crystals)");
	break;
    case 7:
	proutn("Torpedoes     %d", torps);
	break;
    case 8:
	proutn("Shields       ");
	if (game.damage[DSHIELD] != 0)
	    proutn("DAMAGED,");
	else if (shldup)
	    proutn("UP,");
	else
	    proutn("DOWN,");
	proutn(" %d%% %.1f units",
	       (int)((100.0*shield)/inshld + 0.5), shield);
	break;
    case 9:
	proutn("Klingons Left %d", game.state.remkl);
	break;
    case 10:
	attakreport(1);
	break;
	/*
	 * Note: attakreport() can in some cases produce two lines of
	 * output.  If that happens, and QUADSIZE is the normal 10, items
	 * 11 and up will be printed past the bottom of the quadrant display.
	 * Under the curses display logic they will get lost because they're
	 * written outside the report window.
	 */
    case 11:	/* ESR */
	proutn("Bases Left    %d", game.state.rembase);
	break;
    }
}
		
int srscan(int l) 
{
    static char requests[][3] =
	{"","da","co","po","ls","wa","en","to","sh","kl","ti", "ba"};
    int leftside=TRUE, rightside=TRUE, i, j, jj, req=0, nn=FALSE;
    int goodScan=TRUE;
    switch (l) {
    case SCAN_FULL: // SRSCAN
	if (game.damage[DSRSENS] != 0) {
	    /* Allow base's sensors if docked */
	    if (condit != IHDOCKED) {
		prout("   S.R. SENSORS DAMAGED!");
		goodScan=FALSE;
	    }
	    else
		prout("  [Using Base's sensors]");
	}
	else proutn("     Short-range scan\n\r");
	if (goodScan) game.starch[quadx][quady] = game.damage[DRADIO]>0.0 ? game.state.galaxy[quadx][quady]+SUPERNOVA_PLACE:1;
	scan();
	if (isit("chart")) nn = TRUE;
	if (isit("no")) rightside = FALSE;
	chew();
	proutn("    1 2 3 4 5 6 7 8 9 10\n\r");
	break;
    case SCAN_REQUEST:
	while (scan() == IHEOL)
	    proutn("Information desired? ");
	chew();
	for (req = 1; req <= sizeof(requests)/sizeof(requests[0]); req++)
	    if (strncmp(citem,requests[req],min(2,strlen(citem)))==0)
		break;
	if (req > sizeof(requests)/sizeof(requests[0])) {
	    prout("UNRECOGNIZED REQUEST. Legal requests are:\n"
		  "  date, condition, position, lsupport, warpfactor,\n"
		  "  energy, torpedoes, shields, klingons, time, bases.");
	    return FALSE;
	}
	// no break
    case SCAN_STATUS: // STATUS
	chew();
	leftside = FALSE;
	skip(1);
	// no break
    case SCAN_NO_LEFTSIDE: // REQUEST
	leftside=FALSE;
	break;
    }
    if (condit != IHDOCKED) newcnd();
    for (i = 1; i <= max(QUADSIZE, sizeof(requests)/sizeof(requests[0])); i++) {
	jj = (req!=0 ? req : i);
	if (leftside && i <= QUADSIZE) {
	    proutn("%2d  ", i);
	    for (j = 1; j <= QUADSIZE; j++) {
		sectscan(goodScan, i, j);
	    }
	}
	if (rightside)
	    status(jj);
	if (i<sizeof(requests)/sizeof(requests[0])) proutn("\n\r");
	if (req!=0) return(goodScan);
    }
    prout("");
    if (nn) chart(1);
    return(goodScan);
}
			
			
void eta(void)
{
    int ix1, ix2, iy1, iy2, prompt=FALSE;
    int wfl;
    double ttime, twarp, tpower;
    if (game.damage[DCOMPTR] != 0.0) {
	prout("COMPUTER DAMAGED, USE A POCKET CALCULATOR.");
	skip(1);
	return;
    }
    if (scan() != IHREAL) {
	prompt = TRUE;
	chew();
	proutn("Destination quadrant and/or sector? ");
	if (scan()!=IHREAL) {
	    huh();
	    return;
	}
    }
    iy1 = aaitem +0.5;
    if (scan() != IHREAL) {
	huh();
	return;
    }
    ix1 = aaitem + 0.5;
    if (scan() == IHREAL) {
	iy2 = aaitem + 0.5;
	if (scan() != IHREAL) {
	    huh();
	    return;
	}
	ix2 = aaitem + 0.5;
    }
    else {
	if (quady>ix1) ix2 = 1;
	else ix2=QUADSIZE;
	if (quadx>iy1) iy2 = 1;
	else iy2=QUADSIZE;
    }

    if (ix1 > GALSIZE || ix1 < 1 || iy1 > GALSIZE || iy1 < 1 ||
	ix2 > QUADSIZE || ix2 < 1 || iy2 > QUADSIZE || iy2 < 1) {
	huh();
	return;
    }
    dist = sqrt(square(iy1-quadx+0.1*(iy2-sectx))+
		square(ix1-quady+0.1*(ix2-secty)));
    wfl = FALSE;

    if (prompt) prout("Answer \"no\" if you don't know the value:");
    while (TRUE) {
	chew();
	proutn("Time or arrival date? ");
	if (scan()==IHREAL) {
	    ttime = aaitem;
	    if (ttime > game.state.date) ttime -= game.state.date; // Actually a star date
	    if (ttime <= 1e-10 ||
		(twarp=(floor(sqrt((10.0*dist)/ttime)*10.0)+1.0)/10.0) > 10) {
		prout("We'll never make it, sir.");
		chew();
		return;
	    }
	    if (twarp < 1.0) twarp = 1.0;
	    break;
	}
	chew();
	proutn("Warp factor? ");
	if (scan()== IHREAL) {
	    wfl = TRUE;
	    twarp = aaitem;
	    if (twarp<1.0 || twarp > 10.0) {
		huh();
		return;
	    }
	    break;
	}
	prout("Captain, certainly you can give me one of these.");
    }
    while (TRUE) {
	chew();
	ttime = (10.0*dist)/square(twarp);
	tpower = dist*twarp*twarp*twarp*(shldup+1);
	if (tpower >= energy) {
	    prout("Insufficient energy, sir.");
	    if (shldup==0 || tpower > energy*2.0) {
		if (!wfl) return;
		proutn("New warp factor to try? ");
		if (scan() == IHREAL) {
		    wfl = TRUE;
		    twarp = aaitem;
		    if (twarp<1.0 || twarp > 10.0) {
			huh();
			return;
		    }
		    continue;
		}
		else {
		    chew();
		    skip(1);
		    return;
		}
	    }
	    prout("But if you lower your shields,");
	    proutn("remaining");
	    tpower /= 2;
	}
	else
	    proutn("Remaining");
	prout(" energy will be %.2f.", energy-tpower);
	if (wfl) {
	    prout("And we will arrive at stardate %.2f.",
		  game.state.date+ttime);
	}
	else if (twarp==1.0)
	    prout("Any warp speed is adequate.");
	else {
	    prout("Minimum warp needed is %.2f,", twarp);
	    prout("and we will arrive at stardate %.2f.",
		  game.state.date+ttime);
	}
	if (game.state.remtime < ttime)
	    prout("Unfortunately, the Federation will be destroyed by then.");
	if (twarp > 6.0)
	    prout("You'll be taking risks at that speed, Captain");
	if ((isatb==1 && game.state.isy == ix1 && game.state.isx == iy1 &&
	     game.future[FSCDBAS]< ttime+game.state.date)||
	    (game.future[FCDBAS]<ttime+game.state.date && baty==ix1 && batx == iy1))
	    prout("The starbase there will be destroyed by then.");
	proutn("New warp factor to try? ");
	if (scan() == IHREAL) {
	    wfl = TRUE;
	    twarp = aaitem;
	    if (twarp<1.0 || twarp > 10.0) {
		huh();
		return;
	    }
	}
	else {
	    chew();
	    skip(1);
	    return;
	}
    }
			
}
