#include <stdio.h>
#include <string.h>

struct AddressTmpl
{
    char sStreet[51];
    char sCity[51];
    char sCountry[51];
};

enum Gender
{
    Male     = 1,
    Female   = 2
};

struct PersonTmpl
{
    int nPersonID;

    char chRecordType;
#define REC_TYPE_PERSONAL  1
#define REC_TYPE_HOME      2
#define REC_TYPE_WORK      3

    union
    {
        struct /* record type 1 */
        {
            char sFirstName[31];
            char sLastName[32];
            char sBirthdate[11]; /* YYYY/MM/DD date format */
            enum Gender theGender;
        }PersonalInfo;

        struct /* record type 2 */
        {
            struct AddressTmpl homeAddress;
            char sPhone[31];
            char sEmail[51];
        }HomeDetails;

        struct /* record type 3 */
        {
            char sCompany[51];
            struct AddressTmpl workAddress;
            char sPhone[31];
            char sFax[31];
            char sEmail[51];
        }WorkDetails;
    }Details;
} __attribute__((packed));


void citireFisierTest(char* inputFile, char* outputFile)
{
    FILE *fisier_test = fopen("testc.ini", "r");
    char line[256];
    int inSection = 0;

    if(!fisier_test){
        printf("Eroare");
        return;
    }

    while (fgets(line, sizeof(line), fisier_test) != NULL){
        line[strcspn(line, "\r\n")] = 0;

        if(strcmp(line, "[testc]") == 0)
            inSection = 1;

        if(inSection == 1){
            if(strncmp(line, "in=", 3) == 0)
                strcpy(inputFile, line + 3);
            else{
                if(strncmp(line, "out=", 4) == 0)
                    strcpy(outputFile, line + 4);
            }
        }
    }

    fclose(fisier_test);
}

int nrCifre(int n)
{
    if(n == 0)
        return 0;
    else
        return 1 + nrCifre(n / 10);
}

void afisareSpatii(int n, FILE *outputFile)
{
    int i;
    for(i = 0; i < n; i++)
        fprintf(outputFile, " ");
}

int main() {
    char inputFile[256], outputFile[256];

    citireFisierTest(inputFile, outputFile);

    FILE *fisier_binar = fopen(inputFile, "rb");
    FILE *fisier_ascii = fopen(outputFile, "w");

    char spatiu[31] = {' '};

    struct PersonTmpl person;
    size_t readSize;

    while((readSize = fread(&person, sizeof(struct PersonTmpl), 1, fisier_binar)) == 1){
        switch (person.chRecordType) {
            case REC_TYPE_PERSONAL:
                fprintf(fisier_ascii, "%d", person.nPersonID);
                afisareSpatii(4 - nrCifre(person.nPersonID), fisier_ascii);
                fprintf(fisier_ascii, "%c", person.chRecordType);
                fprintf(fisier_ascii, "%s", person.Details.PersonalInfo.sLastName);
                afisareSpatii(30 - strlen(person.Details.PersonalInfo.sLastName), fisier_ascii);
                fprintf(fisier_ascii, "%s", person.Details.PersonalInfo.sFirstName);
                afisareSpatii(30 - strlen(person.Details.PersonalInfo.sFirstName), fisier_ascii);
                fprintf(fisier_ascii, "%s", person.Details.PersonalInfo.sBirthdate);
                afisareSpatii(10 - strlen(person.Details.PersonalInfo.sBirthdate), fisier_ascii);
                fprintf(fisier_ascii, "%c\n", person.Details.PersonalInfo.theGender == Male ? 'M' : 'F');
            break;

            case REC_TYPE_HOME:
                fprintf(fisier_ascii, "%d,%c %s,%s,%s,%s,%s\n",
                         person.nPersonID,
                         person.chRecordType,
                         person.Details.HomeDetails.homeAddress.sStreet,
                         person.Details.HomeDetails.homeAddress.sCity,
                         person.Details.HomeDetails.homeAddress.sCountry,
                         person.Details.HomeDetails.sPhone,
                         person.Details.HomeDetails.sEmail);
                break;

            case REC_TYPE_WORK:
                fprintf(fisier_ascii, "%d,%d,%s,%s,%s,%s,%s,%s,%s\n",
                         person.nPersonID,
                         person.chRecordType,
                         person.Details.WorkDetails.sCompany,
                         person.Details.WorkDetails.workAddress.sStreet,
                         person.Details.WorkDetails.workAddress.sCity,
                         person.Details.WorkDetails.workAddress.sCountry,
                         person.Details.WorkDetails.sPhone,
                         person.Details.WorkDetails.sFax,
                         person.Details.WorkDetails.sEmail);
                break;

            default: break;
        }
    }

    fclose(fisier_binar);
    fclose(fisier_ascii);

    return 0;
}


