#include <stdio.h>

int main(){
    int days;
    printf("Enter Number of Days: ");
    scanf("%d", &days);

    int y = 0, m = 0, d = 0;
    if(days>=365){
        y = days/365;
        days %= 365;
    }
    if(days>=30){
        m = days/30;
        days %= 30;
    }
    d = days;

    printf("Year : %d \nMonth : %d \nDays : %d", y, m, d);

    return 0;
}

