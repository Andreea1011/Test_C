#include <stdio.h>
#include <string.h>

#define COLUMN_LENGTH 20

struct AddressTmpl {
    char sStreet[51];
    char sCity[51];
    char sCountry[51];
};

enum Gender {
    Male = 1,
    Female = 2
};

struct PersonTmpl {
    int nPersonID;
    char chRecordType;

#define REC_TYPE_PERSONAL  1
#define REC_TYPE_HOME      2
#define REC_TYPE_WORK      3

    union {
        struct /* record type 1 */
        {
            char sFirstName[31];
            char sLastName[32];
            char sBirthdate[11]; /* YYYY/MM/DD date format */
            enum Gender theGender;
        } PersonalInfo;

        struct /* record type 2 */
        {
            struct AddressTmpl homeAddress;
            char sPhone[31];
            char sEmail[51];
        } HomeDetails;

        struct /* record type 3 */
        {
            char sCompany[51];
            struct AddressTmpl workAddress;
            char sPhone[31];
            char sFax[31];
            char sEmail[51];
        } WorkDetails;
    } Details;
} __attribute__((packed));

void citireSetari(char *inputFile, char *outputFile) {
    FILE *fisier_configurare = fopen("testc.ini", "r");
    char line[256];
    int inSection = 0;

    if (!fisier_configurare) {
        printf("Eroare");
        return;
    }

    while (fgets(line, sizeof(line), fisier_configurare) != NULL) {
        line[strcspn(line, "\r\n")] = 0;

        if (strcmp(line, "[testc]") == 0) inSection = 1;

        if (inSection == 1) {
            if (strncmp(line, "in=", 3) == 0) {
                strcpy(inputFile, line + 3);
            } else if (strncmp(line, "out=", 4) == 0) {
                strcpy(outputFile, line + 4);
            }
        }
    }

    fclose(fisier_configurare);
}

int main() {
    char inputFile[256], outputFile[256];

    citireSetari(inputFile, outputFile);

    FILE *fisier_binar = fopen(inputFile, "rb");
    FILE *fisier_ascii = fopen(outputFile, "w");

    struct PersonTmpl person;

    while ((fread(&person, sizeof(struct PersonTmpl), 1, fisier_binar)) == 1) {
        switch (person.chRecordType) {
            case REC_TYPE_PERSONAL:
                fprintf(fisier_ascii, "%*d%*d %-*.*s%-*.*s %-*.*s %c\n",
                        0, person.nPersonID,
                        4, person.chRecordType,
                        30, COLUMN_LENGTH, person.Details.PersonalInfo.sLastName,
                        30, COLUMN_LENGTH, person.Details.PersonalInfo.sFirstName,
                        10, COLUMN_LENGTH, person.Details.PersonalInfo.sBirthdate,
                        person.Details.PersonalInfo.theGender == Male ? 'M' : 'F');
                break;

            case REC_TYPE_HOME:
                fprintf(fisier_ascii,
                        "%*d% *d %-*.*s%-*.*s%-*.*s%-*.*s%-*.*s\n",
                        0, person.nPersonID,
                        4, person.chRecordType,
                        30, COLUMN_LENGTH, person.Details.HomeDetails.homeAddress.sStreet,
                        30, COLUMN_LENGTH, person.Details.HomeDetails.homeAddress.sCity,
                        30, COLUMN_LENGTH, person.Details.HomeDetails.homeAddress.sCountry,
                        15, COLUMN_LENGTH, person.Details.HomeDetails.sPhone,
                        30, COLUMN_LENGTH, person.Details.HomeDetails.sEmail);
                break;

            case REC_TYPE_WORK:
                fprintf(fisier_ascii,
                        "%*d% *d %-*.*s%-*.*s%-*.*s%-*.*s%-*.*s%-*.*s%-*.*s\n",
                        0, person.nPersonID,
                        4, person.chRecordType,
                        30, COLUMN_LENGTH, person.Details.WorkDetails.sCompany,
                        30, COLUMN_LENGTH, person.Details.WorkDetails.workAddress.sStreet,
                        30, COLUMN_LENGTH, person.Details.WorkDetails.workAddress.sCity,
                        30, COLUMN_LENGTH, person.Details.WorkDetails.workAddress.sCountry,
                        15, COLUMN_LENGTH, person.Details.WorkDetails.sPhone,
                        15, COLUMN_LENGTH, person.Details.WorkDetails.sFax,
                        30, COLUMN_LENGTH, person.Details.WorkDetails.sEmail);
                break;

            default:
                break;
        }
    }

    fclose(fisier_binar);
    fclose(fisier_ascii);

    return 0;
}