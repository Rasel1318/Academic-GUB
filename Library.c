#include <stdio.h>
#include <stdlib.h>


typedef struct Node{
	int val;
	struct Node* next;
}Node;

int n = 0;
Node* create(int v){
	Node* t = (Node*)malloc(sizeof(Node));
	t->val = v;
	return t;
}

Node* insertAtBegining(Node* head, int data){
	Node* t = create(data);
	if(head==NULL) { n++; return t;}
	else{
		t->next = head;
		n++;
		return t;
	}
}

void insertAtEnd(Node* head, int data){
	Node* t = create(data);
	Node *root = head;
	while(root->next!=NULL) 
		root = root->next;
	root->next = t;
	n++;
}

void insertAtPosition(Node* head, int p, int data){
	if(p==1){
		insertAtBegining(head, data);
		return;
	}else if(p>=n){
		// printf("dfjl %d", n);
		insertAtEnd(head, data);
		return;
	}
	int pos = 1;
	Node* t = create(data);
	Node* root = head;

	while(pos+1!=p){
		root = root->next;
		pos++;
	}
	// printf("\n%d\n", pos);
	Node* last = root->next;
	root->next = t;
	t->next = last;
	n++;
}
Node* deleteAtBegining(Node* head){
	Node* curr = head->next;
	n--;
	free(head);
	return curr;
}
void deleteAtEnd(Node* head){
	Node* t = head;
	while(t->next->next!=NULL){
		t = t->next;
	}
	t->next = NULL;
	n--;
}
void deleteAtPosition(Node* head, int p){
	if(p==1){
		deleteAtBegining(head);
		return;
	}else if(p>=n){
		deleteAtEnd(head);
		return;
	}
	int pos = 1;
	Node* root = head;

	while(pos+1!=p){
		root = root->next;
		pos++;
	}
	Node* curr = root->next;
	root->next = root->next->next;
	free(curr);
	n--;
}



void pr(Node* head){
	while(head!=NULL){
		printf("%d ", head->val);
		head = head->next;
	}printf("\n");
}

int main(){
	Node* root = NULL;

	root = insertAtBegining(root, 5);
	root = insertAtBegining(root, 10);
	root = insertAtBegining(root, 15);
	// printf("\n%d\n", n);

	insertAtEnd(root, 4);
	insertAtEnd(root, 2);
	insertAtEnd(root, 1);
	// printf("\n%d\n", n);

	insertAtPosition(root, 3, 34);
	insertAtPosition(root, 3, 344);
	insertAtPosition(root, 7, 234);

	deleteAtPosition(root, 3);
	deleteAtPosition(root, 6);

	pr(root);
    return 0;
}
