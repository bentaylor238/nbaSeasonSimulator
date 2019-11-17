//
// Created by Ben Taylor on 11/16/19.
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

struct Player {
    string name;
    int rating;
};

struct Team {
    Player players[15];
    string name;
    float rating;
    char conference;
    int wins;
    int losses;
    bool hasTraded = false;
};

Team* createPlayers();
float createRating(Team &team);
void playGame(Team &team1, Team &team2);
void tradePlayers(Team &team1, Team &team2);
void recalculateRating(Team &team);
Team* topEightInConference(Team *teams, string conf);
void printTeams(Team *teams, int numTeams);
int playoffGames(float team1, float team2);

int main(int argc, char **argv){
    struct Team *nbaTeams = (Team*)malloc(sizeof(Team)*30);
    nbaTeams = createPlayers();

    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);

    auto start = std::chrono::system_clock::now();

    srand(rank);

    if (rank == 3) {
        for (int i = 0; i < 30; i++) {
            cout << nbaTeams[i].name << " " << nbaTeams[i].rating << endl;
        }
    }

    // Simulate the first 40 games of the season for each team using 4 different processes
    if (!rank) {
//        cout << "Process 0" << endl;
        for (int i = 0; i < 30; i++) {
            for (int j = 1; j <= 5; j++) {
                playGame(nbaTeams[i], nbaTeams[(i + j) % 30]);
//                cout << "game " << i << " played on process " << rank << endl;
            }
        }
        int receiving = 1;
        MPI_Send(&receiving,1,MPI_INT,rank+1,0,MCW);
        cout << "Process 0 finished playing" << endl;
        MPI_Recv(&receiving, 1, MPI_INT, size-1, 0, MCW, MPI_STATUS_IGNORE);
    }
    else{
        int receiving = 1;
//        cout << "Process " << rank << endl;
        MPI_Recv(&receiving, 1, MPI_INT, rank-1,0,MCW, MPI_STATUS_IGNORE);
        for (int i = 0; i < 30; i++) {
            for (int j = 1; j <= 5; j++) {
                playGame(nbaTeams[i], nbaTeams[(i + j) % 30]);
//                cout << "game " << i << " played on process " << rank << endl;
            }
        }
        cout << "Process "<< rank << " finished playing" << endl;
        MPI_Send(&receiving,1,MPI_INT,(rank+1)%size, 0, MCW);
    }
    MPI_Barrier(MCW);
    sleep(1);

    // Trade deadline, so teams are allowed to trade with each other
    if (!rank) {
        for (int i = 0; i < (int)(30.0/size); i++) {
            cout << "Process " << rank << " is determining whether to trade team " << i << " " << (float)nbaTeams[i].wins/(nbaTeams[i].wins+nbaTeams[i].losses) << " " << nbaTeams[i].rating <<endl;
            if ((float)nbaTeams[i].wins/(nbaTeams[i].wins+nbaTeams[i].losses) < nbaTeams[i].rating && !nbaTeams[i].hasTraded) {
                int tradeConfirmed[2];
                MPI_Send(&i, 1, MPI_INT, rank+1, 0, MCW);
                MPI_Recv(&tradeConfirmed, 2, MPI_INT, rank+1, 0, MCW, MPI_STATUS_IGNORE);
                if (tradeConfirmed[0]) {
                    cout << "Process 0 is trading team " << i << " with team " << tradeConfirmed[1] << endl;
                    tradePlayers(nbaTeams[i], nbaTeams[tradeConfirmed[1]]);
                }
            }
        }
        int tradeSuggested = -1;
        MPI_Send(&tradeSuggested, 1, MPI_INT, rank+1, 0, MCW);


        int proposedTrade;
        MPI_Recv(&proposedTrade, 1, MPI_INT, size-1, 0, MCW, MPI_STATUS_IGNORE);
        while (proposedTrade != -1) {
            bool trade = false;
            for (int i = 0; i < (int)(30.0/size); i++) {
                if ((float)nbaTeams[i].wins/(nbaTeams[i].wins+nbaTeams[i].losses) < nbaTeams[i].rating && !nbaTeams[i].hasTraded) {
                    int tradeConfirmed[2] = {1, i};
                    MPI_Send(tradeConfirmed, 2, MPI_INT, size-1, 0, MCW);
                    trade = true;
                    break;
                }
            }
            int tradeRejected[2] = {0, 0};
            if (!trade) MPI_Send(tradeRejected, 2, MPI_INT, size-1, 0, MCW);
            MPI_Recv(&proposedTrade, 1, MPI_INT, size-1, 0, MCW, MPI_STATUS_IGNORE);
        }
        cout << "Finished trading with Process " << size-1 << endl;
        cout << "All trading has finished" << endl;
    }
    else {
        int proposedTrade;
        MPI_Recv(&proposedTrade, 1, MPI_INT, rank-1, 0, MCW, MPI_STATUS_IGNORE);
        int j = (int)(30.0/size)*rank;
        while (proposedTrade != -1) {
            bool trade = false;
            for (int i = j; i < (int)(30.0/size*(rank+1)); i++) {j++;
                if ((float)nbaTeams[i].wins/(nbaTeams[i].wins+nbaTeams[i].losses) < nbaTeams[i].rating && !nbaTeams[i].hasTraded) {
                    int tradeConfirmed[2] = {1, i};
                    MPI_Send(tradeConfirmed, 2, MPI_INT, rank-1, 0, MCW);
                    trade = true;
                    break;
                }
            }
            int tradeRejected[2] = {0, 0};
            if (!trade) MPI_Send(tradeRejected, 2, MPI_INT, rank-1, 0, MCW);
            MPI_Recv(&proposedTrade, 1, MPI_INT, rank-1, 0, MCW, MPI_STATUS_IGNORE);
        }
        cout << "Finished trading with Process "<< rank-1 << endl;

        for (int i = (int)(30.0*rank/size); i < (int)(30.0*(rank+1)/size); i++) {
            cout << "Process " << rank << " is determining whether to trade team " << i << " " << (float)nbaTeams[i].wins/(nbaTeams[i].wins+nbaTeams[i].losses) << " " << nbaTeams[i].rating <<endl;
            if ((float)nbaTeams[i].wins/(nbaTeams[i].wins+nbaTeams[i].losses) < nbaTeams[i].rating && !nbaTeams[i].hasTraded) {
                int tradeConfirmed[2];
//                int tradeSuggested[5] = {i, 0, 12, 13, 14};
                MPI_Send(&i, 1, MPI_INT, (rank+1)%size, 0, MCW);
                MPI_Recv(&tradeConfirmed, 2, MPI_INT, (rank+1)%size, 0, MCW, MPI_STATUS_IGNORE);
                if (tradeConfirmed[0]) {
                    cout << "Process " << rank << " reports that trade is about to happen with team " << tradeConfirmed[1] << endl;
                    tradePlayers(nbaTeams[i], nbaTeams[tradeConfirmed[1]]);
                }
            }
        }
        int tradeSuggested = -1;
        MPI_Send(&tradeSuggested, 1, MPI_INT, (rank+1)%size, 0, MCW);
        cout << "Process " << rank << " finished trading" << endl;
    }
    MPI_Barrier(MCW);

    // Simulate the remaining games
    if (!rank) {
        for (int i = 30; i < 60; i++) {
            for (int j = 1; j <= 5; j++) {
                playGame(nbaTeams[i], nbaTeams[(i + j) % 30]);
//                cout << "game " << i << " played on process " << rank << endl;
            }
        }
        int receiving = 1;
        MPI_Send(&receiving,1,MPI_INT,rank+1,0,MCW);
        cout << "Process 0 finished playing" << endl;
        MPI_Recv(&receiving, 1, MPI_INT, size-1, 0, MCW, MPI_STATUS_IGNORE);
    }
    else{
        int receiving = 1;
        MPI_Recv(&receiving, 1, MPI_INT, rank-1,0,MCW, MPI_STATUS_IGNORE);
        for (int i = 30; i < 60; i++) {
            for (int j = 1; j <= 5; j++) {
                playGame(nbaTeams[i], nbaTeams[(i + j) % 30]);
//                cout << "game " << i << " played on process " << rank << endl;
            }
        }
        cout << "Process "<< rank << " finished playing" << endl;
        MPI_Send(&receiving,1,MPI_INT,(rank+1)%size, 0, MCW);
    }
    MPI_Barrier(MCW);

    // Report individual process information back to 0
    if (!rank) {
        for (int i = 0; i < (30 * (size-1)); i++) {
            int teaminfo[3];
            MPI_Recv(&teaminfo, 3, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE);
            nbaTeams[teaminfo[0]].wins += teaminfo[1];
            nbaTeams[teaminfo[0]].losses += teaminfo[2];
        }
    }
    else {
        for (int i = 0; i < 30; i++) {
            int teaminfo[3] = {i, nbaTeams[i].wins, nbaTeams[i].losses};
            MPI_Send(teaminfo, 3, MPI_INT, 0, 0, MCW);
        }
    }

    // Report teams and their rankings per conference
    Team *west = (Team *) malloc(sizeof(Team) * 8);
    Team *east = (Team *) malloc(sizeof(Team) * 8);
    if (!rank) {
        cout << endl << "Western Conference Playoff Standings:" << endl;
        west = topEightInConference(nbaTeams, "W");
        cout << endl << "Eastern Conference Playoff Standings:" << endl;
        east = topEightInConference(nbaTeams, "E");
        cout << endl;
    }

    // Do playoffs with just top 16 teams
    if (!rank) {
        Team *round2West = (Team *) malloc(sizeof(Team) * 4);
        Team *round2East = (Team *) malloc(sizeof(Team) * 4);
        Team *round3West = (Team *) malloc(sizeof(Team) * 2);
        Team *round3East = (Team *) malloc(sizeof(Team) * 2);
        Team westChamp, eastChamp;
        for (int i = 0; i < 4; i++) {
            float round[2] = {west[i].rating, west[7-i].rating};
            MPI_Send(round, 2, MPI_FLOAT, rank+1, 0, MCW);
            int winner;
            MPI_Recv(&winner, 1, MPI_INT, rank+1, 0, MCW, MPI_STATUS_IGNORE);
            if (!winner) round2West[i] = west[i];
            else round2West[i] = west[7-i];
            cout << round2West[i].name << " is going to the next round" << endl;
        }
        for (int i = 0; i < 4; i++) {
            float round[2] = {east[i].rating, east[7-i].rating};
            MPI_Send(round, 2, MPI_FLOAT, rank+2, 0, MCW);
            int winner;
            MPI_Recv(&winner, 1, MPI_INT, rank+2, 0, MCW, MPI_STATUS_IGNORE);
            if (!winner) round2East[i] = east[i];
            else round2East[i] = east[7-i];
            cout << round2East[i].name << " is going to the next round" << endl;
        }
        cout << "First round of the playoffs completed" << endl << endl;
        for (int i = 0; i < 2; i++) {
            float round[2] = {round2West[i].rating, round2West[3-i].rating};
            MPI_Send(round, 2, MPI_FLOAT, rank+1, 0, MCW);
            int winner;
            MPI_Recv(&winner, 1, MPI_INT, rank+1, 0, MCW, MPI_STATUS_IGNORE);
            if (!winner) round3West[i] = round2West[i];
            else round3West[i] = round2West[3-i];
            cout << round3West[i].name << " is going to the next round" << endl;
        }
        for (int i = 0; i < 2; i++) {
            float round[2] = {round2East[i].rating, round2East[3-i].rating};
            MPI_Send(round, 2, MPI_FLOAT, rank+2, 0, MCW);
            int winner;
            MPI_Recv(&winner, 1, MPI_INT, rank+2, 0, MCW, MPI_STATUS_IGNORE);
            if (!winner) round3East[i] = round2East[i];
            else round3East[i] = round2East[3-i];
            cout << round3East[i].name << " is going to the next round" << endl;
        }
        cout << "Second round of the playoffs completed" << endl << endl;

        //Conference Championships
        float round[2] = {round3West[0].rating, round3West[1].rating};
        MPI_Send(round, 2, MPI_FLOAT, rank+1, 0, MCW);
        int winner;
        MPI_Recv(&winner, 1, MPI_INT, rank+1, 0, MCW, MPI_STATUS_IGNORE);
        if (!winner) westChamp = round3West[0];
        else westChamp = round3West[1];
        cout << westChamp.name << " is going to the next round" << endl;

        round[0] = round3East[0].rating; round[1] = round3East[1].rating;
        MPI_Send(round, 2, MPI_FLOAT, rank+2, 0, MCW);
        MPI_Recv(&winner, 1, MPI_INT, rank+2, 0, MCW, MPI_STATUS_IGNORE);
        if (!winner) eastChamp = round3East[0];
        else eastChamp = round3East[1];
        cout << eastChamp.name << " is going to the next round" << endl;
        cout << "Conference Championships completed" << endl << endl;

        round[0] = westChamp.rating; round[1] = eastChamp.rating;
        MPI_Send(round, 2, MPI_FLOAT, rank+1, 0, MCW);
        MPI_Recv(&winner, 1, MPI_INT, rank+1, 0, MCW, MPI_STATUS_IGNORE);
        if (!winner) { // West champ won
            cout << westChamp.name << " has won the NBA Championship" << endl;
        }
        else { // East Champ won
            cout << eastChamp.name << " has won the NBA Championship" << endl;
        }

        round[0] = -1.0; round[1] = -1.0;
        MPI_Send(round, 2, MPI_FLOAT, rank+1, 0, MCW);
        MPI_Send(round, 2, MPI_FLOAT, rank+2, 0, MCW);
    }
    else if (rank != (size-1)) {
        float round[2];
        MPI_Recv(&round, 2, MPI_FLOAT, 0, 0, MCW, MPI_STATUS_IGNORE);
        while (round[0] != -1.0) {
            int winner = playoffGames(round[0], round[1]);
            MPI_Send(&winner, 1, MPI_INT, 0, 0, MCW);
            MPI_Recv(&round, 2, MPI_FLOAT, 0, 0, MCW, MPI_STATUS_IGNORE);
        }
    }

    if (!rank) {
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        cout << "The NBA season took " << elapsed_seconds.count() << " seconds" << endl;
    }

    cout << "Process " << rank << " has finished its duties" << endl;

    MPI_Finalize();

    return 0;
}

int playoffGames(float team1, float team2) {
    int team1Wins = 0;
    int team2Wins = 0;
    for (int i = 0; i < 7; i++) {
        int team1Points = 80;
        int team2Points = 80;
        while (team1Points == team2Points) {
            int rand1 = team1 * (rand() % 100);
            int rand2 = team2 * (rand() % 100);
            int score1 = ((rand() % 40) + rand1) / 2;
            int score2 = ((rand() % 40) + rand2) / 2;
            team1Points += score1;
            team2Points += score2;
        }
        if (team1Points > team2Points) {
            team1Wins++;
            if (team1Wins == 4) break;
        }
        else {
            team2Wins++;
            if (team2Wins == 4) break;
        }
    }

    return team1Wins == 4 ? 0 : 1;
}

Team* topEightInConference(Team *teams, string conf) {
    Team *best = (Team*)malloc(sizeof(Team)*8);
    for (int i = 0; i < 8; i++) {
        best[i].wins = 0;
    }

    for (int i = 0; i < 30; i++) {
        if (teams[i].conference == conf[0] && teams[i].wins > best[0].wins) {
            for (int j = 7; j >=0; j--) {
                if (j == 0) best[j] = teams[i];
                else best[j] = best[j-1];
            }
        }
        else if (teams[i].conference == conf[0] && teams[i].wins > best[1].wins) {
            for (int j = 7; j >=1; j--) {
                if (j == 1) best[j] = teams[i];
                else best[j] = best[j-1];
            }
        }
        else if (teams[i].conference == conf[0] && teams[i].wins > best[2].wins) {
            for (int j = 7; j >=2; j--) {
                if (j == 2) best[j] = teams[i];
                else best[j] = best[j-1];
            }
        }
        else if (teams[i].conference == conf[0] && teams[i].wins > best[3].wins) {
            for (int j = 7; j >=3; j--) {
                if (j == 3) best[j] = teams[i];
                else best[j] = best[j-1];
            }
        }
        else if (teams[i].conference == conf[0] && teams[i].wins > best[4].wins) {
            for (int j = 7; j >=4; j--) {
                if (j == 4) best[j] = teams[i];
                else best[j] = best[j-1];
            }
        }
        else if (teams[i].conference == conf[0] && teams[i].wins > best[5].wins) {
            for (int j = 7; j >=5; j--) {
                if (j == 5) best[j] = teams[i];
                else best[j] = best[j-1];
            }
        }
        else if (teams[i].conference == conf[0] && teams[i].wins > best[6].wins) {
            for (int j = 7; j >=6; j--) {
                if (j == 6) best[j] = teams[i];
                else best[j] = best[j-1];
            }
        }
        else if (teams[i].conference == conf[0] && teams[i].wins > best[7].wins) {
            best[7] = teams[i];
        }
    }
    printTeams(best, 8);

    return best;
}

void printTeams(Team *teams, int numTeams) {
    for (int i = 0; i < numTeams; i++) {
        cout << i+1 << ") " << teams[i].name << " " << teams[i].wins << " " << teams[i].losses << endl;
    }
}

void tradePlayers(Team &team1, Team &team2) {
    if (team1.hasTraded) return;
    if (team2.hasTraded) return;
    team1.hasTraded = true; team2.hasTraded = true;
    cout << "Made it to the trading function" << endl;
    Player team1Players[4]; team1Players[0] = team1.players[0]; team1Players[1] = team1.players[12]; team1Players[2] = team1.players[13]; team1Players[3] = team1.players[14];
    Player team2Players[4]; team2Players[0] = team2.players[5]; team2Players[1] = team2.players[6]; team2Players[2] = team2.players[7]; team2Players[3] = team2.players[8];

   cout << team1.name << " " << team2.name << endl;

    Player temp = team1.players[0];
    team1.players[0] = team2.players[5];
    team2.players[5] = temp;
    cout << team1.players[0].name << " " << team2.players[5].name << endl;

    temp = team1.players[12];
    team1.players[12] = team2.players[6];
    team2.players[6] = temp;
    cout << team1.players[12].name << " " << team2.players[6].name << endl;

    temp = team1.players[13];
    team1.players[13] = team2.players[7];
    team2.players[7] = temp;
    cout << team1.players[13].name << " " << team2.players[7].name << endl;

    temp = team1.players[14];
    team1.players[14] = team2.players[8];
    team2.players[8] = temp;
    cout << team1.players[14].name << " " << team2.players[8].name << endl;
    recalculateRating(team1);
    recalculateRating(team2);
    cout << endl;
}

void recalculateRating(Team &team) {
    int first, second, third, fourth, fifth;
    first = second = third = fourth = fifth = -100;
    for (int i = 0; i < 15; i++) {
        if (team.players[i].rating > first) {
            fifth = fourth;
            fourth = third;
            third = second;
            second = first;
            first = team.players[i].rating;
        }
        else if (team.players[i].rating > second) {
            fifth = fourth;
            fourth = third;
            third = second;
            second = team.players[i].rating;
        }
        else if (team.players[i].rating > third) {
            fifth = fourth;
            fourth = third;
            third = team.players[i].rating;
        }
        else if (team.players[i].rating > fourth) {
            fifth = fourth;
            fourth = team.players[i].rating;
        }
        else if (team.players[i].rating > fifth) {
            fifth = team.players[i].rating;
        }
    }
    int topfive = 0;
    topfive += (first*10); topfive+= (second*9); topfive+= (third*8); topfive+=(fourth*7); topfive +=(fifth*6);
    float averageFive = (float)topfive/15.0;
    float averageTen = (float)team.players[9].rating;
    float totalAverage = (averageFive+averageTen)/5.0;
    team.rating = totalAverage * 0.01;
    cout << team.name << " " << team.rating << endl;
}

void playGame(Team &team1, Team &team2) {
    int team1Points = 80; int team2Points = 80;
    while (team1Points == team2Points) {
        int rand1 = team1.rating*(rand()%100);
        int rand2 = team2.rating*(rand()%100);
        int score1 = ((rand()%40)+rand1)/2;
        int score2 = ((rand()%40)+rand2)/2;
        team1Points += score1;
        team2Points += score2;
    }
    if (team1Points > team2Points) {team1.wins++;team2.losses++;}
    else {team2.wins++;team1.losses++;}
}


float createRating(Team &team) {
    int topfive = 0;
    for (int i = 0; i < 5; i++) {
        topfive += (team.players[i].rating*(10-i));
    }
    float averageFive = (float)topfive/15.0;

    int nextTen = 0;
    for (int i = 5; i < 15; i++) {
        nextTen += (team.players[i].rating*(15/i));
    }
    float averageTen = (float)nextTen/14.0;
    float totalAverage = (averageFive+averageTen)/5.0;
    return totalAverage * 0.01;
}

Team* createPlayers() {
    struct Team *nbaTeams;
    nbaTeams = (Team*)malloc(sizeof(Team)*30);
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
    nbaTeams[0].rating = createRating(nbaTeams[0]);
    nbaTeams[0].conference = 'E';


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
    nbaTeams[1].rating = createRating(nbaTeams[1]);
    nbaTeams[1].conference = 'E';


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
    nbaTeams[2].rating = createRating(nbaTeams[2]);
    nbaTeams[2].conference = 'E';

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
    nbaTeams[3].rating = createRating(nbaTeams[3]);
    nbaTeams[3].conference = 'E';

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
    nbaTeams[4].rating = createRating(nbaTeams[4]);
    nbaTeams[4].conference = 'E';

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
    nbaTeams[5].rating = createRating(nbaTeams[5]);
    nbaTeams[5].conference = 'E';

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
    nbaTeams[6].rating = createRating(nbaTeams[6]);
    nbaTeams[6].conference = 'W';

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
    nbaTeams[7].rating = createRating(nbaTeams[7]);
    nbaTeams[7].conference = 'W';

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
    nbaTeams[8].rating = createRating(nbaTeams[8]);
    nbaTeams[8].conference = 'E';

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
    nbaTeams[9].rating = createRating(nbaTeams[9]);
    nbaTeams[9].conference = 'W';

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
    nbaTeams[10].rating = createRating(nbaTeams[10]);
    nbaTeams[10].conference = 'W';

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
    nbaTeams[11].rating = createRating(nbaTeams[11]);
    nbaTeams[11].conference = 'E';

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
    nbaTeams[12].rating = createRating(nbaTeams[12]);
    nbaTeams[12].conference = 'W';

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
    nbaTeams[13].rating = createRating(nbaTeams[13]);
    nbaTeams[13].conference = 'W';

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
    nbaTeams[14].rating = createRating(nbaTeams[14]);
    nbaTeams[14].conference = 'W';

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
    nbaTeams[15].rating = createRating(nbaTeams[15]);
    nbaTeams[15].conference = 'E';

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
    nbaTeams[16].rating = createRating(nbaTeams[16]);
    nbaTeams[16].conference = 'E';

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
    nbaTeams[17].rating = createRating(nbaTeams[17]);
    nbaTeams[17].conference = 'W';

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
    nbaTeams[18].rating = createRating(nbaTeams[18]);
    nbaTeams[18].conference = 'W';

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
    nbaTeams[19].rating = createRating(nbaTeams[19]);
    nbaTeams[19].conference = 'E';

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
    nbaTeams[20].rating = createRating(nbaTeams[20]);
    nbaTeams[20].conference = 'W';

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
    nbaTeams[21].rating = createRating(nbaTeams[21]);
    nbaTeams[21].conference = 'E';

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
    nbaTeams[22].rating = createRating(nbaTeams[22]);
    nbaTeams[22].conference = 'E';

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
    nbaTeams[23].rating = createRating(nbaTeams[23]);
    nbaTeams[23].conference = 'W';

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
    nbaTeams[24].rating = createRating(nbaTeams[24]);
    nbaTeams[24].conference = 'W';

    nbaTeams[25].name = "Sacramento Kings";
    nbaTeams[25].wins = 0; nbaTeams[25].losses = 0;
    nbaTeams[25].players[0].name = "De'Aaron Fox"; nbaTeams[25].players[0].rating = 85;
    nbaTeams[25].players[1].name = "Marvin Bagley III"; nbaTeams[25].players[1].rating = 83;
    nbaTeams[25].players[2].name = "Buddy Hield"; nbaTeams[25].players[2].rating = 82;
    nbaTeams[25].players[3].name = "Harrison Barnes"; nbaTeams[25].players[3].rating = 79;
    nbaTeams[25].players[4].name = "Nemanja Bjelica"; nbaTeams[25].players[4].rating=78;
    nbaTeams[25].players[5].name = "Richaun Holmes"; nbaTeams[25].players[5].rating = 77;
    nbaTeams[25].players[6].name = "Bogdan Bogdanovic"; nbaTeams[25].players[6].rating = 76;
    nbaTeams[25].players[7].name = "Dewayne Dedmon"; nbaTeams[25].players[7].rating = 76;
    nbaTeams[25].players[8].name = "Trevor Ariza"; nbaTeams[25].players[8].rating = 75;
    nbaTeams[25].players[9].name = "Harry Giles"; nbaTeams[25].players[9].rating = 74;
    nbaTeams[25].players[10].name = "Yogi Ferrell"; nbaTeams[25].players[10].rating = 74;
    nbaTeams[25].players[11].name = "Cory Joseph"; nbaTeams[25].players[11].rating = 73;
    nbaTeams[25].players[12].name = "Caleb Swanigan"; nbaTeams[25].players[12].rating=70;
    nbaTeams[25].players[13].name = "Justin James"; nbaTeams[25].players[13].rating = 70;
    nbaTeams[25].players[14].name = "Wenyen Gabriel"; nbaTeams[25].players[14].rating = 68;
    nbaTeams[25].rating = createRating(nbaTeams[25]);
    nbaTeams[25].conference = 'W';

    nbaTeams[26].name = "San Antonio Spurs";
    nbaTeams[26].wins = 0; nbaTeams[26].losses = 0;
    nbaTeams[26].players[0].name = "LeMarcus Aldridge"; nbaTeams[26].players[0].rating = 86;
    nbaTeams[26].players[1].name = "DeMar DeRozan"; nbaTeams[26].players[1].rating = 86;
    nbaTeams[26].players[2].name = "Rudy Gay"; nbaTeams[26].players[2].rating = 81;
    nbaTeams[26].players[3].name = "Dejounte Murray"; nbaTeams[26].players[3].rating = 80;
    nbaTeams[26].players[4].name = "Derrick White"; nbaTeams[26].players[4].rating=79;
    nbaTeams[26].players[5].name = "Jakob Poeltl"; nbaTeams[26].players[5].rating = 77;
    nbaTeams[26].players[6].name = "Patty Mills"; nbaTeams[26].players[6].rating = 76;
    nbaTeams[26].players[7].name = "DeMarre Carroll"; nbaTeams[26].players[7].rating = 76;
    nbaTeams[26].players[8].name = "Bryn Forbes"; nbaTeams[26].players[8].rating = 75;
    nbaTeams[26].players[9].name = "Trey Lyles"; nbaTeams[26].players[9].rating = 75;
    nbaTeams[26].players[10].name = "Marco Belinelli"; nbaTeams[26].players[10].rating = 74;
    nbaTeams[26].players[11].name = "Lonnie Walker IV"; nbaTeams[26].players[11].rating = 73;
    nbaTeams[26].players[12].name = "Luka Samanic"; nbaTeams[26].players[12].rating=72;
    nbaTeams[26].players[13].name = "Keldon Johnson"; nbaTeams[26].players[13].rating = 72;
    nbaTeams[26].players[14].name = "Chimezie Metu"; nbaTeams[26].players[14].rating = 71;
    nbaTeams[26].rating = createRating(nbaTeams[26]);
    nbaTeams[26].conference = 'W';

    nbaTeams[27].name = "Toronto Raptors";
    nbaTeams[27].wins = 0; nbaTeams[27].losses = 0;
    nbaTeams[27].players[0].name = "Pascal Siakim"; nbaTeams[27].players[0].rating = 87;
    nbaTeams[27].players[1].name = "Kyle Lowry"; nbaTeams[27].players[1].rating = 86;
    nbaTeams[27].players[2].name = "Serge Ibaka"; nbaTeams[27].players[2].rating = 80;
    nbaTeams[27].players[3].name = "Fred VanVleet"; nbaTeams[27].players[3].rating = 79;
    nbaTeams[27].players[4].name = "Marc Gasol"; nbaTeams[27].players[4].rating=79;
    nbaTeams[27].players[5].name = "OG Anunoby"; nbaTeams[27].players[5].rating = 77;
    nbaTeams[27].players[6].name = "Rondae Hollis-Jefferson"; nbaTeams[27].players[6].rating = 75;
    nbaTeams[27].players[7].name = "Norman Powell"; nbaTeams[27].players[7].rating = 74;
    nbaTeams[27].players[8].name = "Stanley Johnson"; nbaTeams[27].players[8].rating = 73;
    nbaTeams[27].players[9].name = "Patrick McCaw"; nbaTeams[27].players[9].rating = 72;
    nbaTeams[27].players[10].name = "Matt Thomas"; nbaTeams[27].players[10].rating = 72;
    nbaTeams[27].players[11].name = "Chris Boucher"; nbaTeams[27].players[11].rating = 71;
    nbaTeams[27].players[12].name = "Terence Davis"; nbaTeams[27].players[12].rating=70;
    nbaTeams[27].players[13].name = "Malcom Miller"; nbaTeams[27].players[13].rating = 69;
    nbaTeams[27].players[14].name = "Dewan Hernandez"; nbaTeams[27].players[14].rating = 68;
    nbaTeams[27].rating = createRating(nbaTeams[27]);
    nbaTeams[27].conference = 'E';

    nbaTeams[28].name = "Utah Jazz";
    nbaTeams[28].wins = 0; nbaTeams[28].losses = 0;
    nbaTeams[28].players[0].name = "Donovan Mitchell"; nbaTeams[28].players[0].rating = 89;
    nbaTeams[28].players[1].name = "Rudy Gobert"; nbaTeams[28].players[1].rating = 86;
    nbaTeams[28].players[2].name = "Mike Conley"; nbaTeams[28].players[2].rating = 83;
    nbaTeams[28].players[3].name = "Bojan Bogdanovic"; nbaTeams[28].players[3].rating = 83;
    nbaTeams[28].players[4].name = "Joe Ingles"; nbaTeams[28].players[4].rating=78;
    nbaTeams[28].players[5].name = "Ed Davis"; nbaTeams[28].players[5].rating = 76;
    nbaTeams[28].players[6].name = "Emmanuel Mudiay"; nbaTeams[28].players[6].rating = 76;
    nbaTeams[28].players[7].name = "Dante Exum"; nbaTeams[28].players[7].rating = 75;
    nbaTeams[28].players[8].name = "Jeff Green"; nbaTeams[28].players[8].rating = 75;
    nbaTeams[28].players[9].name = "Royce O'Neale"; nbaTeams[28].players[9].rating = 73;
    nbaTeams[28].players[10].name = "Georges Niang"; nbaTeams[28].players[10].rating = 72;
    nbaTeams[28].players[11].name = "Nigel Williams-Goss"; nbaTeams[28].players[11].rating = 72;
    nbaTeams[28].players[12].name = "Tony Bradley Jr."; nbaTeams[28].players[12].rating=70;
    nbaTeams[28].players[13].name = "Miye Oni"; nbaTeams[28].players[13].rating = 69;
    nbaTeams[28].players[14].name = "Stanton Kidd"; nbaTeams[28].players[14].rating = 68;
    nbaTeams[28].rating = createRating(nbaTeams[28]);
    nbaTeams[28].conference = 'W';

    nbaTeams[29].name = "Washington Wizards";
    nbaTeams[29].wins = 0; nbaTeams[29].losses = 0;
    nbaTeams[29].players[0].name = "Bradley Beal"; nbaTeams[29].players[0].rating = 87;
    nbaTeams[29].players[1].name = "John Wall"; nbaTeams[29].players[1].rating = 86;
    nbaTeams[29].players[2].name = "Thomas Bryant"; nbaTeams[29].players[2].rating = 78;
    nbaTeams[29].players[3].name = "Isaiah Thomas"; nbaTeams[29].players[3].rating = 78;
    nbaTeams[29].players[4].name = "Rui Hachimura"; nbaTeams[29].players[4].rating=76;
    nbaTeams[29].players[5].name = "Davis Bertans"; nbaTeams[29].players[5].rating = 76;
    nbaTeams[29].players[6].name = "Troy Brown Jr."; nbaTeams[29].players[6].rating = 74;
    nbaTeams[29].players[7].name = "Moritz Wagner"; nbaTeams[29].players[7].rating = 74;
    nbaTeams[29].players[8].name = "Ish Smith"; nbaTeams[29].players[8].rating = 73;
    nbaTeams[29].players[9].name = "C.J. Miles"; nbaTeams[29].players[9].rating = 73;
    nbaTeams[29].players[10].name = "Ian Mahinmi"; nbaTeams[29].players[10].rating = 72;
    nbaTeams[29].players[11].name = "Isaac Bonga"; nbaTeams[29].players[11].rating = 72;
    nbaTeams[29].players[12].name = "Admiral Schofield"; nbaTeams[29].players[12].rating=71;
    nbaTeams[29].players[13].name = "Jordan Mcrae"; nbaTeams[29].players[13].rating = 68;
    nbaTeams[29].players[14].name = "Chris Chiozza"; nbaTeams[29].players[14].rating = 68;
    nbaTeams[29].rating = createRating(nbaTeams[29]);
    nbaTeams[29].conference = 'E';

    return nbaTeams;
}
