#include "sst.h"
#include <string.h>
#include <time.h>

void dstrct() {
	/* Finish with a BANG! */
	chew();
	if (game.damage[DCOMPTR] != 0.0) {
		prout("Computer damaged; cannot execute destruct sequence.");
		return;
	}
	prouts("---WORKING---"); skip(1);
        prouts("SELF-DESTRUCT-SEQUENCE-ACTIVATED"); skip(1);
	prouts("   10"); skip(1);
	prouts("       9"); skip(1);
	prouts("          8"); skip(1);
	prouts("             7"); skip(1);
	prouts("                6"); skip(1);
	skip(1);
	prout("ENTER-CORRECT-PASSWORD-TO-CONTINUE-");
	skip(1);
	prout("SELF-DESTRUCT-SEQUENCE-OTHERWISE-");
	skip(1);
	prout("SELF-DESTRUCT-SEQUENCE-WILL-BE-ABORTED");
	skip(1);
	scan();
	chew();
	if (strcmp(game.passwd, citem) != 0) {
		prouts("PASSWORD-REJECTED;"); skip(1);
		prouts("CONTINUITY-EFFECTED");
		skip(2);
		return;
	}
	prouts("PASSWORD-ACCEPTED"); skip(1);
	prouts("                   5"); skip(1);
	prouts("                      4"); skip(1);
	prouts("                         3"); skip(1);
	prouts("                            2"); skip(1);
	prouts("                              1"); skip(1);
	if (Rand() < 0.15) {
		prouts("GOODBYE-CRUEL-WORLD");
		skip(1);
	}
	kaboom();
}

void kaboom(void) {
	stars();
	if (ship==IHE) prouts("***");
	prouts("********* Entropy of ");
	crmshp();
	prouts(" maximized *********");
	skip(1);
	stars();
	skip(1);
	if (nenhere != 0) {
		double whammo = 25.0 * energy;
		int l=1;
		while (l <= nenhere) {
			if (game.kpower[l]*game.kdist[l] <= whammo) 
				deadkl(game.kx[l],game.ky[l], game.quad[game.kx[l]][game.ky[l]], game.kx[l], game.ky[l]);
			l++;
		}
	}
	finish(FDILITHIUM);
}
				

void finish(FINTYPE ifin) {
	int igotit = 0;
	alldone = 1;
	skip(3);
	prout("It is stardate %.1f.", game.state.date);
        skip(1);
	switch (ifin) {
		case FWON: // Game has been won
			if (game.state.nromrem != 0)
				prout("The remaining %d Romulans surrender to Starfleet Command.",
					   game.state.nromrem);

			prout("You have smashed the Klingon invasion fleet and saved");
			prout("the Federation.");
			gamewon=1;
			if (alive) {
				double badpt;
				badpt = 5.*game.state.starkl + casual + 10.*game.state.nplankl +
						45.*nhelp+100.*game.state.basekl;
				if (ship == IHF) badpt += 100.0;
				else if (ship == 0) badpt += 200.0;
				if (badpt < 100.0) badpt = 0.0;	// Close enough!
				if (game.state.date-indate < 5.0 ||
					// killsPerDate >= RateMax
					(game.state.killk+game.state.killc+game.state.nsckill)/(game.state.date-indate) >=
					0.1*skill*(skill+1.0) + 0.1 + 0.008*badpt) {
					skip(1);
					prout("In fact, you have done so well that Starfleet Command");
					switch (skill) {
						case 1:
							prout("promotes you one step in rank from \"Novice\" to \"Fair\".");
							break;
						case 2:
							prout("promotes you one step in rank from \"Fair\" to \"Good\".");
							break;
						case 3:
							prout("promotes you one step in rank from \"Good\" to \"Expert\".");
							break;
						case 4:
							prout("promotes you to Commodore Emeritus.");
							skip(1);
							prout("Now that you think you're really good, try playing");
							prout("the \"Emeritus\" game. It will splatter your ego.");
							break;
						case 5:
							skip(1);
                                                        proutn("Computer-  ");
							prouts("ERROR-ERROR-ERROR-ERROR");
                                                        skip(2);
                                                        prouts("  YOUR-SKILL-HAS-EXCEEDED-THE-CAPACITY-OF-THIS-PROGRAM");
                                                        skip(1);
                                                        prouts("  THIS-PROGRAM-MUST-SURVIVE");
                                                        skip(1);
                                                        prouts("  THIS-PROGRAM-MUST-SURVIVE");
							skip(1);
                                                        prouts("  THIS-PROGRAM-MUST-SURVIVE");
							skip(1);
                                                        prouts("  THIS-PROGRAM-MUST?- MUST ? - SUR? ? -?  VI");
                                                        skip(2);
							prout("Now you can retire and write your own Star Trek game!");
							skip(1);
							break;
					}
					if (skill > 3) {
						if (thawed
#ifdef DEBUG
							&& !idebug
#endif
							)
							prout("You cannot get a citation, so...");
						else {
							proutn("Do you want your Commodore Emeritus Citation printed? ");
							chew();
							if (ja()) {
								igotit = 1;
							}
						}
					}
				}
				// Only grant long life if alive (original didn't!)
				skip(1);
				prout("LIVE LONG AND PROSPER.");
			}
			score();
			if (igotit != 0) plaque();
			return;
		case FDEPLETE: // Federation Resources Depleted
			prout("Your time has run out and the Federation has been");
			prout("conquered.  Your starship is now Klingon property,");
			prout("and you are put on trial as a war criminal.  On the");
			proutn("basis of your record, you are ");
			if (game.state.remkl*3.0 > inkling) {
				prout("aquitted.");
				skip(1);
				prout("LIVE LONG AND PROSPER.");
			}
			else {
				prout("found guilty and");
				prout("sentenced to death by slow torture.");
				alive = 0;
			}
			score();
			return;
		case FLIFESUP:
			prout("Your life support reserves have run out, and");
			prout("you die of thirst, starvation, and asphyxiation.");
			prout("Your starship is a derelict in space.");
			break;
		case FNRG:
			prout("Your energy supply is exhausted.");
			skip(1);
			prout("Your starship is a derelict in space.");
			break;
		case FBATTLE:
			proutn("The ");
			crmshp();
			prout("has been destroyed in battle.");
			skip(1);
			prout("Dulce et decorum est pro patria mori.");
			break;
		case FNEG3:
			prout("You have made three attempts to cross the negative energy");
			prout("barrier which surrounds the galaxy.");
			skip(1);
			prout("Your navigation is abominable.");
			score();
			return;
		case FNOVA:
			prout("Your starship has been destroyed by a nova.");
			prout("That was a great shot.");
			skip(1);
			break;
		case FSNOVAED:
			proutn("The ");
			crmshp();
			prout(" has been fried by a supernova.");
			prout("...Not even cinders remain...");
			break;
		case FABANDN:
			prout("You have been captured by the Klingons. If you still");
			prout("had a starbase to be returned to, you would have been");
			prout("repatriated and given another chance. Since you have");
			prout("no starbases, you will be mercilessly tortured to death.");
			break;
		case FDILITHIUM:
			prout("Your starship is now an expanding cloud of subatomic particles");
			break;
		case FMATERIALIZE:
			prout("Starbase was unable to re-materialize your starship.");
			prout("Sic transit gloria muntdi");
			break;
		case FPHASER:
			proutn("The ");
			crmshp();
			prout(" has been cremated by its own phasers.");
			break;
		case FLOST:
			prout("You and your landing party have been");
			prout("converted to energy, disipating through space.");
			break;
		case FMINING:
			prout("You are left with your landing party on");
			prout("a wild jungle planet inhabited by primitive cannibals.");
			skip(1);
			prout("They are very fond of \"Captain Kirk\" soup.");
			skip(1);
			proutn("Without your leadership, the ");
			crmshp();
			prout(" is destroyed.");
			break;
		case FDPLANET:
			prout("You and your mining party perish.");
			skip(1);
			prout("That was a great shot.");
			skip(1);
			break;
		case FSSC:
			prout("The Galileo is instantly annihilated by the supernova.");
			// no break;
		case FPNOVA:
			prout("You and your mining party are atomized.");
			skip(1);
			proutn("Mr. Spock takes command of the ");
			crmshp();
			prout(" and");
			prout("joins the Romulans, reigning terror on the Federation.");
			break;
		case FSTRACTOR:
			prout("The shuttle craft Galileo is also caught,");
			prout("and breaks up under the strain.");
			skip(1);
			prout("Your debris is scattered for millions of miles.");
			proutn("Without your leadership, the ");
			crmshp();
			prout(" is destroyed.");
			break;
		case FDRAY:
			prout("The mutants attack and kill Spock.");
			prout("Your ship is captured by Klingons, and");
			prout("your crew is put on display in a Klingon zoo.");
			break;
		case FTRIBBLE:
			prout("Tribbles consume all remaining water,");
			prout("food, and oxygen on your ship.");
			skip(1);
			prout("You die of thirst, starvation, and asphyxiation.");
			prout("Your starship is a derelict in space.");
			break;
		case FHOLE:
			prout("Your ship is drawn to the center of the black hole.");
			prout("You are crushed into extremely dense matter.");
			break;
	}
	if (ship==IHF) ship= 0;
	else if (ship == IHE) ship = IHF;
	alive = 0;
	if (game.state.remkl != 0) {
		double goodies = game.state.remres/inresor;
		double baddies = (game.state.remkl + 2.0*game.state.remcom)/(inkling+2.0*incom);
		if (goodies/baddies >= 1.0+0.5*Rand()) {
			prout("As a result of your actions, a treaty with the Klingon");
			prout("Empire has been signed. The terms of the treaty are");
			if (goodies/baddies >= 3.0+Rand()) {
				prout("favorable to the Federation.");
				skip(1);
				prout("Congratulations!");
			}
			else
				prout("highly unfavorable to the Federation.");
		}
		else
			prout("The Federation will be destroyed.");
	}
	else {
		prout("Since you took the last Klingon with you, you are a");
		prout("martyr and a hero. Someday maybe they'll erect a");
		prout("statue in your memory. Rest in peace, and try not");
		prout("to think about pigeons.");
		gamewon = 1;
	}
	score();
}

void score(void) {
	double timused = game.state.date - indate;
	int ithperd, iwon, klship;

	iskill = skill;
	if ((timused == 0 || game.state.remkl != 0) && timused < 5.0) timused = 5.0;
	perdate = (game.state.killc + game.state.killk + game.state.nsckill)/timused;
	ithperd = 500*perdate + 0.5;
	iwon = 0;
	if (gamewon) iwon = 100*skill;
	if (ship == IHE) klship = 0;
	else if (ship == IHF) klship = 1;
	else klship = 2;
	if (gamewon == 0) game.state.nromrem = 0; // None captured if no win
	iscore = 10*game.state.killk + 50*game.state.killc + ithperd + iwon
			 - 100*game.state.basekl - 100*klship - 45*nhelp -5*game.state.starkl - casual
			 + 20*game.state.nromkl + 200*game.state.nsckill - 10*game.state.nplankl + game.state.nromrem;
	if (alive == 0) iscore -= 200;
	skip(2);
	prout("Your score --");
	if (game.state.nromkl)
		prout("%6d Romulans destroyed                 %5d",
			   game.state.nromkl,20*game.state.nromkl);
	if (game.state.nromrem)
		prout("%6d Romulans captured                  %5d",
			   game.state.nromrem, game.state.nromrem);
	if (game.state.killk)
		prout("%6d ordinary Klingons destroyed        %5d",
			   game.state.killk, 10*game.state.killk);
	if (game.state.killc)
		prout("%6d Klingon commanders destroyed       %5d",
			   game.state.killc, 50*game.state.killc);
	if (game.state.nsckill)
		prout("%6d Super-Commander destroyed          %5d",
			   game.state.nsckill, 200*game.state.nsckill);
	if (ithperd)
		prout("%6.2f Klingons per stardate              %5d",
			   perdate, ithperd);
	if (game.state.starkl)
		prout("%6d stars destroyed by your action     %5d",
			   game.state.starkl, -5*game.state.starkl);
	if (game.state.nplankl)
		prout("%6d planets destroyed by your action   %5d",
			   game.state.nplankl, -10*game.state.nplankl);
	if (game.state.basekl)
		prout("%6d bases destroyed by your action     %5d",
			   game.state.basekl, -100*game.state.basekl);
	if (nhelp)
		prout("%6d calls for help from starbase       %5d",
			   nhelp, -45*nhelp);
	if (casual)
		prout("%6d casualties incurred                %5d",
			   casual, -casual);
	if (klship)
		prout("%6d ship(s) lost or destroyed          %5d",
			   klship, -100*klship);
	if (alive==0)
		prout("Penalty for getting yourself killed        -200");
	if (gamewon) {
		proutn("Bonus for winning ");
		switch (skill) {
			case 1: proutn("Novice game  "); break;
			case 2: proutn("Fair game    "); break;
			case 3: proutn("Good game    "); break;
			case 4: proutn("Expert game  "); break;
			case 5: proutn("Emeritus game"); break;
		}
		prout("           %5d", iwon);
	}
	skip(1);
	prout("TOTAL SCORE                               %5d", iscore);
}

void plaque(void) {
	FILE *fp=NULL;
	time_t t;
	char *timestring;
	int nskip;
	char winner[128];

	skip(2);
	
	while (fp == NULL) {
                proutn("File or device name for your plaque: ");
                cgetline(winner, sizeof(winner));
		fp = fopen(winner, "w");
		if (fp==NULL) {
                        prout("Invalid name.");
		}
	}

        proutn("Enter name to go on plaque (up to 30 characters): ");
        cgetline(winner, sizeof(winner));
	/* The 38 below must be 64 for 132-column paper */
	nskip = 38 - strlen(winner)/2;

	fprintf(fp,"\n\n\n\n");
	/* --------DRAW ENTERPRISE PICTURE. */
        fprintf(fp, "                                       EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n" );
	fprintf(fp, "                                      EEE                      E  : :                                         :  E\n" );
	fprintf(fp, "                                    EE   EEE                   E  : :                   NCC-1701              :  E\n");
        fprintf(fp, "EEEEEEEEEEEEEEEE        EEEEEEEEEEEEEEE  : :                              : E\n");
        fprintf(fp, " E                                     EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");
	fprintf(fp, "                      EEEEEEEEE               EEEEEEEEEEEEE                 E  E\n");
	fprintf(fp, "                               EEEEEEE   EEEEE    E          E              E  E\n");
	fprintf(fp, "                                      EEE           E          E            E  E\n");
	fprintf(fp, "                                                       E         E          E  E\n");
	fprintf(fp, "                                                         EEEEEEEEEEEEE      E  E\n");
	fprintf(fp, "                                                      EEE :           EEEEEEE  EEEEEEEE\n");
	fprintf(fp, "                                                    :E    :                 EEEE       E\n");
	fprintf(fp, "                                                   .-E   -:-----                       E\n");
	fprintf(fp, "                                                    :E    :                            E\n");
	fprintf(fp, "                                                      EE  :                    EEEEEEEE\n");
	fprintf(fp, "                                                       EEEEEEEEEEEEEEEEEEEEEEE\n");
	fprintf(fp, "\n\n\n");
	fprintf(fp, "                                                       U. S. S. ENTERPRISE\n");
	fprintf(fp, "\n\n\n\n");
	fprintf(fp, "                                  For demonstrating outstanding ability as a starship captain\n");
	fprintf(fp, "\n");
	fprintf(fp, "                                                Starfleet Command bestows to you\n");
	fprintf(fp, "\n");
	fprintf(fp,"%*s%s\n\n", nskip, "", winner);
	fprintf(fp, "                                                           the rank of\n\n");
	fprintf(fp, "                                                       \"Commodore Emeritus\"\n\n");
	fprintf(fp, "                                                          ");
	switch (iskill) {
		case 4: fprintf(fp," Expert level\n\n"); break;
		case 5: fprintf(fp,"Emeritus level\n\n"); break;
		default: fprintf(fp," Cheat level\n\n"); break;
	}
	t = time(NULL);
	timestring = ctime(&t);
	fprintf(fp, "                                                 This day of %.6s %.4s, %.8s\n\n",
			timestring+4, timestring+20, timestring+11);
	fprintf(fp,"                                                        Your score:  %d\n\n", iscore);
	fprintf(fp,"                                                    Klingons per stardate:  %.2f\n", perdate);
	fclose(fp);
}
