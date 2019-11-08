//
// Created by Ben Taylor on 10/28/19.
//

#include <iostream>
#include <mpi.h>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <chrono>
#include <ctime>

#define MCW MPI_COMM_WORLD

using namespace std;

Team* createPlayers();

struct Player {
    string name;
    int rating;
};

struct Team {
    Player players[15];
    string name;
    float rating;
    int wins;
    int losses;
};

int main(int argc, char **argv){
    Team nbaTeams[30] = createPlayers();
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);

    auto start = std::chrono::system_clock::now();

    MPI_Finalize();

    return 0;
}

Team* createPlayers() {
    Team nbaTeams[30];//ty, jc, jp, al, dh, kh, dj, cr, cp, db, vc, ac, et, bf, tw;
    nbaTeams[0].name = "Atlanta Hawks";
    nbaTeams[0].wins = 0; nbaTeams[0].losses = 0;
    nbaTeams[0].players[0].name = "Trae Young"; nbaTeams[0].players[0].rating = 85;
    nbaTeams[0].players[1].name = "John Collins"; nbaTeams[0].players[1].rating = 84;
    nbaTeams[0].players[2].name = "Jabari Parker"; nbaTeams[0].players[2].rating = 78;
    nbaTeams[0].players[3].name = "Alex Len"; nbaTeams[0].players[3].rating = 77;
    nbaTeams[0].players[4].name = "De'Andre Hunter"; nbaTeams[0].players[4].rating=77;
    nbaTeams[0].players[5].name = "Kevin Huerter"; nbaTeams[0].players[5].rating = 76;
    nbaTeams[0].players[6].name = "Damian Jones"; nbaTeams[0].players[6].rating = 75;
    nbaTeams[0].players[7].name = "Cam Reddish"; nbaTeams[0].players[7].rating = 75;
    nbaTeams[0].players[8].name = "Chandler Parsons"; nbaTeams[0].players[8].rating = 74;
    nbaTeams[0].players[9].name = "DeAndre Bembry"; nbaTeams[0].players[9].rating = 73;
    nbaTeams[0].players[10].name = "Vince Carter"; nbaTeams[0].players[10].rating = 73;
    nbaTeams[0].players[11].name = "Allen Crabbe"; nbaTeams[0].players[11].rating = 73;
    nbaTeams[0].players[12].name = "Evan Turner"; nbaTeams[0].players[12].rating=73;
    nbaTeams[0].players[13].name = "Bruno Fernando"; nbaTeams[0].players[13].rating = 71;
    nbaTeams[0].players[14].name = "Tyrone Wallace"; nbaTeams[0].players[14].rating = 69;

    nbaTeams[1].name = "Boston Celtics";
    nbaTeams[1].wins = 0; nbaTeams[1].losses = 0;
    nbaTeams[1].players[0].name = "Kemba Walker"; nbaTeams[1].players[0].rating = 88;
    nbaTeams[1].players[1].name = "Jayson Tatum"; nbaTeams[1].players[1].rating = 86;
    nbaTeams[1].players[2].name = "Gordon Hayward"; nbaTeams[1].players[2].rating = 82;
    nbaTeams[1].players[3].name = "Marcus Smart"; nbaTeams[1].players[3].rating = 82;
    nbaTeams[1].players[4].name = "Jaylen Brown"; nbaTeams[1].players[4].rating=81;
    nbaTeams[1].players[5].name = "Enes Kanter"; nbaTeams[1].players[5].rating = 80;
    nbaTeams[1].players[6].name = "Daniel Theis"; nbaTeams[1].players[6].rating = 76;
    nbaTeams[1].players[7].name = "Robert Williams III"; nbaTeams[1].players[7].rating = 74;
    nbaTeams[1].players[8].name = "Semi Ojeleye"; nbaTeams[1].players[8].rating = 73;
    nbaTeams[1].players[9].name = "Brad Wanamaker"; nbaTeams[1].players[9].rating = 73;
    nbaTeams[1].players[10].name = "Vincent Poirier"; nbaTeams[1].players[10].rating = 73;
    nbaTeams[1].players[11].name = "Romeo Langford"; nbaTeams[1].players[11].rating = 72;
    nbaTeams[1].players[12].name = "Grant Williams"; nbaTeams[1].players[12].rating=72;
    nbaTeams[1].players[13].name = "Carsen Edwards"; nbaTeams[1].players[13].rating = 70;
    nbaTeams[1].players[14].name = "Javonte Green"; nbaTeams[1].players[14].rating = 69;

    nbaTeams[2].name = "Brooklyn Nets";
    nbaTeams[2].wins = 0; nbaTeams[2].losses = 0;
    nbaTeams[2].players[0].name = "Kevin Durant"; nbaTeams[2].players[0].rating = 96;
    nbaTeams[2].players[1].name = "Kyrie Irving"; nbaTeams[2].players[1].rating = 92;
    nbaTeams[2].players[2].name = "DeAndre Jordan"; nbaTeams[2].players[2].rating = 81;
    nbaTeams[2].players[3].name = "Jarrett Allen"; nbaTeams[2].players[3].rating = 80;
    nbaTeams[2].players[4].name = "Spencer Dinwiddie"; nbaTeams[2].players[4].rating=80;
    nbaTeams[2].players[5].name = "Joe Harris"; nbaTeams[2].players[5].rating = 78;
    nbaTeams[2].players[6].name = "Caris LeVert"; nbaTeams[2].players[6].rating = 78;
    nbaTeams[2].players[7].name = "Taurean Prince"; nbaTeams[2].players[7].rating = 78;
    nbaTeams[2].players[8].name = "Rodions Kurucs"; nbaTeams[2].players[8].rating = 75;
    nbaTeams[2].players[9].name = "Wilson Chandler"; nbaTeams[2].players[9].rating = 75;
    nbaTeams[2].players[10].name = "Garrett Temple"; nbaTeams[2].players[10].rating = 74;
    nbaTeams[2].players[11].name = "David Nwaba"; nbaTeams[2].players[11].rating = 73;
    nbaTeams[2].players[12].name = "Nicolas Claxton"; nbaTeams[2].players[12].rating=72;
    nbaTeams[2].players[13].name = "Dzanan Musa"; nbaTeams[2].players[13].rating = 71;
    nbaTeams[2].players[14].name = "Timothe Luwawu-Cabarrot"; nbaTeams[2].players[14].rating = 70;

    nbaTeams[3].name = "Charlotte Hornets";
    nbaTeams[3].wins = 0; nbaTeams[3].losses = 0;
    nbaTeams[3].players[0].name = "Cody Zeller"; nbaTeams[3].players[0].rating = 79;
    nbaTeams[3].players[1].name = "Terry Rozier"; nbaTeams[3].players[1].rating = 79;
    nbaTeams[3].players[2].name = "Nicolas Batum"; nbaTeams[3].players[2].rating = 77;
    nbaTeams[3].players[3].name = "Michael Kidd-Gilchrist"; nbaTeams[3].players[3].rating = 76;
    nbaTeams[3].players[4].name = "Marvin Williams"; nbaTeams[3].players[4].rating=76;
    nbaTeams[3].players[5].name = "Willy Hernangomez"; nbaTeams[3].players[5].rating = 76;
    nbaTeams[3].players[6].name = "Miles Bridges"; nbaTeams[3].players[6].rating = 76;
    nbaTeams[3].players[7].name = "Devonte' Graham"; nbaTeams[3].players[7].rating = 76;
    nbaTeams[3].players[8].name = "PJ Washington"; nbaTeams[3].players[8].rating = 76;
    nbaTeams[3].players[9].name = "Malik Monk"; nbaTeams[3].players[9].rating = 75;
    nbaTeams[3].players[10].name = "Bismack Biyombo"; nbaTeams[3].players[10].rating = 75;
    nbaTeams[3].players[11].name = "Dwayne Bacon"; nbaTeams[3].players[11].rating = 73;
    nbaTeams[3].players[12].name = "Cody Martin"; nbaTeams[3].players[12].rating=70;
    nbaTeams[3].players[13].name = "Jalen McDaniels"; nbaTeams[3].players[13].rating = 69;
    nbaTeams[3].players[14].name = "Robert Franks"; nbaTeams[3].players[14].rating = 67;

    nbaTeams[4].name = "Chicago Bulls";
    nbaTeams[4].wins = 0; nbaTeams[4].losses = 0;
    nbaTeams[4].players[0].name = "Zach LaVine"; nbaTeams[4].players[0].rating = 84;
    nbaTeams[4].players[1].name = "Lauri Markkanen"; nbaTeams[4].players[1].rating = 82;
    nbaTeams[4].players[2].name = "Wendell Carter Jr."; nbaTeams[4].players[2].rating = 81;
    nbaTeams[4].players[3].name = "Thaddeus Young"; nbaTeams[4].players[3].rating = 79;
    nbaTeams[4].players[4].name = "Otto Porter Jr."; nbaTeams[4].players[4].rating=78;
    nbaTeams[4].players[5].name = "Coby White"; nbaTeams[4].players[5].rating = 76;
    nbaTeams[4].players[6].name = "Tomas Satoransky"; nbaTeams[4].players[6].rating = 75;
    nbaTeams[4].players[7].name = "Kris Dunn"; nbaTeams[4].players[7].rating = 74;
    nbaTeams[4].players[8].name = "Denzel Valentine"; nbaTeams[4].players[8].rating = 74;
    nbaTeams[4].players[9].name = "Cristiano Felicio"; nbaTeams[4].players[9].rating = 72;
    nbaTeams[4].players[10].name = "Chandler Hutchison"; nbaTeams[4].players[10].rating = 72;
    nbaTeams[4].players[11].name = "Ryan Arcidiacono"; nbaTeams[4].players[11].rating = 72;
    nbaTeams[4].players[12].name = "Shaquille Harrison"; nbaTeams[4].players[12].rating=72;
    nbaTeams[4].players[13].name = "Daniel Gafford"; nbaTeams[4].players[13].rating = 71;
    nbaTeams[4].players[14].name = "Luke Kornet"; nbaTeams[4].players[14].rating = 71;

    nbaTeams[5].name = "Cleveland Cavaliers";
    nbaTeams[5].wins = 0; nbaTeams[5].losses = 0;
    nbaTeams[5].players[0].name = "Kevin Love"; nbaTeams[5].players[0].rating = 84;
    nbaTeams[5].players[1].name = "Tristan Thompason"; nbaTeams[5].players[1].rating = 80;
    nbaTeams[5].players[2].name = "Jordan Clarkson"; nbaTeams[5].players[2].rating = 79;
    nbaTeams[5].players[3].name = "Larry Nance Jr."; nbaTeams[5].players[3].rating = 77;
    nbaTeams[5].players[4].name = "Collin Sexton"; nbaTeams[5].players[4].rating=77;
    nbaTeams[5].players[5].name = "Cedi Osman"; nbaTeams[5].players[5].rating = 75;
    nbaTeams[5].players[6].name = "John Henson"; nbaTeams[5].players[6].rating = 75;
    nbaTeams[5].players[7].name = "Darius Garland"; nbaTeams[5].players[7].rating = 74;
    nbaTeams[5].players[8].name = "Ante Zizic"; nbaTeams[5].players[8].rating = 73;
    nbaTeams[5].players[9].name = "Matthew Dellavedova"; nbaTeams[5].players[9].rating = 72;
    nbaTeams[5].players[10].name = "Brandon Knight"; nbaTeams[5].players[10].rating = 72;
    nbaTeams[5].players[11].name = "Kevin Porter Jr."; nbaTeams[5].players[11].rating = 72;
    nbaTeams[5].players[12].name = "Alfonzo McKinnie"; nbaTeams[5].players[12].rating=72;
    nbaTeams[5].players[13].name = "Dylan Windler"; nbaTeams[5].players[13].rating = 71;
    nbaTeams[5].players[14].name = "Dean Wade"; nbaTeams[5].players[14].rating = 68;

    nbaTeams[6].name = "Dallas Mavericks";
    nbaTeams[6].wins = 0; nbaTeams[6].losses = 0;
    nbaTeams[6].players[0].name = "Luka Doncic"; nbaTeams[6].players[0].rating = 90;
    nbaTeams[6].players[1].name = "Kristaps Porzingis"; nbaTeams[6].players[1].rating = 87;
    nbaTeams[6].players[2].name = "Tim Hardaway Jr."; nbaTeams[6].players[2].rating = 77;
    nbaTeams[6].players[3].name = "Delon Wright"; nbaTeams[6].players[3].rating = 77;
    nbaTeams[6].players[4].name = "J.J. Barea"; nbaTeams[6].players[4].rating=76;
    nbaTeams[6].players[5].name = "Jalen Brunson"; nbaTeams[6].players[5].rating = 76;
    nbaTeams[6].players[6].name = "Dwight Powell"; nbaTeams[6].players[6].rating = 75;
    nbaTeams[6].players[7].name = "Boban Marjanovic"; nbaTeams[6].players[7].rating = 75;
    nbaTeams[6].players[8].name = "Justin Jackson"; nbaTeams[6].players[8].rating = 74;
    nbaTeams[6].players[9].name = "Seth Curry"; nbaTeams[6].players[9].rating = 74;
    nbaTeams[6].players[10].name = "Dorian Finney-Smith"; nbaTeams[6].players[10].rating = 73;
    nbaTeams[6].players[11].name = "Maxi Kleber"; nbaTeams[6].players[11].rating = 73;
    nbaTeams[6].players[12].name = "Ryan Broekhoff"; nbaTeams[6].players[12].rating=72;
    nbaTeams[6].players[13].name = "Courtney Lee"; nbaTeams[6].players[13].rating = 72;
    nbaTeams[6].players[14].name = "Isaiah Roby"; nbaTeams[6].players[14].rating = 70;

    nbaTeams[7].name = "Denver Nuggets";
    nbaTeams[7].wins = 0; nbaTeams[7].losses = 0;
    nbaTeams[7].players[0].name = "Nikola Jokic"; nbaTeams[7].players[0].rating = 89;
    nbaTeams[7].players[1].name = "Jamal Murray"; nbaTeams[7].players[1].rating = 84;
    nbaTeams[7].players[2].name = "Paul Millsap"; nbaTeams[7].players[2].rating = 82;
    nbaTeams[7].players[3].name = "Will Barton"; nbaTeams[7].players[3].rating = 79;
    nbaTeams[7].players[4].name = "Gary Harris"; nbaTeams[7].players[4].rating=78;
    nbaTeams[7].players[5].name = "Mason Plumlee"; nbaTeams[7].players[5].rating = 78;
    nbaTeams[7].players[6].name = "Malik Beasley"; nbaTeams[7].players[6].rating = 77;
    nbaTeams[7].players[7].name = "Monte Morris"; nbaTeams[7].players[7].rating = 77;
    nbaTeams[7].players[8].name = "Michael Porter Jr."; nbaTeams[7].players[8].rating = 76;
    nbaTeams[7].players[9].name = "Jerami Grant"; nbaTeams[7].players[9].rating = 76;
    nbaTeams[7].players[10].name = "Juan Hernangomez"; nbaTeams[7].players[10].rating = 75;
    nbaTeams[7].players[11].name = "Torrey Craig"; nbaTeams[7].players[11].rating = 74;
    nbaTeams[7].players[12].name = "Bol Bol"; nbaTeams[7].players[12].rating=73;
    nbaTeams[7].players[13].name = "Jarred Vanderbilt"; nbaTeams[7].players[13].rating = 70;
    nbaTeams[7].players[14].name = "Vlatko Cancar"; nbaTeams[7].players[14].rating = 70;

    nbaTeams[8].name = "Detroit Pistons";
    nbaTeams[8].wins = 0; nbaTeams[8].losses = 0;
    nbaTeams[8].players[0].name = "Andre Drummond"; nbaTeams[8].players[0].rating = 89;
    nbaTeams[8].players[1].name = "Blake Griffin"; nbaTeams[8].players[1].rating = 88;
    nbaTeams[8].players[2].name = "Derrick Rose"; nbaTeams[8].players[2].rating = 84;
    nbaTeams[8].players[3].name = "Reggie Jackson"; nbaTeams[8].players[3].rating = 80;
    nbaTeams[8].players[4].name = "Luke Kennard"; nbaTeams[8].players[4].rating=77;
    nbaTeams[8].players[5].name = "Christian Wood"; nbaTeams[8].players[5].rating = 76;
    nbaTeams[8].players[6].name = "Bruce Brown"; nbaTeams[8].players[6].rating = 73;
    nbaTeams[8].players[7].name = "Sekou Doumbouya"; nbaTeams[8].players[7].rating = 73;
    nbaTeams[8].players[8].name = "Tony Snell"; nbaTeams[8].players[8].rating = 73;
    nbaTeams[8].players[9].name = "Markieff Morris"; nbaTeams[8].players[9].rating = 73;
    nbaTeams[8].players[10].name = "Langston Galloway"; nbaTeams[8].players[10].rating = 72;
    nbaTeams[8].players[11].name = "Thon Maker"; nbaTeams[8].players[11].rating = 71;
    nbaTeams[8].players[12].name = "Tim Frazier"; nbaTeams[8].players[12].rating=71;
    nbaTeams[8].players[13].name = "Sviatoslav Mykhailiuk"; nbaTeams[8].players[13].rating = 70;
    nbaTeams[8].players[14].name = "Khyri Thomas"; nbaTeams[8].players[14].rating = 69;

    nbaTeams[9].name = "Golden State Warriors";
    nbaTeams[9].wins = 0; nbaTeams[9].losses = 0;
    nbaTeams[9].players[0].name = "Stephen Curry"; nbaTeams[9].players[0].rating = 95;
    nbaTeams[9].players[1].name = "Klay Thompson"; nbaTeams[9].players[1].rating = 89;
    nbaTeams[9].players[2].name = "Draymond Green"; nbaTeams[9].players[2].rating = 86;
    nbaTeams[9].players[3].name = "D'Angelo Russell"; nbaTeams[9].players[3].rating = 85;
    nbaTeams[9].players[4].name = "Willie Cauley-Stein"; nbaTeams[9].players[4].rating=78;
    nbaTeams[9].players[5].name = "Kevon Looney"; nbaTeams[9].players[5].rating = 77;
    nbaTeams[9].players[6].name = "Eric Paschall"; nbaTeams[9].players[6].rating = 77;
    nbaTeams[9].players[7].name = "Glenn Robinson III"; nbaTeams[9].players[7].rating = 75;
    nbaTeams[9].players[8].name = "Omari Spellman"; nbaTeams[9].players[8].rating = 74;
    nbaTeams[9].players[9].name = "Marquese Chriss"; nbaTeams[9].players[9].rating = 74;
    nbaTeams[9].players[10].name = "Damion Lee"; nbaTeams[9].players[10].rating = 73;
    nbaTeams[9].players[11].name = "Alec Burks"; nbaTeams[9].players[11].rating = 72;
    nbaTeams[9].players[12].name = "Jacob Evans"; nbaTeams[9].players[12].rating=71;
    nbaTeams[9].players[13].name = "Jordan Poole"; nbaTeams[9].players[13].rating = 71;
    nbaTeams[9].players[14].name = "Alen Smailagic"; nbaTeams[9].players[14].rating = 71;

    nbaTeams[10].name = "Houston Rockets";
    nbaTeams[10].wins = 0; nbaTeams[10].losses = 0;
    nbaTeams[10].players[0].name = "James Harden"; nbaTeams[10].players[0].rating = 96;
    nbaTeams[10].players[1].name = "Russell Westbrook"; nbaTeams[10].players[1].rating = 90;
    nbaTeams[10].players[2].name = "Clint Capela"; nbaTeams[10].players[2].rating = 84;
    nbaTeams[10].players[3].name = "P.J. Tucker"; nbaTeams[10].players[3].rating = 79;
    nbaTeams[10].players[4].name = "Eric Gordon"; nbaTeams[10].players[4].rating=77;
    nbaTeams[10].players[5].name = "Gerald Green"; nbaTeams[10].players[5].rating = 76;
    nbaTeams[10].players[6].name = "Austin Rivers"; nbaTeams[10].players[6].rating = 75;
    nbaTeams[10].players[7].name = "Danuel House"; nbaTeams[10].players[7].rating = 74;
    nbaTeams[10].players[8].name = "Thabo Sefolosha"; nbaTeams[10].players[8].rating = 74;
    nbaTeams[10].players[9].name = "Tyson Chandler"; nbaTeams[10].players[9].rating = 73;
    nbaTeams[10].players[10].name = "Nene"; nbaTeams[10].players[10].rating = 72;
    nbaTeams[10].players[11].name = "Ben McLemore"; nbaTeams[10].players[11].rating = 71;
    nbaTeams[10].players[12].name = "Ryan Anderson"; nbaTeams[10].players[12].rating=70;
    nbaTeams[10].players[13].name = "Gary Clark"; nbaTeams[10].players[13].rating = 69;
    nbaTeams[10].players[14].name = "Isaiah Hartenstein"; nbaTeams[10].players[14].rating = 68;

    nbaTeams[11].name = "Indiana Pacers";
    nbaTeams[11].wins = 0; nbaTeams[11].losses = 0;
    nbaTeams[11].players[0].name = "Victor Oladipo"; nbaTeams[11].players[0].rating = 87;
    nbaTeams[11].players[1].name = "Malcom Brogdon"; nbaTeams[11].players[1].rating = 86;
    nbaTeams[11].players[2].name = "Domantas Sabonis"; nbaTeams[11].players[2].rating = 82;
    nbaTeams[11].players[3].name = "Myles Turner"; nbaTeams[11].players[3].rating = 82;
    nbaTeams[11].players[4].name = "T.J. Warren"; nbaTeams[11].players[4].rating=80;
    nbaTeams[11].players[5].name = "Jeremy Lamb"; nbaTeams[11].players[5].rating = 79;
    nbaTeams[11].players[6].name = "T.J. McConnell"; nbaTeams[11].players[6].rating = 75;
    nbaTeams[11].players[7].name = "T.J. Leaf"; nbaTeams[11].players[7].rating = 73;
    nbaTeams[11].players[8].name = "Doug McDermott"; nbaTeams[11].players[8].rating = 73;
    nbaTeams[11].players[9].name = "Aaron Holiday"; nbaTeams[11].players[9].rating = 73;
    nbaTeams[11].players[10].name = "Goga Bitadze"; nbaTeams[11].players[10].rating = 73;
    nbaTeams[11].players[11].name = "Justin Holiday"; nbaTeams[11].players[11].rating = 72;
    nbaTeams[11].players[12].name = "Edmond Sumner"; nbaTeams[11].players[12].rating=71;
    nbaTeams[11].players[13].name = "JaKarr Sampson"; nbaTeams[11].players[13].rating = 71;
    nbaTeams[11].players[14].name = "Alize Johnson"; nbaTeams[11].players[14].rating = 70;

    nbaTeams[12].name = "Los Angeles Clippers";
    nbaTeams[12].wins = 0; nbaTeams[12].losses = 0;
    nbaTeams[12].players[0].name = "Kawhi Leonard"; nbaTeams[12].players[0].rating = 97;
    nbaTeams[12].players[1].name = "Paul George"; nbaTeams[12].players[1].rating = 93;
    nbaTeams[12].players[2].name = "Montrezl Harrell"; nbaTeams[12].players[2].rating = 85;
    nbaTeams[12].players[3].name = "Lou Williams"; nbaTeams[12].players[3].rating = 84;
    nbaTeams[12].players[4].name = "Patrick Beverley"; nbaTeams[12].players[4].rating=79;
    nbaTeams[12].players[5].name = "JaMychal Green"; nbaTeams[12].players[5].rating = 78;
    nbaTeams[12].players[6].name = "Ivica Zubac"; nbaTeams[12].players[6].rating = 78;
    nbaTeams[12].players[7].name = "Landry Shamet"; nbaTeams[12].players[7].rating = 75;
    nbaTeams[12].players[8].name = "Maurice Harkless"; nbaTeams[12].players[8].rating = 73;
    nbaTeams[12].players[9].name = "Mfiondu Kabengele"; nbaTeams[12].players[9].rating = 72;
    nbaTeams[12].players[10].name = "Rodney McGruder"; nbaTeams[12].players[10].rating = 72;
    nbaTeams[12].players[11].name = "Patrick Patterson"; nbaTeams[12].players[11].rating = 72;
    nbaTeams[12].players[12].name = "Jerome Robinson"; nbaTeams[12].players[12].rating=71;
    nbaTeams[12].players[13].name = "Terrance Mann"; nbaTeams[12].players[13].rating = 69;
    nbaTeams[12].players[14].name = "Derrick Walton Jr."; nbaTeams[12].players[14].rating = 67;

    nbaTeams[13].name = "Los Angeles Lakers";
    nbaTeams[13].wins = 0; nbaTeams[13].losses = 0;
    nbaTeams[13].players[0].name = "LeBron James"; nbaTeams[13].players[0].rating = 97;
    nbaTeams[13].players[1].name = "Anthony Davis"; nbaTeams[13].players[1].rating = 94;
    nbaTeams[13].players[2].name = "Kyle Kuzma"; nbaTeams[13].players[2].rating = 84;
    nbaTeams[13].players[3].name = "DeMarcus Cousins"; nbaTeams[13].players[3].rating = 81;
    nbaTeams[13].players[4].name = "Dwight Howard"; nbaTeams[13].players[4].rating=81;
    nbaTeams[13].players[5].name = "Danny Green"; nbaTeams[13].players[5].rating = 78;
    nbaTeams[13].players[6].name = "JaVale McGee"; nbaTeams[13].players[6].rating = 77;
    nbaTeams[13].players[7].name = "Rajon Rondo"; nbaTeams[13].players[7].rating = 76;
    nbaTeams[13].players[8].name = "Avery Bradley"; nbaTeams[13].players[8].rating = 76;
    nbaTeams[13].players[9].name = "Ketavious Caldwell-Pop"; nbaTeams[13].players[9].rating = 74;
    nbaTeams[13].players[10].name = "Quinn Cook"; nbaTeams[13].players[10].rating = 73;
    nbaTeams[13].players[11].name = "Alex Caruso"; nbaTeams[13].players[11].rating = 72;
    nbaTeams[13].players[12].name = "Troy Daniels"; nbaTeams[13].players[12].rating=72;
    nbaTeams[13].players[13].name = "Jared Dudley"; nbaTeams[13].players[13].rating = 72;
    nbaTeams[13].players[14].name = "Talen Horton-Tucker"; nbaTeams[13].players[14].rating = 70;

    nbaTeams[14].name = "Memphis Grizzlies";
    nbaTeams[14].wins = 0; nbaTeams[14].losses = 0;
    nbaTeams[14].players[0].name = "Jaren Jackson Jr."; nbaTeams[14].players[0].rating = 82;
    nbaTeams[14].players[1].name = "Ja Morant"; nbaTeams[14].players[1].rating = 81;
    nbaTeams[14].players[2].name = "Jonas Valanciunas"; nbaTeams[14].players[2].rating = 80;
    nbaTeams[14].players[3].name = "Andre Iguodala"; nbaTeams[14].players[3].rating = 78;
    nbaTeams[14].players[4].name = "Brandon Clarke"; nbaTeams[14].players[4].rating=76;
    nbaTeams[14].players[5].name = "Jae Crowder"; nbaTeams[14].players[5].rating = 75;
    nbaTeams[14].players[6].name = "Dillon Brooks"; nbaTeams[14].players[6].rating = 74;
    nbaTeams[14].players[7].name = "Kyle Anderson"; nbaTeams[14].players[7].rating = 74;
    nbaTeams[14].players[8].name = "Josh Jackson"; nbaTeams[14].players[8].rating = 74;
    nbaTeams[14].players[9].name = "Tyus Jones"; nbaTeams[14].players[9].rating = 73;
    nbaTeams[14].players[10].name = "Bruno Caboclo"; nbaTeams[14].players[10].rating = 72;
    nbaTeams[14].players[11].name = "Grayson Allen"; nbaTeams[14].players[11].rating = 72;
    nbaTeams[14].players[12].name = "De'Anthony Melton"; nbaTeams[14].players[12].rating=72;
    nbaTeams[14].players[13].name = "Solomon Hill"; nbaTeams[14].players[13].rating = 70;
    nbaTeams[14].players[14].name = "Marko Guduric"; nbaTeams[14].players[14].rating = 69;

    nbaTeams[15].name = "Miami Heat";
    nbaTeams[15].wins = 0; nbaTeams[15].losses = 0;
    nbaTeams[15].players[0].name = "Jimmy Butler"; nbaTeams[15].players[0].rating = 88;
    nbaTeams[15].players[1].name = "Bam Adebayo"; nbaTeams[15].players[1].rating = 81;
    nbaTeams[15].players[2].name = "Goran Dragic"; nbaTeams[15].players[2].rating = 79;
    nbaTeams[15].players[3].name = "Justise Winslow"; nbaTeams[15].players[3].rating = 78;
    nbaTeams[15].players[4].name = "Meyers Leonard"; nbaTeams[15].players[4].rating=78;
    nbaTeams[15].players[5].name = "Kelly Olynyk"; nbaTeams[15].players[5].rating = 77;
    nbaTeams[15].players[6].name = "Dion Waiters"; nbaTeams[15].players[6].rating = 76;
    nbaTeams[15].players[7].name = "Tyler Herro"; nbaTeams[15].players[7].rating = 76;
    nbaTeams[15].players[8].name = "Kendrick Nunn"; nbaTeams[15].players[8].rating = 76;
    nbaTeams[15].players[9].name = "James Johnson"; nbaTeams[15].players[9].rating = 75;
    nbaTeams[15].players[10].name = "Derrick Jones Jr."; nbaTeams[15].players[10].rating = 74;
    nbaTeams[15].players[11].name = "Duncan Robinson"; nbaTeams[15].players[11].rating = 73;
    nbaTeams[15].players[12].name = "KZ Okpala"; nbaTeams[15].players[12].rating=71;
    nbaTeams[15].players[13].name = "Udonis Haslem"; nbaTeams[15].players[13].rating = 70;
    nbaTeams[15].players[14].name = "Daryl Macon"; nbaTeams[15].players[14].rating = 68;

    nbaTeams[16].name = "Milwaukee Bucks";
    nbaTeams[16].wins = 0; nbaTeams[16].losses = 0;
    nbaTeams[16].players[0].name = "Giannis Antetokounmpo"; nbaTeams[16].players[0].rating = 96;
    nbaTeams[16].players[1].name = "Khris Middleton"; nbaTeams[16].players[1].rating = 86;
    nbaTeams[16].players[2].name = "Eric Bledsoe"; nbaTeams[16].players[2].rating = 84;
    nbaTeams[16].players[3].name = "Brook Lopez"; nbaTeams[16].players[3].rating = 77;
    nbaTeams[16].players[4].name = "Ersan Ilyasova"; nbaTeams[16].players[4].rating=76;
    nbaTeams[16].players[5].name = "George Hill"; nbaTeams[16].players[5].rating = 76;
    nbaTeams[16].players[6].name = "Wesley Matthews"; nbaTeams[16].players[6].rating = 76;
    nbaTeams[16].players[7].name = "Robin Lopez"; nbaTeams[16].players[7].rating = 75;
    nbaTeams[16].players[8].name = "Kyle Korver"; nbaTeams[16].players[8].rating = 75;
    nbaTeams[16].players[9].name = "Pat Connaughton"; nbaTeams[16].players[9].rating = 74;
    nbaTeams[16].players[10].name = "Sterling Brown"; nbaTeams[16].players[10].rating = 73;
    nbaTeams[16].players[11].name = "Donte DiVincenzo"; nbaTeams[16].players[11].rating = 73;
    nbaTeams[16].players[12].name = "D.J. Wilson"; nbaTeams[16].players[12].rating=72;
    nbaTeams[16].players[13].name = "Frank Mason"; nbaTeams[16].players[13].rating = 71;
    nbaTeams[16].players[14].name = "Dragan Bender"; nbaTeams[16].players[14].rating = 71;

    nbaTeams[17].name = "Minnesota Timberwolves";
    nbaTeams[17].wins = 0; nbaTeams[17].losses = 0;
    nbaTeams[17].players[0].name = "Karl-Anthony Towns"; nbaTeams[17].players[0].rating = 90;
    nbaTeams[17].players[1].name = "Andrew Wiggins"; nbaTeams[17].players[1].rating = 80;
    nbaTeams[17].players[2].name = "Robert Covington"; nbaTeams[17].players[2].rating = 80;
    nbaTeams[17].players[3].name = "Jeff Teague"; nbaTeams[17].players[3].rating = 79;
    nbaTeams[17].players[4].name = "Gorgui Dieng"; nbaTeams[17].players[4].rating=76;
    nbaTeams[17].players[5].name = "Josh Okogie"; nbaTeams[17].players[5].rating = 76;
    nbaTeams[17].players[6].name = "Shabazz Napier"; nbaTeams[17].players[6].rating = 75;
    nbaTeams[17].players[7].name = "Noah Vonleh"; nbaTeams[17].players[7].rating = 75;
    nbaTeams[17].players[8].name = "Jarrett Culver"; nbaTeams[17].players[8].rating = 74;
    nbaTeams[17].players[9].name = "Jordan Bell"; nbaTeams[17].players[9].rating = 74;
    nbaTeams[17].players[10].name = "Jake Layman"; nbaTeams[17].players[10].rating = 74;
    nbaTeams[17].players[11].name = "Treveon Graham"; nbaTeams[17].players[11].rating = 71;
    nbaTeams[17].players[12].name = "Keita Bates-Diop"; nbaTeams[17].players[12].rating=70;
    nbaTeams[17].players[13].name = "Naz Reid"; nbaTeams[17].players[13].rating = 70;
    nbaTeams[17].players[14].name = "Jaylen Nowell"; nbaTeams[17].players[14].rating = 70;

    nbaTeams[18].name = "New Orleans Pelicans";
    nbaTeams[18].wins = 0; nbaTeams[18].losses = 0;
    nbaTeams[18].players[0].name = "Brandon INgram"; nbaTeams[18].players[0].rating = 85;
    nbaTeams[18].players[1].name = "Jrue Holiday"; nbaTeams[18].players[1].rating = 84;
    nbaTeams[18].players[2].name = "Zion Williamson"; nbaTeams[18].players[2].rating = 81;
    nbaTeams[18].players[3].name = "Lonzo Ball"; nbaTeams[18].players[3].rating = 79;
    nbaTeams[18].players[4].name = "J.J. Redick"; nbaTeams[18].players[4].rating=78;
    nbaTeams[18].players[5].name = "Jahlil Okafor"; nbaTeams[18].players[5].rating = 77;
    nbaTeams[18].players[6].name = "Josh Hart"; nbaTeams[18].players[6].rating = 77;
    nbaTeams[18].players[7].name = "Derrick Favors"; nbaTeams[18].players[7].rating = 77;
    nbaTeams[18].players[8].name = "E'Twaun Moore"; nbaTeams[18].players[8].rating = 74;
    nbaTeams[18].players[9].name = "Jaxson Hayes"; nbaTeams[18].players[9].rating = 74;
    nbaTeams[18].players[10].name = "Kenrich Williams"; nbaTeams[18].players[10].rating = 73;
    nbaTeams[18].players[11].name = "Nicolo Melli"; nbaTeams[18].players[11].rating = 73;
    nbaTeams[18].players[12].name = "Frank Jackson"; nbaTeams[18].players[12].rating=72;
    nbaTeams[18].players[13].name = "Darius Miller"; nbaTeams[18].players[13].rating = 72;
    nbaTeams[18].players[14].name = "Nickeil Alexander-Walker"; nbaTeams[18].players[14].rating = 72;

    nbaTeams[19].name = "New York Knicks";
    nbaTeams[19].wins = 0; nbaTeams[19].losses = 0;
    nbaTeams[19].players[0].name = "Julius Randle"; nbaTeams[19].players[0].rating = 81;
    nbaTeams[19].players[1].name = "Mitchell Robinson"; nbaTeams[19].players[1].rating = 80;
    nbaTeams[19].players[2].name = "RJ Barrett"; nbaTeams[19].players[2].rating = 79;
    nbaTeams[19].players[3].name = "Marcus Morris Sr"; nbaTeams[19].players[3].rating = 79;
    nbaTeams[19].players[4].name = "Bobby Portis"; nbaTeams[19].players[4].rating=78;
    nbaTeams[19].players[5].name = "Taj Gibson"; nbaTeams[19].players[5].rating = 77;
    nbaTeams[19].players[6].name = "Kevin Knox"; nbaTeams[19].players[6].rating = 76;
    nbaTeams[19].players[7].name = "Dennis Smith Jr."; nbaTeams[19].players[7].rating = 76;
    nbaTeams[19].players[8].name = "Elfrid Payton"; nbaTeams[19].players[8].rating = 76;
    nbaTeams[19].players[9].name = "Damyean Dotson"; nbaTeams[19].players[9].rating = 75;
    nbaTeams[19].players[10].name = "Allonzo Trier"; nbaTeams[19].players[10].rating = 75;
    nbaTeams[19].players[11].name = "Reggie Bullock"; nbaTeams[19].players[11].rating = 74;
    nbaTeams[19].players[12].name = "Ivan Rabb"; nbaTeams[19].players[12].rating=74;
    nbaTeams[19].players[13].name = "Frank Ntilikina"; nbaTeams[19].players[13].rating = 73;
    nbaTeams[19].players[14].name = "Wayne Ellington"; nbaTeams[19].players[14].rating = 73;

    nbaTeams[20].name = "Oklahoma City Thunder";
    nbaTeams[20].wins = 0; nbaTeams[20].losses = 0;
    nbaTeams[20].players[0].name = "Chris Paul"; nbaTeams[20].players[0].rating = 85;
    nbaTeams[20].players[1].name = "Shai Gilgeous-Alexander"; nbaTeams[20].players[1].rating = 83;
    nbaTeams[20].players[2].name = "Danilo Gallinari"; nbaTeams[20].players[2].rating = 82;
    nbaTeams[20].players[3].name = "Steven Adams"; nbaTeams[20].players[3].rating = 81;
    nbaTeams[20].players[4].name = "Nerlens Noel"; nbaTeams[20].players[4].rating=78;
    nbaTeams[20].players[5].name = "Dennis Schroder"; nbaTeams[20].players[5].rating = 78;
    nbaTeams[20].players[6].name = "Andre Roberson"; nbaTeams[20].players[6].rating = 76;
    nbaTeams[20].players[7].name = "Hamidou Diallo"; nbaTeams[20].players[7].rating = 72;
    nbaTeams[20].players[8].name = "Mike Muscala"; nbaTeams[20].players[8].rating = 72;
    nbaTeams[20].players[9].name = "Terrance Ferguson"; nbaTeams[20].players[9].rating = 71;
    nbaTeams[20].players[10].name = "Justin Patton"; nbaTeams[20].players[10].rating = 71;
    nbaTeams[20].players[11].name = "Abdel Nader"; nbaTeams[20].players[11].rating = 70;
    nbaTeams[20].players[12].name = "Deonte Burton"; nbaTeams[20].players[12].rating=70;
    nbaTeams[20].players[13].name = "Darius Bazley"; nbaTeams[20].players[13].rating = 70;
    nbaTeams[20].players[14].name = "Luguentz Dor"; nbaTeams[20].players[14].rating = 68;

    nbaTeams[21].name = "Orlando Magic";
    nbaTeams[21].wins = 0; nbaTeams[21].losses = 0;
    nbaTeams[21].players[0].name = "Nikola Vucevic"; nbaTeams[21].players[0].rating = 84;
    nbaTeams[21].players[1].name = "Aaron Gordon"; nbaTeams[21].players[1].rating = 80;
    nbaTeams[21].players[2].name = "Evan Fournier"; nbaTeams[21].players[2].rating = 78;
    nbaTeams[21].players[3].name = "Jonathan Isaac"; nbaTeams[21].players[3].rating = 78;
    nbaTeams[21].players[4].name = "Al-Farouq Aminu"; nbaTeams[21].players[4].rating=77;
    nbaTeams[21].players[5].name = "Terrence Ross"; nbaTeams[21].players[5].rating = 76;
    nbaTeams[21].players[6].name = "Mohamed Bamba"; nbaTeams[21].players[6].rating = 76;
    nbaTeams[21].players[7].name = "D.J. Augustin"; nbaTeams[21].players[7].rating = 75;
    nbaTeams[21].players[8].name = "Markelle Fultz"; nbaTeams[21].players[8].rating = 75;
    nbaTeams[21].players[9].name = "Khem Birch"; nbaTeams[21].players[9].rating = 73;
    nbaTeams[21].players[10].name = "Wesley Iwundu"; nbaTeams[21].players[10].rating = 72;
    nbaTeams[21].players[11].name = "Michael Carter-Williams"; nbaTeams[21].players[11].rating = 72;
    nbaTeams[21].players[12].name = "Melvin Frazier"; nbaTeams[21].players[12].rating=70;
    nbaTeams[21].players[13].name = "Amile Jefferson"; nbaTeams[21].players[13].rating = 69;
    nbaTeams[21].players[14].name = "Josh Magette"; nbaTeams[21].players[14].rating = 68;

    nbaTeams[22].name = "Philadelphia 76ers";
    nbaTeams[22].wins = 0; nbaTeams[22].losses = 0;
    nbaTeams[22].players[0].name = "Joel Embiid"; nbaTeams[22].players[0].rating = 92;
    nbaTeams[22].players[1].name = "Ben Simmons"; nbaTeams[22].players[1].rating = 87;
    nbaTeams[22].players[2].name = "Al Horford"; nbaTeams[22].players[2].rating = 87;
    nbaTeams[22].players[3].name = "Tobias Harris"; nbaTeams[22].players[3].rating = 86;
    nbaTeams[22].players[4].name = "Josh Richardson"; nbaTeams[22].players[4].rating=80;
    nbaTeams[22].players[5].name = "Mike Scott"; nbaTeams[22].players[5].rating = 75;
    nbaTeams[22].players[6].name = "Furkan Korkmaz"; nbaTeams[22].players[6].rating = 74;
    nbaTeams[22].players[7].name = "Matisse Thybulle"; nbaTeams[22].players[7].rating = 74;
    nbaTeams[22].players[8].name = "Kyle O'Quinn"; nbaTeams[22].players[8].rating = 74;
    nbaTeams[22].players[9].name = "Trey Burke"; nbaTeams[22].players[9].rating = 74;
    nbaTeams[22].players[10].name = "Jonah Bolden"; nbaTeams[22].players[10].rating = 73;
    nbaTeams[22].players[11].name = "James Ennis"; nbaTeams[22].players[11].rating = 73;
    nbaTeams[22].players[12].name = "Zhaire Smithh"; nbaTeams[22].players[12].rating=72;
    nbaTeams[22].players[13].name = "Raul Neto"; nbaTeams[22].players[13].rating = 72;
    nbaTeams[22].players[14].name = "Shake Milton"; nbaTeams[22].players[14].rating = 71;

    nbaTeams[23].name = "Phoenix Suns";
    nbaTeams[23].wins = 0; nbaTeams[23].losses = 0;
    nbaTeams[23].players[0].name = "Devin Booker"; nbaTeams[23].players[0].rating = 87;
    nbaTeams[23].players[1].name = "Deandre Ayton"; nbaTeams[23].players[1].rating = 83;
    nbaTeams[23].players[2].name = "Kelly Oubre Jr."; nbaTeams[23].players[2].rating = 80;
    nbaTeams[23].players[3].name = "Ricky Rubio"; nbaTeams[23].players[3].rating = 79;
    nbaTeams[23].players[4].name = "Aron Baynes"; nbaTeams[23].players[4].rating=78;
    nbaTeams[23].players[5].name = "Dario Saric"; nbaTeams[23].players[5].rating = 77;
    nbaTeams[23].players[6].name = "Frank Kaminsky"; nbaTeams[23].players[6].rating = 76;
    nbaTeams[23].players[7].name = "Mikal Bridges"; nbaTeams[23].players[7].rating = 75;
    nbaTeams[23].players[8].name = "Tyler Johnson"; nbaTeams[23].players[8].rating = 75;
    nbaTeams[23].players[9].name = "Cheick Diallo"; nbaTeams[23].players[9].rating = 74;
    nbaTeams[23].players[10].name = "Ty Jerome"; nbaTeams[23].players[10].rating = 72;
    nbaTeams[23].players[11].name = "Jevon Carter"; nbaTeams[23].players[11].rating = 72;
    nbaTeams[23].players[12].name = "Cameron Johnson"; nbaTeams[23].players[12].rating=71;
    nbaTeams[23].players[13].name = "Elie Okobo"; nbaTeams[23].players[13].rating = 69;
    nbaTeams[23].players[14].name = "Jalen Lecque"; nbaTeams[23].players[14].rating = 68;

    nbaTeams[24].name = "Portland Trailblazers";
    nbaTeams[24].wins = 0; nbaTeams[24].losses = 0;
    nbaTeams[24].players[0].name = "Damian Lillard"; nbaTeams[24].players[0].rating = 92;
    nbaTeams[24].players[1].name = "C.J. McCollum"; nbaTeams[24].players[1].rating = 86;
    nbaTeams[24].players[2].name = "Jusuf Nurkic"; nbaTeams[24].players[2].rating = 83;
    nbaTeams[24].players[3].name = "Hassan Whiteside"; nbaTeams[24].players[3].rating = 81;
    nbaTeams[24].players[4].name = "Rodney Hood"; nbaTeams[24].players[4].rating=76;
    nbaTeams[24].players[5].name = "Kent Bazemore"; nbaTeams[24].players[5].rating = 75;
    nbaTeams[24].players[6].name = "Mario Hezonja"; nbaTeams[24].players[6].rating = 75;
    nbaTeams[24].players[7].name = "Pau Gasol"; nbaTeams[24].players[7].rating = 75;
    nbaTeams[24].players[8].name = "Zack Collins"; nbaTeams[24].players[8].rating = 74;
    nbaTeams[24].players[9].name = "Anfernee Simons"; nbaTeams[24].players[9].rating = 74;
    nbaTeams[24].players[10].name = "Nassir Little"; nbaTeams[24].players[10].rating = 73;
    nbaTeams[24].players[11].name = "Skal Labissiere"; nbaTeams[24].players[11].rating = 72;
    nbaTeams[24].players[12].name = "Anthony Tolliver"; nbaTeams[24].players[12].rating=72;
    nbaTeams[24].players[13].name = "Gary Trent Jr."; nbaTeams[24].players[13].rating = 71;
    nbaTeams[24].players[14].name = "Jaylen Hoard"; nbaTeams[24].players[14].rating = 68;

}
