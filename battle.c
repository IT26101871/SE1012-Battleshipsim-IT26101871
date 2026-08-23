#include<stdio.h>
int main(){
	int size;
	printf("Enter battlefield size: ");
	scanf("%d", &size);
	printf("\nBattlesield: 0 to %d\n ", size);
	printf("Battleship position: (%d,%d)\n",size/2,size/2);
	return 0;
}
