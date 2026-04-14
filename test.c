#include<stdio.h> 

int main(){
    int n;
    printf("Enter a Number : ");
    scanf("%d", &n);

    int newN = 0;
    while(n!=0){
        newN *= 10;
        newN += n%10;
        n /= 10;
    }
    printf("Reversed number : %d", newN);
    return 0;
}