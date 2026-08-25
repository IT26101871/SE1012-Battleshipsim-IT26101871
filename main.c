#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_ESCORTS 20
#define G 9.81

struct Escort
{
    int id;
    int type;
    int x;
    int y;

    float impactPower;
    float gamma;
    float minAngle;
    float maxAngle;
    float minVelocity;
    float maxVelocity;

    float firingTime;

    int destroyed;
    int firingCount;
};

struct Battleship
{
    int type;
    int x;
    int y;

    float maxVelocity;
    float gamma;
    float firingTime;

    int firingCount;
};

float randomFloat(float min, float max)
{
    return min + ((float)rand() / RAND_MAX) * (max - min);
}

float distanceBetween(int x1, int y1, int x2, int y2)
{
    return sqrt(
        pow(x2 - x1, 2) +
        pow(y2 - y1, 2)
    );
}

/* Check whether projectile can reach target */
int canHit(
    float distance,
    float velocity,
    float angle
)
{
    float angleRad;
    float range;

    angleRad = angle * M_PI / 180.0;

    range =
        (velocity * velocity *
         sin(2 * angleRad)) / G;

    if (range >= distance)
    {
        return 1;
    }

    return 0;
}

/* Set Escort Ship properties */
void setupEscort(struct Escort *e)
{
    if (e->type == 1)
    {
        /* EA */
        e->impactPower = 0.08;
        e->minAngle = 20;
        e->maxAngle = 90;
        e->minVelocity = 20;
        e->maxVelocity = 60;
        e->gamma = 0.02;
        e->firingTime = 4;
    }
    else if (e->type == 2)
    {
        /* EB */
        e->impactPower = 0.06;
        e->minAngle = 30;
        e->maxAngle = 90;
        e->minVelocity = 20;
        e->maxVelocity = 60;
        e->gamma = 0.03;
        e->firingTime = 5;
    }
    else if (e->type == 3)
    {
        /* EC */
        e->impactPower = 0.07;
        e->minAngle = 25;
        e->maxAngle = 90;
        e->minVelocity = 20;
        e->maxVelocity = 60;
        e->gamma = 0.04;
        e->firingTime = 6;
    }
    else if (e->type == 4)
    {
        /* ED */
        e->impactPower = 0.05;
        e->minAngle = 50;
        e->maxAngle = 90;
        e->minVelocity = 20;
        e->maxVelocity = 60;
        e->gamma = 0.05;
        e->firingTime = 7;
    }
    else
    {
        /* EE */
        e->impactPower = 0.04;
        e->minAngle = 70;
        e->maxAngle = 90;
        e->minVelocity = 20;
        e->maxVelocity = 60;
        e->gamma = 0.06;
        e->firingTime = 8;
    }
}

/* Display Escort Ship */
void displayEscort(struct Escort e)
{
    printf(
        "E%d  Type: E%d  Position: (%d,%d)\n",
        e.id,
        e.type,
        e.x,
        e.y
    );

    printf(
        "    Impact: %.2f  Angle: %.0f-%.0f\n",
        e.impactPower,
        e.minAngle,
        e.maxAngle
    );

    printf(
        "    Velocity: %.0f-%.0f  Firing Time: %.1f\n",
        e.minVelocity,
        e.maxVelocity,
        e.firingTime
    );
}

int main()
{
    int choice;
    int confirm;

    int battlefieldSize;
    int numberOfShips;
    int numberOfPoints;

    int i;
    int j;
    int k;

    int destroyedCount;

    float battleshipDamage;

    float distance;
    float angle;
    float velocity;

    struct Battleship B;
    struct Escort E[MAX_ESCORTS];

    FILE *file;
    char line[300];

    srand(time(NULL));

    do
    {
        printf("\n");
        printf("====================================\n");
        printf("       NAVAL BATTLE SIMULATOR\n");
        printf("====================================\n");

        printf("1. Start Simulation\n");
        printf("2. View Instructions\n");
        printf("3. Simulation Statistics\n");
        printf("4. Exit\n");

        printf("\nEnter choice: ");
        scanf("%d", &choice);

        /* ==================================
           START SIMULATION
           ================================== */

        if (choice == 1)
        {
            destroyedCount = 0;
            battleshipDamage = 0;

            printf("\n========== SETUP ==========\n");

            printf("Enter battlefield size D: ");
            scanf("%d", &battlefieldSize);

            printf("Enter number of Escort Ships: ");
            scanf("%d", &numberOfShips);

            if (numberOfShips > MAX_ESCORTS)
            {
                numberOfShips = MAX_ESCORTS;

                printf(
                    "Maximum %d ships allowed.\n",
                    MAX_ESCORTS
                );
            }

            printf("Enter number of path points: ");
            scanf("%d", &numberOfPoints);

            /* Battleship type */
            printf("\nBattleship Types:\n");
            printf("1. USS Iowa\n");
            printf("2. King George V\n");
            printf("3. Richelieu\n");
            printf("4. Sovetsky Soyuz\n");

            printf("Enter Battleship type: ");
            scanf("%d", &B.type);

            B.x = rand() % battlefieldSize;
            B.y = rand() % battlefieldSize;

            printf(
                "\nBattleship starting position: (%d,%d)\n",
                B.x,
                B.y
            );

            printf(
                "Enter Battleship maximum velocity: "
            );
            scanf("%f", &B.maxVelocity);

            printf(
                "Enter Battleship gamma value: "
            );
            scanf("%f", &B.gamma);

            printf(
                "Enter Battleship firing time: "
            );
            scanf("%f", &B.firingTime);

            B.firingCount = 0;

            /* ==================================
               CREATE ESCORT SHIPS
               ================================== */

            for (i = 0; i < numberOfShips; i++)
            {
                E[i].id = i + 1;

                E[i].type =
                    rand() % 5 + 1;

                E[i].x =
                    rand() % battlefieldSize;

                E[i].y =
                    rand() % battlefieldSize;

                E[i].destroyed = 0;

                E[i].firingCount = 0;

                setupEscort(&E[i]);
            }

            /* ==================================
               SAVE INITIAL CONDITIONS
               ================================== */

            file =
                fopen("results.txt", "w");

            if (file == NULL)
            {
                printf(
                    "Error creating results file.\n"
                );
                return 1;
            }

            fprintf(
                file,
                "NAVAL BATTLE SIMULATOR\n"
            );

            fprintf(
                file,
                "========================\n\n"
            );

            fprintf(
                file,
                "INITIAL CONDITIONS\n"
            );

            fprintf(
                file,
                "Battlefield Size: %d x %d\n",
                battlefieldSize,
                battlefieldSize
            );

            fprintf(
                file,
                "Battleship Type: %d\n",
                B.type
            );

            fprintf(
                file,
                "Battleship Position: (%d,%d)\n",
                B.x,
                B.y
            );

            fprintf(
                file,
                "Battleship Max Velocity: %.2f\n",
                B.maxVelocity
            );

            fprintf(
                file,
                "Battleship Gamma: %.4f\n",
                B.gamma
            );

            fprintf(
                file,
                "Battleship Firing Time: %.2f\n",
                B.firingTime
            );

            fprintf(
                file,
                "Number of Escort Ships: %d\n\n",
                numberOfShips
            );

            for (i = 0; i < numberOfShips; i++)
            {
                fprintf(
                    file,
                    "E%d Type E%d Position (%d,%d) "
                    "Impact %.2f Angle %.0f-%.0f "
                    "Velocity %.0f-%.0f "
                    "FiringTime %.2f Gamma %.4f\n",

                    E[i].id,
                    E[i].type,
                    E[i].x,
                    E[i].y,

                    E[i].impactPower,
                    E[i].minAngle,
                    E[i].maxAngle,

                    E[i].minVelocity,
                    E[i].maxVelocity,

                    E[i].firingTime,
                    E[i].gamma
                );
            }

            fclose(file);

            /* ==================================
               DISPLAY BATTLEFIELD
               ================================== */

            printf("\n========== BATTLEFIELD ==========\n");

            printf(
                "Size: %d x %d\n",
                battlefieldSize,
                battlefieldSize
            );

            printf(
                "Battleship: (%d,%d)\n",
                B.x,
                B.y
            );

            printf("\nEscort Ships:\n");

            for (i = 0; i < numberOfShips; i++)
            {
                displayEscort(E[i]);
            }

            /* ==================================
               SIMULATION
               ================================== */

            printf("\n========== SIMULATION ==========\n");

            for (j = 1;
                 j <= numberOfPoints;
                 j++)
            {
                /*
                   Generate Battleship movement point
                */

                B.x =
                    rand() % battlefieldSize;

                B.y =
                    rand() % battlefieldSize;

                printf(
                    "\nPath Point %d\n",
                    j
                );

                printf(
                    "Battleship: (%d,%d)\n",
                    B.x,
                    B.y
                );

                /*
                   Find and attack Escort Ships
                   in range.
                   
                   Strategy:
                   Nearest ship first.
                */

                for (k = 0;
                     k < numberOfShips;
                     k++)
                {
                    int nearest = -1;

                    float nearestDistance =
                        999999;

                    /*
                       Find nearest alive ship
                    */

                    for (i = 0;
                         i < numberOfShips;
                         i++)
                    {
                        if (E[i].destroyed == 1)
                        {
                            continue;
                        }

                        distance =
                            distanceBetween(
                                B.x,
                                B.y,
                                E[i].x,
                                E[i].y
                            );

                        if (distance <
                            nearestDistance)
                        {
                            nearestDistance =
                                distance;

                            nearest = i;
                        }
                    }

                    if (nearest == -1)
                    {
                        break;
                    }

                    /*
                       Random projectile
                    */

                    angle =
                        randomFloat(
                            0,
                            90
                        );

                    velocity =
                        randomFloat(
                            B.maxVelocity * 0.5,
                            B.maxVelocity
                        );

                    /*
                       Check whether target
                       is reachable
                    */

                    if (canHit(
                            nearestDistance,
                            velocity,
                            angle
                        ))
                    {
                        B.firingCount++;

                        /*
                           Impact degradation
                        */

                        float BImpact;

                        BImpact =
                            exp(
                                -B.gamma
                                *
                                B.firingCount
                            );

                        printf(
                            "\nB fires at E%d\n",
                            E[nearest].id
                        );

                        printf(
                            "Distance: %.2f\n",
                            nearestDistance
                        );

                        printf(
                            "Angle: %.2f\n",
                            angle
                        );

                        printf(
                            "Velocity: %.2f\n",
                            velocity
                        );

                        printf(
                            "Current B impact: %.4f\n",
                            BImpact
                        );

                        /*
                           In early/simple simulation,
                           one successful B attack
                           destroys E.
                        */

                        E[nearest].destroyed = 1;

                        destroyedCount++;

                        printf(
                            "E%d destroyed!\n",
                            E[nearest].id
                        );
                    }
                }

                /*
                   Escort Ships attack Battleship
                */

                printf(
                    "\n--- Escort Attacks ---\n"
                );

                for (i = 0;
                     i < numberOfShips;
                     i++)
                {
                    if (E[i].destroyed == 1)
                    {
                        continue;
                    }

                    distance =
                        distanceBetween(
                            E[i].x,
                            E[i].y,
                            B.x,
                            B.y
                        );

                    /*
                       Use random angle and velocity
                    */

                    angle =
                        randomFloat(
                            E[i].minAngle,
                            E[i].maxAngle
                        );

                    velocity =
                        randomFloat(
                            E[i].minVelocity,
                            E[i].maxVelocity
                        );

                    if (canHit(
                            distance,
                            velocity,
                            angle
                        ))
                    {
                        E[i].firingCount++;

                        /*
                           Impact degradation:
                           IPn = IP0 * e^(-gamma*n)
                        */

                        float currentImpact;

                        currentImpact =
                            E[i].impactPower
                            *
                            exp(
                                -E[i].gamma
                                *
                                E[i].firingCount
                            );

                        battleshipDamage +=
                            currentImpact;

                        printf(
                            "E%d fired at Battleship\n",
                            E[i].id
                        );

                        printf(
                            "Current impact: %.4f\n",
                            currentImpact
                        );

                        printf(
                            "Total B damage: %.2f%%\n",
                            battleshipDamage * 100
                        );

                        /*
                           Battleship destroyed
                           if damage reaches 100%
                        */

                        if (battleshipDamage >= 1.0)
                        {
                            printf(
                                "\nBATTLESHIP DESTROYED!\n"
                            );

                            j =
                                numberOfPoints + 1;

                            break;
                        }
                    }
                }

                if (battleshipDamage >= 1.0)
                {
                    break;
                }
            }

            /* ==================================
               FINAL RESULTS
               ================================== */

            printf("\n");
            printf(
                "====================================\n"
            );

            printf(
                "          FINAL RESULTS\n"
            );

            printf(
                "====================================\n"
            );

            printf(
                "Escort Ships Destroyed: %d\n",
                destroyedCount
            );

            printf(
                "Escort Ships Remaining: %d\n",
                numberOfShips -
                destroyedCount
            );

            printf(
                "Battleship Damage: %.2f%%\n",
                battleshipDamage * 100
            );

            /*
               Save final results
            */

            file =
                fopen(
                    "results.txt",
                    "a"
                );

            if (file != NULL)
            {
                fprintf(
                    file,
                    "\n\nFINAL RESULTS\n"
                );

                fprintf(
                    file,
                    "================\n"
                );

                fprintf(
                    file,
                    "Escort Ships Destroyed: %d\n",
                    destroyedCount
                );

                fprintf(
                    file,
                    "Escort Ships Remaining: %d\n",
                    numberOfShips -
                    destroyedCount
                );

                fprintf(
                    file,
                    "Battleship Damage: %.2f%%\n",
                    battleshipDamage * 100
                );

                fprintf(
                    file,
                    "\nCURRENT IMPACT FACTORS\n"
                );

                for (i = 0;
                     i < numberOfShips;
                     i++)
                {
                    float currentImpact;

                    if (E[i].firingCount > 0)
                    {
                        currentImpact =
                            E[i].impactPower
                            *
                            exp(
                                -E[i].gamma
                                *
                                E[i].firingCount
                            );
                    }
                    else
                    {
                        currentImpact =
                            E[i].impactPower;
                    }

                    fprintf(
                        file,
                        "E%d: %.4f\n",
                        E[i].id,
                        currentImpact
                    );
                }

                fclose(file);
            }

            printf(
                "\nResults saved to results.txt\n"
            );
        }

        /* ==================================
           INSTRUCTIONS
           ================================== */

        else if (choice == 2)
        {
            printf("\n");
            printf(
                "========== INSTRUCTIONS ==========\n"
            );

            printf(
                "1. Start the simulation.\n"
            );

            printf(
                "2. Enter battlefield size.\n"
            );

            printf(
                "3. Select a Battleship type.\n"
            );

            printf(
                "4. Enter the number of Escort Ships.\n"
            );

            printf(
                "5. Escort Ships are placed randomly.\n"
            );

            printf(
                "6. Each Escort Ship has a type and properties.\n"
            );

            printf(
                "7. Battleship attacks Escort Ships.\n"
            );

            printf(
                "8. Nearest Escort Ship is selected first.\n"
            );

            printf(
                "9. Escort Ships can attack the Battleship.\n"
            );

            printf(
                "10. Repeated firing reduces impact power.\n"
            );

            printf(
                "11. Results are saved in results.txt.\n"
            );

            printf(
                "12. Use Simulation Statistics to view previous results.\n"
            );
        }

        /* ==================================
           STATISTICS
           ================================== */

        else if (choice == 3)
        {
            printf("\n");
            printf(
                "========== SIMULATION STATISTICS ==========\n"
            );

            file =
                fopen(
                    "results.txt",
                    "r"
                );

            if (file == NULL)
            {
                printf(
                    "No previous simulation results found.\n"
                );
            }
            else
            {
                while (
                    fgets(
                        line,
                        sizeof(line),
                        file
                    )
                )
                {
                    printf(
                        "%s",
                        line
                    );
                }

                fclose(file);
            }
        }

        /* ==================================
           EXIT
           ================================== */

        else if (choice == 4)
        {
            printf(
                "\nAre you sure you want to exit?\n"
            );

            printf(
                "Enter 1 for Yes, 0 for No: "
            );

            scanf(
                "%d",
                &confirm
            );

            if (confirm == 1)
            {
                printf(
                    "\nThank you for using the simulator.\n"
                );
            }
            else
            {
                choice = 0;
            }
        }

        else
        {
            printf(
                "\nInvalid choice!\n"
            );
        }

    } while (choice != 4);

    return 0;
}