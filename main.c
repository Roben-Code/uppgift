#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <windows.h>

#include "SafeInput.h"

typedef struct {
    int cardID;
    time_t created;
    bool access;
} Player;

void remoteOpenDoor();
void addAndRemoveCard(Player **cards, int *cardCount);
void listALLCards(Player *cards, int cardCount);
void showMenu(int cardCount);
void printCard(Player *p);
void createCard(Player *p);
void fakeTestCard(Player *cards, int cardCount);
Player* findCard(Player *cards, int cardCount, int cardID);

int main() {
    Player *dynamicAllCards = NULL;
    int cardCount = 0;

    time_t currentDate = time(NULL);
    printf("%lld\n", (long long)currentDate);

    char buff[20];
    strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&currentDate));
    printf("nu är det %s\n", buff);

    while (true) {
        showMenu(cardCount);

        int selection;
        if (!GetInputInt("Select an option: ", &selection)) {
            printf("Invalid input. Please try again.\n");
            continue;
        }

        if (selection == 1) {
            remoteOpenDoor();

            if (dynamicAllCards == NULL) {
                dynamicAllCards = malloc(sizeof(Player));
                if (!dynamicAllCards) {
                    fprintf(stderr, "Memory allocation failed.\n");
                    exit(EXIT_FAILURE);
                }
            } else {
                Player *temp = realloc(dynamicAllCards, (cardCount + 1) * sizeof(Player));
                if (!temp) {
                    fprintf(stderr, "Memory allocation failed.\n");
                    free(dynamicAllCards);
                    exit(EXIT_FAILURE);
                }
                dynamicAllCards = temp;
            }

            createCard(&dynamicAllCards[cardCount]);
            cardCount++;
        } else if (selection == 2) {
            listALLCards(dynamicAllCards, cardCount);
        } else if (selection == 3) {
            addAndRemoveCard(&dynamicAllCards, &cardCount);
        } else if (selection == 4) {
            break;
        } else if (selection == 9) {
            fakeTestCard(dynamicAllCards, cardCount);
        } else {
            printf("Invalid selection. Please try again.\n");
        }
    }

    free(dynamicAllCards);
    return 0;
}

void showMenu(int cardCount) {
    printf("1. Remote open door\n");
    printf("2. List all cards - Total: %d\n", cardCount);
    printf("3. Add/Remove access\n");
    printf("4. Exit\n");
    printf("9. FAKE TEST SCAN CARD\n");
}

void remoteOpenDoor() {
    printf("CURRENTLY LAMP IS: Green\n");
    printf("Door is now open.\n");
    Sleep(3000); // Wait for 3 seconds
    printf("Door is now closed.\n");
}

Player* findCard(Player *cards, int cardCount, int cardID) {
    for (int i = 0; i < cardCount; i++) {
        if (cards[i].cardID == cardID) {
            return &cards[i];
        }
    }
    return NULL;
}

void printCard(Player *p) {
    printf("*** INFO ABOUT CARD ***\n");
    printf("Card ID: %d | Access: %s\n", p->cardID, p->access ? "Granted" : "Denied");
}

void createCard(Player *p) {
    printf("Enter card ID: ");
    while (!GetInputInt("", &p->cardID)) {
        printf("Invalid input. Please enter a valid card ID: ");
    }

    printf("Enter 1 for access, 2 for no access: ");
    int accessChoice;
    while (!GetInputInt("", &accessChoice) || (accessChoice != 1 && accessChoice != 2)) {
        printf("Invalid choice. Enter 1 for access or 2 for no access: ");
    }

    p->access = (accessChoice == 1);
    p->created = time(NULL);
}

void listALLCards(Player *cards, int cardCount) {
    if (cardCount == 0) {
        printf("No cards in the system.\n");
        return;
    }

    printf("\nAll cards in system:\n");
    printf("--------------------------------------\n");

    char buff[20];
    for (int i = 0; i < cardCount; i++) {
        strftime(buff, 20, "%Y-%m-%d", localtime(&cards[i].created));
        printf("Card Number: %d | Access: %s | Added: %s\n",
               cards[i].cardID,
               cards[i].access ? "Granted" : "Denied",
               buff);
    }

    printf("--------------------------------------\n");
    printf("Press Enter to continue...\n");
    getchar();
}

void addAndRemoveCard(Player **cards, int *cardCount) {
    int cardID;
    printf("Enter card number: ");
    while (!GetInputInt("", &cardID)) {
        printf("Invalid input. Please enter a valid card number: ");
    }

    Player *existingCard = findCard(*cards, *cardCount, cardID);

    if (existingCard) {
        printf("This card currently has %s.\n", existingCard->access ? "access" : "no access");
        printf("Enter 1 to grant access, 2 to revoke access: ");

        int pick;
        while (!GetInputInt("", &pick) || (pick != 1 && pick != 2)) {
            printf("Invalid selection. Enter 1 to grant access or 2 to revoke access: ");
        }

        existingCard->access = (pick == 1);
        printf("Access %s for card %d.\n", existingCard->access ? "granted" : "revoked", cardID);
    } else {
        printf("This card does not exist. Creating a new card.\n");

        Player *temp = realloc(*cards, (*cardCount + 1) * sizeof(Player));
        if (!temp) {
            fprintf(stderr, "Memory allocation failed.\n");
            return;
        }
        *cards = temp;

        Player *newCard = &(*cards)[*cardCount];
        newCard->cardID = cardID;
        printf("Enter 1 for access, 2 for no access: ");

        int pick;
        while (!GetInputInt("", &pick) || (pick != 1 && pick != 2)) {
            printf("Invalid selection. Enter 1 for access or 2 for no access: ");
        }

        newCard->access = (pick == 1);
        newCard->created = time(NULL);

        (*cardCount)++;
        printf("Card %d created with %s access.\n", cardID, newCard->access ? "granted" : "revoked");
    }
}

void fakeTestCard(Player *cards, int cardCount) {
    char input[10];
    while (true) {
        printf("Please scan card to enter or X to go back to admin menu\n");
        printf("CURRENTLY LAMP IS: Off\n");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0; // Remove newline character

        if (strcasecmp(input, "X") == 0) {
            printf("Returning to admin menu.\n");
            break;
        }

        char *endptr;
        long scannedCardID = strtol(input, &endptr, 10);
        if (*endptr != '\0' || scannedCardID < INT_MIN || scannedCardID > INT_MAX) {
            printf("Invalid card ID. Try again.\n");
            continue;
        }

        Player *card = findCard(cards, cardCount, (int)scannedCardID);

        if (card && card->access) {
            printf("CURRENTLY LAMP IS: Green\n");
        } else {
            printf("CURRENTLY LAMP IS: Red\n");
        }
    }
}
